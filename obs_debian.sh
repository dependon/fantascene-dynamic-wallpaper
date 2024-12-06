git clone --depth=1 https://gitee.com/liuminghang/fantascene-dynamic-wallpaper.git
tar -czvf fantascene-dynamic-wallpaper_1.9.8.orig.tar.gz ./fantascene-dynamic-wallpaper/
cd fantascene-dynamic-wallpaper/
tar -czvf ../fantascene-dynamic-wallpaper_1.9.8.debian.tar.gz ./debian
cd ..
md5sum fantascene-dynamic-wallpaper_1.9.8.orig.tar.gz
md5sum fantascene-dynamic-wallpaper_1.9.8.debian.tar.gz
ls -l
