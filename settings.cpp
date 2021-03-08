/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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
 */

#include "settings.h"

#include <QListWidgetItem>
#include <QFileDialog>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>

Settings::Settings(QWidget *parent)
    : DMainWindow(parent)
    , m_layout(new QVBoxLayout(this))
    , m_listView(new QListWidget)
    , m_videoSwitchBtn(new DSwitchButton)
{

    setMinimumSize(800, 600);

    m_listView->setViewMode(QListView::ListMode);

    m_layout->setMargin(10);
    m_layout->setSpacing(5);

    QPushButton *addBtn = new QPushButton(tr("Add"));
    QPushButton *removeBtn = new QPushButton(tr("Remove"));
    QPushButton *cleanBtn = new QPushButton(tr("Clean"));
    QPushButton *applyBtn = new QPushButton(tr("Apply"));

    QHBoxLayout *listLayout = new QHBoxLayout;
    listLayout->addWidget(m_listView);

    QVBoxLayout *btnsLayout = new QVBoxLayout;
    btnsLayout->addWidget(addBtn);
    btnsLayout->addWidget(removeBtn);
    btnsLayout->addWidget(cleanBtn);
    btnsLayout->addWidget(applyBtn);

    listLayout->addLayout(btnsLayout);

    m_layout->addLayout(listLayout);

    setLayout(m_layout);

    connect(addBtn, &QPushButton::clicked, this, &Settings::addOne);
    connect(removeBtn, &QPushButton::clicked, this, &Settings::removeOne);
    connect(cleanBtn, &QPushButton::clicked, this, &Settings::clean);
    connect(applyBtn, &QPushButton::clicked, this, &Settings::finished);

}


void Settings::addOne()
{
    QString file_path = QFileDialog::getExistingDirectory(this, tr("Add Path"), "/home");
    if (file_path.isEmpty())
        return;

    QListWidgetItem *item = new QListWidgetItem;
    item->setText(file_path);

    m_listView->addItem(item);
    m_list << file_path;
}

void Settings::removeOne()
{
    const int index = m_listView->currentRow();
    QListWidgetItem *item = m_listView->takeItem(index);
    delete item;

    m_list.removeAt(index);
}

void Settings::clean()
{
    for (int i(0); i != m_list.count(); ++i) {
        QListWidgetItem *item = m_listView->takeItem(i);
        delete item;
    }

    m_list.clear();
}

void Settings::finished()
{
    //    emit requestSetFolder(m_list, m_is);
}
