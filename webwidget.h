#ifndef WEBWIDGET_H
#define WEBWIDGET_H

#include <QWebEngineView>
class webWidget : public QWebEngineView
{
    Q_OBJECT
public:
    webWidget(QWidget *parent = Q_NULLPTR);

};

#endif // WEBWIDGET_H
