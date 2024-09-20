#include "historywidget.h"
#include "ui_historywidget.h"
#include "view.h"
#include "application.h"
#include "playlistsettingdialog.h"
#include "db/dbmanager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSet>
#include <QDebug>

historyWidget::historyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::historyWidget)
{
    ui->setupUi(this);
    m_viewHistory = new view(this);
    ui->verticalLayout->addWidget(m_viewHistory);
    setWindowTitle(tr("Wallpaper History"));

    connect(dApp, &Application::addPaperView, m_viewHistory, &view::addPath);
    //文件
    m_viewHistory->setFiles(dApp->m_allPath);


    m_viewPlayList = new view(this);
    ui->layoutPlaylist->addWidget(m_viewPlayList);
    connect(dApp, &Application::addplaylist, m_viewPlayList, &view::addPath);
    //文件
    m_viewPlayList->setFiles(dApp->m_playlistPath);
    ui->loopBox->setChecked(dApp->m_isPlayList);

    if(dApp->m_currentScreenNum > 1)
    {
        ui->setWallPaper2->setVisible(true);
    }
    else
    {
        ui->setWallPaper2->setVisible(false);
    }

    connect(m_viewHistory,&view::sigClickedChange,this,&historyWidget::on_clickedChange,Qt::QueuedConnection);

    ui->lbl_Name->setWordWrap(true);
    ui->lbl_Path->setWordWrap(true);
    ui->lbl_thumbPath->setWordWrap(true);

}

historyWidget::~historyWidget()
{
    delete ui;
}

void historyWidget::showEvent(QShowEvent *event)
{
    return QWidget::showEvent(event);
}

void historyWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void historyWidget::on_setWallPaper_clicked()
{
    if (m_viewHistory->m_allItemInfo.size() > m_viewHistory->currentIndex().row() && m_viewHistory->currentIndex().row() >= 0) {
        QString path = m_viewHistory->m_allItemInfo[m_viewHistory->currentIndex().row()].path;
        dApp->setWallPaper(path);
    }
    Q_EMIT dApp->saveSetting();

}

