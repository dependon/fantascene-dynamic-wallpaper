# 星火动态壁纸

### 在Linux下的基于X11的动态壁纸管理器

[![biMxKK.gif](https://s4.ax1x.com/2022/02/24/biMxKK.gif)](https://imgtu.com/i/biMxKK)

### 官网:

[https://www.fantacy.online/](https://www.fantacy.online/)

## 其他语言介绍

[English](README.md) [中文项目介绍](README_zh.md)


## 更新日志

[English](md/UpdateLog.md)

[中文](md/UpdateLog_zh.md)

## 支持语言（当前使用的机器翻译）

#### 如果可能的话，您可以提交.ts文件的更新以帮助我们

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

https://gitee.com/liuminghang/fantascene-dynamic-wallpaper/wikis/pages

## 最新版本下载，来自obs打包，覆盖debian10-testing ubuntu 18+ fedora openfuse等(Open Build Service),建议使用这里安装最新，appimage可能显示不全(通过调整视频比例解决问题)

[https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper](https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper)


AppImage:，在ubuntu16.04打包，兼容大部分版本，但是mpv版本为很旧版本，能够使用deb，尽量使用deb包

下载地址 https://sourceforge.net/projects/fantascene-dynamic-wallpaper/

Windows:

下载地址 https://sourceforge.net/projects/fantascene-dynamic-wallpaper/


## qq群(及时反馈和分享bug需求等) 2023/08/29

群号:917552088
请申请的时候备注下加群的信息(比如:使用动态壁纸等，防止有小广告选手混入其中)

## 许可
```
版权所有 (C) 2020-2024 LIUMINGHANG <liuminghang0821@gmail.com>
```

这个项目在GPLv3下发布：

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://raw.github.com/dependon/fantascene-dynamic-wallpaper/master/LICENSE)


## 关于 ubuntu 可能会显示双图标的问题

解决:需要在更多设置中关闭桌面显示即可

造成双图标的原因，本质上我们软件是采用设置桌面的形式(即最底层，在部分操作系统是图标和壁纸揉在一块的，也就是图标和壁纸是单窗体，并且无法分割，但是部分ubuntu的桌面，我不清楚是mate的环境还是unity的环境，设计与windows类似，壁纸和图标分离。也就是我们软件作为了壁纸，而图标层依然在我们的上一层，而我们软件实现了简单的桌面的功能(这也是有些人认为没有实现像windows一样的动态壁纸的原因，被骂的原因之一)，我们在更多设置点是否显示桌面点击否即可)

## 原理

将mpv窗口和webview管理的窗口设置为桌面属性

即壁纸和图标是分离的，即不需要我们软件提供的桌面，如果是揉在一起的一个整体，则需要，否则无法实现(所以如果是原生桌面的这样，除非官方提供接口，否则无法实现)

## 维基

请查找项目wiki。


### fedora and centos stream

Please see: [https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper](https://software.opensuse.org//download.html?project=home%3Aliuminghang&package=fantascene-dynamic-wallpaper)

### archlinux and manjaro

yay -S fantascene-dynamic-wallpaper-git

或者

yay -S fantascene-dynamic-wallpaper

### windows 简易demo

Please see: [https://github.com/dependon/mpv-dynamic-wallpaper](https://github.com/dependon/mpv-dynamic-wallpaper)

## 构建依赖

```
qt >=5.8
mpv >=29.0
```

### debian/Deepin/Other

```
git clone https://gitee.com/liuminghang/fantascene-dynamic-wallpaper

cd fantascene-dynamic-wallpaper

sh start_makeLocal.sh 

```

## 打包为deb

debian and ubuntu and deepin

```
git clone https://gitee.com/liuminghang/fantascene-dynamic-wallpaper

cd fantascene-dynamic-wallpaper

sh start_deb.sh

```

## 感谢

感谢[shenmo](https://gitee.com/spark-store-project)的大力支持。[欢迎体验星火商店，专为linux应用分发打造的社区商店](https://www.spark-app.store)

感谢[M0rtzz](https://github.com/M0rtzz)的提供pkg转化mp4教程。

感谢[Akiba illusion](https://github.com/AkibaIllusionLinux)的大力支持。

感谢[taotieren](https://github.com/taotieren)对arch的帮助

感谢[gesangtome](https://github.com/gesangtome)修复了一些项目的问题。

感谢[raspin0](https://github.com/raspin0)提供的波兰语翻译。

感谢[Alvaro samudio](https://github.com/alvarosamudio)提供的西班牙语翻译。

感谢[lisuke](https://github.com/lisuke) 修改bug

感谢 [gfdgd_xi](https://github.com/gfdgd-xi) 对龙芯编译的支持,欢迎体验[GXDE OS](https://www.gxde.top/)

感谢 [SoHugePenguin](https://github.com/SoHugePenguin) 增加暗黑风格qss和优化程序体验

## 贡献
```
贡献总是被欢迎的。如有必要，请创建新的合并请求或者问题。
```
## 最简单的例子,可以在较低版本的mpv和qt编译,且代码结构简单(适合嵌入式设备)

[https://github.com/dependon/simple-wallpaper](https://github.com/dependon/simple-wallpaper)

## 一种可能的`scene.pkg`解包为`.mp4`方案

[方案步骤](md/pkg2mp4.md)

## 截图

#### manjaro xfce4
<img src="https://s4.ax1x.com/2022/02/24/biMa9I.gif" alt="show" />

#### manjaro kde
<img src="https://s4.ax1x.com/2022/02/24/biMN4A.gif" alt="show" />

#### cutefish
<img src="https://s4.ax1x.com/2022/02/24/biKHpt.gif" alt="show" />

#### ubuntu
<img src="https://s4.ax1x.com/2022/02/24/biuJKO.gif" alt="show" />
