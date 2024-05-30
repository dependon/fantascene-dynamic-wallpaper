/*
 * desktop-file-memos: A desktop classification app on Linux.
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Yue Lan<lanyue@kylinos.cn>
 *
 */
//#define protected public

#include "iconview.h"
#include "filemodel.h"
#include "iconprovider.h"
#include "fileoperationjob.h"
#include "delegateicon.h"
#include "customsortfilterproxymodel.h"

#include <QFileSystemModel>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QDesktopServices>
#include <QUrl>
#include <QMimeDatabase>
#include <QSettings>
#include <QModelIndex>
#include <QFileInfo>
#include <QMutexLocker>

//#include <gio/gdesktopappinfo.h>

#include <QMenu>
#include <QAction>

#include <QApplication>
#include <QClipboard>

#include <QMimeData>

#include <QThread>

#include <QDebug>
#include <QLineEdit>
#include <QKeyEvent>
#include <QProcess>
#include "application.h"
#include "inimanager.h"
#include "gioclass.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFileSystemWatcher>
#include <QTimer>

#define ICONSIZE_SMALL 90
#define ICONSIZE_MEDIUM 140
#define ICONSIZE_BIG 180

QString IconView::readSettings(const QString &path, const QString &group, const QString &key)
{
    QSettings settings(path, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup(group);
    QString value = settings.value(key).toString();
    return value;
}

IconView::IconView(int id, QString rootPath, QWidget *parent)
    : QListView(parent)
{
//    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setFrameShape(QListView::NoFrame);
    setStyleSheet("background-color:transparent");
    //setMouseTracking(true);
    setCursor(Qt::ArrowCursor);
    this->viewport()->setStyleSheet("background-color:transparent");

    setStyleSheet("color: white;"
                  "font-size:16px;"
                  "background-color: transparent;"
                  //"border: 1px black;"
                  //"border-radius: 10px;"
                  /*"padding: 2px 4px;"*/);

    QScrollBar *scrollbar = new QScrollBar;
    scrollbar->setStyleSheet("QScrollBar"
                             "{"
                             "background-color:rgba(0,0,0,0.4);"
                             "width:20px;"
                             "border-left:0px;border-right:0px;"
                             "padding-top:20px;"
                             "padding-bottom:20px;"
                             "}"
                             "QScrollBar::add-line:vertical"
                             "{"
                             "height:20px;width:20px;"
                             "subcontrol-position:bottom;"
                             "}"
                             "QScrollBar::sub-line:vertical"
                             "{"
                             "height:20px;width:20px;"
                             "subcontrol-position:top;"
                             "}"
                             "QScrollBar::handle:vertical"
                             "{"
                             "width:20px;"
                             "min-height:50px;"
                             "}");

    setVerticalScrollBar(scrollbar);

    setFlow(QListView::TopToBottom);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //QIcon::setThemeName("ukui-icon-theme");

    mId = id;

    fileModel = new FileModel;
    m_rootIndex = fileModel->setRootPath(rootPath);

    m_proxyModel = new CustomSortFilterProxyModel;
    m_proxyModel->setSourceModel(fileModel);
    m_proxyModel->sort(0,Qt::AscendingOrder);

//    setModel(fileModel);
//    setRootIndex(fileModel->index(rootPath));

//    DelegateIcon *deleget = new DelegateIcon;
//    deleget->setFileModel(fileModel);
    setModel(m_proxyModel);
    setRootIndex(m_proxyModel->mapFromSource(fileModel->index(rootPath)));

    DelegateIcon *deleget = new DelegateIcon;
    deleget->setFileModel(fileModel);
    deleget->setPath(rootPath);
    deleget->setIconView(this);
    setItemDelegate(deleget);

    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setDragEnabled(true);
    setDropIndicatorShown(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::InternalMove);

    setDefaultDropAction(Qt::MoveAction);

    setSelectionRectVisible(true);
    setMovement(QListView::Snap); //gird move
    setLayoutMode(QListView::Batched);
    setResizeMode(QListView::Adjust); //auto redo layout
    setGridSize(QSize(ICONSIZE_SMALL, ICONSIZE_SMALL));
    setIconSize(QSize(ICONSIZE_SMALL/2, ICONSIZE_SMALL/2));
    m_currentSize = ICONSIZE_SMALL;
    setUniformItemSizes(true); //same size for all items
    setSpacing(4);
    setViewMode(QListView::IconMode);
    selectionModel()->selectedIndexes();
    //setWordWrap(true);


    setTextElideMode(Qt::ElideMiddle);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    QMenu *viewMenu = new QMenu();

    viewMenu->setWindowFlags(Qt::FramelessWindowHint);
    viewMenu->setAttribute(Qt::WA_TranslucentBackground);

    QAction *openAction = new QAction(viewMenu);
    openAction->setText(tr("Open"));
    viewMenu->addAction(openAction);

    m_openSelect = new QMenu(viewMenu);
    m_openSelect->setTitle(tr("Open With"));
    viewMenu->addMenu(m_openSelect);

    m_createNew = new QMenu(viewMenu);
    m_createNew->setTitle(tr("New Built"));
    viewMenu->addMenu(m_createNew);

    //TODO: add open with...
    QAction *selectAllAction = new QAction(viewMenu);
    selectAllAction->setText(tr("Select all"));
    selectAllAction->setShortcut(QKeySequence("Ctrl+A"));
    viewMenu->addAction(selectAllAction);

    QAction *terminalAction = new QAction(viewMenu);
    terminalAction->setText(tr("Open Terminal"));
    terminalAction->setShortcut(QKeySequence("Ctrl+Alt+T"));
    viewMenu->addAction(terminalAction);


    QAction *newFolderAction = new QAction(viewMenu);
    newFolderAction->setText(tr("New Folder"));
    newFolderAction->setShortcut(QKeySequence("Ctrl+Shift+A"));
    viewMenu->addAction(newFolderAction);

    QMenu *iconSortMenu = new QMenu(viewMenu);
    iconSortMenu->setTitle(tr("Sort Order"));
    viewMenu->addMenu(iconSortMenu);

    iconRefreash = new QAction(viewMenu);
    iconRefreash->setText(tr("Refresh Sort"));
    viewMenu->addAction(iconRefreash);

    QAction *sortName = new QAction(iconSortMenu);
    sortName->setText(tr("Name"));
    iconSortMenu->addAction(sortName);

    QAction *sortTime= new QAction(iconSortMenu);
    sortTime->setText(tr("Change Date"));
    iconSortMenu->addAction(sortTime);

    QAction *sortFileSize= new QAction(iconSortMenu);
    sortFileSize->setText(tr("File Size"));
    iconSortMenu->addAction(sortFileSize);

    QAction *sortFileType= new QAction(iconSortMenu);
    sortFileType->setText(tr("File Type"));
    iconSortMenu->addAction(sortFileType);

    QMenu *iconSizeMenu = new QMenu(viewMenu);
    iconSizeMenu->setTitle(tr("Icon Size"));
    viewMenu->addMenu(iconSizeMenu);

    iconSmall = new QAction(iconSizeMenu);
    iconSmall->setText(tr("Small"));
    iconSizeMenu->addAction(iconSmall);

    iconMedium = new QAction(iconSizeMenu);
    iconMedium->setText(tr("Medium"));
    iconSizeMenu->addAction(iconMedium);

    iconBig = new QAction(iconSizeMenu);
    iconBig->setText(tr("Big"));
    iconSizeMenu->addAction(iconBig);

    viewMenu->addSeparator();

    QAction *copyAction = new QAction(viewMenu);
    copyAction->setText(tr("Copy"));
    copyAction->setShortcut(QKeySequence("Ctrl+C"));
    viewMenu->addAction(copyAction);

    QAction *cutAction = new QAction(viewMenu);
    cutAction->setText(tr("Cut"));
    cutAction->setShortcut(QKeySequence("Ctrl+X"));
    viewMenu->addAction(cutAction);

    QAction *pasteAction = new QAction(viewMenu);
    pasteAction->setText(tr("Paste"));
    pasteAction->setShortcut(QKeySequence("Ctrl+V"));
    viewMenu->addAction(pasteAction);

    QAction *renameAction = new QAction(viewMenu);
    renameAction->setText(tr("Rename"));
    renameAction->setShortcut(QKeySequence("F2"));
    viewMenu->addAction(renameAction);

    QAction *trashAction = new QAction(viewMenu);
    trashAction->setText(tr("Trash"));
    trashAction->setShortcut(QKeySequence("Delete"));
    viewMenu->addAction(trashAction);

    QAction *wallpaperAction = new QAction(viewMenu);
    wallpaperAction->setText(tr("Set Wallpaper"));
    wallpaperAction->setShortcut(QKeySequence("F3"));
    viewMenu->addAction(wallpaperAction);


    //TODO: add show propertries action
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &IconView::customContextMenuRequested, [ = ]() {
        m_openSelect->clear();
        m_createNew->clear();

        QAction *newTXTction = new QAction(m_createNew);
        newTXTction->setText(tr("New TXT"));
        newTXTction->setShortcut(QKeySequence("Ctrl+Shift+B"));
        m_createNew->addAction(newTXTction);
        connect(newTXTction, &QAction::triggered, this, &IconView::slotsnewTxt);

        if (!QApplication::clipboard()->mimeData()->hasUrls()) {
            pasteAction->setEnabled(false);
        } else {
            pasteAction->setEnabled(true);
        }
        if (!indexAt(mLastPos).isValid()) {
            clearSelection();
            openAction->setEnabled(false);
            trashAction->setEnabled(false);
            m_openSelect->setEnabled(false);
            copyAction->setEnabled(false);
            cutAction->setEnabled(false);
            renameAction->setEnabled(false);
            m_createNew->setEnabled(true);
            //新建
            {
                QString path = QDir::homePath()+"/.Templates";
                QDir directory(path);
                QStringList files = directory.entryList(QDir::Files);
                for(QString file : files)
                {
                    QString usePath = path +"/"+file;

                    QIcon icon = GioClass::getIcon(usePath);
                    QAction * action =new QAction(icon,file,nullptr);
                    action->setData(usePath);
                    m_createNew->addAction(action);
                    connect(action, &QAction::triggered, this, &IconView::onNewActionTriggered);
                }
            }
        } else {
            openAction->setEnabled(true);
            trashAction->setEnabled(true);
            m_openSelect->setEnabled(true);
            copyAction->setEnabled(true);
            cutAction->setEnabled(true);
            renameAction->setEnabled(false);
            m_createNew->setEnabled(false);
            if (selectedIndexes().count() == 1) {
                QString fileName = selectedIndexes().first().data().toString();
                if (!fileName.endsWith(".desktop")) {
                    renameAction->setEnabled(true);
                }

                QModelIndexList indexes = selectedIndexes();
                QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
                for (int i = 0, imax = indexes.count(); i < imax; ++i) {
                    if (model) {
                        QModelIndex sourceIndex = m_proxyModel->mapToSource(indexes[i]);
                        QString fileName = model->filePath(sourceIndex);
                        QString MIME = QMimeDatabase().mimeTypeForFile(fileName).name();

                        QStringList canUse = GioClass::getCanUseApps(MIME);
                        for(QString useFile : canUse)
                        {
                            QIcon icon = GioClass::getIcon(useFile);
                            QString name = GioClass::getDesktop2Name(useFile);
                            QAction * action =new QAction(icon,name,nullptr);
                            action->setData(useFile);
                            m_openSelect->addAction(action);
                            connect(action, &QAction::triggered, this, &IconView::onActionTriggered);
                        }

                        //other
                        QAction * action =new QAction(tr("Select Other Application"),nullptr);
                        m_openSelect->addAction(action);
                        connect(action, &QAction::triggered, this, &IconView::onSetOhterActionTriggered);
                    }
                }
            }
        }
        viewMenu->exec(QCursor::pos());
    });

    connect(openAction, &QAction::triggered, this, &IconView::openFile);
    connect(selectAllAction, &QAction::triggered,  this, &IconView::selectAll);
    connect(copyAction, &QAction::triggered, this, &IconView::copyFile);
    connect(cutAction, &QAction::triggered, this, &IconView::cutFile);
    connect(pasteAction, &QAction::triggered, this, &IconView::pauseFile);
    connect(renameAction, &QAction::triggered, this, &IconView::renameFile);
    connect(trashAction, &QAction::triggered, this, &IconView::deleteFile);
    connect(newFolderAction, &QAction::triggered, this, &IconView::slotsnewFolder);

    connect(terminalAction, &QAction::triggered, this, &IconView::slotsopenTerminal);
    connect(iconSmall, &QAction::triggered, this, &IconView::slotsIconSizeSmall);
    connect(iconMedium, &QAction::triggered, this, &IconView::slotsIconSizeMedium);
    connect(iconBig, &QAction::triggered, this, &IconView::slotsIconSizeBig);

    connect(wallpaperAction, &QAction::triggered, this, &IconView::slotsWallpaperAction);
    connect(this, &IconView::doubleClicked, this, &IconView::openFile);

    sortName->setData(Qt::DisplayRole);
    sortTime->setData(Qt::UserRole + 1);
    sortFileSize->setData(Qt::UserRole + 2);
    sortFileType->setData(Qt::UserRole + 3);

    connect(sortName, &QAction::triggered, this, &IconView::slotsSort);
    connect(sortTime, &QAction::triggered, this, &IconView::slotsSort);
    connect(sortFileSize, &QAction::triggered, this, &IconView::slotsSort);
    connect(sortFileType, &QAction::triggered, this, &IconView::slotsSort);
    connect(sortFileType, &QAction::triggered, this, &IconView::slotsSort);


    connect(iconRefreash, &QAction::triggered, this, &IconView::onIconRefreash);

    iconSmall->setCheckable(true);
    iconMedium->setCheckable(true);
    iconBig->setCheckable(true);

    m_rootPath = rootPath;

    if(IniManager::instance()->contains("WallPaper/IconSize"))
    {
        int iconSize = IniManager::instance()->value("WallPaper/IconSize").toInt();
        if(iconSize>10)
        {
            if(iconSize == ICONSIZE_SMALL)
            {
                iconSmall->setChecked(true);
                m_currentSize = ICONSIZE_SMALL;
            }
            else if(iconSize == ICONSIZE_MEDIUM)
            {
                iconMedium->setChecked(true);
                m_currentSize = ICONSIZE_MEDIUM;
            }
            else if(iconSize == ICONSIZE_BIG)
            {
                iconBig->setChecked(true);
                m_currentSize = ICONSIZE_BIG;
            }
            setGridSize(QSize(iconSize, iconSize));
            setIconSize(QSize(iconSize/2, iconSize/2));
            m_currentSize = iconSize;
        }
    }

    if(IniManager::instance()->contains("WallPaper/SortFilter"))
    {
        int iSortFilter = IniManager::instance()->value("WallPaper/SortFilter").toInt();
        if(iSortFilter>=0)
        {
            m_proxyModel->setSortRole(iSortFilter);
        }
    }
    else
    {
        m_proxyModel->setSortRole(Qt::UserRole + 1);
    }

    startFileSystemWatcher();

    connect(this,&QListView::indexesMoved,this,&IconView::onindexesMoved);

    loadDragPositions();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

IconView::~IconView()
{

}

void IconView::onActionTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString path = action->data().toString();
        QModelIndexList indexes = selectedIndexes();
        QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
        //set Defualt
        for (int i = 0, imax = indexes.count(); i < imax; ++i) {
            if(model)
            {
                QModelIndex sourceIndex = m_proxyModel->mapToSource(indexes[i]);
                QString fileName = model->filePath(sourceIndex);
                QString MIME = QMimeDatabase().mimeTypeForFile(fileName).name();
                GioClass::setDefautlApp(MIME,path);
            }
        }
        //open
        openExeFile(false);
    }
}

