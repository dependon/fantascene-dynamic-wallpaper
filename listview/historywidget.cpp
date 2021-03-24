#include "historywidget.h"
#include "ui_historywidget.h"
#include "view.h"
#include "application.h"

#include <QFileDialog>
historyWidget::historyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::historyWidget)
{
    ui->setupUi(this);
    m_view = new view();
    ui->verticalLayout->addWidget(m_view);
    setWindowTitle("历史壁纸");
}

historyWidget::~historyWidget()
{
    m_view->deleteLater();
    m_view = nullptr;
    delete ui;
}

void historyWidget::showEvent(QShowEvent *event)
{
//    if (m_view && m_view->m_allItemInfo.size() != dApp->m_allPath.size()) {

//    }
    return QWidget::showEvent(event);
}

void historyWidget::on_setWallPaper_clicked()
{
    if (m_view->m_allItemInfo.size() > m_view->currentIndex().row()) {
        QString path = m_view->m_allItemInfo[m_view->currentIndex().row()].path;
        dApp->setWallPaper(path);
    }

}

void historyWidget::on_delWallPaper_clicked()
{
    if (m_view->m_allItemInfo.size() > m_view->currentIndex().row()) {
        QString path = m_view->m_allItemInfo[m_view->currentIndex().row()].path;
        dApp->m_allPath.removeOne(path);
        m_view->m_allItemInfo.removeAt(m_view->currentIndex().row());
        m_view->refresh();
        emit dApp->saveSetting();
    }
}

void historyWidget::on_importBtn_clicked()
{
    QStringList list = QFileDialog::getOpenFileNames();
    for (QString path : list) {
        if (!dApp->getThumbnail(path).isNull()) {
            dApp->m_allPath.push_back(path);
            //去重
            dApp->m_allPath = dApp->m_allPath.toSet().toList();
            m_view->addPath(path);
        }
    }
}
