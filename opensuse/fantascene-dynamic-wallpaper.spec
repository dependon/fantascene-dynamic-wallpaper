#
# spec file for package fantascene-dynamic-wallpaper
#
# Copyright (c) 2026 liuminghang <liuminghang0821@gmail.com>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license in this file, and the notice in the file header of the
# package applies to the whole package.
#

%define _appname fantascene-dynamic-wallpaper

Name:           %{_appname}
Version:        2.1.6
Release:        0
Summary:        Dynamic wallpaper application for Linux desktops
License:        GPL-3.0-only
URL:            https://github.com/dependon/fantascene-dynamic-wallpaper
Source0:        %{_appname}-%{version}.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  make
BuildRequires:  pkgconfig
# Qt 6 modules: core/gui/widgets/dbus/concurrent/sql/network + openglwidgets
BuildRequires:  qt6-base-devel
BuildRequires:  qt6-multimedia-devel
BuildRequires:  qt6-multimediawidgets-devel
BuildRequires:  qt6-charts-devel
# openSUSE splits Qt6 WebEngine devel per module (no qt6-webengine-devel)
BuildRequires:  qt6-webenginecore-devel
BuildRequires:  qt6-webenginewidgets-devel
# lrelease6 在 qt6-tools-linguist 子包
BuildRequires:  qt6-tools
BuildRequires:  qt6-tools-linguist
BuildRequires:  qt6-macros
# PKGCONFIG deps from src/fantascene-dynamic-wallpaper.pro:
#   xcb-ewmh mpv wayland-client x11 xext xrender gio-2.0 glib-2.0 gio-unix-2.0
BuildRequires:  mpv-devel
BuildRequires:  xcb-util-wm-devel
BuildRequires:  wayland-devel
BuildRequires:  glib2-devel
BuildRequires:  mtdev-devel
BuildRequires:  libX11-devel
BuildRequires:  libXext-devel
BuildRequires:  libXrender-devel
BuildRequires:  libxcb-devel
# LayerShellQt (optional at build time, detected via /usr/include/LayerShellQt/Window)
BuildRequires:  layer-shell-qt6-devel

# Runtime deps mirroring debian/control Depends:
Requires:       wget
Requires:       ffmpeg
Requires:       aria2
# Qt SQL uses the SQLite driver for the local playlist/history database
Requires:       qt6-sql-sqlite
# libmpv runtime lib comes in via shlibs of the binary; the player itself
# and extra codecs are useful but not strictly required
Recommends:     mpv
Recommends:     gstreamer-plugins-good
Recommends:     gstreamer-plugins-libav

%description
Fantascene Dynamic Wallpaper is a Qt-based dynamic wallpaper application.
It renders videos, web pages and images as the desktop background with
multi-monitor support, playlist management and online wallpaper download,
using mpv for video playback and LayerShellQt/Wayland or X11 for
integration with the desktop.

%prep
%setup -q -n %{_appname}-%{version}

%build
# Keep env sane in OBS chroots lacking a UTF-8 locale (same as debian/rules)
export LC_ALL=C.UTF-8
mkdir -p build
cd build
qmake6 DEFINES+="VERSION=%{version}" ..
make %{?_smp_mflags}

%install
export LC_ALL=C.UTF-8
cd build
make install INSTALL_ROOT=%{buildroot}

%files
%license LICENSE LICENSE.desktop-file-memos LICENSE.mpv LICENSE.qt5platform-pluins
%doc README.md README_zh.md
%{_bindir}/%{_appname}
%{_datadir}/applications/%{_appname}.desktop
%{_datadir}/applications/%{_appname}-xcb.desktop
%{_datadir}/icons/%{_appname}.png
%{_datadir}/dbus-1/services/com.deepin.dde.fantascene.service
%dir %{_datadir}/%{_appname}
%{_datadir}/%{_appname}/translations
%{_datadir}/%{_appname}/normal
