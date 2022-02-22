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

#include "iconview.h"
#include "filemodel.h"
#include "iconprovider.h"
#include "fileoperationjob.h"
#include "delegateicon.h"

#include <QFileSystemModel>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QDesktopServices>
#include <QUrl>

#include <gio/gdesktopappinfo.h>

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
    fileModel->setRootPath(rootPath);
    setModel(fileModel);
    setRootIndex(fileModel->index(rootPath));
    //setRootIndex(fileModel->index("/home/kylin"));

    DelegateIcon *deleget = new DelegateIcon;
    deleget->setFileModel(fileModel);
    deleget->setIconView(this);
    setItemDelegate(deleget);

    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setDragEnabled(true);
    setDropIndicatorShown(true);
    setAcceptDrops(true);
    setDefaultDropAction(Qt::MoveAction);

    int size = fileModel->getIconProvider()->getIconSize();
    setIconSize(QSize(size, size));

    //setUniformItemSizes(true); //same size for all items
    setMovement(QListView::Snap); //gird move
    //setWordWrap(true); //text auto change row
    //setViewportMargins(0,10,0,10);
    //setContentsMargins(31,31,31,31);
    setResizeMode(QListView::Adjust); //auto redo layout
    setGridSize(QSize(120, 120));
    setIconSize(QSize(65, 65));
//    setTextElideMode();
    setSpacing(4);
    setViewMode(QListView::IconMode);
//    setMovement(QListView::Static);
    selectionModel()->selectedIndexes();
    //setWordWrap(true);

    setTextElideMode(Qt::ElideMiddle);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    QMenu *viewMenu = new QMenu();
    QAction *openAction = new QAction(viewMenu);
    openAction->setText(tr("Open"));
    viewMenu->addAction(openAction);
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

    QAction *newTXTction = new QAction(viewMenu);
    newTXTction->setText(tr("New TXT"));
    newTXTction->setShortcut(QKeySequence("Ctrl+Shift+B"));
    viewMenu->addAction(newTXTction);


    viewMenu->addSeparator();

    QAction *copyAction = new QAction(viewMenu);
    copyAction->setText(tr("Copy"));
    copyAction->setShortcut(QKeySequence("Ctrl+C"));
    viewMenu->addAction(copyAction);


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


    //TODO: add show propertries action
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &IconView::customContextMenuRequested, [ = ]() {
        //qDebug()<<"menu request"<<mLastPos;
        if (!QApplication::clipboard()->mimeData()->hasUrls()) {
            pasteAction->setEnabled(false);
        } else {
            pasteAction->setEnabled(true);
        }
        if (!indexAt(mLastPos).isValid()) {
            //qDebug()<<"blank";
            clearSelection();
            openAction->setEnabled(false);
            trashAction->setEnabled(false);
            copyAction->setEnabled(false);
            renameAction->setEnabled(false);
        } else {
            //qDebug()<<"selected";
            openAction->setEnabled(true);
            trashAction->setEnabled(true);
            copyAction->setEnabled(true);
            renameAction->setEnabled(false);
            if (selectedIndexes().count() == 1) {
                QString fileName = selectedIndexes().first().data().toString();
                if (!fileName.endsWith(".desktop")) {
                    renameAction->setEnabled(true);
                }
            }
        }
        viewMenu->exec(QCursor::pos());
    });

    connect(openAction, &QAction::triggered, this, &IconView::openFile);

    connect(selectAllAction, &QAction::triggered,  this, &IconView::selectAll);

    connect(copyAction, &QAction::triggered, this, &IconView::copyFile);

    connect(pasteAction, &QAction::triggered, this, &IconView::pauseFile);

    connect(renameAction, &QAction::triggered, this, &IconView::renameFile);

    connect(trashAction, &QAction::triggered, this, &IconView::deleteFile);

    connect(newFolderAction, &QAction::triggered, this, &IconView::slotsnewFolder);

    connect(newTXTction, &QAction::triggered, this, &IconView::slotsnewTxt);

    connect(terminalAction, &QAction::triggered, this, &IconView::slotsopenTerminal);


    connect(this, &IconView::doubleClicked, this, &IconView::openFile);

    m_rootPath = rootPath;
}

