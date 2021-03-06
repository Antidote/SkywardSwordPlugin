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

#include "PlaytimeWidget.hpp"
#include "ui_PlaytimeWidget.h"

PlaytimeWidget::PlaytimeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaytimeWidget),
    m_blink(false)
{
    ui->setupUi(this);

    m_blinkTimer.setInterval(qApp->cursorFlashTime()); // Blink according to the cursor's flash time
    m_blinkTimer.start();
    connect(&m_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkText()));
}

PlaytimeWidget::~PlaytimeWidget()
{
    delete ui;
}

void PlaytimeWidget::setPlaytime(Playtime val)
{
    ui->hoursSpinBox->setValue(val.Hours);
    ui->minsSpinBox ->setValue(val.Minutes);
    ui->secsSpinBox ->setValue(val.Seconds);
    ui->msSpinBox   ->setValue(val.Milliseconds);
    ui->usSpinBox   ->setValue(val.Microseconds);
}

void PlaytimeWidget::blinkText()
{
    m_blink ^= 1; // Toggle blink

    if (this->isEnabled())
    {
        ui->minSepLbl->setProperty("blink", m_blink);
        ui->secSepLbl->setProperty("blink", m_blink);
        ui->msSepLbl ->setProperty("blink", m_blink);
        ui->usSepLbl ->setProperty("blink", m_blink);
        style()->polish(ui->minSepLbl);
        style()->polish(ui->secSepLbl);
        style()->polish(ui->msSepLbl);
        style()->polish(ui->usSepLbl);
    }
}

void PlaytimeWidget::valueChanged()
{
    Playtime ret;
    ret.Hours = ui->hoursSpinBox->value();
    ret.Minutes = ui->minsSpinBox->value();
    ret.Seconds = ui->secsSpinBox->value();
    ret.Milliseconds = ui->msSpinBox->value();
    ret.Microseconds = ui->usSpinBox->value();

    emit playtimeChanged(ret);
}


void PlaytimeWidget::clearTime()
{
    ui->hoursSpinBox->setValue(0);
    ui->minsSpinBox ->setValue(0);
    ui->secsSpinBox ->setValue(0);
    ui->msSpinBox   ->setValue(0);
    ui->usSpinBox   ->setValue(0);
}
