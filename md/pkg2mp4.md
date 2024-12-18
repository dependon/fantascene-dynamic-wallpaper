# 一种可能的`scene.pkg`解包为`.mp4`方案

## ①前提

首先系统需要为Windows（因为Linux中没有找到好用的解包工具）

下载解包工具：

```sh
git clone https://github.com/M0rtzz/RePKG-GUI.git
```

找到所需壁纸的`.pkg`文件：

比如我的这个之前订阅的，但是作者删除了，所以只留下了这个`.pkg`文件：

![1](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:20:31_image-20240523202024277.png)

效果如下：

![2](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:43:06_QQ2024523-204136-HD20245232042231.gif)

右键该壁纸选择`在资源管理器中打开`：

![3](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:20:53_image-20240523202053876.png)

复制该`scene.pkg`到一个位置备用并记下路径，并准备一个存放解包结果的文件夹：

![4](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:22:48_image-20240523202248074.png)

## ②解包

运行`RePKG-GUI.exe`：

![5](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:24:10_image-20240523202410211.png)

拖拽或选择刚才的`scene.pkg`并选择输出路径:

![6](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:24:46_image-20240523202446144.png)

点击提取：

![7](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:25:10_image-20240523202510279.png)

解包结果如下：

![8](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:25:43_image-20240523202543918.png)

## ③制作自己的壁纸

右键任务栏的`Wallpaper Engine`并创建壁纸：

![9](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:27:17_image-20240523202717474.png)

点击使用模板：

![10](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:28:36_image-20240523202836644.png)

起一个项目名称，模板随便选一个（应该是吧:)），分辨率不用管（解包之后的`.json`文件中有壁纸的分辨率，下文会替换）：

![11](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:30:55_image-20240523203055084.png)

点击`确认`之后进入编辑器：

![12](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:31:22_image-20240523203121981.png)

之后关闭编辑器，进入`Wallpaper Engine`，你会发现多了一个名叫`test`的壁纸：

![13](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:32:29_image-20240523203229535.png)

之后右键该壁纸选择`在资源管理器中打开`：

![14](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:33:06_image-20240523203306846.png)

![15](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:34:55_image-20240523203454996.png)

之后将刚才解包过的文件复制到此处覆盖：

![16](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:37:02_image-20240523203702616.png)

这时候再打开`Wallpaper Engine`单击`test`后单击`在编辑器中打开`：

![17](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:38:50_image-20240523203850826.png)

此时你会发现，壁纸已经制作完成了：

![18](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:45:10_QQ2024523-204352-HD20245232044362.gif)

## ④制作`.mp4`

此时我们制作的壁纸右键有了`导出.mpkg文件`这一选项：

![19](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:46:11_image-20240523204611311.png)

而原始的`scene.pkg`右键则没有此选项：

![20](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:47:14_image-20240523204714845.png)

此时我们选择`导出.mpkg文件`，选项按我这么选：

![21](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:47:52_image-20240523204752694.png)

点击`确定`后选择输出位置：

![22](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:48:31_image-20240523204831762.png)

完成后重命名`.mpkg`为`.pkg`：

![23](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:50:28_image-20240523205028573.png)

此时再将此`.pkg`文件用`RePKG-GUI.exe`解包：

![24](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:51:25_image-20240523205125307.png)

![25](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:51:42_image-20240523205142495.png)

解包成功之后，就得到了我们需要的`.mp4`文件：

![26](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:52:14_image-20240523205214871.png)

![27](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:55:15_QQ2024523-20546-HD20245232054481.gif)

但是此时得到的`.mp4`文件是没有BGM的，需要BGM的兄弟可以使用`PR`等视频剪辑软件和刚才第一次解包`scene.pkg`的输出目录中`sounds/`下的`.mp3`文件一起剪辑一个带有BGM的`.mp4`，之后就可以正常使用`.mp4`愉快的在Linux中想用你喜欢的壁纸啦~

![28](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/20:56:51_image-20240523205651723.png)

效果如下（黑边应该是录制BUG）：

![29](https://jsd.cdn.zzko.cn/gh/M0rtzz/ImageHosting@master/images/Year:2024/Month:05/Day:23/21:17:40_5%E6%9C%8823%E6%97%A52024523211262.gif)

感谢观看~
