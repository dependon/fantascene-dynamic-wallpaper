# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Fantascene Dynamic Wallpaper is a Linux X11-based dynamic wallpaper application written in C++ using Qt 5/6. It renders videos, images, and web content as desktop backgrounds with support for multi-monitor configurations and custom desktop environments.

## Build Commands

### Local Build (development)
```bash
sh start_makeLocal.sh
```
This creates a `build/` directory, runs qmake, compiles with 4 jobs, and installs to system.

### Debian Package Build
```bash
sh start_deb.sh
```
Builds a `.deb` package using `dpkg-buildpackage`. Installs dependencies first including `mk-build-deps`.

### Manual Build
```bash
mkdir build && cd build
qmake ..
make -j4
sudo make install
```

### Dependencies
- Qt >= 5.8 (Qt 6 supported)
- MPV >= 29.0
- X11 libraries: `libx11-xcb-dev`, `libxcb-ewmh-dev`, `libxext-dev`, `libxrender-dev`
- Qt modules: `libqt5x11extras5-dev`, `qtwebengine5-dev` (or webkit), `qtmultimedia5-dev`, `libqt5charts5-dev`
- Additional: `libglib2.0-dev`, `build-essential`, `g++`

### Translation Build
The .pro file handles `.ts` to `.qm` conversion using `lrelease`. It first checks system PATH, then `/usr/lib/qt6/bin/lrelease` for Qt 6 compatibility.

## Architecture

### Entry Point
`main.cpp` initializes the application with:
- X11 platform enforcement (`QT_QPA_PLATFORM=xcb`)
- Custom logging to `~/.cache/fantascene/log/`
- Single-instance lock using `QLockFile`
- D-Bus service registration
- Main window (1024x700 fixed size)

### Core Classes

**Application** (`application.h/cpp`) - Global singleton managing:
- Wallpaper state across screens
- Thumbnail caching (`~/.config/fantascene-dynamic-wallpaper/.thumbnail`)
- Translation loading
- Multi-screen mode tracking
- Configuration persistence
- Playlist management

**Wallpaper** (`wallpaper.h/cpp`) - The desktop background widget:
- Renders media via `MediaPluginWidget`
- Handles screen mode: `IdCopyScreen` (clone), `IdLayoutScreen` (layout), `IdManualScreen` (manual)
- Integrates with X11 window management through `SetDesktop`

**SetDesktop** (`setdesktop.h/cpp`) - X11 desktop integration:
- Registers windows as desktop properties
- Manages multiple screen windows
- Handles desktop transparency

**MediaPluginWidget** - Media backend abstraction:
- Wraps `MPVWidget` (primary video backend)
- Falls back to Qt Multimedia when MPV unavailable

### Directory Structure

- `src/media/` - Media playback components (MPV, Qt Multimedia)
- `src/desktop/` - Custom desktop implementation with icon view
- `src/download/` - Wallpaper marketplace and download manager
- `src/listview/` - Playlist, local/online wallpaper list widgets
- `src/othertools/` - System monitoring (CPU, memory, network), time display
- `src/help/` - Help dialogs
- `src/ini/` - INI configuration management
- `src/db/` - SQLite database for history/playlists
- `src/translations/` - 30+ language `.ts` files
- `debian/` - Debian packaging configuration

### Multi-Screen Support

The application supports three modes configured via the settings UI:
1. **Copy Mode**: Same wallpaper on all screens
2. **Layout Mode**: Wallpaper stretched across screens
3. **Manual Mode**: Different wallpapers per screen

Screen-specific windows are tracked in `Application::m_screenWid` with wallpaper paths in `m_currentPath` (primary) and `m_currentPath2` (secondary).

### D-Bus Integration

Service: `com.deepin.dde.fantascene` defined in `dbuswallpaperservice.h/cpp`

Key signals for wallpaper control: `setWallPaper()`, `setMpvPlay()`, `setMpvstop()`, `setMpvVolume()`

### Desktop Icon Handling

The app includes a custom desktop (`desktop/` directory) for environments where wallpaper and icons cannot be separated (e.g., some Ubuntu configurations). Users can disable the built-in desktop in "More Settings" if their environment separates wallpaper from icons natively.

## Development Notes

- Qt version detection via `QT_MAJOR_VERSION` - conditional code for Qt 6 differences
- Platform-specific code uses `#ifdef Q_OS_LINUX` and `#ifdef Q_OS_WIN`
- X11 direct access through `xcb_connection_t*` via `Application::getXcb_connection_t()`
- Translation path defaults to `/usr/share/fantascene-dynamic-wallpaper/translations`
- Logs are rotated automatically (files >10 days old deleted)

## Desktop Environment Compatibility

Special handling detected in `Application::CheckSystem()`:
- DDE 23
- GXDE
- UKUI
- LingMo

Desktop-specific workarounds are applied based on these detections.