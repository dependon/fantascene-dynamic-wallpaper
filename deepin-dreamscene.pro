TEMPLATE = app

QT += gui x11extras widgets dbus
CONFIG += c++11 link_pkgconfig
PKGCONFIG += xcb-ewmh dtkwidget mpv
TARGET = deepin-dreamscene

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

target.path=/opt/deepin-dreamscene


DISTFILES += \
    com.deepin.dde.DreamScene.service

dbus_service.files += com.deepin.dde.DreamScene.service
dbus_service.path = /usr/share/dbus-1/services

INSTALLS += target dbus_service

FORMS += \
    settingwindow.ui
