## 这是什么？

Linux下基于X11的动态壁纸，暂时不支持wayland下面

## 大体效果演示

如:manjaro gnome :
[![biMxKK.gif](https://s4.ax1x.com/2022/02/24/biMxKK.gif)](https://imgtu.com/i/biMxKK)

## 如果你喜欢这个玩具，给我一个点赞
wiki还在路上，功能还是很简单的，基本上摸索下就ok

## 如果你的linux是deepin
sudo apt install com.github.fantascene

## 如果你的linux是manjaro或者arch
你可以通过下面的方式安装

方法一:获得我打包的最新版本

yay -S fantascene-dynamic-wallpaper

方法二:通过最新提交代码编译的版本

yay -S fantascene-dynamic-wallpaper-git


## 如果你想自行编译(ubuntu /deepin / other linux)
你需要满足
qt >=5.8

mpv >=29.0

如果你的qt版本低于5.8，可能需要自行改造一些代码

下面是你需要安装的东西

sudo apt install qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev libqt5opengl5-dev qttools5-dev-tools qtbase5-dev qt5-qmake qtwebengine5-dev libxext-dev libxrender-dev libqt5x11extras5-dev libglib2.0-dev

sudo apt install libxcb-shape0-dev

## 编译
git clone https://github.com/dependon/fantascene-dynamic-wallpaper

cd fantascene-dynamic-wallpaper

mkdir build

cd build

qmake ..

make

## 如果你想生成deb包

dpkg-buildpackage - b - us - uc 

然后去上一级目录下找deb包

## 当前开发进度:

目前还是有很多bug，只是现在工作忙，且目前只能支持双屏，支持web壁纸和视频壁纸，采用mpv和qtwebengine来开发的

## bilibili demo展示(能够在bilibili一键三连就更好了)

适应所有linux的方案(桌面非原生):

[https://www.bilibili.com/video/BV1a3411j7Aj](https://www.bilibili.com/video/BV1a3411j7Aj)

适应所有deepin/uos的方案(桌面原生):

[https://www.bilibili.com/video/BV1bB4y1c7Fq](https://www.bilibili.com/video/BV1bB4y1c7Fq/)

## 联系我

liuminghang0821@gmail.com

liuminghang0821@qq.com


## 感谢(排名部分先后)
Thank [Akiba illusion](https://github.com/AkibaIllusionLinux) its great support

Thank [taotieren](https://github.com/taotieren) for his help on arch

Thank [gesangtome](https://github.com/gesangtome) for fixing some project bugs

Thanks for the Polish translation provided by  [raspin0](https://github.com/raspin0)

Thanks for the Spanish translation provided by [Alvaro samudio](https://github.com/alvarosamudio)

Thank you for submitting bugs by email

## 其他

如果有更好的方法制作动态壁纸，欢迎大家讨论和分享

github:[https://github.com/dependon/fantascene-dynamic-wallpaper](https://github.com/dependon/fantascene-dynamic-wallpaper)

gitee:[https://gitee.com/liuminghang/fantascene-dynamic-wallpaper](https://gitee.com/liuminghang/fantascene-dynamic-wallpaper)

## 其他linux表现

manjaro xfce4:

[![biMa9I.gif](https://s4.ax1x.com/2022/02/24/biMa9I.gif)](https://imgtu.com/i/biMa9I)

manjaro kde :

[![biMN4A.gif](https://s4.ax1x.com/2022/02/24/biMN4A.gif)](https://imgtu.com/i/biMN4A)

cutefish :

[![biKHpt.gif](https://s4.ax1x.com/2022/02/24/biKHpt.gif)](https://imgtu.com/i/biKHpt)

ubuntu :

[![biuJKO.gif](https://s4.ax1x.com/2022/02/24/biuJKO.gif)](https://imgtu.com/i/biuJKO)

## License

fantascene-dynamic-wallpaper is released under GPLv3+. 

Copyright (C) 2020 ~ 2022 LiuMingHang. 