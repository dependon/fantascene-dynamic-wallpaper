TEMPLATE = app

QT += gui core x11extras widgets dbus webengine webenginewidgets
CONFIG += c++11 link_pkgconfig no_keywords
PKGCONFIG += xcb-ewmh mpv x11 xext xrender
TARGET = fantascene-dynamic-wallpaper

PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0
LIBS +=-lgio-2.0 -lglib-2.0

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    wallpaper.cpp \
    dbuswallpaperservice.cpp \
    mpvwidget.cpp \
    settingwindow.cpp \
    application.cpp \
    listview/delegate.cpp \
    listview/view.cpp \
    listview/historywidget.cpp \
    moresetting.cpp \
    listview/wallpaperengineplugin.cpp \
    webwidget.cpp \
    listview/playlistsettingdialog.cpp \
    desktop.cpp \
    desktop/filemodel.cpp \
    desktop/fileoperationjob.cpp \
    desktop/iconprovider.cpp \
    desktop/iconview.cpp \
    desktop/delegateicon.cpp

HEADERS += \
    wallpaper.h \
    dbuswallpaperservice.h \
    mpvwidget.h \
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
    listview/playlistsettingdialog.h \
    desktop.h \
    desktop/filemodel.h \
    desktop/fileoperationjob.h \
    desktop/iconprovider.h \
    desktop/iconview.h \
    desktop/delegateicon.h

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
#fantacy
OTHER_FILES+=$$PWD/install/*

target.path=/opt/apps/com.github.fantacy/files/bin/

icon.path=/opt/apps/com.github.fantacy/entries/icons/
icon.files= $$PWD/install/fantacy.svg

desktop1.path=/opt/apps/com.github.fantacy/entries/applications
desktop1.files=$$PWD/install/com.github.fantacy.desktop

other.path=/opt/apps/com.github.fantacy/files/bin/
other.files= $$PWD/install/*

info.path=/opt/apps/com.github.fantacy/
info.files= $$PWD/info/*

translations.path = /opt/apps/com.github.fantacy/files/bin/translations
translations.files = $$PWD/translations/*.qm


INSTALLS += target  desktop1  translations info other icon

FORMS += \
    settingwindow.ui \
    listview/historywidget.ui \
    moresetting.ui \
    listview/wallpaperengineplugin.ui \
    listview/playlistsettingdialog.ui \
    desktop.ui

TRANSLATIONS += \
    translations/fantascene-dynamic-wallpaper_zh_CN.ts \
    translations/fantascene-dynamic-wallpaper_es.ts
