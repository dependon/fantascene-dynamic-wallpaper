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

## Wiki

Detailed instructions will be written to wiki later.
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

### debian ubuntu master deepinV20.9 uos

dowload path: [master](https://github.com/dependon/fantascene-dynamic-wallpaper/releases)

### fedora and centos stream

Please see: [fantascene-dynamic-wallpaper](https://linux-opensource.cn/git-web/opensource/fantascene-dynamic-wallpaper)

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