IconView::~IconView()
{

}
void IconView::copyImageToClipboard(const QStringList &paths)
{
    //  Get clipboard
    QClipboard *cb = qApp->clipboard();

    // Ownership of the new data is transferred to the clipboard.
    QMimeData *newMimeData = new QMimeData();

    // Copy file (gnome)
    QByteArray gnomeFormat = QByteArray("copy\n");
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
    const static QString xface_term_emu = QStringLiteral("/usr/bin/xface-terminal");
    const static QString xface3_term_emu = QStringLiteral("/usr/bin/xface3-terminal");
    const static QString xface4_term_emu = QStringLiteral("/usr/bin/xface4-terminal");
    const static QString cutefish_term_emu = QStringLiteral("/usr/bin/cutefish-terminal");

    if (QFileInfo::exists(deepin_default_term)) {
        return deepin_default_term;
    } else if (QFileInfo::exists(debian_x_term_emu)) {
        return debian_x_term_emu;
    } else if (QFileInfo::exists(kde_term_emu)) {
        return kde_term_emu;
    } else if (QFileInfo::exists(gnome_term_emu)) {
        return gnome_term_emu;
    } else if (QFileInfo::exists(xface_term_emu)) {
        return xface_term_emu;
    } else if (QFileInfo::exists(xface3_term_emu)) {
        return xface3_term_emu;
    } else if (QFileInfo::exists(xface4_term_emu)) {
        return xface4_term_emu;
    } else if (QFileInfo::exists(cutefish_term_emu)) {
        return cutefish_term_emu;
    }

    return QStandardPaths::findExecutable("xterm");
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
    copyImageToClipboard(addlist);
}

void IconView::pauseFile()
{
    QList<QUrl> urls = QApplication::clipboard()->mimeData()->urls();
    FileOperationJob *fileOpJob = new FileOperationJob;
    fileOpJob->setOperationFlag(FILE_OPERATION_COPY);
    fileOpJob->setOrigList(urls);
    fileOpJob->setDestDir(fileModel->filePath(rootIndex()));
    QThread *fileOpThread = new QThread;
    fileOpJob->moveToThread(fileOpThread);
    connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
    connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
    connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
    fileOpThread->start();
}

void IconView::openFile()
{
    QModelIndexList list = selectedIndexes();
    if (list.count() == 1) {
        qDebug() << fileModel->filePath(list.at(0));
        if (!fileModel->filePath(list.at(0)).endsWith(QString(".desktop"))) {
            QUrl url("file://" + fileModel->filePath(list.at(0)));
            QDesktopServices::openUrl(url);
        } else {
            qDebug() << "desktop file";
            std::string tmp_str = fileModel->filePath(list.at(0)).toStdString();
            const char *file_path = tmp_str.c_str();
            GDesktopAppInfo *app_info = g_desktop_app_info_new_from_filename(file_path);
            g_desktop_app_info_launch_uris_as_manager(app_info,
                                                      nullptr,
                                                      nullptr,
                                                      G_SPAWN_DEFAULT,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr);
            g_object_unref(app_info);
        }
    }
}

void IconView::deleteFile()
{
    QModelIndexList list = this->selectedIndexes();
    FileOperationJob *fileOpJob = new FileOperationJob;
    fileOpJob->setOperationFlag(FILE_OPERATION_TRASH);
    fileOpJob->setOrigList(fileModel->mimeData(list)->urls());
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
        if (!a.exists(desktopPath + "/" + tr("New Txt Files") + QString::number(i) + ".txt")) {
            a.setFileName(desktopPath + "/" + tr("New Txt Files") + QString::number(i) + ".txt");
            if (!a.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "打开失败";
            }
            a.close();
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

void IconView::paintEvent(QPaintEvent *e)
{
    return QListView::paintEvent(e);
}

void IconView::mousePressEvent(QMouseEvent *e)
{
    mLastPos = e->pos();
    Q_EMIT sigMouseClick(0);
    //qDebug()<<"press event: "<<mLastPos;
    QListView::mousePressEvent(e);
}

void IconView::mouseMoveEvent(QMouseEvent *e)
{
//    Q_EMIT sigMouseMove();
    //setCursor(Qt::ArrowCursor);
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
        //QList<QUrl> urls = e->mimeData()->urls();
        //qDebug()<<urls<<e->mimeData()->formats();
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
                fileOpJob->setDestDir(fileModel->filePath(this->rootIndex()));

                QThread *fileOpThread = new QThread;
                fileOpJob->moveToThread(fileOpThread);
                connect(fileOpThread, &QThread::started, fileOpJob, &FileOperationJob::jobStart);
                connect(fileOpThread, &QThread::finished, fileOpJob, &FileOperationJob::deleteLater);
                connect(fileOpThread, &QThread::finished, fileOpThread, &QThread::deleteLater);
                fileOpThread->start();
            }
        }
    } else {
        e->ignore();
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
    } else if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_V) {
        pauseFile();
    } else if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_A) {
        slotsnewFolder();
    } else if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_B) {
        slotsnewTxt();
    } else if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_A) {
        selectAll();
    } else {
        return QListView::keyPressEvent(event);
    }
}
