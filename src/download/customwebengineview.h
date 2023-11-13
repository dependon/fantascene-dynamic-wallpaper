#ifndef CUSTOMWEBENGINEVIEW_H
#define CUSTOMWEBENGINEVIEW_H
#ifdef USE_WEBENGINE
#include <QWebEngineView>
#include <QWebEnginePage>
class CustomWebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    CustomWebEngineView(QWidget* parent = nullptr);

protected:
    QWebEngineView* createWindow(QWebEnginePage::WebWindowType _type) override;
};
#endif
#endif // CUSTOMWEBENGINEVIEW_H
