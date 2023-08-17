TEMPLATE = app

TARGET = fantascene-dynamic-wallpaper
# Build binary with debuginfo and REL by default.
CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    DESTDIR = debug
}
CONFIG(release, debug|release) {
    DESTDIR = release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.u

QT += gui core x11extras widgets dbus
qtHaveModule(webengine){
   QT += webengine webenginewidgets
   DEFINES += USE_WEBENGINE
}
QT += concurrent

CONFIG += c++11 link_pkgconfig no_keywords
PKGCONFIG += xcb-ewmh mpv x11 xext xrender
#CONFIG += waylandcompositor
PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0
LIBS +=-lgio-2.0 -lglib-2.0

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    desktop/customsortfilterproxymodel.cpp \
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
    desktop/filemodel.cpp \
    desktop/fileoperationjob.cpp \
    desktop/iconprovider.cpp \
    desktop/iconview.cpp \
    desktop/delegateicon.cpp \
    loadTranslation.cpp \
    desktop/gioclass.cpp \
    ini/inimanager.cpp \
    listview/localwidget.cpp

HEADERS += \
    desktop/customsortfilterproxymodel.h \
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
    desktop/filemodel.h \
    desktop/fileoperationjob.h \
    desktop/iconprovider.h \
    desktop/iconview.h \
    desktop/delegateicon.h \
    loadTranslation.h \
    desktop/gioclass.h \
    ini/inimanager.h \
    listview/localwidget.h

RESOURCES += \
    qrc.qrc

# For DEBUG
CONFIG(debug, debug|release) {
# Add dubug support for GDB
    QMAKE_CXXFLAG += -O0 -g
}
CONFIG(release, debug|release) {
    QMAKE_CXXFLAG += -O2 -fdata-sections -ffunction-sections
}

INCLUDEPATH += $$PWD/ini

# No matter the build type is release or debug, we always need to generate the qm file.
TRANSLATIONS = $$files($$PWD/translations/*.ts)
for(tsfile, TRANSLATIONS) {
    qmfile = $$replace(tsfile, .ts$, .qm)
    system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
}

isEmpty(PREFIX){
    PREFIX = /usr
}

OTHER_FILES+=$$PWD/install/*

unix:!android: target.path = $${PREFIX}/bin

icon.path=$${PREFIX}/share/icons
icon.files= $$PWD/install/fantascene-dynamic-wallpaper.png

desktop.path = $${PREFIX}/share/applications/
desktop.files = $$PWD/install/fantascene-dynamic-wallpaper.desktop

#desktopleft.path = $${PREFIX}/share/deepin/dde-file-manager/oem-menuextensions/
#desktopleft.files = $$PWD/install/fantascene-dynamic-wallpaper.desktop

DISTFILES += \
    com.deepin.dde.DreamScene.service

dbus_service.files += com.deepin.dde.fantascene.service
dbus_service.path = $${PREFIX}/share/dbus-1/services

APPSHAREDIR = $${PREFIX}/share/fantascene-dynamic-wallpaper

translations.path = $$APPSHAREDIR/translations
translations.files = $$PWD/translations/*.qm

video.path=$${PREFIX}/share/fantascene-dynamic-wallpaper/normal/
video.files=$$PWD/install/normal.mp4

INSTALLS += target  icon desktop  translations video

FORMS += \
    settingwindow.ui \
    listview/historywidget.ui \
    moresetting.ui \
    listview/wallpaperengineplugin.ui \
    listview/playlistsettingdialog.ui \
    desktop.ui \
    listview/localwidget.ui

TRANSLATIONS += \
    translations/fantascene-dynamic-wallpaper_zh_CN.ts \
    translations/fantascene-dynamic-wallpaper_es.ts
