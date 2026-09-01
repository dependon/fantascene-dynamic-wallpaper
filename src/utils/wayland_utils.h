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

#ifndef SRC_UTILS_WAYLAND_UTILS_H_
#define SRC_UTILS_WAYLAND_UTILS_H_

#include <QPointer>
#include <QWidget>

class MediaPluginWidget;
class QScreen;
class QStackedLayout;
class webWidget;

namespace Utils {
namespace LayerShell {

void DetectLayerShellSupport();
bool IsLayerShellAvailable();

class WaylandWallpaperSurface final : public QWidget {
 public:
  explicit WaylandWallpaperSurface(QScreen* screen);
  ~WaylandWallpaperSurface() override;

  QScreen* screen() const;
  void SetPrimary(bool is_primary);
  bool IsPrimary() const;

  void SetFile(const QString& path);
  void SetVolume(qint32 volume);
  void SetPlayerValue(const QString& key, const QString& value);
  void SetEventPenetration(bool enabled);
  void SetOverlayWidget(QWidget* widget);
  void Play();
  void Pause();
  void Stop();

 private:
  void ConfigureLayerShell();

  QPointer<QScreen> screen_;
  QStackedLayout* layout_ = nullptr;
  MediaPluginWidget* media_ = nullptr;
  webWidget* web_view_ = nullptr;
  QPointer<QWidget> overlay_widget_;
  bool is_primary_ = false;
  qint32 requested_volume_ = 0;
};

}  // namespace LayerShell
}  // namespace Utils

#endif  // SRC_UTILS_WAYLAND_UTILS_H_
