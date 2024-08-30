# Fantascene-dynamic-wallpaper(Spark-dynamic-wallpaper)

### Managed animated wallpaper based on X11 under Linux.

### Chinese official website

[https://www.fantacy.online](https://www.fantacy.online)

## Latest version download (Open Build Service)

[https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper](https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper)

AppImage: 1.8.4， Packaged in Ubuntu 16.04, compatible with most versions, but the MPV version is very old and the probability of crashing when video switching web wallpapers is high. If you can use deb, try to use the deb package as much as possible

DownloadPath: https://www.fantacy.online/fantascene-dynamic-wallpaper-1.8.4_x86_64.AppImage

## 1.8.4

Fixed the issue where some wallpapers in the warehouse were not displayed.

##Important update for version 1.8.3:

New wallpaper repository Spark Hub, available for online download (Testing).
 
### Give me a star on github page if you like this toy :)

[![biMxKK.gif](https://s4.ax1x.com/2022/02/24/biMxKK.gif)](https://imgtu.com/i/biMxKK)

[中文项目介绍](README_zh.md)

## Settings for Gnome or Kylin os or Deepin V23 

If your Gnome desktop environment has a desktop-file-utils plugin     (Kylin and Deepin V23 are the same way)

Open More Settings Button

Set Show desktop icon to false

Set Place on top of the original desktop to false.

Principle properties:

Because the desktop of Kylin OS also has transparency properties with desktop file utils after setting up transparent wallpapers, it is now possible to use the native desktop on both Kylin OS and gnome desktops, which is relatively simple

Processing of Deepin V23: The same settings as before, two no's, will be set to disable the DDE wallpaper plugin after saving, and then restart the DDE desktop to make it transparent with only icons

When closing the program (not killing), it will determine the current environment, actively restore Kylin's wallpaper and v23's wallpaper plugin, restart the DDE desktop, and other operations

DDE will involve restarting DDE

Kylin and Gnome desktop environments do not require a desktop restart

The above operations are all completed by the program (1.7.5 and above, to achieve this function)

## Update Log

[English](UpdateLog.md)

[中文日志](UpdateLog_zh.md)

## License
```
COPYRIGHT (C) 2020-2024 LIUMINGHANG <liuminghang0821@gmail.com>
```

THE PROJECT IS RELEASED UNDER THE：

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://raw.github.com/dependon/fantascene-dynamic-wallpaper/master/LICENSE)

## Wiki

Detailed instructions will be written to wiki later.


## Regarding the issue where ubuntu may display dual icons

Solution: You need to turn off desktop display in more settings (set False)

The reason for the double icon, Essentially, our software adopts the form of setting up a desktop At the bottom level, in some operating systems, icons and wallpapers are mixed together, which means that icons and wallpapers are single form and cannot be separated. However, in some Ubuntu desktops, I am not sure if it is a Mate environment or a Unity environment. The design is similar to Windows, where wallpapers and icons are separated. That is, our software serves as wallpapers, while the icon layer is still on top of us, and our software implements simple desktop functions (This is also one of the reasons why some people believe that dynamic wallpapers like Windows have not been implemented, and one of the reasons why they have been criticized.) We need to choose whether to display desktop in more settings and click "no".)

## Principle:

Set MPV windows and webview managed windows to desktop properties

The wallpaper and icons are separated, which means that we do not need the desktop provided by our software. If it is a kneaded whole, it is necessary, otherwise it cannot be implemented (so if it is a native desktop, it cannot be implemented unless the official interface is provided)

## Experience

The inspiration for the software comes from the deepin desktop dde-desktop. After modifying the source code, we can use dbus to make the wallpaper transparent, achieving our first version of dynamic wallpaper. Later, due to the need to frequently recompile dde-desktop, our mindset was quite explosive, so we chose not to maintain it as the main task in the future. Interested parties can compile the code and deep branch it themselves. We need to install dependencies on the dde-file-manager and deepin-moview to compile and package it for use

Later, I wrote a simple desktop for the general version

## qq群(及时反馈和分享bug需求等) 2023/08/29

群号:917552088
 请申请的时候备注下加群的信息(比如:使用动态壁纸等，防止有小广告选手混入其中)

## 特殊版本deepinv23(2024/4/11)：

## 特殊版本deepinv20(2024/4/11)：

## 特殊版本uos家庭版(2024/4/11)：

都在星火商店下载最新的


## Installation

### AppImage 2023-08-02

dowload path: [https://www.appimagehub.com/p/2068198](https://www.appimagehub.com/p/2068198)


### Fedora and OpenSUSE
[![build result](https://build.opensuse.org/projects/home:liuminghang/packages/fantascene-dynamic-wallpaper/badge.svg?type=default)](https://build.opensuse.org/package/show/home:liuminghang/fantascene-dynamic-wallpaper)

[![build result](https://build.opensuse.org/projects/home:liuminghang/packages/fantascene-fedora/badge.svg?type=default)](https://build.opensuse.org/package/show/home:liuminghang/fantascene-fedora)

Please see: [fantascene-dynamic-wallpaper](https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper)

### archlinux and manjaro

yay -S fantascene-dynamic-wallpaper-git

### windows 

Please see: [https://github.com/dependon/mpv-dynamic-wallpaper](https://github.com/dependon/mpv-dynamic-wallpaper)

## Build Dependencies

### debian/Deepin/Other

```
qt >=5.8

mpv >=29.0

sudo apt install qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev libqt5opengl5-dev qttools5-dev-tools qtbase5-dev qt5-qmake qtwebengine5-dev libxext-dev libxrender-dev libqt5x11extras5-dev libglib2.0-dev qt5-default build-essential g++ libxcb-shape0-dev build-essential

```

## Build from SCM

debian and ubuntu and deepin
```
dpkg-buildpackage -b -us -uc

```

### Other Linux variants
```
git clone https://github.com/dependon/fantascene-dynamic-wallpaper

cd fantascene-dynamic-wallpaper

mkdir build

cd build

qmake ..

make all # For debug and release
```


## Thanks

Thank [Akiba illusion](https://github.com/AkibaIllusionLinux) its great support.

Thank [taotieren](https://github.com/taotieren) for his help on arch.

Thank [gesangtome](https://github.com/gesangtome) for fixing some project bugs.

Thanks for the Polish translation provided by  [raspin0](https://github.com/raspin0).

Thanks for the Spanish translation provided by [Alvaro samudio](https://github.com/alvarosamudio).

Thanks for the pkg2mp4.md provided by [M0rtzz](https://github.com/M0rtzz).

## Contribute
```
Contributions are always welcome, Please create a new PR or issue if necessary.
```

## Extremely simple example

[https://github.com/dependon/simple-wallpaper](https://github.com/dependon/simple-wallpaper)

## ScreenShot

#### manjaro xfce4
<img src="https://s4.ax1x.com/2022/02/24/biMa9I.gif" alt="show" />

#### manjaro kde
<img src="https://s4.ax1x.com/2022/02/24/biMN4A.gif" alt="show" />

#### cutefish
<img src="https://s4.ax1x.com/2022/02/24/biKHpt.gif" alt="show" />

#### ubuntu
<img src="https://s4.ax1x.com/2022/02/24/biuJKO.gif" alt="show" />
