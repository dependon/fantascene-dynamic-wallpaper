# 星火动态壁纸

官网:

[https://www.fantacy.online/](https://www.fantacy.online/)

## 1.9.2
优化了国内网络走直连，不走cdn

## 1.8.4 

适配gxde os(原生桌面)
修复了壁纸仓库部分接收到不显示的问题

## 1.8.3版本重要更新:

由幻梦改名星火(1.8.3版本开始)

新增壁纸仓库spark-hub，在线下载(试运行)


### 在Linux下的基于X11的动态壁纸管理器

[![biMxKK.gif](https://s4.ax1x.com/2022/02/24/biMxKK.gif)](https://imgtu.com/i/biMxKK)

## 最新版本下载，来自obs打包，覆盖debian10-testing ubuntu 18+ fedora openfuse等(Open Build Service)

[https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper](https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper)


AppImage:，在ubuntu16.04打包，兼容大部分版本，但是mpv版本为很旧版本，能够使用deb，尽量使用deb包

下载地址 https://sourceforge.net/projects/fantascene-dynamic-wallpaper/

Windows:

下载地址 https://sourceforge.net/projects/fantascene-dynamic-wallpaper/

## gnome桌面环境（如ubuntu os）和kylin os或deepin v23 环境的配置(使用本来的原生桌面，不改变右键菜单和桌面图标布局)

如果你的gnome桌面环境有 desktop-file-utils 插件（如ubuntu）【kylin和deepin v23 是相同的设置】

在更多设置中设置如下并保存即可

设置显示桌面图标为否(这个桌面图标指的是本软件的绘制桌面图标)

设置壁纸置于桌面上层为否(就是把壁纸放到插件或者说叫原生桌面下边层)

原理性质:

因为kylin os的桌面在设置了透明壁纸后也同样和desktop-file-utils具备透明属性，所以现在可以在kylin os和gnome两个桌面下使用原生桌面，相对比较简单

deepin V23的处理:同样的设置和前面相同，两个否，保存后会设置禁用DDE的壁纸插件，然后重新启动DDE桌面就是透明仅仅只有图标的桌面

关闭程序的时候(非杀死)会判断当前的环境，主动恢复kylin的壁纸和v23的壁纸插件，重启dde桌面等操作

dde会涉及重启dde

kylin和gnome的桌面环境不需要重启桌面

以上的操作均由程序完成(1.7.5及往上，实现该功能)


## 更新日志

[English](UpdateLog.md)

[中文](UpdateLog_zh.md)


## qq群(及时反馈和分享bug需求等) 2023/08/29

群号:917552088
 请申请的时候备注下加群的信息(比如:使用动态壁纸等，防止有小广告选手混入其中)

## 特殊版本deepinv23(2024/4/11)：

## 特殊版本deepinv20(2024/4/11)：

## 特殊版本uos家庭版(2024/4/11)：

都在星火商店下载最新的

## 1.7.3 更新通知:

默认的视频vo改为了libmpv，因为部分机型会出现窗口化mpv窗口

## 1.5.0 更新通知:

支持了在wayland环境下的运行，兼容wayland环境
编译了debian10 和debian testing(原则上12可以使用)的x86版本
编译了debian10 的arm版本

## 许可
```
版权所有 (C) 2020-2023 LIUMINGHANG <liuminghang0821@gmail.com>
```

这个项目在GPLv3下发布：

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://raw.github.com/dependon/fantascene-dynamic-wallpaper/master/LICENSE)


## 关于 ubuntu 可能会显示双图标的问题

解决:需要在更多设置中关闭桌面显示即可

造成双图标的原因，本质上我们软件是采用设置桌面的形式(即最底层，在部分操作系统是图标和壁纸揉在一块的，也就是图标和壁纸是单窗体，并且无法分割，但是部分ubuntu的桌面，我不清楚是mate的环境还是unity的环境，设计与windows类似，壁纸和图标分离。也就是我们软件作为了壁纸，而图标层依然在我们的上一层，而我们软件实现了简单的桌面的功能(这也是有些人认为没有实现像windows一样的动态壁纸的原因，被骂的原因之一)，我们在更多设置点是否显示桌面点击否即可)

## 原理

将mpv窗口和webview管理的窗口设置为桌面属性

即壁纸和图标是分离的，即不需要我们软件提供的桌面，如果是揉在一起的一个整体，则需要，否则无法实现(所以如果是原生桌面的这样，除非官方提供接口，否则无法实现)

## 心得

软件的灵感来源于deepin桌面dde-desktop修改了源码后可以通过dbus将壁纸设为透明，这样实现我们的第一版动态壁纸，后来，因为需要频繁重新编译dde-desktop，心态比较爆炸，所以后面选择不作为主要维护，有兴趣的可以自行编译代码，deepin分支，需要安装文管和影院的依赖即可编译打包使用

后来写了一个简单的桌面用作通用版

## 维基

关于这个软件的项目说明在将来会写到wiki里。

## qq群(及时反馈和分享bug需求等) 2023/08/29

群号:917552088 

请申请的时候备注下加群的信息(比如:使用动态壁纸等，防止有小广告选手混入其中)

## 安装(下载地址主要来自github，链接看运气)

## AppImageHub

dowload path: [https://www.appimagehub.com/p/2068198](https://www.appimagehub.com/p/2068198)

### 主版本（程序自己写的简易桌面）

dowload path: [master](https://github.com/dependon/fantascene-dynamic-wallpaper/releases/)

### fedora and centos stream

Please see: [https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper](https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper)

### archlinux and manjaro

yay -S fantascene-dynamic-wallpaper-git

### windows 简易demo

Please see: [https://github.com/dependon/mpv-dynamic-wallpaper](https://github.com/dependon/mpv-dynamic-wallpaper)

## 构建依赖

debian/深度操作系统/其他Linux

```
qt >=5.6

mpv >=29.0

sudo apt install qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev libqt5opengl5-dev qttools5-dev-tools qtbase5-dev qt5-qmake qtwebengine5-dev libxext-dev libxrender-dev libqt5x11extras5-dev libglib2.0-dev build-essential g++ libxcb-shape0-dev build-essential
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

感谢[shenmo](https://gitee.com/spark-store-project)的大力支持。

感谢[M0rtzz](https://github.com/M0rtzz)的提供pkg转化mp4教程。

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

## 一种可能的`scene.pkg`解包为`.mp4`方案

[方案步骤](pkg2mp4.md)

## 截图

#### manjaro xfce4
<img src="https://s4.ax1x.com/2022/02/24/biMa9I.gif" alt="show" />

#### manjaro kde
<img src="https://s4.ax1x.com/2022/02/24/biMN4A.gif" alt="show" />

#### cutefish
<img src="https://s4.ax1x.com/2022/02/24/biKHpt.gif" alt="show" />

#### ubuntu
<img src="https://s4.ax1x.com/2022/02/24/biuJKO.gif" alt="show" />
