# Fantascene-dynamic-wallpaper

### Managed animated wallpaper based on X11 under Linux.

### Give me a star on github page if you like this toy :)

[![biMxKK.gif](https://s4.ax1x.com/2022/02/24/biMxKK.gif)](https://imgtu.com/i/biMxKK)

## License
```
COPYRIGHT (C) 2020-2022 LIUMINGHANG <liuminghang0821@gmail.com>
```

THE PROJECT IS RELEASED UNDER THE：

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://raw.github.com/dependon/fantascene-dynamic-wallpaper/master/LICENSE)

## Wiki

Detailed instructions will be written to wiki later.

## Installation

### debian ubuntu master

dowload path: [master](https://github.com/dependon/fantascene-dynamic-wallpaper/releases/download/1.3.7/fantascene-dynamic-wallpaper_master_1.4.1_amd64.deb)

### fedora and centos stream

Please see: [fantascene-dynamic-wallpaper](https://linux-opensource.cn/git-web/opensource/fantascene-dynamic-wallpaper)

### deepinV20.9

dowload path: [deepinv20.9](https://github.com/dependon/fantascene-dynamic-wallpaper/releases/download/1.3.7/com.deepin.fantacy_1.4.5_amd64.deb)

### uos

x86_x64 dowload path: [1.3.7amd64](https://github.com/dependon/fantascene-dynamic-wallpaper/releases/download/1.3.7/fantascene-dynamic-wallpaper-uos_1.3.7_amd64.deb)

arm dowload path: [1.3.7arm64](https://github.com/dependon/fantascene-dynamic-wallpaper/releases/download/1.3.7/com.github.fantacy_1.3.7_arm64.uos_normal.deb)

arm(no web) dowload path: [1.3.7arm64_noweb](https://github.com/dependon/fantascene-dynamic-wallpaper/releases/download/1.3.7/com.github.fantacy_1.3.7_arm64.uos_noweb.deb)

### archlinux and manjaro

yay -S fantascene-dynamic-wallpaper-git

### windows 

Please see: [https://github.com/dependon/mpv-dynamic-wallpaper](https://github.com/dependon/mpv-dynamic-wallpaper)

### ubuntu18.04

Please see: [https://github.com/dependon/simple-wallpaper](https://github.com/dependon/simple-wallpaper)

dowload path:[simple-wallpaper](https://github.com/dependon/fantascene-dynamic-wallpaper/releases/download/1.3.7/simple-wallpaper_1.0.0_amd64_ubuntu18.04.deb)

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
