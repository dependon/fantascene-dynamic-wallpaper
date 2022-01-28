TEMPLATE = app

QT += gui x11extras widgets dbus webengine webenginewidgets
CONFIG += c++11 link_pkgconfig
PKGCONFIG += xcb-ewmh dtkwidget mpv x11
TARGET = fantascene-dynamic-wallpaper-uos

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
    webwidget.cpp \
    listview/playlistsettingdialog.cpp

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



DISTFILES += \
    com.deepin.dde.DreamScene.service

target.path=/usr/bin/

other.path=/opt/durapps/fantascene-dynamic-wallpaper-uos
other.files= $$PWD/install/*

dbus_service.files += com.deepin.dde.fantascene.service
dbus_service.path = /usr/share/dbus-1/services

APPSHAREDIR = /usr/share/fantascene-dynamic-wallpaper-uos

translations.path = $$APPSHAREDIR/translations
translations.files = $$PWD/translations/*.qm

desktop.path = /usr/share/applications/
desktop.files = $$PWD/install/fantascene-dynamic-wallpaper-uos.desktop

icon.path=/usr/share/icons
icon.files= $$PWD/install/fantascene-dynamic-wallpaper-uos.png

#info.path=/opt/apps/com.github.fantascene/
#info.files= $$PWD/info/*

INSTALLS += target  other desktop translations icon

FORMS += \
    settingwindow.ui \
    listview/historywidget.ui \
    moresetting.ui \
    listview/wallpaperengineplugin.ui \
    listview/playlistsettingdialog.ui

TRANSLATIONS += \
    translations/fantascene-dynamic-wallpaper-uos_zh_CN.ts \
    translations/fantascene-dynamic-wallpaper-uos_es.ts
