# Fantascene-dynamic-wallpaper

### Managed animated wallpaper based on X11 under Linux.

### Give me a star on github page if you like this toy :)

[![biMxKK.gif](https://s4.ax1x.com/2022/02/24/biMxKK.gif)](https://imgtu.com/i/biMxKK)

## License
```
COPYRIGHT (C) 2020-2023 LIUMINGHANG <liuminghang0821@gmail.com>
```

THE PROJECT IS RELEASED UNDER THE：

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://raw.github.com/dependon/fantascene-dynamic-wallpaper/master/LICENSE)

## Latest version download (Open Build Service)

[https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper](https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper)

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

https://depend.lanzouw.com/ioqv21ugmhyb
密码:fmzn

## 特殊版本deepinv20(2024/4/11)：

https://depend.lanzouw.com/izqx21uksx2f
密码:1b9q

## 特殊版本uos家庭版(2024/4/11)：

https://depend.lanzouw.com/iwenK1uksy0j
密码:bwhz

## 1.5.0 update log

兼容wayland环境
Compatible with wayland environment

debian10:
x86:fantascene-dynamic-wallpaper_1.5.0-1_debian10_amd64.deb
arm:fantascene-dynamic-wallpaper_1.5.0-0_debian10_arm64.deb

debian testing(debian 12):
fantascene-dynamic-wallpaper_1.5.0-1_debianTesting_amd64

请看清楚安装的包，对应的debian10是使用的qt5.11.3编译， debian12是5.15.1编译
Please take a clear look at the installed package. The corresponding Debian10 is compiled using QT5.11.3, while Debian12 is compiled using 5.15.1
1.4.6 ：deepinv20.9 is com.deepin.fantacy_1.4.6_amd64.deb

## Installation

### AppImage 2023-08-02

dowload path: [https://www.appimagehub.com/p/2068198](https://www.appimagehub.com/p/2068198)

### debian ubuntu master deepinV20.9 uos

dowload path: [master](https://github.com/dependon/fantascene-dynamic-wallpaper/releases)

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
qt >=5.10

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
