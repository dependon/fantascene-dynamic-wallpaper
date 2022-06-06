# 幻梦动态壁纸

### 在Linux下的基于X11的动态壁纸管理器

### 如果你喜欢这个玩具，请在Github页面给我一个星星。

[![biMxKK.gif](https://s4.ax1x.com/2022/02/24/biMxKK.gif)](https://imgtu.com/i/biMxKK)


## 许可
```
版权所有 (C) 2020-2022 LIUMINGHANG <liuminghang0821@gmail.com>
```

这个项目在GPLv3下发布：

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://raw.github.com/dependon/fantascene-dynamic-wallpaper/master/LICENSE)


## 维基

关于这个软件的项目说明在将来会写到wiki里。


## 安装

Fedora和CentOS Stream

请查看: [fantascene-dynamic-wallpaper](https://linux-opensource.cn/git-web/opensource/fantascene-dynamic-wallpaper)

深度操作系统
```
https://depend.lanzouj.com/iMrzI05rohwb
密码:4cfc

```

统信操作系统
```
Please download and install the package from release page

(带web控件)
https://depend.lanzouj.com/izwGV052dqif
密码:d8hx

(不带web控件)
https://depend.lanzouj.com/ihukx052dpxe
密码:bkk9
```

manjaro
```
yay -S fantascene-dynamic-wallpaper
```

archlinux
```
yay -S fantascene-dynamic-wallpaper-git
```

## 构建依赖

debian/深度操作系统/其他Linux

```
qt >=5.10

mpv >=29.0

sudo apt install qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev libqt5opengl5-dev qttools5-dev-tools qtbase5-dev qt5-qmake qtwebengine5-dev libxext-dev libxrender-dev libqt5x11extras5-dev libglib2.0-dev qt5-default

sudo apt install libxcb-shape0-dev
```


## 从源中构建

debian and ubuntu
```
dpkg-buildpackage -b -us -uc
```

其他Linux变体
```
git clone https://github.com/dependon/fantascene-dynamic-wallpaper

cd fantascene-dynamic-wallpaper

mkdir build

cd build

qmake ..

make all # 构建带有调试符号和剥离调试符号的可执行文件
```


## 感谢

感谢[Akiba illusion](https://github.com/AkibaIllusionLinux)的大力支持。

感谢[taotieren](https://github.com/taotieren)对ARCH的帮助

感谢[gesangtome](https://github.com/gesangtome)修复了一些项目的问题。

感谢[raspin0](https://github.com/raspin0)提供的波兰语翻译。

感谢[Alvaro samudio](https://github.com/alvarosamudio)提供的西班牙语翻译。


## 贡献
```
贡献总是被欢迎的。如有必要，请创建新的合并请求或者问题。
```
## 最简单的例子,可以在较低版本的mpv和qt编译,且代码结构简单(适合嵌入式设备)

[https://github.com/dependon/simple-wallpaper](https://github.com/dependon/simple-wallpaper)

## 截图

#### manjaro xfce4
<img src="https://s4.ax1x.com/2022/02/24/biMa9I.gif" alt="show" />

#### manjaro kde
<img src="https://s4.ax1x.com/2022/02/24/biMN4A.gif" alt="show" />

#### cutefish
<img src="https://s4.ax1x.com/2022/02/24/biKHpt.gif" alt="show" />

#### ubuntu
<img src="https://s4.ax1x.com/2022/02/24/biuJKO.gif" alt="show" />
