#ifndef WALLPAPERENGINEPLUGIN_H
#define WALLPAPERENGINEPLUGIN_H

#include <QWidget>
#include <QMap>

struct wallpaperEengineData {
    QString JsonPath;
    QString JsonBasePath;
    QString descrption;
    QString path;
    QString general;
    QString preview;
    QString tags;
    QString title;
    QString type;
    QString visibility;
    QString file;
};
class view;
namespace Ui {
class wallpaperEnginePlugin;
}

class wallpaperEnginePlugin : public QWidget
{
    Q_OBJECT

public:
    explicit wallpaperEnginePlugin(QWidget *parent = nullptr);
    ~wallpaperEnginePlugin();

    void FindFile(const QString &_filePath);
    void readJson(const QString &path);
    void showView();

    void refresh(const QString &path);
private slots:
    void on_setEnginePath_clicked();

    void on_setBtn_clicked();

private:
    Ui::wallpaperEnginePlugin *ui;

    QStringList m_JasonList;
    QMap <QString, wallpaperEengineData> m_map;

    view *m_view{nullptr};

};

#endif // WALLPAPERENGINEPLUGIN_H
