
## Latest version code description (applicable to all Linux)
The latest code can be compiled and used directly, but it can't make the desktop completely transparent. It can adjust the transparency of wallpaper by itself, so as to realize dynamic wallpaper. X11 method is used to change the transparency of form

## build //linux(ubuntu /deepin  other linux)
sudo apt install qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev libqt5opengl5-dev qttools5-dev-tools qtbase5-dev qt5-qmake qtwebengine5-dev libxext-dev libxrender-dev libqt5x11extras5-dev

git clone https://github.com/dependon/fantascene-dynamic-wallpaper

cd fantascene-dynamic-wallpaper

mkdir build

cd build

qmake ..

make

## 2.0.1(Other special versions 特殊版本) （With the use of their own compiled document management, the self-test is stable and will not be stuck on the desktop 配合自己编译的文管使用,自测稳定,不会卡桌面等情况）
Since the desktop version is constantly upgraded, it also needs to constantly compile the desktop or other operations to maintain stability, otherwise it may get stuck. Therefore, from 2.0 1. From the beginning, this version will only leave the function of dream dynamic wallpaper itself. It provides the version of document management for installation and download, that is, the deep document management compiled by me must be installed (because it is necessary to open a background transparent interface). Friends who like DIY desktop strongly recommend it. If you like this software, I hope you will like it. For developers, If you have any suggestions or unclear points, you can contact me through email or submit issues. I can see that the version of the document management compiled by myself is 8.0 1. In order to prevent direct update based on the current version, if you need to delete, you need to manually downgrade each software package. If there is a problem, you can contact me. Please do not uninstall directly. There will be a problem

（由于桌面版本不断的升级,也需要不断编译桌面或者等操作来维持稳定,不然可能会存在卡死的情况,所以,从2.0.1这个版本开始将只留下幻梦动态壁纸本身的功能,提供文管的版本一起安装下载,也就是必须要安装我编译的深度文管(因为要开一个背景透明的接口,不得已),喜欢DIY桌面的朋友强烈建议使用,如果你喜欢这个软件希望你点个赞,对于开发者,有什么建议和不太清楚的地方可以联系我,通过邮箱,或者是提交issues,我都能看得到,自己编译的文管的版本都是打的8.0.1,为了防止基于现在的版本直接更新了,如果需要删除,需要手动降级各个软件包即可,如果有问题,可以联系我,请不要直接卸载,会有问题）

I put the download address in Lanzou cloud（下载地址我放在了蓝奏云）:

deepinV20.3: [https://depend.lanzoui.com/iXwewwk7fba](https://depend.lanzoui.com/iXwewwk7fba)

uos_amd64:[https://depend.lanzoui.com/il0Lzwk35xi](https://depend.lanzoui.com/il0Lzwk35xi)

uos_arm64:[https://depend.lanzoui.com/i35HSwxevkf](https://depend.lanzoui.com/i35HSwxevkf)

Installation method:, unzip, and right-click to enter the command line(安装方法:,解压,右键进入命令行):

sudo dpkg -i *

You may be prompted when installing. Please follow the relevant instructions

Seven of them are text management packages, including desktop, and one package is its own program,

After installing this version of the desktop, you can use the source code to debug the dynamic wallpaper. Due to problems in the previous version, it is very difficult to debug (that is, you need to install this desktop with transparent DBUS interface to debug the software)

The current version will be smoother and support web and video wallpaper

(可能安装的时候会提示缺少什么东西,请按照相关缺少安装

其中七个包是文管,包含桌面,1个包是本身程序的,

安装了这个版本的桌面,就可以使用源代码进行调试动态壁纸,以前的版本因为有问题,调试起来很困难(也就是说需要安装这个加上背景透明dbus接口的桌面才可以调试本软件)

当前版本会更加流畅,支持web和视频壁纸两种)

github:[https://github.com/dependon/fantascene-dynamic-wallpaper](https://github.com/dependon/fantascene-dynamic-wallpaper)

gitee:[https://gitee.com/liuminghang/fantascene-dynamic-wallpaper](https://gitee.com/liuminghang/fantascene-dynamic-wallpaper)


## If you like this toy, light the little star
Detailed instructions will be written to wiki later

## deepin linux 
sudo apt install com.github.fantascene

## manjaro deepin linux (arch)

yay -S fantascene-dynamic-wallpaper-git
## 1.1.3
Here the desktop source code is put in, modified a bit, to ensure that out of the library compilation, do not know whether stable (这里将桌面的源码都放进来了，修改了一点，保证脱离库编译，不知道是否稳定,所以需要文件管理器的环境，放出源内的第二行，更新一下，执行下面的依赖操作，就可以将编译环境弄好，建议直接用下方命令打包成deb)

## 1.1.1 
To solve the problem that sometimes can't be opened, the web wallpaper event is added

## 1.1.0 
Added web wallpaper, but it won't be recorded in history

## 1.0.6 modify:

the default language is changed to English

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