void IconView::onSetOhterActionTriggered()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr,tr("Please Select App"),"/usr/share/applications/");
    if (filePath.isEmpty()) {
        return;
    }
    qDebug()<<"set:" <<filePath;
    QString path = filePath;
    QModelIndexList indexes = selectedIndexes();

    bool bsok = false;
    //set Defualt
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
    for (int i = 0, imax = indexes.count(); i < imax; ++i) {
        if (model) {
            QModelIndex sourceIndex = m_proxyModel->mapToSource(indexes[i]);
            QString fileName = model->filePath(sourceIndex);
            QString MIME = QMimeDatabase().mimeTypeForFile(fileName).name();
            bsok = GioClass::setDefautlApp(MIME,path);
        }
    }
    //open
    if(bsok)
    {
        openExeFile(false);
    }
    else
    {
        QMessageBox::information(this,tr(""),tr(""));
    }
}

void IconView::onNewActionTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString path = action->data().toString();
        QString apath = action->text();
        QFileInfo fileInfo (path);
        QString suffix = fileInfo.completeSuffix();
        QString baseName = fileInfo.baseName();
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QFile a;
        int i = 1;
        while (i < 99) {
            QString copPath = desktopPath + "/" + baseName + QString::number(i) +"." +suffix;
            if (!a.exists(copPath))
            {
                QFile::copy(path,copPath);
                a.close();
                i = 100;
                break;
            }
            i++;
        }
    }
}

