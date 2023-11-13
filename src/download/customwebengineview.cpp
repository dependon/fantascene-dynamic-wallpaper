#include "customwebengineview.h"
#ifdef USE_WEBENGINE
CustomWebEngineView::CustomWebEngineView(QWidget *parent)
    : QWebEngineView(parent)
{

}

QWebEngineView *CustomWebEngineView::createWindow(QWebEnginePage::WebWindowType _type)
{
    return QWebEngineView::createWindow(_type);
//    return this;
//    // 创建新的QWebEngineView实例并在新窗口中打开链接
//    CustomWebEngineView* newWebView = new CustomWebEngineView();
//    newWebView->resize(this->size());
//    return newWebView;
}
#endif
