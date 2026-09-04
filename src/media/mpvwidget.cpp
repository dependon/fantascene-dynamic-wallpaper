/*
 * Copyright (C) 2020 ~ 2022 LiuMingHang.
 *
 * Author:     LiuMingHang <liuminghang0821@gmail.com>
 *
 * Maintainer: LiuMingHang <liuminghang0821@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "mpvwidget.h"
#include <stdexcept>
#include <QtGui/QOpenGLContext>
#include <QtCore/QMetaObject>
#include "application.h"
#include "inimanager.h"
#include <QDateTime>
#include <QDebug>
#include <QGuiApplication>
#include <QStandardPaths>

#if defined(Q_OS_LINUX) && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/qguiapplication_platform.h>
#elif defined(Q_OS_LINUX)
#include <qpa/qplatformnativeinterface.h>
#endif

namespace {

QString normalizedHwdec(QString hwdec)
{
    hwdec = hwdec.trimmed();

    // "gpu" and "gpu-next" are video-output names, not hardware decoder
    // backends.  Older Fantascene versions stored them as the hwdec value,
    // which makes current mpv silently fall back to software decoding.
    if (hwdec.isEmpty() || hwdec == QLatin1String("gpu") ||
            hwdec == QLatin1String("gpu-next")) {
        return QStringLiteral("auto-safe");
    }

    return hwdec;
}

bool setMpvOption(mpv_handle *mpv, const char *name, const QString &value)
{
    const QByteArray utf8Value = value.toUtf8();
    const int error = mpv_set_option_string(mpv, name, utf8Value.constData());
    if (error < 0) {
        qWarning().noquote() << "Failed to set mpv option" << name << "to"
                             << value << ":" << mpv_error_string(error);
        return false;
    }
    return true;
}

} // namespace

static void wakeup(void *ctx)
{
    QMetaObject::invokeMethod((MpvWidget *)ctx, "on_mpv_events", Qt::QueuedConnection);
}

static void *get_proc_address(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

#ifdef Q_OS_LINUX
static void *nativeWaylandDisplay()
{
    if (!QGuiApplication::platformName().startsWith(QLatin1String("wayland")))
        return nullptr;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (auto *wayland = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>())
        return wayland->display();
#else
    if (auto *native = QGuiApplication::platformNativeInterface())
        return native->nativeResourceForIntegration(QByteArrayLiteral("wl_display"));
#endif
    return nullptr;
}
#endif

MpvWidget::MpvWidget(QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
{
    setAttribute(Qt::WA_TranslucentBackground);
    mpv = mpv_create();
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

    // 反推版本号
//    unsigned int major = (MPV_CLIENT_API_VERSION >> 16);
//    unsigned int minor = (MPV_CLIENT_API_VERSION & 0xFFFF);


    mpv_set_option_string(mpv, "terminal", "no");
    mpv_set_option_string(mpv, "msg-level", "all=v");

    // A wallpaper is continuously downscaled and composited, often once per
    // output. Prefer mpv's lightweight GPU pipeline over its default Lanczos
    // scaler; this substantially reduces the cost of 4K sources on 1080p
    // displays without changing the decoded frame rate.
    setMpvOption(mpv, "profile", QStringLiteral("fast"));

#if MPV_MAKE_VERSION(1,108) < MPV_CLIENT_API_VERSION
    if(IniManager::instance()->contains("WallPaper/vo"))
    {
        dApp->m_moreData.vo = IniManager::instance()->value("WallPaper/vo").toString();
        QString ss;
        if(dApp->m_moreData.vo.length() > 0)
        {
            ss = dApp->m_moreData.vo ;
            setMpvOption(mpv, "vo", dApp->m_moreData.vo);
        }
        else
        {
            //默认改为libmpv
            setMpvOption(mpv, "vo", QStringLiteral("libmpv"));
        }
    }
	else
	{
		//默认改为libmpv
		setMpvOption(mpv, "vo", QStringLiteral("libmpv"));
	}
#else
    // Make use of the MPV_SUB_API_OPENGL_CB API.
    setMpvOption(mpv, "vo", QStringLiteral("opengl-cb"));
#endif

    const QString configuredHwdec = IniManager::instance()->contains("WallPaper/hwdec")
            ? IniManager::instance()->value("WallPaper/hwdec").toString()
            : QString();
    dApp->m_moreData.hwdec = normalizedHwdec(configuredHwdec);
    QString mpvHwdec = dApp->m_moreData.hwdec;
#ifdef Q_OS_LINUX
    // mpv's safe automatic selection currently prefers vaapi-copy with the
    // libmpv render API.  On Linux, try direct VA-API first so decoded DRM
    // PRIME frames stay on the GPU; retain auto-safe as a portable fallback.
    if (mpvHwdec == QLatin1String("auto") ||
            mpvHwdec == QLatin1String("auto-safe")) {
        mpvHwdec.prepend(QStringLiteral("vaapi,"));
    }
#endif
    if (!setMpvOption(mpv, "hwdec", mpvHwdec)) {
        dApp->m_moreData.hwdec = QStringLiteral("auto-safe");
        setMpvOption(mpv, "hwdec", dApp->m_moreData.hwdec);
    }

    // Options such as vo and hwdec are startup options. Setting them after
    // mpv_initialize() fails, so initialize only after all of them are ready.
    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

#if MPV_MAKE_VERSION(1,108) < MPV_CLIENT_API_VERSION
#else
    mpv_gl = (mpv_opengl_cb_context *)mpv_get_sub_api(mpv, MPV_SUB_API_OPENGL_CB);
    if (!mpv_gl)
        throw std::runtime_error("OpenGL not compiled in");
    mpv_opengl_cb_set_update_callback(mpv_gl, MpvWidget::on_update, (void *)this);
    connect(this, SIGNAL(frameSwapped()), this,SLOT(swapped()));
#endif
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_request_log_messages(mpv, "info");
    mpv_set_wakeup_callback(mpv, wakeup, this);

    QList<QVariant> list;
    list << "no-correct-pts";
//    mpv::qt::command_variant(mpv, list);
    mpv::qt::set_property_variant(mpv, "correct-pts", "no");
    if(IniManager::instance()->contains("WallPaper/fps"))
    {
        dApp->m_moreData.fps = IniManager::instance()->value("WallPaper/fps").toInt();
        mpv::qt::set_property_variant(mpv, "fps", dApp->m_moreData.fps);
    }
    else {
        mpv::qt::set_property_variant(mpv, "fps", "0");
    }

    if(IniManager::instance()->contains("WallPaper/voiceVolume"))
    {
       int iVulume =  IniManager::instance()->value("WallPaper/voiceVolume").toInt();
       mpv::qt::set_property_variant(mpv, "volume", iVulume);
    }
    if(IniManager::instance()->contains("WallPaper/videoAspect"))
    {
       double videoAspect = IniManager::instance()->value("WallPaper/videoAspect").toDouble();
       mpv::qt::set_property_variant(mpv, "video-aspect", videoAspect);
    }


    mpv::qt::set_property_variant(mpv, "loop", "inf");

    connect(dApp, &Application::sigscreenshot, this, [ = ] {
        m_bScrrenShot = true;
    });

    setProperty("loop", true);
    setProperty("panscan", 1);

}

MpvWidget::~MpvWidget()
{
    makeCurrent();
#if MPV_MAKE_VERSION(1,108) < MPV_CLIENT_API_VERSION
    if (mpv_gl)
        mpv_render_context_free(mpv_gl);

    //opengl cb crash boom!!!!
    mpv_terminate_destroy(mpv);
#else
    if (mpv_gl)
        mpv_opengl_cb_set_update_callback(mpv_gl, NULL, NULL);
#endif


}

void MpvWidget::command(const QVariant &params)
{
    mpv::qt::command_variant(mpv, params);
}

void MpvWidget::setProperty(const QString &name, const QVariant &value)
{
    mpv::qt::set_property_variant(mpv, name, value);
}

QVariant MpvWidget::getProperty(const QString &name) const
{
    return mpv::qt::get_property_variant(mpv, name);
}

void MpvWidget::initializeGL()
{
#if MPV_MAKE_VERSION(1,108) < MPV_CLIENT_API_VERSION
    mpv_opengl_init_params gl_init_params{get_proc_address, nullptr};
    void *waylandDisplay = nullptr;
#ifdef Q_OS_LINUX
    waylandDisplay = nativeWaylandDisplay();
#endif
    mpv_render_param params[] {
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
#ifdef Q_OS_LINUX
        {waylandDisplay ? MPV_RENDER_PARAM_WL_DISPLAY : MPV_RENDER_PARAM_INVALID,
         waylandDisplay},
#endif
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    if (mpv_render_context_create(&mpv_gl, mpv, params) < 0)
        throw std::runtime_error("failed to initialize mpv GL context");
    mpv_render_context_set_update_callback(mpv_gl, MpvWidget::on_update, reinterpret_cast<void *>(this));
#else
    int r = mpv_opengl_cb_init_gl(mpv_gl, NULL, get_proc_address, NULL);
    if (r < 0)
        throw std::runtime_error("could not initialize OpenGL");

#endif

}

void MpvWidget::paintGL()
{
    int iwidth = width();
    int iheight = height();
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
    double dwidth = iwidth * devicePixelRatio();
    double dheight = iheight * devicePixelRatio();
#else
    double dwidth = iwidth * devicePixelRatioF();
    double dheight = iheight * devicePixelRatioF();
#endif

    int deviceiwidth = dwidth;
    int deviceiheight = dheight;
#if MPV_MAKE_VERSION(1,108) < MPV_CLIENT_API_VERSION
    mpv_opengl_fbo mpfbo{static_cast<int>(defaultFramebufferObject()), deviceiwidth, deviceiheight, 0};
    int flip_y{1};

    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    // See render_gl.h on what OpenGL environment mpv expects, and
    // other API details.
    mpv_render_context_render(mpv_gl, params);
#else
//    mpv_opengl_cb_draw(mpv_gl, defaultFramebufferObject(), width(), -height());
    mpv_opengl_cb_draw(mpv_gl, defaultFramebufferObject(), deviceiwidth, -deviceiheight);
#endif
    if (m_bScrrenShot) {
        m_bScrrenShot = false;
        QPixmap pix = QPixmap::fromImage(grabFramebuffer());
        QDateTime::currentMSecsSinceEpoch();
        QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png";
        pix.save(path);
    }
//    if (dApp->m_cuurentMode == IdCopyScreen && dApp->m_currentScreenNum > 1) {
//        QPixmap pix = QPixmap::fromImage(grabFramebuffer());
//        emit dApp->refreshPix(pix);
//    }
}
#if MPV_MAKE_VERSION(1,108) < MPV_CLIENT_API_VERSION
#else
void MpvWidget::swapped()
{
    mpv_opengl_cb_report_flip(mpv_gl, 0);
}
#endif
void MpvWidget::on_mpv_events()
{
    // Process all events, until the event queue is empty.
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        handle_mpv_event(event);
    }
}

void MpvWidget::handle_mpv_event(mpv_event *event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        mpv_event_property *prop = (mpv_event_property *)event->data;
        if (strcmp(prop->name, "time-pos") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE) {
                double time = *(double *)prop->data;
                Q_EMIT positionChanged(time);
            }
        } else if (strcmp(prop->name, "duration") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE) {
                double time = *(double *)prop->data;
                Q_EMIT durationChanged(time);
            }
        }
        break;
    }
    case MPV_EVENT_VIDEO_RECONFIG:
        qInfo().noquote() << "mpv video output:"
                          << "hwdec=" << getProperty(QStringLiteral("hwdec-current")).toString()
                          << "interop=" << getProperty(QStringLiteral("hwdec-interop")).toString()
                          << "vo=" << getProperty(QStringLiteral("current-vo")).toString()
                          << "gpu-context=" << getProperty(QStringLiteral("current-gpu-context")).toString();
        break;
    case MPV_EVENT_LOG_MESSAGE: {
        const mpv_event_log_message *message =
                static_cast<const mpv_event_log_message *>(event->data);
        qInfo().noquote() << QStringLiteral("mpv[%1/%2]")
                                .arg(QString::fromUtf8(message->prefix),
                                     QString::fromUtf8(message->level))
                          << QString::fromUtf8(message->text).trimmed();
        break;
    }
    default: ;
        // Ignore uninteresting or unknown events.
    }
}

// Make Qt invoke mpv_render_context_render() to draw a new/updated video frame.
void MpvWidget::maybeUpdate()
{
    // If the Qt window is not visible, Qt's update() will just skip rendering.
    // This confuses mpv's render API, and may lead to small occasional
    // freezes due to video rendering timing out.
    // Handle this by manually redrawing.
    // Note: Qt doesn't seem to provide a way to query whether update() will
    //       be skipped, and the following code still fails when e.g. switching
    //       to a different workspace with a reparenting window manager.
    if (window()->isMinimized()) {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        doneCurrent();
    } else {
        update();
    }
}

void MpvWidget::on_update(void *ctx)
{
    QMetaObject::invokeMethod((MpvWidget *)ctx, "maybeUpdate");
}
