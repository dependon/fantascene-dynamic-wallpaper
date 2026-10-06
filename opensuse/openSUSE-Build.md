# openSUSE 构建 / 打包指南（fantascene-dynamic-wallpaper）

本文记录在 openSUSE（Tumbleweed / Leap 16+）上编译、打 RPM 包所需的全部依赖，
以及为 OBS（openSUSE Build Service）打包做的准备。Debian 侧的打包参照 `debian/control`。

## 一、依赖包映射（Debian → openSUSE）

### 构建依赖（debian/control Build-Depends）

| Debian 包 | openSUSE 包 | 说明 |
|---|---|---|
| `build-essential`, `g++` | `gcc-c++` `make` | 编译工具链 |
| `pkg-config` | `pkgconf-pkg-config` | pkg-config |
| `qt6-base-dev` | `qt6-base-devel` | Qt6 Core/Gui/Widgets/DBus/Concurrent/Sql/Network/OpenGLWidgets；`qmake6` 由其依赖 `qt6-macros` 提供 |
| `qt6-multimedia-dev` | `qt6-multimedia-devel` + `qt6-multimediawidgets-devel` | **注意**：openSUSE 把 MultimediaWidgets 拆为独立包，缺了会报 `Unknown module(s) in QT: multimediawidgets` |
| `qt6-charts-dev` | `qt6-charts-devel` | qtHaveModule(charts) → USE_CHARTS |
| `qt6-webengine-dev` | `qt6-webenginecore-devel` + `qt6-webenginewidgets-devel` + `qt6-webenginequick-devel` | **注意**：openSUSE 没有 `qt6-webengine-devel`，按模块拆分；程序只用 widgets，`.pro` 已加 `webenginewidgets` 回退分支，quick-devel 可选 |
| `qt6-tools-dev-tools` | `qt6-tools` | 提供 `lrelease6`（openSUSE 的 Qt 工具带 `6` 后缀） |
| — | `qt6-macros` | 提供 qmake6（通常被 qt6-base-devel 拉入，显式声明更稳） |
| `libmpv-dev` | `mpv-devel` | PKGCONFIG += mpv |
| `libxcb-ewmh-dev` | `xcb-util-wm-devel` | PKGCONFIG += xcb-ewmh |
| `libwayland-dev` | `wayland-devel` | PKGCONFIG += wayland-client |
| `libglib2.0-dev` | `glib2-devel` | PKGCONFIG += gio-2.0 glib-2.0 gio-unix-2.0 |
| `libmtdev-dev` | `mtdev-devel` | 与 Debian 保持一致 |
| `libx11-xcb-dev` | `libX11-devel` | X11/xcb 头文件包含在 libX11-devel |
| `libxext-dev` | `libXext-devel` | PKGCONFIG += xext |
| `libxrender-dev` | `libXrender-devel` | PKGCONFIG += xrender |
| `libxcb-shape0-dev` | `libxcb-devel` | shape 扩展头文件在主 libxcb-devel 内 |
| `liblayershellqtinterface-dev` | `layer-shell-qt6-devel` | LayerShellQt 探测：`/usr/include/LayerShellQt/Window` |
| `fakeroot`, `debhelper` | （不需要）/ `rpm-build` | RPM 打包不需要 fakeroot |

### 运行依赖（debian/control Depends）

| Debian 包 | openSUSE 包 | 说明 |
|---|---|---|
| `wget` | `wget` | 同名 |
| `ffmpeg` | `ffmpeg` | 同名 |
| `aria2` | `aria2` | 同名 |
| `${shlibs:Depends}`（libmpv2 等） | `mpv`（Recommends） | libmpv2 由链接自动拉入 |
| （Qt6 sqlite 驱动） | `qt6-sql-sqlite` | 程序用 Qt SQL 存播放列表/历史 |
| — | `gstreamer-plugins-good` / `gstreamer-plugins-libav`（Recommends） | Qt Multimedia 编解码兜底 |

### 一键安装（需要 sudo）

