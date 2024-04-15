TEMPLATE = app

QT += gui x11extras widgets dbus webengine webenginewidgets
QT += concurrent
QT += sql
CONFIG += c++11 link_pkgconfig
PKGCONFIG += xcb-ewmh dtkwidget mpv x11 xext
TARGET = fantascene-dynamic-wallpaper

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += MY_V23SUPER
SOURCES += main.cpp \
    db/dbmanager.cpp \
    download/customwebengineview.cpp \
    download/downloadwidget.cpp \
    help/helpdialog.cpp \
    ini/inimanager.cpp \
    listview/localwidget.cpp \
    setdesktop.cpp \
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
    webwidget.cpp \
    listview/playlistsettingdialog.cpp

HEADERS += \
    db/dbmanager.h \
    download/customwebengineview.h \
    download/downloadwidget.h \
    help/helpdialog.h \
    ini/inimanager.h \
    listview/localwidget.h \
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
    webwidget.h \
    listview/playlistsettingdialog.h

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

INCLUDEPATH += $$PWD/ini
INCLUDEPATH += $$PWD/download

DISTFILES += \
    com.deepin.dde.DreamScene.service

target.path=/usr/bin/

other.path=/opt/durapps/fantascene-dynamic-wallpaper
other.files= $$PWD/install/*

dbus_service.files += com.deepin.dde.fantascene.service
dbus_service.path = /usr/share/dbus-1/services

APPSHAREDIR = /usr/share/fantascene-dynamic-wallpaper

translations.path = $$APPSHAREDIR/translations
translations.files = $$PWD/translations/*.qm

desktop.path = /usr/share/applications/
desktop.files = $$PWD/install/fantascene-dynamic-wallpaper.desktop

icon.path=/usr/share/icons
icon.files= $$PWD/install/fantascene-dynamic-wallpaper.png

#info.path=/opt/apps/com.github.fantascene/
#info.files= $$PWD/info/*

INSTALLS += target  other desktop translations icon

FORMS += \
    download/downloadwidget.ui \
    help/helpdialog.ui \
    listview/localwidget.ui \
    settingwindow.ui \
    listview/historywidget.ui \
    moresetting.ui \
    listview/wallpaperengineplugin.ui \
    listview/playlistsettingdialog.ui

TRANSLATIONS += \
    translations/fantascene-dynamic-wallpaper_zh_CN.ts \
    translations/fantascene-dynamic-wallpaper_es.ts
