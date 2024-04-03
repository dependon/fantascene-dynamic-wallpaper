#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T09:14:17
#
#-------------------------------------------------

include($$PWD/dde-desktop-build.pri)
include($$PWD/util/util.pri)
include($$PWD/dbus/dbus.pri)
include(../dde-file-manager-lib/dde-file-manager-lib.pri)
include($$PWD/../dde-wallpaper-chooser/dde-wallpaper-chooser.pri)
# 集成测试标签
include($$PWD/../../3rdparty/accessibility/accessibility-suite.pri)
#无热区设置
#include($$PWD/../dde-zone/dde-zone.pri)
DEFINES += DISABLE_ZONE

# 获取标签系统设置
AC_FUNC_ENABLE = true
#AC_FUNC_ENABLE = $$(ENABLE_AC_FUNC)
# 检查集成测试标签
equals( AC_FUNC_ENABLE, true ){
    DEFINES += ENABLE_ACCESSIBILITY
    message("dde-desktop enabled accessibility function with set: " $$AC_FUNC_ENABLE)
}

QT       += core gui widgets dbus x11extras network concurrent multimedia #multimediawidgets svg
QT       += core gui svg dbus x11extras concurrent multimedia dbus xml KCodecs network
#private
QT       += gui-private
#QT       +=qml-private
LIBS	 += -lKF5Codecs
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

greaterThan(QT_MINOR_VERSION, 7): QT += gui-private
else: QT += platformsupport-private

TEMPLATE    = app
TARGET      = dde-desktop
DESTDIR     = $$BUILD_DIST
CONFIG      += c++11 link_pkgconfig
PKGCONFIG   += xcb xcb-ewmh xcb-shape dframeworkdbus gio-qt x11

CONFIG(release, debug|release) {
    TRANSLATIONS = $$files($$PWD/translations/*.ts)
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    for(tsfile, TRANSLATIONS) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
}

SOURCES += \
    main.cpp \
    config/config.cpp \
    desktop.cpp \
    view/canvasviewhelper.cpp \
#    view/canvasview.cpp \
    model/dfileselectionmodel.cpp \
    view/canvasgridview.cpp \
    presenter/apppresenter.cpp \
    presenter/gridmanager.cpp \
    dbus/dbusdisplay.cpp \
    presenter/dfmsocketinterface.cpp \
    view/watermaskframe.cpp \
    dbus/dbusdock.cpp \
    view/desktopitemdelegate.cpp \
    presenter/gridcore.cpp \
    dbus/dbusmonitor.cpp \
    screen/abstractscreen.cpp \
    screen/abstractscreenmanager.cpp \
    screen/screenmanager.cpp \
    screen/screenobject.cpp \
    view/backgroundmanager.cpp \
    screen/screenhelper.cpp \
    view/backgroundwidget.cpp \
    screen/screenmanagerwayland.cpp \
    screen/screenobjectwayland.cpp \
    dbus/licenceInterface.cpp \
    view/canvasviewmanager.cpp \
#    dbusadapt/filedialogmanager_adaptor.cpp \



HEADERS += \
    config/config.h \
    desktop.h \
    view/canvasviewhelper.h \
    model/dfileselectionmodel.h \
    view/private/canvasviewprivate.h \
    global/coorinate.h \
    global/singleton.h \
    view/canvasgridview.h \
    presenter/apppresenter.h \
    presenter/gridmanager.h \
    dbus/dbusdisplay.h \
    presenter/dfmsocketinterface.h \
    view/watermaskframe.h \
    dbus/dbusdock.h \
    view/desktopitemdelegate.h \
    presenter/gridcore.h \
    deventfilter.h \
    dbus/dbusmonitor.h \
    dbus/licenceInterface.h \
    screen/abstractscreen.h \
    screen/abstractscreenmanager.h \
    screen/abstractscreenmanager_p.h \
    screen/screenmanager.h \
    screen/screenobject.h \
    view/backgroundmanager.h \
    screen/screenhelper.h \
    view/backgroundwidget.h \
    screen/screenmanagerwayland.h \
    screen/screenobjectwayland.h \
    view/canvasviewmanager.h \
    accessibility/ac-desktop-define.h \
    accessibility/acobjectlist.h \
    desktopprivate.h \
    presenter/dfmsocketinterface_p.h \
    dbusadapt/filedialogmanager_adaptor.h \

RESOURCES +=

# Automating generation .qm files from .ts files

CONFIG(release, debug|release) {
    !system($$PWD/translate_generation.sh): error("Failed to generate translation")
    !system($$PWD/translate_ts2desktop.sh): error("Failed to generate translation")
}

TRANSLATIONS += $$PWD/translations/$${TARGET}.ts \
                $$PWD/translations/$${TARGET}_zh_CN.ts

include($$PWD/development.pri)

target.path=/opt/durapps/fantascene-dynamic-wallpaper

translations.path = /opt/durapps/fantascene-dynamic-wallpaper/translations
translations.files = $$PWD/translations/*.qm

INSTALLS += target translations
