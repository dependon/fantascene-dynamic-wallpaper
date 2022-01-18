#include "historywidget.h"
#include "ui_historywidget.h"
#include "view.h"
#include "application.h"

#include <QFileDialog>
#include <QMessageBox>

historyWidget::historyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::historyWidget)
{
    ui->setupUi(this);
    m_view = new view();
    ui->verticalLayout->addWidget(m_view);
    setWindowTitle(tr("Wallpaper History"));

    connect(dApp, &Application::addPaperView, m_view, &view::addPath);
    //文件
    m_view->setFiles(dApp->m_allPath);

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
    if (m_view->m_allItemInfo.size() > m_view->currentIndex().row() && m_view->currentIndex().row() >= 0) {
        QString path = m_view->m_allItemInfo[m_view->currentIndex().row()].path;
        dApp->setWallPaper(path);
    }
    emit dApp->saveSetting();

}

void historyWidget::on_delWallPaper_clicked()
{
    if (m_view->m_allItemInfo.size() > m_view->currentIndex().row() && m_view->currentIndex().row() >= 0) {
        QString path = m_view->m_allItemInfo[m_view->currentIndex().row()].path;
        dApp->m_allPath.removeOne(path);
        m_view->m_allItemInfo.removeAt(m_view->currentIndex().row());
        m_view->refresh();
    } else if (m_view->currentIndex().row() < 0 && m_view->m_allItemInfo.size() > 0) {
        if (QMessageBox::Yes == QMessageBox::information(NULL, "删除!!", "是否删除所有历史导入", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
            m_view->m_allItemInfo.clear();
            dApp->m_allPath.clear();
            m_view->refresh();
        }
    }
    emit dApp->saveSetting();
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
    emit dApp->saveSetting();
}
