TEMPLATE = app

QT += gui x11extras widgets dbus webengine webenginewidgets
CONFIG += c++11 link_pkgconfig
PKGCONFIG += xcb-ewmh dtkwidget mpv x11
TARGET = fantascene-dynamic-wallpaper

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    wallpaper.cpp \
    dbuswallpaperservice.cpp \
    mpvwidget.cpp \
    settings.cpp \
    settingwindow.cpp \
    application.cpp \
    listview/delegate.cpp \
    listview/view.cpp \
    listview/historywidget.cpp \
    moresetting.cpp \
    listview/wallpaperengineplugin.cpp \
    webwidget.cpp

HEADERS += \
    wallpaper.h \
    dbuswallpaperservice.h \
    mpvwidget.h \
    settings.h \
    settingwindow.h \
    application.h \
    data.h \
    listview/delegate.h \
    listview/view.h \
    listview/historywidget.h \
    setdesktop.h \
    moresetting.h \
    listview/wallpaperengineplugin.h \
    webwidget.h

RESOURCES += \
    qrc.qrc

CONFIG(release, debug|release) {
    TRANSLATIONS = $$files($$PWD/translations/*.ts)
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    for(tsfile, TRANSLATIONS) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
}

OTHER_FILES+=$$PWD/install/*
target.path=/opt/durapps/fantascene-dynamic-wallpaper

other.path=/opt/durapps/fantascene-dynamic-wallpaper
other.files= $$PWD/install/*

desktop.path = /usr/share/applications/
desktop.files = $$PWD/install/fantascene-dynamic-wallpaper.desktop

desktopleft.path = /usr/share/deepin/dde-file-manager/oem-menuextensions/
desktopleft.files = $$PWD/install/fantascene-dynamic-wallpaper.desktop

DISTFILES += \
    com.deepin.dde.DreamScene.service

dbus_service.files += com.deepin.dde.fantascene.service
dbus_service.path = /usr/share/dbus-1/services

APPSHAREDIR = /usr/share/fantascene-dynamic-wallpaper

translations.path = $$APPSHAREDIR/translations
translations.files = $$PWD/translations/*.qm

INSTALLS += target  other desktop desktopleft translations

FORMS += \
    settingwindow.ui \
    listview/historywidget.ui \
    moresetting.ui \
    listview/wallpaperengineplugin.ui

TRANSLATIONS += \
    translations/fantascene-dynamic-wallpaper_zh_CN.ts \
    translations/fantascene-dynamic-wallpaper_es.ts