void IconView::startFileSystemWatcher()
{
    fileSystemWatcher = new QFileSystemWatcher(this);
    connect(fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, &IconView::handleDirectoryChanged);
    fileSystemWatcher->addPath(m_rootPath);
}

void IconView::handleDirectoryChanged(const QString &path)
{
    return ;
    QStringList filePaths = dragPositions.keys();
    for (const QString &filePath : filePaths) {
        if (filePath.startsWith(path)) {
            QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
            QModelIndex index = m_proxyModel->mapFromSource(model->index(filePath));
            qDebug() << "515" <<rectForIndex(index) ;
            if (index.isValid()) {
                QByteArray dragPositionData = dragPositions.value(filePath);
                QStringList dragPositionList = QString(dragPositionData).split(',');
                if (dragPositionList.size() == 2) {
                    int x = dragPositionList[0].toInt();
                    int y = dragPositionList[1].toInt();
//                    moveFileToPosition(this,m_proxyModel,m_rootPath,x,y);
                        qDebug()<< "::"<<QPoint(x,y) << flipX(QPoint(x,y));;
                        qDebug()<< this->gridSize();
                        m_isRefreash = true;
                        setPositionForIndex(QPoint(x,y),index);
                }
            }
        }
    }
}

void IconView::onindexesMoved(const QModelIndexList &indexes)
{
    QMutexLocker locker(&m_mutex);
    qDebug()<<indexes.size();
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
    for(QModelIndex index : indexes)
    {
        QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
        QString filePath = model->filePath(sourceIndex);
        QRect rect= rectForIndex(index);
        qDebug()<<"move:" << rect;
        QByteArray dragPositionData = QByteArray::number(rect.x()) + "," + QByteArray::number(rect.y());
        dragPositions.insert(filePath, dragPositionData);
    }
    saveDragPositions();
}