void historyWidget::on_delWallPaper_clicked()
{
    if (m_viewHistory->m_allItemInfo.size() > m_viewHistory->currentIndex().row() && m_viewHistory->currentIndex().row() >= 0) {
        QString path = m_viewHistory->m_allItemInfo[m_viewHistory->currentIndex().row()].path;
        dApp->removeLocalPaths(QStringList(path));
        m_viewHistory->m_allItemInfo.removeAt(m_viewHistory->currentIndex().row());
        m_viewHistory->refresh();
    }  else if (m_viewHistory->currentIndex().row() < 0 && m_viewHistory->m_allItemInfo.size() > 0) {
        if (QMessageBox::Yes == QMessageBox::information(nullptr, tr("Delete!!"), tr("Delete all history imports ?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
            m_viewHistory->m_allItemInfo.clear();
            dApp->clearLocalPaths();
            m_viewHistory->refresh();
        }
    }
    Q_EMIT dApp->saveSetting();
}

void historyWidget::on_importBtn_clicked()
{
    QStringList list ;
    if(dApp->m_isNoMpvPause)
    {
        Q_EMIT dApp->setMpvpause();
        dApp->m_isNoMpvPause = false;
        list = QFileDialog::getOpenFileNames();

        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
    }
    for (QString path : list) {
        if (!dApp->getThumbnail(path).isNull()) {
            dApp->addLocalPaths(QStringList(path));
            m_viewHistory->addPath(path);
        }
    }
    Q_EMIT dApp->saveSetting();
}

void historyWidget::on_addPlaylistBtn_clicked()
{
    if (m_viewHistory->m_allItemInfo.size() > m_viewHistory->currentIndex().row() && m_viewHistory->currentIndex().row() >= 0) {
        QString path = m_viewHistory->m_allItemInfo[m_viewHistory->currentIndex().row()].path;
        dApp->addPlayListaths(QStringList(path));
        dApp->m_playlistPath = dApp->convertQStringListToSet( dApp->m_allPath).values();
        m_viewPlayList->setFiles(dApp->m_playlistPath);
        m_viewPlayList->refresh();
    }
    Q_EMIT dApp->saveSetting();
}

void historyWidget::on_moreSetting_clicked()
{
    PlaylistSettingDialog a;
    a.exec();
}

void historyWidget::on_loopBox_clicked(bool checked)
{
    dApp->setisPlayList(checked);
    Q_EMIT dApp->saveSetting();
}

void historyWidget::on_deletePlaylist_clicked()
{
    if (m_viewPlayList->m_allItemInfo.size() > m_viewPlayList->currentIndex().row() && m_viewPlayList->currentIndex().row() >= 0) {
        QString path = m_viewPlayList->m_allItemInfo[m_viewPlayList->currentIndex().row()].path;
        dApp->removePlayListPaths(QStringList(path));
        m_viewPlayList->m_allItemInfo.removeAt(m_viewPlayList->currentIndex().row());
        m_viewPlayList->refresh();
    } else if (m_viewPlayList->currentIndex().row() < 0 && m_viewPlayList->m_allItemInfo.size() > 0) {
        if (QMessageBox::Yes == QMessageBox::information(nullptr, tr("Delete!!"), tr("Delete all playback ?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
            m_viewPlayList->m_allItemInfo.clear();
            dApp->clearPlayListPaths();
            m_viewPlayList->refresh();
        }
    }
    Q_EMIT dApp->saveSetting();
}

void historyWidget::on_setWallPaper2_clicked()
{
    if (m_viewHistory->m_allItemInfo.size() > m_viewHistory->currentIndex().row() && m_viewHistory->currentIndex().row() >= 0) {
        QString path = m_viewHistory->m_allItemInfo[m_viewHistory->currentIndex().row()].path;
        dApp->setWallPaper2(path);
    }
    Q_EMIT dApp->saveSetting();
}

void historyWidget::on_clickedChange(const QString &path)
{
    WallpaperData wallpaperData = DBManager::instance()->getData(path);
    QString thumbnail = dApp->getThumbnailPath(path);
    ui->lbl_Name->setText(wallpaperData.name);
    ui->lbl_Path->setText(wallpaperData.path);
    ui->lbl_thumbPath->setText(thumbnail);
    ui->lbl_thumb->setPixmap(dApp->getThumbnail(path));
}

void historyWidget::on_btn_reload_clicked()
{
    QString path = ui->lbl_Path->text();
    QPixmap pix = dApp->reloadThumbnail(path);
    for(int i =0 ; i< m_viewHistory->m_allItemInfo.size();i++)
    {
        if(m_viewHistory->m_allItemInfo.at(i).path == path)
        {
            m_viewHistory->m_allItemInfo[i].image = pix;
        }
    }
    m_viewHistory->refresh();
    ui->lbl_thumb->setPixmap(pix);
}


void historyWidget::on_btn_select_clicked()
{
    QString str;
    if(dApp->m_isNoMpvPause)
    {
        Q_EMIT dApp->setMpvpause();
        dApp->m_isNoMpvPause = false;
        str = QFileDialog::getOpenFileName();

        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
    }
    else
    {
        str = QFileDialog::getOpenFileName();
    }
    if(str.length() < 0)
    {
        return ;
    }
    QString path = ui->lbl_Path->text();
    QPixmap pix = dApp->setNewThumbnail(path,str);
    for(int i =0 ; i< m_viewHistory->m_allItemInfo.size();i++)
    {
        if(m_viewHistory->m_allItemInfo.at(i).path == path)
        {
            m_viewHistory->m_allItemInfo[i].image = pix;
        }
    }
    m_viewHistory->refresh();
    ui->lbl_thumb->setPixmap(pix);

}

