# Fedora 构建 / 打包指南（fantascene-dynamic-wallpaper）

本文记录在 Fedora 上编译、打 RPM 包所需的全部依赖，以及 OBS 包
[`home:liuminghang/fantascene-fedora`](https://build.opensuse.org/package/show/home:liuminghang/fantascene-fedora)
的更新流程。openSUSE 侧见 [`../opensuse/openSUSE-Build.md`](../opensuse/openSUSE-Build.md)。

## 一、依赖包映射（Debian → Fedora）

### 构建依赖（debian/control Build-Depends）

| Debian 包 | Fedora 包 | 说明 |
|---|---|---|
| `build-essential`, `g++` | `gcc-c++` `make` | 编译工具链 |
| `pkg-config` | （qt6-qtbase-devel 依赖自带） | |
| `qt6-base-dev` | `qt6-qtbase-devel` | Qt6 Core/Gui/Widgets/DBus/Concurrent/Sql/Network/OpenGLWidgets + `qmake6` |
| `qt6-multimedia-dev` | `qt6-qtmultimedia-devel` | 含 Multimedia 和 MultimediaWidgets |
| `qt6-charts-dev` | `qt6-qtcharts-devel` | qtHaveModule(charts) → USE_CHARTS |
| `qt6-webengine-dev` | `qt6-qtwebengine-devel` | .pro 的 webenginewidgets 回退分支适用 |
| `qt6-tools-dev-tools` | `qt6-qttools` | `lrelease-qt6` / `/usr/lib64/qt6/bin/lrelease` |
| `libmpv-dev` | `mpv-devel` | PKGCONFIG += mpv |
| `libxcb-ewmh-dev` | `xcb-util-wm-devel` | PKGCONFIG += xcb-ewmh |
| `libwayland-dev` | `wayland-devel` | PKGCONFIG += wayland-client |
| `libglib2.0-dev` | `glib2-devel` | PKGCONFIG += gio-2.0 glib-2.0 gio-unix-2.0 |
| `libmtdev-dev` | `mtdev-devel` | 与 Debian 保持一致 |
| `libx11-xcb-dev` | `libX11-devel` | X11/xcb 头文件 |
| `libxext-dev` | `libXext-devel` | PKGCONFIG += xext |
| `libxrender-dev` | `libXrender-devel` | PKGCONFIG += xrender |
| `libxcb-shape0-dev` | `libxcb-devel` | shape 扩展头文件在主 libxcb-devel 内 |
| `liblayershellqtinterface-dev` | `layer-shell-qt-devel` | KDE LayerShellQt 的 Fedora 包 |

### 运行依赖（debian/control Depends）

| Debian 包 | Fedora 处理 | 说明 |
|---|---|---|
| `wget` | `Requires: wget` | |
| `aria2` | `Requires: aria2` | |
| `ffmpeg` | **`Recommends`** | ffmpeg 不在 Fedora 官方仓库（RPM Fusion 才有），不能硬依赖 |
| （Qt6 sqlite 驱动） | `qt6-qtbase`（内置） | Fedora 的 SQLite 插件随 qt6-qtbase 发布 |
| — | `gstreamer1-plugins-good`（Recommends） | Qt Multimedia 编解码兜底 |

### 一键安装（需要 sudo）

```bash
sudo dnf install -y \
  gcc-c++ make rpm-build \
  qt6-qtbase-devel qt6-qtmultimedia-devel qt6-qtcharts-devel \
  qt6-qtwebengine-devel qt6-qttools \
  mpv-devel libxcb-devel xcb-util-wm-devel \
  libX11-devel libXext-devel libXrender-devel \
  glib2-devel wayland-devel mtdev-devel \
  layer-shell-qt-devel \
  wget aria2 qt6-qtbase mpv gstreamer1-plugins-good
```

（或直接 `fedora/build_fedora.sh deps`，效果相同。）

## 二、本地编译 / 打包

| 命令 | 作用 |
|---|---|
| `fedora/build_fedora.sh deps` | 安装上面全部依赖 |
| `fedora/build_fedora.sh build` | 仅编译：`qmake6` + `make -j`，产物在 `build/src/release/` |
| `fedora/build_fedora.sh tar` | 生成 OBS 源码包 `fantascene-dynamic-wallpaper-<版本>.tar.gz` |
| `fedora/build_fedora.sh rpm` | tar + `rpmbuild -bb`，RPM 输出到 `rpmbuild/RPMS/` |
| `fedora/build_fedora.sh all` | deps + rpm 一步到位 |

## 三、OBS 包（home:liuminghang/fantascene-fedora）

1. 上传两个文件：`fantascene-dynamic-wallpaper-<版本>.tar.gz`
   （`fedora/obs_fedora.sh` 生成）和 `fedora/fantascene-dynamic-wallpaper.spec`：
   ```bash
   cd ~/Downloads/home:liuminghang/fantascene-fedora   # osc 工作副本
   cp ~/fantascene-dynamic-wallpaper/fantascene-dynamic-wallpaper-<版本>.tar.gz .
   cp ~/fantascene-dynamic-wallpaper/fedora/fantascene-dynamic-wallpaper.spec .
   osc add <tarball 文件绝对路径>
   osc ci <包目录绝对路径> -m "Update to <版本>"
   ```
   注意：osc 对含冒号的路径用相对路径会误报 "not a working copy"，**必须用绝对路径**。
2. spec 的 `Source0` 与 openSUSE 包一致，指向 GitHub release tag tarball；
   OBS 会优先使用包内已上传的同名 tarball，tag 未同步前构建不受影响。
3. 升级版本时同步：`debian/changelog`、本 spec `Version:`、openSUSE spec `Version:`。

## 四、已做的代码改动（与 openSUSE 打包共用）

- `src/fantascene-dynamic-wallpaper.pro`：lrelease 探测链 ——
  `lrelease` → `lrelease6`（openSUSE）→ **`lrelease-qt6`**（Fedora）→
  `/usr/lib/qt6/bin/lrelease`（Debian）→ **`/usr/lib64/qt6/bin/lrelease`**（Fedora）。
- `.pro` 的 `webenginewidgets` 回退分支同样适用于 Fedora。
- `dbus_service` 补进 `INSTALLS` 的修复同样生效。
