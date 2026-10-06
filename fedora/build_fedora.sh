#!/bin/bash
# Fedora 本地构建/打包脚本 (Fedora 40+)
# 用法:
#   fedora/build_fedora.sh deps   # 安装全部构建/运行依赖 (需要 sudo/dnf)
#   fedora/build_fedora.sh build  # 仅编译 (qmake6 + make)
#   fedora/build_fedora.sh rpm    # 生成源码 tarball 并构建 RPM 包
#   fedora/build_fedora.sh all    # deps + rpm
set -euo pipefail
# 脚本位于 fedora/ 子目录，切到项目根目录执行
cd "$(dirname "$0")/.."

VERSION=$(head -n1 debian/changelog | sed -E 's/^[^(]*\(([^-)]+).*/\1/')
NAME=fantascene-dynamic-wallpaper
echo ">>> ${NAME} version: ${VERSION}"

# Debian Build-Depends (debian/control) -> Fedora 包名映射, 详见 fedora/Fedora-Build.md
BUILD_DEPS=(
    gcc-c++ make
    qt6-qtbase-devel qt6-qtmultimedia-devel qt6-qtcharts-devel
    qt6-qtwebengine-devel qt6-qttools
    mpv-devel libxcb-devel xcb-util-wm-devel
    libX11-devel libXext-devel libXrender-devel
    glib2-devel wayland-devel mtdev-devel
    layer-shell-qt-devel
    rpm-build
)
RUNTIME_DEPS=(wget aria2 qt6-qtbase mpv gstreamer1-plugins-good)

do_deps() {
    echo ">>> 安装构建依赖..."
    sudo dnf install -y "${BUILD_DEPS[@]}" "${RUNTIME_DEPS[@]}"
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
    rpmbuild -bb --define "_sourcedir $PWD" --define "_specdir $PWD/fedora" \
        --define "_builddir $PWD/rpmbuild/BUILD" \
        --define "_rpmdir $PWD/rpmbuild/RPMS" \
        --define "_srcrpmdir $PWD/rpmbuild/SRPMS" \
        "fedora/${NAME}.spec"
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
