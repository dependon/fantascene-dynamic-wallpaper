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

QT += gui core widgets dbus

qtHaveModule(webengine){
   QT += webengine webenginewidgets
   DEFINES += USE_WEBENGINE
}

!qtHaveModule(webengine) {
    qtHaveModule(webkit) {
        QT += webkit webkitwidgets
        DEFINES += USE_WEBKIT
    }
}

contains(QT_MAJOR_VERSION, 6) {
    QT += openglwidgets
}

QT += concurrent sql network

CONFIG += c++11 link_pkgconfig no_keywords
unix {
 #QT += x11extras
 PKGCONFIG += xcb-ewmh mpv x11 xext xrender
#CONFIG += waylandcompositor
 PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0
 LIBS +=-lgio-2.0 -lglib-2.0

}

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    desktop/customsortfilterproxymodel.cpp \
    download/tcpclient.cpp \
    listview/onlineclient.cpp \
    wallpaper.cpp \
    dbuswallpaperservice.cpp \
    mpvwidget.cpp \
    settingwindow.cpp \
    application.cpp \
    listview/delegate.cpp \
    listview/view.cpp \
    listview/historywidget.cpp \
    setdesktop.cpp \
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
    listview/localwidget.cpp \
    help/helpdialog.cpp \
    download/downloadwidget.cpp \
    download/customwebengineview.cpp \
    db/dbmanager.cpp

HEADERS += \
    desktop/customsortfilterproxymodel.h \
    download/tcpclient.h \
    listview/onlineclient.h \
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
    listview/localwidget.h \
    help/helpdialog.h \
    download/downloadwidget.h \
    download/customwebengineview.h \
    db/dbmanager.h

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
INCLUDEPATH += $$PWD/download
unix {
# No matter the build type is release or debug, we always need to generate the qm file.
TRANSLATIONS = $$files($$PWD/translations/*.ts)
for(tsfile, TRANSLATIONS) {
    qmfile = $$replace(tsfile, .ts$, .qm)
    system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
}
}

DISTFILES += \
    com.deepin.dde.DreamScene.service


OTHER_FILES+=$$PWD/install/*

isEmpty(PREFIX){
    PREFIX = /usr
}
APPSHAREDIR = $${PREFIX}/share/fantascene-dynamic-wallpaper

#DEFINES+= DEEPIN_STORE
#DEEPIN STORE
contains(DEFINES,DEEPIN_STORE){
target.path=/opt/apps/com.github.fantacy/files/bin/

icon.path=$${PREFIX}/share/icons
icon.files= $$PWD/install/fantacy.svg

desktop1.path=/opt/apps/com.github.fantacy/entries/applications
desktop1.files=$$PWD/install/com.github.fantacy.desktop

#other.path=/opt/apps/com.github.fantacy/files/bin/
#other.files= $$PWD/install/*

info.path=/opt/apps/com.github.fantacy/
info.files= $$PWD/info/*

translations.path = /opt/apps/com.github.fantacy/files/bin/translations
translations.files = $$PWD/translations/*.qm

video.path=$${PREFIX}/share/fantascene-dynamic-wallpaper/normal/deepin/
video.files=$$PWD/install/normal.mp4

INSTALLS += target  desktop1  translations info other icon video

}else{

target.path = $${PREFIX}/bin

icon.path=$${PREFIX}/share/icons
icon.files= $$PWD/install/fantascene-dynamic-wallpaper.png

desktop.path = $${PREFIX}/share/applications/
desktop.files = $$PWD/install/fantascene-dynamic-wallpaper.desktop

#desktopleft.path = $${PREFIX}/share/deepin/dde-file-manager/oem-menuextensions/
#desktopleft.files = $$PWD/install/fantascene-dynamic-wallpaper.desktop

dbus_service.files += com.deepin.dde.fantascene.service
dbus_service.path = $${PREFIX}/share/dbus-1/services

translations.path = $$APPSHAREDIR/translations
translations.files = $$PWD/translations/*.qm

video.path=$${PREFIX}/share/fantascene-dynamic-wallpaper/normal/
video.files=$$PWD/install/normal.mp4

touming.path=$${PREFIX}/share/fantascene-dynamic-wallpaper/normal/
touming.files=$$PWD/install/touming.png

INSTALLS += target  icon desktop  translations video touming
}




FORMS += \
    listview/onlineclient.ui \
    settingwindow.ui \
    listview/historywidget.ui \
    moresetting.ui \
    listview/wallpaperengineplugin.ui \
    listview/playlistsettingdialog.ui \
    desktop.ui \
    listview/localwidget.ui \
    help/helpdialog.ui \
    download/downloadwidget.ui

TRANSLATIONS += \
    translations/fantascene-dynamic-wallpaper_zh_CN.ts \ #中文
    translations/fantascene-dynamic-wallpaper_zh_TW.ts \ #中文繁体
    translations/fantascene-dynamic-wallpaper_es.ts \ #西班牙语
    translations/fantascene-dynamic-wallpaper_pl.ts \ #波兰语
    translations/fantascene-dynamic-wallpaper_ja.ts \ #日语
    translations/fantascene-dynamic-wallpaper_de.ts \ #德语
    translations/fantascene-dynamic-wallpaper_ko.ts \ #韩语
    translations/fantascene-dynamic-wallpaper_it.ts \ #意大利语
    translations/fantascene-dynamic-wallpaper_fr.ts \ #法语
    translations/fantascene-dynamic-wallpaper_ru.ts \ #俄语
    translations/fantascene-dynamic-wallpaper_pt.ts \ #葡萄牙语
    translations/fantascene-dynamic-wallpaper_pt_BR.ts \  #葡萄牙-巴西
    translations/fantascene-dynamic-wallpaper_fi.ts \ #芬兰语
    translations/fantascene-dynamic-wallpaper_vi.ts \ #越南
    translations/fantascene-dynamic-wallpaper_tr.ts \ #土耳其
    translations/fantascene-dynamic-wallpaper_th.ts \ #泰国
    translations/fantascene-dynamic-wallpaper_hu.ts \ #匈牙利
    translations/fantascene-dynamic-wallpaper_sv.ts \ #瑞典
    translations/fantascene-dynamic-wallpaper_sk.ts \ #斯洛伐克
    translations/fantascene-dynamic-wallpaper_ro.ts \ #罗马尼亚
    translations/fantascene-dynamic-wallpaper_ms.ts  #马来

