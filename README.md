# Fantascene-dynamic-wallpaper

### Managed animated wallpaper based on X11 under Linux.

### Official website

[https://www.fantacy.online](https://www.fantacy.online)

## Other Readme

[English](README.md) [中文项目介绍](README_zh.md)

## Update Log

[English](md/UpdateLog.md) [中文日志](md/UpdateLog_zh.md)

## Supporting languages (Current Using machine translation)

#### if possible, you can submit updates for '.ts' file to help us

English 
中文 
繁體中文(中国台湾)
繁體中文(中国香港) 
Español Polski 
日本語 
Deutsch 
한국어 
Italiano 
Français 
Русский 
Português 
Português (Brasil) 
Suomi 
Tiếng Việt
Türkçe
ภาษาไทย
Magyar
Svenska
Slovenčina
Română
Bahasa Melayu
Nederlands
Eesti keel
Български
Čeština
Dansk sprog
Ελληνική γλώσσα

## wiki

https://github.com/dependon/fantascene-dynamic-wallpaper/wiki

## Latest version download (Open Build Service)

[https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper](https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper)

AppImage: 1.9.8， Packaged in Ubuntu 16.04, compatible with most versions, but the MPV version is very old and the probability of crashing when video switching web wallpapers is high. If you can use deb, try to use the deb package as much as possible

DownloadPath: https://sourceforge.net/projects/fantascene-dynamic-wallpaper/files/1.9.8/

 
### Give me a star on github page if you like this toy :)

[![biMxKK.gif](https://s4.ax1x.com/2022/02/24/biMxKK.gif)](https://imgtu.com/i/biMxKK)


## License
```
COPYRIGHT (C) 2020-2024 LIUMINGHANG <liuminghang0821@gmail.com>
```

THE PROJECT IS RELEASED UNDER THE：

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://raw.github.com/dependon/fantascene-dynamic-wallpaper/master/LICENSE)

## Regarding the issue where ubuntu may display dual icons

Solution: You need to turn off desktop display in more settings (set False)

The reason for the double icon, Essentially, our software adopts the form of setting up a desktop At the bottom level, in some operating systems, icons and wallpapers are mixed together, which means that icons and wallpapers are single form and cannot be separated. However, in some Ubuntu desktops, I am not sure if it is a Mate environment or a Unity environment. The design is similar to Windows, where wallpapers and icons are separated. That is, our software serves as wallpapers, while the icon layer is still on top of us, and our software implements simple desktop functions (This is also one of the reasons why some people believe that dynamic wallpapers like Windows have not been implemented, and one of the reasons why they have been criticized.) We need to choose whether to display desktop in more settings and click "no".)

## Principle:

Set MPV windows and webview managed windows to desktop properties

The wallpaper and icons are separated, which means that we do not need the desktop provided by our software. If it is a kneaded whole, it is necessary, otherwise it cannot be implemented (so if it is a native desktop, it cannot be implemented unless the official interface is provided)

## Experience

The inspiration for the software comes from the deepin desktop dde-desktop. After modifying the source code, we can use dbus to make the wallpaper transparent, achieving our first version of dynamic wallpaper. Later, due to the need to frequently recompile dde-desktop, our mindset was quite explosive, so we chose not to maintain it as the main task in the future. Interested parties can compile the code and deep branch it themselves. We need to install dependencies on the dde-file-manager and deepin-moview to compile and package it for use

Later, I wrote a simple desktop for the general version

## Installation

### Fedora and OpenSUSE

Please see: [fantascene-dynamic-wallpaper](https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper)

### archlinux and manjaro

yay -S fantascene-dynamic-wallpaper-git

yay -S fantascene-dynamic-wallpaper

### windows demo

Please see: [https://sourceforge.net/projects/fantascene-dynamic-wallpaper/files/1.9.1/](https://sourceforge.net/projects/fantascene-dynamic-wallpaper/files/1.9.1/)

## Build Dependencies

### debian/Deepin/Other

```
qt >=5.8

mpv >=29.0

git clone https://github.com/dependon/fantascene-dynamic-wallpaper.git
sudo apt-get install -y qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev qttools5-dev-tools qtbase5-dev qt5-qmake libxext-dev libxrender-dev libqt5x11extras5-dev libglib2.0-dev build-essential g++ libxcb-shape0-dev aria2
sudo apt-get install -y qtwebengine5-dev
sudo apt-get install -y libqt5opengl5-dev
sudo apt-get install -y qt5-default
sudo apt-get install -y qtdeclarative5-dev-tools
sudo apt-get install -y devscripts equivs
sudo apt-get install -y qttools5-dev-tools qttools5-dev

cd fantascene-dynamic-wallpaper

mkdir build

cd build

qmake ..

make -j4
```

## Build from SCM

debian and ubuntu and deepin
```
dpkg-buildpackage -b -us -uc

```

## Thanks

Thanks [shenmo](https://gitee.com/spark-store-project) its great support.

Thanks [taotieren](https://github.com/taotieren) for his help on arch.

Thanks [gesangtome](https://github.com/gesangtome) for fixing some project bugs.

Thanks [Akiba illusion](https://github.com/AkibaIllusionLinux) its great support.

Thanks for the Polish translation provided by  [raspin0](https://github.com/raspin0).

Thanks for the Spanish translation provided by [Alvaro samudio](https://github.com/alvarosamudio).

Thanks for the pkg2mp4.md provided by [M0rtzz](https://github.com/M0rtzz).

Thanks [lisuke](https://github.com/lisuke) support fix bug

## Contribute
```
Contributions are always welcome, Please create a new PR or issue if necessary.
```

## Extremely simple example

[https://github.com/dependon/simple-wallpaper](https://github.com/dependon/simple-wallpaper)

## ScreenShot show

#### manjaro xfce4
<img src="https://s4.ax1x.com/2022/02/24/biMa9I.gif" alt="show" />

#### manjaro kde
<img src="https://s4.ax1x.com/2022/02/24/biMN4A.gif" alt="show" />

#### cutefish
<img src="https://s4.ax1x.com/2022/02/24/biKHpt.gif" alt="show" />

#### ubuntu
<img src="https://s4.ax1x.com/2022/02/24/biuJKO.gif" alt="show" />
