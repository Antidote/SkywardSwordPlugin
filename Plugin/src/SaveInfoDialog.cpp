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

#include "SaveInfoDialog.hpp"
#include "ui_SaveInfoDialog.h"

#include "SkywardSwordGameDocument.hpp"
#include "SkywardSwordEditorForm.hpp"

#include <QFile>
#include <QRadioButton>
#include <QDebug>

SaveInfoDialog::SaveInfoDialog(SkywardSwordGameDocument* document, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveInfoDialog),
    m_document(document)
{
    ui->setupUi(this);
    connect(ui->regionBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(onRegionChanged(int)));
    m_region = m_document->region();
    QFile tmp(":/BannerData/banner.tpl");
    if (tmp.open(QFile::ReadOnly))
    {
        QDataStream dataStream(&tmp);
        char* bannerData = new char[192*64*2];
        dataStream.readRawData(bannerData, 192*64*2);
        ui->bannerImg->setPixmap(QPixmap::fromImage(convertTextureToImage(QByteArray(bannerData, 192*64*2), 192, 64)));
        ui->titleLbl->setText(regionString(m_region, Title));
        ui->subtitleLbl->setText(regionString(m_region, Subtitle));
        tmp.close();
    }
}

SaveInfoDialog::~SaveInfoDialog()
{
    delete ui;
}

void SaveInfoDialog::showEvent(QShowEvent *se)
{
    if (m_document == NULL)
    {
        QDialog::showEvent(se);
        return;
    }

    switch(m_document->region())
    {
    case Region::NTSCU:
        ui->ntscURB->setChecked(true);
    break;
    case Region::NTSCK:
        ui->ntscKRB->setChecked(true);
    break;
    case Region::NTSCJ:
        ui->ntscJRB->setChecked(true);
    break;
    case Region::PAL:
        ui->palRB->setChecked(true);
    break;
    default:
    break;
    }

    int count = 0;

    for (int i = 0; i < 3; i++)
    {
        if (m_document->quest(i) && !m_document->quest(i)->isNew())
            count++;
    }

    SkywardSwordQuestEditorForm* currentQuest = m_document->currentQuest();
    if (currentQuest)
    {
        ui->checkSumLbl->setText(tr("Adventure Checksum: 0x").append(QString("").sprintf("%08X", currentQuest->checksum())));
        ui->adventureCountLbl->setText(tr("Adventure Count: %1").arg(count));
        ui->currentAdventureLbl->setText(currentQuest->isNew() ? tr("New Adventure") : tr("Current Adventure: %1 - %2").arg(m_document->currentQuestIndex() + 1).arg(currentQuest->playerName()));
    }

    QDialog::showEvent(se);
}

void SaveInfoDialog::onRegionChanged(QAbstractButton* btn, bool val)
{
    if (btn == ui->ntscURB && val)
        m_region = Region::NTSCU;
    if (btn == ui->ntscKRB && val)
        m_region = Region::NTSCK;
    if (btn == ui->ntscJRB && val)
        m_region = Region::NTSCJ;
    if (btn == ui->palRB && val)
        m_region = Region::PAL;

    ui->titleLbl->setText(regionString(m_region, Title));
    ui->subtitleLbl->setText(regionString(m_region, Subtitle));
}

void SaveInfoDialog::accept()
{
    if (m_document)
        m_document->setRegion(m_region);


    QDialog::accept();
}

QString SaveInfoDialog::regionString(Region region, StringType type) const
{
    QString file;
    switch(type)
    {
    case Title:
        file = "title"; break;
    case Subtitle:
        file = "subtitle"; break;
    }

    qDebug() << (char)region;
    QFile title(QString(":/BannerData/%1/%2.bin").arg((char)region).arg(file));
    if (title.open(QFile::ReadOnly))
    {
        QString titleString = QString::fromUtf16((ushort*)title.readAll().data());
        title.close();
        return titleString;
    }

    return QString("");
}
