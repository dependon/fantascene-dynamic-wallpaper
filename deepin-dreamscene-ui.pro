TEMPLATE = app

QT += gui x11extras widgets dbus
CONFIG += c++11 link_pkgconfig
PKGCONFIG += xcb-ewmh dtkwidget mpv
TARGET = deepin-dreamscene-ui

SOURCES += main.cpp \
    wallpaper.cpp \
    dbuswallpaperservice.cpp \
    mpvwidget.cpp \
    settings.cpp \
    settingwindow.cpp \
    application.cpp

HEADERS += \
    wallpaper.h \
    dbuswallpaperservice.h \
    mpvwidget.h \
    settings.h \
    settingwindow.h \
    application.h

RESOURCES += \
    qrc.qrc


target.path=/opt/deepin-dreamscene-ui

desktop.files=deepin-dreamscene-ui.desktop
desktop.path=/opt/deepin-dreamscene-ui

dde-desktop.files=dde-desktop
dde-desktop.path=/opt/deepin-dreamscene-ui

config.files=config.ini
config.path=/opt/deepin-dreamscene-ui

icon.files=tray.ico
icon.path=/opt/deepin-dreamscene-ui
DISTFILES += \
    com.deepin.dde.DreamScene.service

dbus_service.files += com.deepin.dde.DreamScene.service
dbus_service.path = /usr/share/dbus-1/services

INSTALLS += target dbus_service desktop dde-desktop config icon

FORMS += \
    settingwindow.ui
