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
    application.h \
    data.h

RESOURCES += \
    qrc.qrc

OTHER_FILES+=$$PWD/install/*
target.path=/opt/durapps/deepin-dreamscene-ui

other.path=/opt/durapps/deepin-dreamscene-ui
other.files= $$PWD/install/*

desktop.path = /usr/share/applications/
desktop.files = $$PWD/install/deepin-dreamscene-ui.desktop

DISTFILES += \
    com.deepin.dde.DreamScene.service

dbus_service.files += com.deepin.dde.DreamScene.service
dbus_service.path = /usr/share/dbus-1/services

INSTALLS += target dbus_service other desktop

FORMS += \
    settingwindow.ui
