# Wayland支持状态

「星火动态壁纸」支持Wayland会话，前提是当前Wayland合成器需要支持`wlr-layer-shell-unstable-v1` 协议。



## 合成器支持性

很多合成器都支持该协议，包括但不限于KDE KWin/Deepin KWin, Sway, Hyprland, Niri, River, LabWC, DWL, Wayfire, Cage, System76 COSMIC, Canonical Mir, Open Kylin Wayland Compositor/GXDE-Wlcom, Deepin Treeland, CuarzoSoftware Louvre...

但是，需要注意的是，GNOME的合成器Mutter不支持此协议，所以动态壁纸程序将不在GNOME Wayland上运行。如果用户使用的是非常远古的Deepin Mutter则是可以的。是否运行不取决于合成器的名字，而是是否支持`wlr-layer-shell-unstable-v1` 协议。



## Qt版本要求

很不幸的是，在Debian上测试时，我们发现依赖的`layer-shell`版本有一个问题：基于Qt5的`layer-shell-qt5`与基于Qt6的`layer-shell-qt`冲突互斥，安装一个会自动卸载另一个。同时，`layer-shell-qt`也算是Plasma桌面的核心库之一，不该有卸载的情况发生，考虑到Plasma6的流行性，决定使用`layer-shell-qt`（基于Qt6），Qt5构建不提供LayerShell与Wayland支持。



## 测试

当前进在GXDE OS 25.4.1的GXDE-Wlcom *REL 2.3.3 // PROTOTYPE_1*版本上进行了测试。



## 接入说明

要支持其余Wayland环境相对轻松——只要合成器支持`wlr-layer-shell-unstable-v1` 协议则壁纸会通过LayerShell协议自动固定到壁纸层，剩下要做的仅仅是最好关闭掉原来桌面环境的壁纸层，但保留图标层。



## 说明

如果您不知道LayerShell是什么，您可能会觉得一头雾水。您也可能有如下疑问：X11上好好的，怎么在Wayland下会失效呢？这里是一些背景知识——



### 问题现状？

动态壁纸看起来虽然也是一个全屏窗口，但它不能是普通的应用程序窗口。

如果在 Wayland 下创建一个普通的全屏窗口，合成器会把它当成`xdg-toplevel`，也就是和浏览器、文件管理器相同的普通应用窗口。它可能：

1. 出现在任务栏和窗口预览中；
2. 挡住桌面图标和其他窗口；
3. 被 Alt+Tab 切换到；
4. 在不支持的桌面环境中变成一个普通全屏窗口。



### 哦，所以LayerShell是？

是一套 Wayland 扩展协议，名称是：`zwlr_layer_shell_v1`。

Wayland 的核心协议只提供 surface、输入、输出等基础能力。

桌面面板、Dock、锁屏提示和壁纸这类“桌面组件”不能按照普通应用窗口来管理，因此合成器可以额外提供 LayerShell，让这些组件声明自己应该位于桌面的哪一层。

可以把合成器管理的画面想成从后向前叠放的图层：

* **background**: 在最后面，适合壁纸
* **bottom**: 普通窗口最下层，适用于某些桌面组件
* **top**: 普通窗口上面，常用于面板和 Dock
* **overlay**: 最前面，常用于OSD、通知等覆盖内容



Fantascene 需要的是 background。它不是简单地请求「把普通窗口降低一点」，而是在创建 surface 时就告诉合成器：「这个 surface 是桌面背景组件，不是普通应用窗口。」



LayerShell 还允许客户端描述这个 surface 如何摆放。例如我们在这次提交里设置了：

```C++
anchors = top | bottom | left | right
```



...表示贴住显示器四边，由合成器把它铺满整个输出；设置 exclusive zone 为 `-1`，表示壁纸使用完整输出区域且不为自己保留工作区（保留工作区会把窗口的空间挤走的）；设置 keyboard interactivity为 `none`，表示壁纸不抢键盘焦点。

这里涉及三个不同的东西：

1. **Wayland 合成器**：例如 KWin 或 wlroots 系合成器，负责真正排列和显示 surface；
2. **zwlr_layer_shell_v1**：客户端和合成器之间约定消息格式的协议；
3. **LayerShellQt**：给 Qt 程序使用的封装库，负责把 QWindow 配置成 layer surface。



LayerShellQt 本身不能强迫桌面支持该协议。程序只能先查看合成器公布的 Wayland registry；发现 `zwlr_layer_shell_v1` 后才能使用。如果合成器没有公布它，那LayerShellQt就不起作用。



### 为什么 X11 的旧实现到了 Wayland 会变成普通窗口？

Fantascene 原来的 Linux 壁纸实现是为 X11 写的。在 X11 中，所有窗口都由X Server 统一管理，每个窗口都有一个全局可见的 XID。程序可以通过 Xlib、XCB 和 EWMH 修改窗口属性，例如：`_NET_WM_WINDOW_TYPE_DESKTOP`, `_NET_WM_STATE_BELOW`; 还可以调用 `lower()`、`XLowerWindow()`，或者查找桌面窗口并调整它们的堆叠顺序。窗口管理器看到这些属性后，就可能把该窗口当成桌面窗口放到最底层。



Wayland 没有 XID、X11 Atom 或 EWMH 窗口类型。每个客户端只能操作自己的`wl_surface`，也不能随意查询、修改或重排其他客户端的窗口。窗口处于哪个层级，由它创建时取得的 Wayland role 和合成器的策略决定。

因此，在原生 Wayland 后端中，下面这些 X11 做法都不能把 `wl_surface` 变成壁纸：

1. 设置 `_NET_WM_WINDOW_TYPE_DESKTOP`
2. 设置 `_NET_WM_STATE_BELOW`
3. 调用 `QWidget::lower()`
4. 设置 `Qt::WindowStaysOnBottomHint`
5. 把窗口大小改成整个屏幕。

其中前两项只属于 X11；后三项最多只是窗口提示或几何操作，并不会赋予 Wayland background 角色。

当顶层 QWidget/QWindow 使用 Qt 的 Wayland 平台插件，而程序又没有给它选择其他 shell role 时，Qt 的默认选择就是：

    wl_surface -> xdg_surface -> xdg_toplevel

所以旧代码即使能够编译运行，最终得到的仍然是普通应用窗口。把它拉伸到全屏、去掉边框、调用 lower()，都不会改变它是 xdg-toplevel 这个事实。



这里还要区分「Wayland 会话」和「原生 Wayland 窗口」：

如果设置 `QT_QPA_PLATFORM=xcb`，程序会通过 XWayland 创建 X11 窗口。此时它仍有 XID，也可能继续使用部分 EWMH 属性，但合成器看到的是一个 XWayland 兼容窗口，不是原生 Wayland 背景 surface。不同合成器是否接受这些旧提示并不一致，多屏、层级和桌面图标行为也无法可靠保证。



本次实现使用 `QT_QPA_PLATFORM=wayland` 创建原生 Wayland 窗口，因此必须用 `zwlr_layer_shell_v1` 明确取得 background 角色，不能再依赖 X11 桌面属性。