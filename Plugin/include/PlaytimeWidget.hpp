// This file is part of Sakura Suite.
//
// Sakura Suite is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Sakura Suite is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sakura Suite.  If not, see <http://www.gnu.org/licenses/>

#ifndef PLAYTIMEWIDGET_H
#define PLAYTIMEWIDGET_H

#include <QWidget>

#include <QTimer>

namespace Ui {
class PlaytimeWidget;
}

#include "Common.hpp"

class PlaytimeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PlaytimeWidget(QWidget *parent = 0);
    ~PlaytimeWidget();

    void setPlaytime(Playtime);

signals:
    void playtimeChanged(Playtime);

private slots:
    void blinkText();
    void valueChanged();
    void clearTime();

private:
    Ui::PlaytimeWidget *ui;
    QTimer              m_blinkTimer;
    bool                m_blink;
};

#endif // PLAYTIMEWIDGET_H
