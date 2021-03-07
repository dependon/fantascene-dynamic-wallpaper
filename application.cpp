#include "application.h"

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
//this->set
    this->setApplicationName(tr("deepin-dreamscene-ui"));
    this->setApplicationDisplayName(tr("deepin-dreamscene-ui"));
    this->setApplicationDescription(tr("deepin-dreamscene-ui\n"
                                       "基于qt5.11.3版本\n"
                                       "基于justforlxz的渲染方法开发\n"
                                       "github地址:https://github.com/dependon/deepin-dreamscene-ui/\n"
                                       "mail: liuminghang0821@gmail.com"));
}

Application::~Application()
{
}
