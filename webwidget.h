#ifndef WEBWIDGET_H
#define WEBWIDGET_H

#include <QWebEngineView>
class webWidget : public QWebEngineView
{
    Q_OBJECT
public:
    webWidget(QWidget *parent = Q_NULLPTR);
protected:
    bool event(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

};

#endif // WEBWIDGET_H