void IconView::onIconRefreash()
{
    dragPositions.clear();
    saveDragPositions();
    QListView::doItemsLayout();
}

void IconView::copyImageToClipboard(const QStringList &paths ,CopyOrCut type )
{
    //  Get clipboard
    QClipboard *cb = qApp->clipboard();

    // Ownership of the new data is transferred to the clipboard.
    QMimeData *newMimeData = new QMimeData();

    // Copy file (gnome)
    QByteArray gnomeFormat;
    if(type == CopyOrCut_COPY)
        gnomeFormat = QByteArray("copy\n");
    else if(type == CopyOrCut_CUT){
        gnomeFormat = QByteArray("cut\n");
    }
    QString text;
    QList<QUrl> dataUrls;
    for (QString path : paths) {
        if (!path.isEmpty())
            text += path + '\n';
        dataUrls << QUrl::fromLocalFile(path);
        gnomeFormat.append(QUrl::fromLocalFile(path).toEncoded()).append("\n");
    }

    newMimeData->setText(text.endsWith('\n') ? text.left(text.length() - 1) : text);
    newMimeData->setUrls(dataUrls);
    gnomeFormat.remove(gnomeFormat.length() - 1, 1);
    newMimeData->setData("x-special/gnome-copied-files", gnomeFormat);

    // Copy Image Date
    cb->setMimeData(newMimeData, QClipboard::Clipboard);
}

