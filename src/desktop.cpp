#include "desktop.h"
#include "ui_desktop.h"

#include <QFileInfo>
#include <QPainter>
#include <QMimeDatabase>
#include <QSettings>
#include <QTextCodec>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QListWidgetItem>
#include <QLabel>
#include <QFileIconProvider>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xproto.h>

#include <X11/extensions/shape.h>
#include <X11/extensions/Xrender.h>
#include <QtX11Extras/QX11Info>

Desktop::Desktop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Desktop)
{
    ui->setupUi(this);

    addList();
    setAttribute(Qt::WA_TranslucentBackground, true);
    ui->listWidget->setFrameShape(QListWidget::NoFrame);
    ui->listWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    ui->listWidget->setStyleSheet("background-color:transparent");
    initMenu();
}

Desktop::~Desktop()
{
    delete ui;
}

void Desktop::initMenu()
{
    m_rightMenu = new QMenu(this);

    QAction *action_newFolder = new QAction(this);
    action_newFolder->setText(tr("New folder"));

    connect(action_newFolder, &QAction::triggered, [ = ] {
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QDir a;
        int i = 1;
        while (i < 99)
        {
            if (a.mkdir(desktopPath + "/" + tr("New folder") + QString::number(i))) {
                i = 100;
                break;
            }
            i++;
        }
        addList();
        ui->listWidget->setStyleSheet("background-color:transparent");

    });
    m_rightMenu->addAction(action_newFolder);

    QAction *action_newTxtFile = new QAction(this);
    action_newTxtFile->setText(tr("New Txt Files"));

    connect(action_newTxtFile, &QAction::triggered, [ = ] {
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QFile a;
        int i = 1;
        while (i < 99)
        {
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

        addList();
        ui->listWidget->setStyleSheet("background-color:transparent");

    });

    m_rightMenu->addAction(action_newTxtFile);

    QAction *action_refreash = new QAction(this);
    action_refreash->setText(tr("Refresh"));

    m_rightMenu->addAction(action_refreash);
}

void Desktop::addList()
{
    ui->listWidget->clear();
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name | QDir::DirsFirst);
    m_infolist.clear();
    m_infolist = dir.entryInfoList();

//    QStringList pathList = dir.entryList();
    for (int i = 0; i < m_infolist.size(); ++i) {
        qDebug() << "entryList: " << i << "-" << m_infolist.at(i);
        if (m_infolist.at(i).filePath() != "." && m_infolist.at(i).filePath() != "..") {

            QIcon icon = getIcon(m_infolist.at(i).filePath());
            QFileInfo fi(m_infolist.at(i).filePath());
            QFileIconProvider icon_provider;
//            QIcon icon = icon_provider.icon(fi);

            QListWidgetItem *LWI = new QListWidgetItem(ui->listWidget);
            QLabel *label = new QLabel(fi.completeBaseName());
            label->setFixedWidth(100);
            label->setMinimumHeight(20);
            label->setAlignment(Qt::AlignCenter);
            LWI->setSizeHint(QSize(100, 100));
            LWI->setIcon(icon);
            LWI->setData(Qt::UserRole + 1, fi.absoluteFilePath());
//            LWI->setText(pathList.at(i));
            ui->listWidget->setItemWidget(LWI, label);
//            ui->listWidget->insertItem(i, LWI);
        }
    }
    QList<QListWidgetItem *> LWIs = ui->listWidget->selectedItems();
    if (LWIs.size() > 0)
        ui->listWidget->scrollToItem(LWIs.at(0), QAbstractItemView::PositionAtCenter);
}