```bash
sudo zypper install -y \
  gcc-c++ make pkgconf-pkg-config rpm-build \
  qt6-base-devel qt6-multimedia-devel qt6-charts-devel \
  qt6-webenginecore-devel qt6-webenginewidgets-devel \
  qt6-tools qt6-macros \
  mpv-devel xcb-util-wm-devel wayland-devel glib2-devel mtdev-devel \
  libX11-devel libXext-devel libXrender-devel libxcb-devel \
  layer-shell-qt6-devel \
  wget ffmpeg aria2 qt6-sql-sqlite mpv gstreamer-plugins-good
```

（或直接 `opensuse/build_opensuse.sh deps`，效果相同。）

## 二、本地编译 / 打包

仓库内脚本（对应 Debian 侧的 `start_makeLocal.sh` / `start_deb.sh`）：

| 命令 | 作用 |
|---|---|
| `opensuse/build_opensuse.sh deps` | 安装上面全部依赖 |
| `opensuse/build_opensuse.sh build` | 仅编译：`qmake6` + `make -j`，产物在 `build/src/release/` |
| `opensuse/build_opensuse.sh tar` | 生成 OBS 源码包 `fantascene-dynamic-wallpaper-<版本>.tar.gz` |
| `opensuse/build_opensuse.sh rpm` | tar + `rpmbuild -bb`，RPM 输出到 `rpmbuild/RPMS/` |
| `opensuse/build_opensuse.sh all` | deps + rpm 一步到位 |

版本号自动取自 `debian/changelog` 第一行（当前 2.1.6），并以
`DEFINES+=VERSION=<版本>` 传给 qmake，与 `debian/rules` 的行为一致。

打包配置由 `src/fantascene-dynamic-wallpaper.pro` 的 `INSTALLS` 完成：
二进制 → `/usr/bin`，desktop 文件 → `/usr/share/applications`，
翻译 → `/usr/share/fantascene-dynamic-wallpaper/translations`，
dbus service → `/usr/share/dbus-1/services`，示例视频/图 → `/usr/share/fantascene-dynamic-wallpaper/normal`。

## 三、OBS 打包准备

1. **文件**：OBS 上只需要两个文件 —— `fantascene-dynamic-wallpaper-<版本>.tar.gz`
   （`opensuse/obs_opensuse.sh` 或 `opensuse/build_opensuse.sh tar` 生成）和
   [`fantascene-dynamic-wallpaper.spec`](fantascene-dynamic-wallpaper.spec)。
2. **上传**：
   ```bash
   osc init home:<你的用户名>/fantascene-dynamic-wallpaper
   cp fantascene-dynamic-wallpaper-2.1.6.tar.gz <该目录>/
   cp opensuse/fantascene-dynamic-wallpaper.spec <该目录>/
   osc addremove && osc ci
   ```
3. **spec 要点**（已处理好的坑）：
   - WebEngine devel 按模块拆分，用 `qt6-webenginecore-devel` + `qt6-webenginewidgets-devel`；
   - `lrelease` 在 openSUSE 叫 `lrelease6`（`.pro` 已加探测分支，见下）；
   - OBS chroot 可能没有 UTF-8 locale，`%build`/`%install` 里已 `export LC_ALL=C.UTF-8`
     （与 `debian/rules` 相同的处理）；
   - `Version:` 与 `debian/changelog` 保持同步，升级版本时两处都要改。

## 四、已做的代码改动

- `src/fantascene-dynamic-wallpaper.pro`：lrelease 探测增加第二步 ——
  `lrelease` → **`lrelease6`**（openSUSE 命名）→ `/usr/lib/qt6/bin/lrelease`（Debian Qt6 路径）。
  此改动同样适用于其他发行版，不影响 Debian 打包。
- `src/fantascene-dynamic-wallpaper.pro`：`qtHaveModule(webengine)` 失败时回退检测
  `webenginewidgets`（openSUSE 的 Qt6 打包不含 `webengine` 模块名；程序实际只用
  `QWebEngineView` 等 widgets 类，不依赖 QML 模块）。Debian 行为不变。
- `src/fantascene-dynamic-wallpaper.pro`：修复 `dbus_service` 定义了但没加进
  `INSTALLS` 的 bug —— 此前 dbus service 文件（`com.deepin.dde.fantascene.service`）
  在 deb 和 rpm 里都不会被安装，现在两边的包都包含它。