QString IconView::terminalPath()
{
    const static QString deepin_default_term = QStringLiteral("/usr/lib/deepin-daemon/default-terminal");
    const static QString debian_x_term_emu = QStringLiteral("/usr/bin/x-terminal-emulator");
    const static QString kde_term_emu = QStringLiteral("/usr/bin/konsole");
    const static QString gnome_term_emu = QStringLiteral("/usr/bin/gnome-terminal");
    const static QString xfce_term_emu = QStringLiteral("/usr/bin/xfce-terminal");
    const static QString xfce3_term_emu = QStringLiteral("/usr/bin/xfce3-terminal");
    const static QString xfce4_term_emu = QStringLiteral("/usr/bin/xfce4-terminal");
    const static QString cutefish_term_emu = QStringLiteral("/usr/bin/cutefish-terminal");
    const static QString yoyo_term_emu = QStringLiteral("/usr/bin/yoyo-terminal");

    if (QFileInfo::exists(deepin_default_term)) {
        return deepin_default_term;
    } else if (QFileInfo::exists(debian_x_term_emu)) {
        return debian_x_term_emu;
    } else if (QFileInfo::exists(kde_term_emu)) {
        return kde_term_emu;
    } else if (QFileInfo::exists(gnome_term_emu)) {
        return gnome_term_emu;
    } else if (QFileInfo::exists(xfce_term_emu)) {
        return xfce_term_emu;
    } else if (QFileInfo::exists(xfce3_term_emu)) {
        return xfce3_term_emu;
    } else if (QFileInfo::exists(xfce4_term_emu)) {
        return xfce4_term_emu;
    } else if (QFileInfo::exists(cutefish_term_emu)) {
        return cutefish_term_emu;
    } else if (QFileInfo::exists(yoyo_term_emu)) {
        return yoyo_term_emu;
    }

    return QStandardPaths::findExecutable("xterm");
}

void IconView::setIconTextSize(int size)
{
    setGridSize(QSize(size, size));
    setIconSize(QSize(size/2, size/2));
}

void IconView::openExeFile(bool isExe)
{
    QProcess *proc = new QProcess;
    QModelIndexList indexes = selectedIndexes();
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
    for (int i = 0, imax = indexes.count(); i < imax; ++i) {
        if(model)
        {
            QModelIndex sourceIndex = m_proxyModel->mapToSource(indexes[i]);
            QString fileName = model->filePath(sourceIndex);
            QString MIME = QMimeDatabase().mimeTypeForFile(fileName).name();

            qDebug() << "Execution file now: " + fileName + ", Mime type is: " + MIME;
            if (MIME == "application/x-desktop" && isExe) {
                QString sexec = readSettings(fileName, "Desktop Entry", "Exec");
                if (!sexec.isNull())
                    proc->setWorkingDirectory(readSettings(fileName, "Desktop Entry", "Path"));
                    proc->start(sexec);
            } else if (MIME == "application/vnd.appimage") {
                proc->start(fileName);
            } else {
                QString Url = QString("file:///") + fileName;
                QDesktopServices::openUrl(QUrl(Url));
            }
        }
    }
}

void IconView::saveDragPositions()
{
    QSettings settings("MyApp", "FileExplorer");
    settings.clear();
    for (const QString &filePath : dragPositions.keys()) {
        settings.setValue(filePath, dragPositions.value(filePath));
    }
}

void IconView::loadDragPositions()
{
    QSettings settings("MyApp", "FileExplorer");
    QStringList filePaths = settings.allKeys();
    for (QString &filePath : filePaths) {
        QByteArray dragPositionData = settings.value(filePath).toByteArray();
        if (!dragPositionData.isEmpty()) {
            QStringList dragPositionList = QString(dragPositionData).split(',');
            if (dragPositionList.size() == 2) {
                int x = dragPositionList[0].toInt();
                int y = dragPositionList[1].toInt();
                if(filePath.at(0)!="/")
                {
                    filePath ="/"+filePath;
                }
                dragPositions.insert(filePath, QByteArray::number(x) + "," + QByteArray::number(y));
            }
        }
    }
    doItemsLayout();
}

