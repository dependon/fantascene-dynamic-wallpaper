#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QWidget>
class view;
namespace Ui {
class historyWidget;
}

class historyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit historyWidget(QWidget *parent = nullptr);
    ~historyWidget();
protected:
    void showEvent(QShowEvent *event) override;
private Q_SLOTS:
    void on_setWallPaper_clicked();

    void on_delWallPaper_clicked();

    void on_importBtn_clicked();

    void on_addPlaylistBtn_clicked();

    void on_moreSetting_clicked();

    void on_loopBox_clicked(bool checked);

    void on_deletePlaylist_clicked();

private:
    Ui::historyWidget *ui;
    view *m_viewHistory{nullptr};
    view *m_viewPlayList{nullptr};
};

#endif // HISTORYWIDGET_H
