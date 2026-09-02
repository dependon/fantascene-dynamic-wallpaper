/*
 * Copyright (C) 2026 CharOfString <root@charofstring.cc>
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

#include "utils/wayland_utils.h"

#include <wayland-client.h>

#include <LayerShellQt/Window>
#include <QFileInfo>
#include <QScreen>
#include <QStackedLayout>
#include <QUrl>
#include <QWindow>
#include <cstring>

#include "media/mediapluginwidget.h"
#include "webwidget.h"

namespace Utils {
namespace LayerShell {
namespace {

bool g_layer_shell_detected = false;
bool g_layer_shell_available = false;

void RegistryGlobal(void* data, wl_registry*, uint32_t, const char* interface,
    uint32_t) {
  if (std::strcmp(interface, "zwlr_layer_shell_v1") == 0) {
    *static_cast<bool*>(data) = true;
  }
}

void RegistryGlobalRemove(void*, wl_registry*, uint32_t) {}

const wl_registry_listener kRegistryListener = {
  RegistryGlobal,
  RegistryGlobalRemove,
};

bool IsWebWallpaper(const QString& path) {
  const QUrl url(path);
  return path.endsWith(QLatin1String(".html"), Qt::CaseInsensitive) ||
    path.endsWith(QLatin1String(".htm"), Qt::CaseInsensitive) ||
    url.scheme() == QLatin1String("http") ||
    url.scheme() == QLatin1String("https");
}

}  // namespace

void DetectLayerShellSupport() {
  if (g_layer_shell_detected) {
    return;
  }

  g_layer_shell_detected = true;
  wl_display* display = wl_display_connect(nullptr);
  if (display == nullptr) {
    return;
  }

  wl_registry* registry = wl_display_get_registry(display);
  if (registry != nullptr) {
    wl_registry_add_listener(registry, &kRegistryListener,
      &g_layer_shell_available);

    if (wl_display_roundtrip(display) < 0) {
      g_layer_shell_available = false;
    }
    wl_registry_destroy(registry);
  }

  wl_display_disconnect(display);
}

bool IsLayerShellAvailable() {
  DetectLayerShellSupport();
  return g_layer_shell_available;
}

WaylandWallpaperSurface::WaylandWallpaperSurface(QScreen* screen)
    : QWidget(nullptr), screen_(screen), layout_(new QStackedLayout(this)) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_NativeWindow);
  setAutoFillBackground(false);

  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->setStackingMode(QStackedLayout::StackOne);
}

WaylandWallpaperSurface::~WaylandWallpaperSurface() = default;

QScreen* WaylandWallpaperSurface::screen() const {
  return screen_.data();
}

void WaylandWallpaperSurface::SetPrimary(bool is_primary) {
  is_primary_ = is_primary;
  if (media_ != nullptr) {
    media_->setVolume(is_primary_ ? requested_volume_ : 0);
  }
}

bool WaylandWallpaperSurface::IsPrimary() const {
  return is_primary_;
}

void WaylandWallpaperSurface::ConfigureLayerShell() {
  QWindow* window = windowHandle();
  if (window == nullptr || screen_.isNull()) {
    return;
  }

  window->setScreen(screen_);

  LayerShellQt::Window* layer_window = LayerShellQt::Window::get(window);
  layer_window->setScreenConfiguration(LayerShellQt::Window::ScreenFromQWindow);
  layer_window->setLayer(LayerShellQt::Window::LayerBackground);

  LayerShellQt::Window::Anchors anchors(LayerShellQt::Window::AnchorTop);
  anchors |= LayerShellQt::Window::AnchorBottom;
  anchors |= LayerShellQt::Window::AnchorLeft;
  anchors |= LayerShellQt::Window::AnchorRight;
  layer_window->setAnchors(anchors);

  layer_window->setExclusiveZone(-1);
  layer_window->setKeyboardInteractivity(
    LayerShellQt::Window::KeyboardInteractivityNone);
  layer_window->setCloseOnDismissed(false);
  layer_window->setScope(
    QStringLiteral("fantascene-wallpaper-%1").arg(screen_->name()));

  resize(screen_->size());
}

void WaylandWallpaperSurface::SetFile(const QString& path) {
  if (IsWebWallpaper(path)) {
    if (media_ != nullptr) {
      layout_->removeWidget(media_);
      delete media_;
      media_ = nullptr;
    }

    if (web_view_ == nullptr) {
      web_view_ = new webWidget(this);
      web_view_->setContextMenuPolicy(Qt::NoContextMenu);
      layout_->addWidget(web_view_);
    }

    web_view_->load(QFileInfo(path).isFile() ? QUrl::fromLocalFile(path)
      : QUrl(path));
    layout_->setCurrentWidget(web_view_);
    web_view_->show();
    if (!overlay_widget_.isNull()) {
      overlay_widget_->raise();
    }

    return;
  }

  if (web_view_ != nullptr) {
    layout_->removeWidget(web_view_);
    delete web_view_;
    web_view_ = nullptr;
  }

  if (media_ == nullptr) {
    media_ = new MediaPluginWidget(this);
    layout_->addWidget(media_);
  }

  media_->setFile(path);
  media_->setVolume(is_primary_ ? requested_volume_ : 0);
  layout_->setCurrentWidget(media_);
  media_->show();
  if (!overlay_widget_.isNull()) {
    overlay_widget_->raise();
  }
}

void WaylandWallpaperSurface::SetVolume(qint32 volume) {
  requested_volume_ = volume;
  if (media_ != nullptr) {
    media_->setVolume(is_primary_ ? volume : 0);
  }
}

void WaylandWallpaperSurface::SetPlayerValue(const QString& key,
    const QString& value) {
  if (media_ == nullptr) {
    return;
  }

  if (key == QLatin1String("video-aspect")) {
    media_->setAspect(value.toDouble());
  } else {
    media_->setProperty(key, value);
  }
}

void WaylandWallpaperSurface::SetEventPenetration(bool enabled) {
  setAttribute(Qt::WA_TransparentForMouseEvents, enabled);
}

void WaylandWallpaperSurface::SetOverlayWidget(QWidget* widget) {
  if (widget == nullptr) {
    if (!overlay_widget_.isNull() && overlay_widget_->parentWidget() == this) {
      overlay_widget_->hide();
    }
    overlay_widget_.clear();
    return;
  }

  overlay_widget_ = widget;
  widget->setParent(this);
  QRect available_geometry =
    screen_.isNull() ? rect() : screen_->availableGeometry();
  if (!screen_.isNull()) {
    available_geometry.translate(-screen_->geometry().topLeft());
  }
  widget->setGeometry(available_geometry);
  widget->show();
  widget->raise();
}

void WaylandWallpaperSurface::Play() {
  ConfigureLayerShell();
  show();
  if (media_ != nullptr) {
    media_->show();
    media_->play();
  }
}

void WaylandWallpaperSurface::Pause() {
  ConfigureLayerShell();
  show();
  if (media_ != nullptr) {
    media_->pause();
  }
}

void WaylandWallpaperSurface::Stop() {
  if (media_ != nullptr) {
    media_->stop();
  }
}

}  // namespace LayerShell
}  // namespace Utils
