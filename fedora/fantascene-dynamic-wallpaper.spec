#
# spec file for package fantascene-dynamic-wallpaper (Fedora)
#
# Fedora/RHEL Qt6 包名, 依赖映射见 fedora/Fedora-Build.md
# 注意: ffmpeg 在 Fedora 官方仓库不可用(RPM Fusion), 因此为 Recommends
#

Name:           fantascene-dynamic-wallpaper
Version:        2.1.6
Release:        1%{?dist}
Summary:        Dynamic wallpaper application for Linux desktops
License:        GPL-3.0-only
URL:            https://github.com/dependon/fantascene-dynamic-wallpaper
Source0:        https://github.com/dependon/fantascene-dynamic-wallpaper/archive/refs/tags/%{name}-%{version}.tar.gz

# qmake 不使用 RPM 的编译标志, 无调试信息, 生成 debugsource 包会失败
%define debug_package %{nil}

BuildRequires:  gcc-c++
BuildRequires:  make
# Qt 6 modules: core/gui/widgets/dbus/concurrent/sql/network/openglwidgets
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtmultimedia-devel
BuildRequires:  qt6-qtcharts-devel
BuildRequires:  qt6-qtwebengine-devel
# lrelease-qt6 (translation build)
BuildRequires:  qt6-qttools
# PKGCONFIG deps from src/fantascene-dynamic-wallpaper.pro:
#   xcb-ewmh mpv wayland-client x11 xext xrender gio-2.0 glib-2.0 gio-unix-2.0
BuildRequires:  mpv-devel
BuildRequires:  libxcb-devel
BuildRequires:  xcb-util-wm-devel
BuildRequires:  libX11-devel
BuildRequires:  libXext-devel
BuildRequires:  libXrender-devel
BuildRequires:  glib2-devel
BuildRequires:  wayland-devel
BuildRequires:  mtdev-devel
# LayerShellQt (optional at build time, detected via header check)
BuildRequires:  layer-shell-qt-devel

Requires:       wget
Requires:       aria2
# Qt SQL uses the SQLite driver for the local playlist/history database
# (shipped in qt6-qtbase on Fedora)
Requires:       qt6-qtbase%{?_isa}
# ffmpeg only in RPM Fusion, video conversion helper uses it if present
Recommends:     ffmpeg
Recommends:     mpv
Recommends:     gstreamer1-plugins-good

%description
Fantascene Dynamic Wallpaper is a Qt-based dynamic wallpaper application.
It renders videos, web pages and images as the desktop background with
multi-monitor support, playlist management and online wallpaper download,
using mpv for video playback and LayerShellQt/Wayland or X11 for
integration with the desktop.

%prep
%autosetup -p1 -n %{name}-%{version}

%build
export LC_ALL=C.UTF-8
mkdir -p build
pushd build
qmake6 "DEFINES+=VERSION=%{version}" ..
popd
%make_build -C build

%install
export LC_ALL=C.UTF-8
%make_install INSTALL_ROOT=%{buildroot} -C build

%files
%license LICENSE LICENSE.desktop-file-memos LICENSE.mpv LICENSE.qt5platform-pluins
%doc README.md README_zh.md
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/applications/%{name}-xcb.desktop
%{_datadir}/icons/%{name}.png
%{_datadir}/dbus-1/services/com.deepin.dde.fantascene.service
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/translations
%{_datadir}/%{name}/normal

%changelog
* Tue Oct 06 2026 dependon <523633637@qq.com> - 2.1.6-1
- Update to 2.1.6: Qt6 packaging for Fedora
