TEMPLATE  = subdirs

DEFINES += MY_V23SUPER


# 如果 MY_MACRO 存在，则进行一些特定的配置
contains(DEFINES, MY_V23SUPER) {
    message("MY_V23SUPER is defined")
} else {
     SUBDIRS  += \
     dde-desktop/filemanager.pro \
}
SUBDIRS  += \
    fantascene-dynamic-wallpaper/fantascene-dynamic-wallpaper.pro \
