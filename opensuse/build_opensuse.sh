#!/bin/bash
# openSUSE 本地构建/打包脚本 (Tumbleweed / Leap 16+)
# 用法 (在项目根目录或任意位置):
#   opensuse/build_opensuse.sh deps   # 安装全部构建/运行依赖 (需要 sudo)
#   opensuse/build_opensuse.sh build  # 仅编译 (qmake6 + make)
#   opensuse/build_opensuse.sh rpm    # 生成源码 tarball 并构建 RPM 包
#   opensuse/build_opensuse.sh all    # deps + rpm
set -euo pipefail
# 脚本位于 opensuse/ 子目录，切到项目根目录执行
cd "$(dirname "$0")/.."

VERSION=$(dpkg-parsechangelog 2>/dev/null | awk '/^Version:/{print $2; exit}') || VERSION=""
if [ -z "$VERSION" ]; then
    # 无 dpkg 环境时从 debian/changelog 第一行提取
    VERSION=$(head -n1 debian/changelog | sed -E 's/^[^(]*\(([^-)]+).*/\1/')
fi
NAME=fantascene-dynamic-wallpaper
echo ">>> ${NAME} version: ${VERSION}"

# Debian Build-Depends (debian/control) -> openSUSE 包名映射, 详见 opensuse/openSUSE-Build.md
BUILD_DEPS=(
    gcc-c++ make pkgconf-pkg-config
    qt6-base-devel qt6-multimedia-devel qt6-multimediawidgets-devel qt6-charts-devel
    qt6-webenginecore-devel qt6-webenginewidgets-devel
    qt6-tools qt6-macros
    mpv-devel xcb-util-wm-devel wayland-devel glib2-devel mtdev-devel
    libX11-devel libXext-devel libXrender-devel libxcb-devel
    layer-shell-qt6-devel
    rpm-build
)
RUNTIME_DEPS=(wget ffmpeg aria2 qt6-sql-sqlite mpv gstreamer-plugins-good)

do_deps() {
    echo ">>> 安装构建依赖..."
    sudo zypper install -y "${BUILD_DEPS[@]}" "${RUNTIME_DEPS[@]}"
}

do_build() {
    echo ">>> 编译..."
    mkdir -p build
    (
        cd build
        qmake6 "DEFINES+=VERSION=${VERSION}" ..
        make -j"$(nproc)"
    )
    echo ">>> 编译完成: build/src/release/${NAME}"
}

do_tarball() {
    echo ">>> 生成 OBS/RPM 源码包 ${NAME}-${VERSION}.tar.gz ..."
    # 先写到 /tmp 避免输出文件改变源目录导致 tar 报 "file changed as we read it"
    tar -czf "/tmp/${NAME}-${VERSION}.tar.gz" \
        --exclude='.git' --exclude='build' --exclude='rpmbuild' --exclude='*.tar.gz' \
        --transform "s,^\.,${NAME}-${VERSION}," .
    mv "/tmp/${NAME}-${VERSION}.tar.gz" "${NAME}-${VERSION}.tar.gz"
}

do_rpm() {
    do_tarball
    echo ">>> 构建 RPM..."
    rpmbuild -bb --define "_sourcedir $PWD" --define "_specdir $PWD/opensuse" \
        --define "_builddir $PWD/rpmbuild/BUILD" \
        --define "_rpmdir $PWD/rpmbuild/RPMS" \
        --define "_srcrpmdir $PWD/rpmbuild/SRPMS" \
        "opensuse/${NAME}.spec"
    echo ">>> RPM 输出: rpmbuild/RPMS/"
}

case "${1:-build}" in
    deps)  do_deps ;;
    build) do_build ;;
    tar)   do_tarball ;;
    rpm)   do_rpm ;;
    all)   do_deps; do_rpm ;;
    *)     echo "用法: $0 {deps|build|tar|rpm|all}"; exit 1 ;;
esac