QIcon Desktop::getIcon(const QString &path)
{
    QString sname = "", TAG = "", Title = "", Artist = "", Album = "", Year = "", Comment = "";
    QFileInfo fileInfo(path);
    sname = fileInfo.fileName();
    QIcon icon;
    QString MIME = QMimeDatabase().mimeTypeForFile(fileInfo.absoluteFilePath()).name();
    //QString filetype = MIME.left(MIME.indexOf("/"));
    if (MIME == "application/x-desktop") {
        sname = readSettings(fileInfo.absoluteFilePath(), "Desktop Entry", "Name");
        QString sicon = readSettings(fileInfo.absoluteFilePath(), "Desktop Entry", "Icon");
        //qDebug() << sicon;
        if (sicon == "")
            sicon = "applications-system-symbolic";
        if (QFileInfo(sicon).isFile()) {
            icon = QIcon(sicon);
        } else {
            icon = QIcon::fromTheme(sicon);
        }
    } else if (MIME == "inode/directory") {
        icon = QIcon::fromTheme("folder");
    } else if (fileInfo.suffix() == "mp3") {
        QFile file(fileInfo.absoluteFilePath());
        file.open(QIODevice::ReadOnly);
        QString ID3, Ver, Revision, Flag;
        bool ok;
        qint64 size;
        ID3 = QString(file.read(3));
        icon = QIcon::fromTheme(MIME.replace("/", "-"));
        QTextCodec *TC = QTextCodec::codecForName("GBK");
        if (ID3 == "ID3") {
            Ver = QString::number(file.read(1).toHex().toInt(&ok, 16));
            Revision = QString::number(file.read(1).toHex().toInt(&ok, 16));
            Flag = QString::number(file.read(1).toHex().toInt(&ok, 16));
            //size = file.read(4).toHex().toLongLong(&ok,16);
            size = (file.read(1).toHex().toInt(&ok, 16) & 0xEF) << 21 | (file.read(1).toHex().toInt(&ok, 16) & 0xEF) << 14 | (file.read(1).toHex().toInt(&ok, 16) & 0xEF) << 7 | file.read(1).toHex().toInt(&ok, 16) & 0xEF;
            while (file.pos() < size) {
                QString FTag(file.read(4));
                qint64 FSize = file.read(4).toHex().toLongLong(&ok, 16);
                //qint64 FSize = file.read(1).toHex().toInt(&ok,16) << 24 | file.read(1).toHex().toInt(&ok,16) << 16 | file.read(1).toHex().toInt(&ok,16) << 8 | file.read(1).toHex().toInt(&ok,16);
                Flag = QString::number(file.read(2).toHex().toInt(&ok, 16));
                QByteArray BA = file.read(FSize);
                if (FTag == "APIC") {
                    BA = BA.right(FSize - 14);
                    QPixmap pixmap;
                    ok = pixmap.loadFromData(BA);
                    if (ok) {
                        icon = QIcon(pixmap);
                    }
                    break;
                } else if (FTag == "TYER") {
                    if (BA.contains("\xFF\xFE")) {
                        Year = QString::fromUtf16(reinterpret_cast<const ushort *>(BA.mid(3, FSize - 3).data()));
                    } else {
                        Year = BA.mid(1, FSize - 2);
                    }
                } else if (FTag == "COMM") {
                    QString language = BA.mid(1, 3);
                    Comment = language + " " + QString::fromUtf16(reinterpret_cast<const ushort *>(BA.mid(10, FSize - 12).data()));
                } else if (FTag == "TIT2") {
                    QByteArray UFlag = BA.left(1);
                    if (UFlag.toHex().toInt() == 0) {
                        Title = TC->toUnicode(BA);
                    } else {
                        Title = QString::fromUtf16(reinterpret_cast<const ushort *>(BA.right(FSize - 3).data()));
                    }
                } else if (FTag == "TPE1") {
                    QByteArray UFlag = BA.left(1);
                    if (UFlag.toHex().toInt() == 0) {
                        Artist = TC->toUnicode(BA);
                    } else {
                        Artist = QString::fromUtf16(reinterpret_cast<const ushort *>(BA.right(FSize - 3).data()));
                    }
                } else if (FTag == "TALB") {
                    QByteArray UFlag = BA.left(1);
                    if (UFlag.toHex().toInt() == 0) {
                        Album = TC->toUnicode(BA);
                    } else {
                        Album = QString::fromUtf16(reinterpret_cast<const ushort *>(BA.right(FSize - 3).data()));
                    }
                }
            }
        }
        qint64 pos = file.size() - 128;
        file.seek(pos);
        TAG = QString(file.read(3));
        if (TAG == "TAG") {
            Title = TC->toUnicode(file.read(30));
            Artist = TC->toUnicode(file.read(30));
            Album = TC->toUnicode(file.read(30));
            Year = QString(file.read(4));
            Comment = TC->toUnicode(file.read(28));
        }
    } else {
        icon = QIcon::fromTheme(MIME.replace("/", "-"));
        if (icon.isNull()) {
            icon = QIcon::fromTheme("unknown");
        }
    }

    if (fileInfo.isSymLink()) {
        QPixmap pixmap = icon.pixmap(200, 200, QIcon::Normal, QIcon::On);
        QPainter painter(&pixmap);
        painter.drawPixmap(100, 100, QIcon::fromTheme("emblem-symbolic-link").pixmap(100, 100, QIcon::Normal, QIcon::On));
        icon = QIcon(pixmap);
    }
    return icon;
}

QString Desktop::readSettings(const QString &path, const QString &group, const QString &key)
{
    QSettings settings(path, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup(group);
    QString value = settings.value(key).toString();
    return value;
}

void Desktop::open(const QString &newpath)
{
    QFileInfo FI(newpath);
    QProcess *proc = new QProcess;
    QString MIME = QMimeDatabase().mimeTypeForFile(newpath).name();
    qDebug() << MIME;
    if (MIME == "application/x-desktop") {
        QString sexec = readSettings(newpath, "Desktop Entry", "Exec");
        proc->setWorkingDirectory(readSettings(newpath, "Desktop Entry", "Path"));
        qDebug() << sexec;
        proc->start(sexec);
    } else if (MIME == "application/vnd.appimage") {
        proc->start(newpath);
    } else {
        QString path = QString("file:///") + newpath;
        QDesktopServices::openUrl(QUrl(path));
    }
}

void Desktop::on_listWidget_doubleClicked(const QModelIndex &index)
{
    QString local_path = m_infolist.at(ui->listWidget->currentRow()).absoluteFilePath();

    open(local_path);
}

void Desktop::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    m_rightMenu->exec(QCursor::pos());
}
