## What is this ? 
Managed animated wallpaper based on X11 under Linux

[![biMxKK.gif](https://s4.ax1x.com/2022/02/24/biMxKK.gif)](https://imgtu.com/i/biMxKK)

## If you like this toy, Give me a star
Detailed instructions will be written to wiki later

## deepin linux 
sudo apt install com.github.fantascene

## manjaro linux (arch)
yay -S fantascene-dynamic-wallpaper

yay -S fantascene-dynamic-wallpaper-git


## build dependencies(ubuntu /deepin / other linux)
qt >=5.8

mpv >=29.0

sudo apt install qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev libqt5opengl5-dev qttools5-dev-tools qtbase5-dev qt5-qmake qtwebengine5-dev libxext-dev libxrender-dev libqt5x11extras5-dev libglib2.0-dev

sudo apt install libxcb-shape0-dev

## build
git clone https://github.com/dependon/fantascene-dynamic-wallpaper

cd fantascene-dynamic-wallpaper

mkdir build

cd build

qmake ..

make

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


## Contact Me

liuminghang0821@gmail.com

liuminghang0821@qq.com

## thanks(In no particular order)
Thank [Akiba illusion](https://github.com/AkibaIllusionLinux) its great support

Thank [taotieren](https://github.com/taotieren) for his help on arch

Thank [gesangtome](https://github.com/gesangtome) for fixing some project bugs

Thanks for the Polish translation provided by  [raspin0](https://github.com/raspin0)

Thanks for the Spanish translation provided by [Alvaro samudio](https://github.com/alvarosamudio)

Thank you for submitting bugs by email

## other

Unreasonable demands can be mentioned at will

If there is a better way to make dynamic wallpaper, welcome to discuss and share

github:https://github.com/dependon/fantascene-dynamic-wallpaper

gitee:https://gitee.com/liuminghang/fantascene-dynamic-wallpaper

## linux demo show 

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
