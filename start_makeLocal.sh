sudo apt-get install -y qtcreator libx11-xcb-dev libxcb-ewmh-dev libmpv-dev mpv ffmpeg dpkg-dev fakeroot libmtdev-dev qttools5-dev-tools qtbase5-dev qt5-qmake libxext-dev libxrender-dev libqt5x11extras5-dev libglib2.0-dev build-essential g++ libxcb-shape0-dev aria2
sudo apt-get install -y qtmultimedia5-dev gstreamer1.0-qt5
sudo apt-get install -y qtwebengine5-dev
sudo apt-get install -y libqt5opengl5-dev
sudo apt-get install -y qt5-default
sudo apt-get install -y qtdeclarative5-dev-tools
sudo apt-get install -y devscripts equivs
sudo apt-get install -y qttools5-dev-tools qttools5-dev
mkdir build
cd build
qmake ..
make -j4
sudo make install
