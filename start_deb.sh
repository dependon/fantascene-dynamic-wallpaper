sudo apt install qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev qttools5-dev-tools qtbase5-dev qt5-qmake libxext-dev libxrender-dev libqt5x11extras5-dev libglib2.0-dev build-essential g++ libxcb-shape0-dev
sudo apt install qtwebengine5-dev
sudo apt install libqt5opengl5-dev
sudo apt install qt5-default
sudo apt install qtdeclarative5-dev-tools
dpkg-buildpackage -b -us -uc
