# Wayland Support Status

Fantascene Dynamic Wallpaper supports Wayland sessions when the current
Wayland compositor implements the `wlr-layer-shell-unstable-v1` protocol.



## Compositor Compatibility

Many compositors support this protocol, including KDE KWin/Deepin KWin, Sway,
Hyprland, Niri, River, LabWC, DWL, Wayfire, Cage, System76 COSMIC, Canonical
Mir, OpenKylin Wayland Compositor/GXDE-Wlcom, Deepin Treeland, and
CuarzoSoftware Louvre.

One important exception is GNOME's Mutter compositor, which does not implement
this protocol. The dynamic wallpaper will therefore not run on GNOME Wayland,
although very old Deepin releases based on a patched Mutter may work. Support
is determined by whether the compositor actually advertises
`wlr-layer-shell-unstable-v1`, not by the compositor's name.



## Qt Version Requirements

Unfortunately, testing on Debian revealed a packaging conflict between the
Qt 5 `layer-shell-qt5` package and the Qt 6 `layer-shell-qt` package: installing
one automatically removes the other. Because `layer-shell-qt` is also a core
library in the Plasma desktop and Plasma 6 is increasingly common, Fantascene
uses the Qt 6 `layer-shell-qt` package. Qt 5 builds do not provide native
Wayland or LayerShell support.



## Testing

This implementation has currently been tested on GXDE OS 25.4.1 with
GXDE-Wlcom *REL 2.3.3 // PROTOTYPE_1*.



## Integration Notes

Supporting other Wayland environments is relatively straightforward: when the
compositor implements `wlr-layer-shell-unstable-v1`, Fantascene automatically
uses LayerShell to attach the wallpaper to the background layer. The remaining
integration step is preferably to disable the desktop environment's original
wallpaper layer while keeping its desktop icon layer enabled.



## Explanation

If you have not encountered LayerShell before, the description above may be
confusing. You may also wonder why an implementation that worked under X11 no
longer works under Wayland. The following sections provide the necessary
background.



### What Is the Problem?

Although a dynamic wallpaper may look like a fullscreen window, it must not be
managed as an ordinary application window.

If an ordinary fullscreen window is created under Wayland, the compositor
treats it as an `xdg-toplevel`, just like a browser or file manager window. It
may then:

1. Appear in the taskbar and window previews;
2. Cover desktop icons and other windows;
3. Appear in the Alt+Tab window switcher;
4. Become an ordinary fullscreen window on an unsupported desktop.



### So, What Is LayerShell?

LayerShell is a Wayland extension protocol whose interface is named
`zwlr_layer_shell_v1`.

The core Wayland protocol provides basic facilities such as surfaces, input,
and outputs. Desktop components such as panels, docks, on-screen displays, and
wallpapers should not be managed like ordinary application windows. A
compositor can therefore implement LayerShell, allowing these components to
declare the desktop layer on which they belong.

Think of the compositor's scene as a stack of layers ordered from back to
front:

* **background**: The rearmost layer, suitable for wallpapers
* **bottom**: Below ordinary windows, suitable for some desktop components
* **top**: Above ordinary windows, commonly used by panels and docks
* **overlay**: The frontmost layer, commonly used by OSDs, notifications, and
  other overlay content



Fantascene needs the **background** layer. It is not merely asking the
compositor to "lower this ordinary window." When the surface is created,
Fantascene tells the compositor: "This surface is a desktop background
component, not an ordinary application window."



LayerShell also lets a client describe how the surface should be positioned.
For example, this implementation sets:

```cpp
anchors = top | bottom | left | right
```



This anchors the surface to all four edges so that the compositor stretches it
across the entire output. An exclusive zone of `-1` lets the wallpaper use the
full output without reserving workspace for itself; reserving workspace would
push application windows away. Keyboard interactivity is set to `none`, so the
wallpaper does not take keyboard focus.

Three separate pieces are involved:

1. **Wayland compositor**: KWin or a wlroots-based compositor, for example,
   which actually arranges and displays surfaces;
2. **zwlr_layer_shell_v1**: The protocol defining the messages exchanged
   between the client and compositor;
3. **LayerShellQt**: A Qt wrapper that configures a `QWindow` as a layer
   surface.



LayerShellQt cannot force a desktop to support the protocol. Fantascene must
first inspect the Wayland registry and may use LayerShell only when the
compositor advertises `zwlr_layer_shell_v1`. If the compositor does not
advertise it, LayerShellQt cannot create a genuine layer surface.



### Why Does the Old X11 Implementation Become an Ordinary Window on Wayland?

Fantascene's original Linux wallpaper implementation was written for X11. In
X11, every window is managed by the X server and has a globally visible XID.
An application can use Xlib, XCB, and EWMH to modify window properties such as
`_NET_WM_WINDOW_TYPE_DESKTOP` and `_NET_WM_STATE_BELOW`. It can also call
`lower()` or `XLowerWindow()`, locate desktop windows, and adjust their stacking
order. After seeing these properties, the window manager may treat the window
as a desktop window and place it at the bottom.



Wayland has no XID, X11 atoms, or EWMH window types. A client can operate only
on its own `wl_surface`; it cannot freely inspect, modify, or reorder another
client's windows. A window's position in the compositor's scene is determined
by the Wayland role assigned when it is created and by compositor policy.

Consequently, none of the following X11 techniques can turn a native Wayland
`wl_surface` into a wallpaper:

1. Setting `_NET_WM_WINDOW_TYPE_DESKTOP`
2. Setting `_NET_WM_STATE_BELOW`
3. Calling `QWidget::lower()`
4. Setting `Qt::WindowStaysOnBottomHint`
5. Resizing the window to cover the entire screen

The first two exist only on X11. The remaining three are, at most, window hints
or geometry operations; they do not assign the Wayland **background** role.

When a top-level `QWidget` or `QWindow` uses Qt's Wayland platform plugin and
the application does not select another shell role, Qt uses this default path:

    wl_surface -> xdg_surface -> xdg_toplevel

The old code can therefore compile and run while still producing an ordinary
application window. Stretching it to fullscreen, removing its borders, or
calling `lower()` does not change the fact that it is an `xdg-toplevel`.



It is also important to distinguish a "Wayland session" from a "native Wayland
window."

If `QT_QPA_PLATFORM=xcb` is set, the application creates an X11 window through
XWayland. That window still has an XID and may continue to use some EWMH
properties, but the compositor sees an XWayland compatibility window rather
than a native Wayland background surface. Compositors differ in whether they
honor these legacy hints, so multi-monitor behavior, stacking, and interaction
with desktop icons cannot be guaranteed.



This implementation uses `QT_QPA_PLATFORM=wayland` to create native Wayland
windows. It must therefore use `zwlr_layer_shell_v1` to obtain the background
role explicitly instead of relying on X11 desktop properties.
