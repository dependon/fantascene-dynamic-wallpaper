git clone --depth=1 https://gitee.com/liuminghang/fantascene-dynamic-wallpaper.git
tar -czvf fantascene-dynamic-wallpaper_1.5.8.orig.tar.gz ./fantascene-dynamic-wallpaper/
tar -czvf fantascene-dynamic-wallpaper_1.5.8.debian.tar.gz ./fantascene-dynamic-wallpaper/debian
cd fantascene-dynamic-wallpaper/
tar -czvf ../fantascene-dynamic-wallpaper_1.5.8.debian.tar.gz ./debian
cd ..