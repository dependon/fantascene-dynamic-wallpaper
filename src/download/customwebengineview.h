#ifndef CUSTOMWEBENGINEVIEW_H
#define CUSTOMWEBENGINEVIEW_H

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

#endif // CUSTOMWEBENGINEVIEW_H
