#include "application.h"
#include <QIcon>

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    this->setApplicationName(tr("deepin-dreamscene-ui"));
    this->setApplicationDisplayName(tr("deepin-dreamscene-ui"));
    this->setApplicationDescription(tr("deepin-dreamscene-ui\n"
                                       "基于qt5.11.3版本\n"
                                       "基于justforlxz的渲染方法开发\n"
                                       "github地址:https://github.com/dependon/deepin-dreamscene-ui/\n"
                                       "gitee地址:https://gitee.com/liuminghang/deepin-dreamscene-ui\n"
                                       "bug可以联系mail: liuminghang0821@gmail.com\n"
                                       "nvdia闭源驱动电脑下某些情况需要自定义模式配置尺寸"));
    this->setProductIcon(QIcon(":/tray.ico"));

}

Application::~Application()
{
    emit quitApp();
}
