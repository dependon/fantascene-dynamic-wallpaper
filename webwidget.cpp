#include "webwidget.h"
#include <QEvent>
#include <QMouseEvent>
webWidget::webWidget(QWidget *parent)
    : QWebEngineView(parent)
{

}

bool webWidget::event(QEvent *event)
{
    qDebug() << event->type();
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    qDebug() << mouseEvent->pos();

}

void webWidget::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "mouseMoveEvent";
    QWebEngineView::mouseMoveEvent(event);
}

void webWidget::showEvent(QShowEvent *event)
{
    qDebug() << "showEvent";
    QWebEngineView::showEvent(event);
}

void webWidget::hideEvent(QHideEvent *event)
{
    qDebug() << "hideEvent";
    QWebEngineView::hideEvent(event);
}

void webWidget::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "dragEnterEvent";
    QWebEngineView::dragEnterEvent(event);
}

void webWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    qDebug() << "dragLeaveEvent";
    QWebEngineView::dragLeaveEvent(event);
}

void webWidget::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << "dragMoveEvent";
    QWebEngineView::dragMoveEvent(event);
}

void webWidget::dropEvent(QDropEvent *event)
{
    qDebug() << "dropEvent";
    QWebEngineView::dropEvent(event);
}
