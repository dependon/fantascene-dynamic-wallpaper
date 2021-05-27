


## 如果你喜欢这个玩具，点下小星星
后续会有详细使用说明写入wiki
## 1.0.3(deepinv20.2.1)/1.0.4(uos) 更新

增加可以配置解码方式

增加可以降低帧率

检测到全屏可以选择不继续模仿

一切为了省电

[![2PUCz4.png](https://z3.ax1x.com/2021/05/27/2PUCz4.png)](https://imgtu.com/i/2PUCz4)

## 1.0.0新增智能模式
该模式，检测到有窗口全屏，暂停播放，窗口不再全屏时，继续播放，我很喜欢这样，哈哈！！

## 程序限制
这个是根据deepinv20及其以上版本制作的，uos在也能找到对应的版本(并且deepinv20.2和20.1有区分)，在下载的deb包有标识，打包采用本地打包

## 原来的名称
deepin-dreamscene-ui

## 正在寻求技术突破项:
视频播放帧率设置

## 目前是个动态壁纸demo [video wallpaper demo for deepin/uos]
关于deepin和uos系统的测试软件包,可以在右侧网页下载

## 演示视频:
[https://www.bilibili.com/video/BV1bB4y1c7Fq](https://www.bilibili.com/video/BV1bB4y1c7Fq/)

## 依赖
sudo apt install qtcreator libdtkwidget-dev libdtkcore-dev libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg

## dde-desktop 根据这个patch打入dde-file-manager，拿出库和dde-desktop
patch :https://gitee.com/liuminghang/transparent-desktop5.11.3/blob/master/0001-feat.patch

## 在已经能编译代码的情况下如何打包
在修改了代码之后，直接在代码主目录下打开终端执行 dpkg-buildpackage -b -us -uc ，即可得到deb包

无理的需求可以随便提

如果有更好的做动态壁纸的方法欢迎讨论和分享

本软件作为学习作用,可以提出一些有理无理的需求,有时间会做下去的.
正在编码能力提高的路上,

目前:用已经编译好的透明dde-desktop在运行时替换原生dde-desktop

绘制动态壁纸，使用硬解码，目前最高支持双屏（功能当前相对简单）

最近几个月都会保持更新

有什么想法和问题都可以联系我liuminghang0821@gmail.com

有问必回

也可以提交issues 和pull requests

个人想法有限，能提意见和需求最好

目前已知问题:
1.在运行中修改屏幕分辨率等操作，也许需要重启程序



github地址:https://github.com/dependon/fantascene-dynamic-wallpaper
gitee地址:https://gitee.com/liuminghang/fantascene-dynamic-wallpaper

