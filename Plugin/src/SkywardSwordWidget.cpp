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

#include "SkywardSwordWidget.hpp"
#include "SkywardSwordTabWidget.hpp"
#include "SkywardSwordPlugin.hpp"
#include "SettingsManager.hpp"
#include "ui_SkywardSwordWidget.h"

SkywardSwordWidget::SkywardSwordWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::SkywardSwordWidget)
{
    ui->setupUi(this);
    connect(ui->infoButton, SIGNAL(clicked()), this, SIGNAL(infoButtonClicked()));
}

SkywardSwordTabWidget* SkywardSwordWidget::tabWidget() const
{
    return ui->tabWidget;
}

void SkywardSwordWidget::setRegion(Region region)
{
    switch(region)
    {
        case Region::NTSCU: ui->ntscURB->setChecked(true); break;
        case Region::NTSCK: ui->ntscKRB->setChecked(true); break;
        case Region::NTSCJ: ui->ntscJRB->setChecked(true); break;
        case Region::PAL:   ui->palRB  ->setChecked(true); break;
        default: return;
    }
}

Region SkywardSwordWidget::region() const
{
    if (ui->regionBtnGroup->checkedButton() == ui->ntscURB)
        return Region::NTSCU;
    else if (ui->regionBtnGroup->checkedButton() == ui->ntscKRB)
        return Region::NTSCK;
    else if (ui->regionBtnGroup->checkedButton() == ui->ntscJRB)
        return Region::NTSCJ;
    else if (ui->regionBtnGroup->checkedButton() == ui->palRB)
        return Region::PAL;

    // Should never happen
    return SkywardSwordPlugin::instance()->settings()->defaultRegion();
}

void SkywardSwordWidget::onButtonToggled(QAbstractButton* btn, bool val)
{
    if (btn == ui->ntscURB && val)
        emit regionChanged(Region::NTSCU);
    if (btn == ui->ntscKRB && val)
        emit regionChanged(Region::NTSCK);
    if (btn == ui->ntscJRB && val)
        emit regionChanged(Region::NTSCJ);
    if (btn == ui->palRB && val)
        emit regionChanged(Region::PAL);
}