void IconView::moveFileToPosition(QListView *listView, QSortFilterProxyModel *proxyModel, const QString &filePath, int x, int y)
{
    QTimer::singleShot(1000,[=]
    {
        // 获取源模型
        QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel *>(proxyModel->sourceModel());
        if (!sourceModel)
        {
            return;
        }

        // 获取文件的源模型索引
        QModelIndex sourceFileIndex = sourceModel->index(filePath);
        if (!sourceFileIndex.isValid())
        {
            return;
        }
        qDebug() <<  rectForIndex(sourceFileIndex) ;
        setPositionForIndex(QPoint(x,y),sourceFileIndex);

        // 将文件移动到新位置
        QModelIndex targetIndex = listView->indexAt(QPoint(x, y));
        if (!targetIndex.isValid())
        {
            targetIndex = proxyModel->index(0, 0);
        }
        // 将源模型索引转换为代理模型索引
        QModelIndex proxySourceFileIndex = proxyModel->mapFromSource(sourceFileIndex);

        qDebug() <<  rectForIndex(proxySourceFileIndex) ;
        setPositionForIndex(QPoint(x,y),proxySourceFileIndex);

        // 将文件移动到目标索引位置
        proxyModel->moveRow(proxySourceFileIndex.parent(), proxySourceFileIndex.row(), targetIndex.parent(), targetIndex.row());

        // 设置当前索引并滚动到指定位置
        QModelIndex newIndex = proxyModel->mapFromSource(sourceModel->index(targetIndex.row(), targetIndex.column()));
        listView->setCurrentIndex(newIndex);
        listView->scrollTo(newIndex);
    });
}

void IconView::doItemsLayout()
{
    qDebug()<< "refreash";
    QStringList filePaths = dragPositions.keys();
//    for(int i =0 ; i< filePaths.count();i++)
//    {
//        QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
//        QString filePath = filePaths.at(i);
//        QModelIndex index = m_proxyModel->mapFromSource(model->index(filePath));
//        int maxCount = model->rowCount(m_rootIndex);
//        QModelIndex targetIndex = model->index(maxCount - 1 -i,0,m_rootIndex);
//        if (targetIndex.isValid() && index.isValid())
//        {
//            // 获取当前位置和目标位置的行号和列号
//            int currentRow = index.row();
//            int targetRow = targetIndex.row();
//            if(currentRow != targetRow)
//            {
////                // 移动索引
////                model->beginMoveRows(QModelIndex(), currentRow, currentRow, QModelIndex(), targetRow);
////                model->endMoveRows();
////                model->changePersistentIndex(index.parent(),targetIndex.parent());
////                model->moveRow(index.parent(),currentRow,targetIndex.parent(),targetRow);

//            }
//        }
//    }

    QListView::doItemsLayout();


    for (const QString &filePath : filePaths) {
        if (filePath.startsWith(m_rootPath))
        {
            QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
            QModelIndex index = m_proxyModel->mapFromSource(model->index(filePath));
            qDebug() << "515" <<rectForIndex(index) ;
            if (index.isValid()) {
                QByteArray dragPositionData = dragPositions.value(filePath);
                QStringList dragPositionList = QString(dragPositionData).split(',');
                if (dragPositionList.size() == 2) {
                    int x = dragPositionList[0].toInt();
                    int y = dragPositionList[1].toInt();
                        m_isRefreash = true;
                        int maxCount = model->rowCount(m_rootIndex);
                        // 将文件移动到新位置
                        QModelIndex targetIndex = this->indexAt(QPoint(x, y));
                        if (targetIndex.isValid())
                        {
                            QRect rect = visualRect(index);
                            setPositionForIndex(QPoint(rect.x(),rect.y()),targetIndex);
                        }
                        qDebug()<<"753:"<< model->rowCount(m_rootIndex);

                        setPositionForIndex(QPoint(x,y),index);
                }
            }
        }
    }
    return ;
}

int IconView::flipX(int x) const
{
    return qMax(viewport()->width(), contentsSize().width()) - x;
}

QPoint IconView::flipX(const QPoint &p) const
{
    return QPoint(flipX(p.x()), p.y());
}

void IconView::copyFile()
{
    QModelIndexList list = this->selectedIndexes();
    QStringList addlist;
    for (QModelIndex index : list) {
        QString path = index.data().toString();
        QString addPath = m_rootPath + "/" + path;
        addlist << addPath;
    }
//    QMimeData *selectionMimeData = fileModel->mimeData(list);
//    QApplication::clipboard()->setMimeData(selectionMimeData);
    copyImageToClipboard(addlist,CopyOrCut_COPY);
}

void IconView::cutFile()
{
    QModelIndexList list = this->selectedIndexes();
    QStringList addlist;
    for (QModelIndex index : list) {
        QString path = index.data().toString();
        QString addPath = m_rootPath + "/" + path;
        addlist << addPath;
    }
//    QMimeData *selectionMimeData = fileModel->mimeData(list);
//    QApplication::clipboard()->setMimeData(selectionMimeData);
    copyImageToClipboard(addlist,CopyOrCut_CUT);
}

