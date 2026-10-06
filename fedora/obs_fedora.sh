#!/bin/bash
# 为 OBS (home:liuminghang/fantascene-fedora) 准备源码包的脚本
# 产物: fantascene-dynamic-wallpaper-<version>.tar.gz + spec 文件
set -euo pipefail
cd "$(dirname "$0")/.."

VERSION=$(head -n1 debian/changelog | sed -E 's/^[^(]*\(([^-)]+).*/\1/')
NAME=fantascene-dynamic-wallpaper

echo ">>> 版本: ${VERSION}"
rm -f "${NAME}-${VERSION}.tar.gz"

# 先写到 /tmp 避免输出文件改变源目录导致 tar 报 "file changed as we read it"
tar -czf "/tmp/${NAME}-${VERSION}.tar.gz" \
    --exclude='.git' --exclude='build' --exclude='rpmbuild' \
    --exclude='*.tar.gz' --exclude='.qmake.stash' \
    --transform "s,^\.,${NAME}-${VERSION}," .
mv "/tmp/${NAME}-${VERSION}.tar.gz" "${NAME}-${VERSION}.tar.gz"

echo ">>> 完成。上传到 OBS 需要的文件:"
echo "    ${NAME}-${VERSION}.tar.gz  (项目根目录)"
echo "    fedora/${NAME}.spec"
