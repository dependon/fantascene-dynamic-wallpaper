Managed animated wallpaper based on X11 under Linux

## If you like this toy, light the little star
Detailed instructions will be written to wiki later

## deepin linux 
sudo apt install com.github.fantascene

## manjaro deepin linux (arch)
yay -S fantascene-dynamic-wallpaper

yay -S fantascene-dynamic-wallpaper-git

## 1.2.8 

1.add polish translation 

2.add playlist

## 1.2.5 (Adaptation of special-shaped screen )

[![7juYBn.png](https://s4.ax1x.com/2022/01/27/7juYBn.png)](https://imgtu.com/i/7juYBn)

## 1.2.0 (Fix stuck desktop)
网盘下载:

deepin:

[https://depend.lanzouj.com/iCbccz7qena](https://depend.lanzouj.com/iCbccz7qena)

password:85v9

uos

[https://depend.lanzouj.com/iZPYGz7qesf](https://depend.lanzouj.com/iZPYGz7qesf)

password:8zyn

uos_arm64

[https://depend.lanzouj.com/iLpGPz7qfcf](https://depend.lanzouj.com/iLpGPz7qfcf)

password:9s4j

## Latest version code description (applicable to all Linux)
The latest code can be compiled and used directly, but it can't make the desktop completely transparent. It can adjust the transparency of wallpaper by itself, so as to realize dynamic wallpaper. X11 method is used to change the transparency of form
### (Suitable for all X11 Linux desktops)
bilibili :[https://www.bilibili.com/video/BV1CR4y1g7sT/](https://www.bilibili.com/video/BV1CR4y1g7sT/)

dowload : [https://depend.lanzouj.com/iQYvFyqjape](https://depend.lanzouj.com/iQYvFyqjape)

password:23x2

## build //linux(ubuntu /deepin  other linux)
qt >=5.8

mpv >=29.0

sudo apt install qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev libqt5opengl5-dev qttools5-dev-tools qtbase5-dev qt5-qmake qtwebengine5-dev libxext-dev libxrender-dev libqt5x11extras5-dev libglib2.0-dev

sudo apt install libxcb-shape0-dev

git clone https://github.com/dependon/fantascene-dynamic-wallpaper

cd fantascene-dynamic-wallpaper

mkdir build

cd build

qmake ..

make


## 1.0.5 new features

Traverse the download directory of the wallpaper engine, emmm
[![2FL7UH.png](https://z3.ax1x.com/2021/05/28/2FL7UH.png)](https://imgtu.com/i/2FL7UH)

## 1.0.3(deepinv20.2.1)/1.0.4(uos) update

Add configurable decoding mode

Increase can reduce the frame rate

If full screen is detected, you can choose not to continue to imitate

It's all about saving electricity

[![2PUCz4.png](https://z3.ax1x.com/2021/05/27/2PUCz4.png)](https://imgtu.com/i/2PUCz4)


##Procedural restrictions

This is based on the version of deepin V20 and above. The corresponding version can also be found in UOS (and deepin v20.2 and 20.1 are different). The downloaded DEB package is marked and packaged locally

## Original name
deepin-dreamscene-ui

## Now it's a dynamic wallpaper demo [video wallpaper demo for deepin/uos]
The test software package of deepin and UOS system can be downloaded from the right page

## demo video:
[https://www.bilibili.com/video/BV1bB4y1c7Fq](https://www.bilibili.com/video/BV1bB4y1c7Fq/)

## How to package when the code can be compiled

After modifying the code, open the terminal directly in the main directory of the code and execute 

dpkg-buildpackage - b - us - uc 

to get DEB package

## other
Unreasonable demands can be mentioned at will

If there is a better way to make dynamic wallpaper, welcome to discuss and share

This software as a learning function, can put forward some reasonable and unreasonable needs, will do it when there is time

We are on the way to improve our coding ability,

At present: replace the native DDE desktop with the compiled transparent DDE desktop at run time

Draw dynamic wallpaper, use hard decoding, at present the highest support double screen (the function is relatively simple)

It will be updated in recent months

If you have any ideas or questions, please contact me liuminghang0821@gmail.com

If you ask, you will answer

You can also submit issues and pull requests

Personal ideas are limited, and it's best to be able to put forward opinions and demands

Currently known problems are as follows:

1. You may need to restart the program to modify the screen resolution
 

github:https://github.com/dependon/fantascene-dynamic-wallpaper

gitee:https://gitee.com/liuminghang/fantascene-dynamic-wallpaper