void IconView::pauseFile()
{
    QList<QUrl> urls = QApplication::clipboard()->mimeData()->urls();

    FileOperationJob *fileOpJob = new FileOperationJob;
    QByteArray ba = QApplication::clipboard()->mimeData()->data("x-special/gnome-copied-files");
    QString tStr(ba);
    if (tStr.startsWith("cut")) {
         fileOpJob->setOperationFlag(FILE_OPERATION_MOVE);
    }
    if (tStr.startsWith("copy")) {
        fileOpJob->setOperationFlag(FILE_OPERATION_COPY);
    }

    fileOpJob->setOrigList(urls);

    fileOpJob->setDestDir(m_rootPath);
    QThread *fileOpThread = new QThread;
    fileOpJob->moveToThread(fileOpThread);
    connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
    connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
    connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
    fileOpThread->start();
}

void IconView::openFile()
{
    openExeFile(true);
}

void IconView::deleteFile()
{
    QModelIndexList list = this->selectedIndexes();
    FileOperationJob *fileOpJob = new FileOperationJob;
    fileOpJob->setOperationFlag(FILE_OPERATION_TRASH);
    fileOpJob->setOrigList(m_proxyModel->mimeData(list)->urls());

    QThread *fileOpThread = new QThread;
    fileOpJob->moveToThread(fileOpThread);
    connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
    connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
    connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
    fileOpThread->start();
}

void IconView::renameFile()
{
    edit(selectedIndexes().first());
}

void IconView::slotsnewFolder()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QDir a;
    int i = 1;
    while (i < 99) {
        if (a.mkdir(desktopPath + "/" + tr("New Folder") + QString::number(i))) {
            i = 100;
            break;
        }
        i++;
    }
}

void IconView::slotsnewTxt()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QFile a;
    int i = 1;
    while (i < 99) {
        QString name = desktopPath + "/" + tr("New Txt Files") + QString::number(i) + ".txt";
        if (!a.exists(name)) {
            a.setFileName(name);
            if (!a.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "打开失败";
            }
            a.close();

//            // 在新文件夹中选中并显示新建文件夹
//            QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
//            if (model) {
//                QModelIndex index = indexAt(QCursor::pos());

//                model->mkdir(index, name);
//                // 重新设置根目录路径以刷新数据
//                QString rootPath = model->rootPath();
//                model->setRootPath(rootPath);

//                QModelIndex sourceIndex = m_proxyModel->mapToSource(model->index(name));
//                setCurrentIndex(sourceIndex);
//                scrollTo(sourceIndex);
//            }
            i = 100;
            break;
        }
        i++;
    }
}

void IconView::slotsopenTerminal()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QDir::setCurrent(desktopPath);
    QProcess::startDetached(terminalPath());
}

void IconView::slotsIconSizeSmall()
{
    double b = double(ICONSIZE_SMALL)/double(m_currentSize);
    m_currentSize = ICONSIZE_SMALL;
    QStringList filePaths = dragPositions.keys();
    for (const QString &filePath : filePaths)
    {
        QByteArray dragPositionData = dragPositions.value(filePath);
        QStringList dragPositionList = QString(dragPositionData).split(',');
        if (dragPositionList.size() == 2) {
            int x = dragPositionList[0].toInt();
            int y = dragPositionList[1].toInt();
            int nX = (double)x * b;
            int nY = (double)y * b;
            if(nX>(width()-m_currentSize) || nY>(height()-m_currentSize) )
            {
                dragPositions.remove(filePath);
            }
            else {
                dragPositions.insert(filePath, QByteArray::number(nX) + "," + QByteArray::number(nY));
            }

        }
    }
    saveDragPositions();

    iconSmall->setChecked(true);
    iconMedium->setChecked(false);
    iconBig->setChecked(false);
    setIconTextSize(ICONSIZE_SMALL);
    IniManager::instance()->setValue("WallPaper/IconSize",ICONSIZE_SMALL);
}

void IconView::slotsIconSizeMedium()
{
    double b = double(ICONSIZE_MEDIUM)/double(m_currentSize);
    m_currentSize = ICONSIZE_MEDIUM;
    QStringList filePaths = dragPositions.keys();
    for (const QString &filePath : filePaths)
    {
        QByteArray dragPositionData = dragPositions.value(filePath);
        QStringList dragPositionList = QString(dragPositionData).split(',');
        if (dragPositionList.size() == 2) {
            int x = dragPositionList[0].toInt();
            int y = dragPositionList[1].toInt();
            int nX = (double)x * b;
            int nY = (double)y * b;
            if(nX>(width()-m_currentSize) || nY>(height()-m_currentSize) )
            {
                dragPositions.remove(filePath);
            }
            else {
                dragPositions.insert(filePath, QByteArray::number(nX) + "," + QByteArray::number(nY));
            }
        }
    }
    saveDragPositions();
    iconSmall->setChecked(false);
    iconMedium->setChecked(true);
    iconBig->setChecked(false);
    setIconTextSize(ICONSIZE_MEDIUM);
    IniManager::instance()->setValue("WallPaper/IconSize",ICONSIZE_MEDIUM);
}

