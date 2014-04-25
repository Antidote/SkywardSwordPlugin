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

#ifndef SKYWARDSWORDWIDGET_H
#define SKYWARDSWORDWIDGET_H

#include <QWidget>
#include "Common.hpp"

namespace Ui{
class SkywardSwordWidget;
}

class QAbstractButton;

class SkywardSwordTabWidget;

class SkywardSwordWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SkywardSwordWidget(QWidget *parent = 0);

    SkywardSwordTabWidget* tabWidget() const;

    void setRegion(Region region);
    Region region() const;
signals:
    void regionChanged(Region);
    void infoButtonClicked();
public slots:
    void onButtonClicked(QAbstractButton* btn, bool val);
private:
    Ui::SkywardSwordWidget* ui;
};

#endif // SKYWARDSWORDWIDGET_H