void IconView::slotsIconSizeBig()
{
    double b = double(ICONSIZE_BIG)/double(m_currentSize);
    m_currentSize = ICONSIZE_BIG;
    QStringList filePaths = dragPositions.keys();
    for (const QString &filePath : filePaths)
    {
        QByteArray dragPositionData = dragPositions.value(filePath);
        QStringList dragPositionList = QString(dragPositionData).split(',');
        if (dragPositionList.size() == 2) {
            int x = dragPositionList[0].toInt();
            int y = dragPositionList[1].toInt();
            int nX = (double)x * b;
            int nY = (double)y * b;
            if(nX>(width()-m_currentSize) || nY>(height()-m_currentSize) )
            {
                dragPositions.remove(filePath);
            }
            else {
                dragPositions.insert(filePath, QByteArray::number(nX) + "," + QByteArray::number(nY));
            }
        }
    }
    saveDragPositions();

    iconSmall->setChecked(false);
    iconMedium->setChecked(false);
    iconBig->setChecked(true);
    setIconTextSize(ICONSIZE_BIG);
    IniManager::instance()->setValue("WallPaper/IconSize",ICONSIZE_BIG);
}

void IconView::slotsWallpaperAction()
{
    //设置壁纸
    Q_EMIT dApp->sigWallpaperAction();
}

void IconView::slotsSort()
{
    dragPositions.clear();
    saveDragPositions();
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        m_proxyModel->setSortRole(action->data().toInt());
        IniManager::instance()->setValue("WallPaper/SortFilter",action->data().toInt());
    }

}

void IconView::paintEvent(QPaintEvent *e)
{
    return QListView::paintEvent(e);
}

void IconView::mousePressEvent(QMouseEvent *e)
{
    mLastPos = e->pos();
    Q_EMIT sigMouseClick(0);
    QListView::mousePressEvent(e);
}

void IconView::mouseMoveEvent(QMouseEvent *e)
{
    return QListView::mouseMoveEvent(e);
}

void IconView::mouseReleaseEvent(QMouseEvent *e)
{
    Q_EMIT sigMouseClick(1);
    return QListView::mouseReleaseEvent(e);
}

void IconView::focusOutEvent(QFocusEvent *e)
{
    return QListView::focusOutEvent(e);
}

void IconView::closeEvent(QCloseEvent *e)
{
    return QListView::closeEvent(e);
}

void IconView::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        if (e->source() == this) {
            e->setDropAction(Qt::MoveAction);
            e->accept();
        } else {
            e->acceptProposedAction();
        }
    } else {
        e->ignore();
    }
}

void IconView::dragMoveEvent(QDragMoveEvent *e)
{
    if (this == e->source()) {
        return QListView::dragMoveEvent(e);
    }
    e->accept();
}

void IconView::dropEvent(QDropEvent *e)
{
    if (this != e->source()) {
        if (e->mimeData()->hasUrls() && !indexAt(e->pos()).isValid()) {
            QList<QUrl> urls = e->mimeData()->urls();
            if (!this->indexAt(e->pos()).isValid()) {
                //do the file move job
                FileOperationJob *fileOpJob = new FileOperationJob;
                fileOpJob->setOperationFlag(FILE_OPERATION_MOVE);
                fileOpJob->setOrigList(urls);
                QFileSystemModel *model = qobject_cast<QFileSystemModel*>(m_proxyModel->sourceModel());
                if(model)
                {
                    QModelIndex sourceIndex = m_proxyModel->mapToSource(this->rootIndex());
                    fileOpJob->setDestDir(model->filePath(sourceIndex));
                }

                QThread *fileOpThread = new QThread;
                fileOpJob->moveToThread(fileOpThread);
                connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
                connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
                connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
                fileOpThread->start();
            }
        }
    } else {
//        e->ignore();
        const QMimeData *mimeData = e->mimeData();
        if (mimeData->hasUrls()) {
            QPoint dragPosition = e->pos();
            QByteArray dragPositionData = QByteArray::number(dragPosition.x()) + "," + QByteArray::number(dragPosition.y());
            QList<QUrl> urlList = mimeData->urls();
            for (const QUrl &url : urlList) {
                QString filePath = url.toLocalFile();
                qDebug() << "Dropped file: " << filePath << " at position: " << dragPosition;
//                dragPositions.insert(filePath, dragPositionData);
            }
        }
        e->acceptProposedAction();

        QListView::dropEvent(e);
    }
    //return QListView::dropEvent(e);
}

void IconView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F2) {
        renameFile();
    } else if (event->key() == Qt::Key_Space) {
        openFile();
    } else if (event->key() == Qt::Key_Delete) {
        deleteFile();
    } else if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_C) {
        copyFile();
    } else if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_X) {
        cutFile();
    } else if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_V) {
        pauseFile();
    } else if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_A) {
        slotsnewFolder();
    } else if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_B) {
        slotsnewTxt();
    } else if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_A) {
        selectAll();
    } else if (event->key() == Qt::Key_Enter ||  event->key() == Qt::Key_Return ) {
        openFile();
    }
    else {
        return QListView::keyPressEvent(event);
    }
}
