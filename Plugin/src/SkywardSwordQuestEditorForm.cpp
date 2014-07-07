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

#include "SkywardSwordQuestEditorForm.hpp"
#include "ui_SkywardSwordEditorForm.h"
#include "SkywardSwordGameDocument.hpp"
#include "SettingsManager.hpp"
#include "SkywardSwordPlugin.hpp"
#include "SkywardSwordWidget.hpp"
#include "SettingsDialog.hpp"
#include "Constants.hpp"

#include <QtEndian>
#include <Athena/Checksums.hpp>
#include <Athena/Utility.hpp>
#include <QSpacerItem>
#include <ImportExportQuestDialog.hpp>

SkywardSwordQuestEditorForm::SkywardSwordQuestEditorForm(const char *data, const char* skipData, Region region, QWidget* parent)
    : QWidget(parent),
      ui(new Ui::SkywardSwordEditorForm),
      m_region(region),
      m_gameData((char*)data),
      m_skipData((char*)skipData),
      m_skipChkColumn(0),
      m_skipChkRow(0)
{
    ui->setupUi(this);
    // Make sure the selected tab is "Player Info"
    ui->tabWidget->setCurrentIndex(0);
    connect(this, SIGNAL(modified()), this, SLOT(onModified()));
    updateData();
    updateQuestChecksum();
    updateSkipChecksum();

    buildSkipTab();

    SettingsDialog* settingsDialog = qobject_cast<SettingsDialog*>(SkywardSwordPlugin::instance()->settingsDialog());
    connect(settingsDialog, SIGNAL(skipDatabaseChanged()), this, SLOT(buildSkipTab()));


#ifdef SS_PREVIEW
    ui->previewLabel->setText("<center><b>PREVIEW BUILD</b></center>");
#elif defined(SS_INTERNAL)
    ui->previewLabel->setText("<center><b>INTERNAL BUILD</b></center>");
#else
    ui->previewLabel->setVisible(false);
#endif
}

SkywardSwordQuestEditorForm::~SkywardSwordQuestEditorForm()
{
    delete[] m_gameData;
    m_gameData = NULL;
    delete[] m_skipData;
    m_skipData = NULL;
    delete ui;
}

void SkywardSwordQuestEditorForm::setGameData(const QByteArray& data)
{
    if (data.size() != 0x53C0)
    {
        qWarning() << "Data must be exactly 0x53C0 bytes got" << hex << data.size() << "bytes";
        return;
    }
    memcpy(m_gameData, data.data(), data.size());
    emit modified();
}

char* SkywardSwordQuestEditorForm::gameData() const
{
    return m_gameData;
}

void SkywardSwordQuestEditorForm::setSkipData(const QByteArray& data)
{
    memcpy(m_skipData, data.data(), data.size());
    emit modified();
}

char* SkywardSwordQuestEditorForm::skipData() const
{
    return m_skipData;
}

int SkywardSwordQuestEditorForm::currentTab()
{
    return ui->tabWidget->currentIndex();
}

void SkywardSwordQuestEditorForm::setCurrentTab(int index)
{
    ui->tabWidget->setCurrentIndex(index);
}

void SkywardSwordQuestEditorForm::onDelete()
{
    QMessageBox mbox(this);
    mbox.setWindowTitle("Are you sure?");
    mbox.setText("Are you sure you wish to delete this game?<br />"
                 "This cannot be undone.(yet)");
    mbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    mbox.exec();

    if (mbox.result() == QMessageBox::Cancel)
        return;

    memset(m_gameData, 0x00, 0x53C0);
    memset(m_skipData, 0x00, 0x24);
    setNew(true);
    emit modified();
}

void SkywardSwordQuestEditorForm::onCreate()
{
    memset(m_gameData, 0x00, 0x53C0);
    memset(m_skipData, 0x00, 0x24);
    setPlayerName(SkywardSwordPlugin::instance()->settings()->defaultPlayerNameForRegion(m_region));
    setCurrentMap("F405");
    setCutsceneMode(true);
    ui->saveTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->curHPSpinBox->setValue(24);
    ui->unkHPSpinBox->setValue(24);
    ui->totalHPSpinBox->setValue(24);
    emit modified();
}

void SkywardSwordQuestEditorForm::onCopy()
{
    emit copy(this);
}

void SkywardSwordQuestEditorForm::onModified()
{
    if (!this->updatesEnabled())
        return;

    this->setUpdatesEnabled(false);
    updateQuestChecksum();
    updateSkipChecksum();
    updateData();
    this->setUpdatesEnabled(true);
}

void SkywardSwordQuestEditorForm::onCheckboxToggled()
{
    if (!this->updatesEnabled())
        return;

    QCheckBox* chkBox = qobject_cast<QCheckBox*>(sender());

    if (!chkBox)
        return;

    qDebug() << chkBox->objectName();
    if (chkBox->property("isCutscene").isValid() && chkBox->property("isCutscene").toBool())
    {
        bool ok = false;
        quint32 offset = chkBox->property("offset").toInt(&ok);
        if (ok)
        {
            quint32 bit = chkBox->property("bit").toInt(&ok);
            if (ok)
                setSkipBit(offset, bit, chkBox->isChecked());
        }
    }
}

void SkywardSwordQuestEditorForm::onImportExport()
{
    if (sender() == ui->importPushButton)
    {
        ImportExportQuestDialog importDiag(this, ImportExportQuestDialog::Import);
        importDiag.exec();
    }
    else if (sender() == ui->exportPushButton)
    {
        ImportExportQuestDialog exportDiag(this, ImportExportQuestDialog::Export);
        exportDiag.exec();
    }
}

void SkywardSwordQuestEditorForm::setRegion(Region region)
{
    m_region = region;
}

Playtime SkywardSwordQuestEditorForm::playtime() const
{
    if (!m_gameData)
        return Playtime();
    Playtime playTime;
    quint64 tmp = qFromBigEndian(*(quint64*)(m_gameData));
    playTime.Hours   = (((tmp / TICKS_PER_SECOND) / 60) / 60);
    playTime.Minutes = (( tmp / TICKS_PER_SECOND) / 60) % 60;
    playTime.Seconds = (( tmp / TICKS_PER_SECOND) % 60);
    return playTime;
}

void SkywardSwordQuestEditorForm::setPlaytime(Playtime val)
{
    quint64 totalSeconds = 0;
    totalSeconds += ( val.Hours   * 60) * 60;
    totalSeconds += ( val.Minutes * 60);
    totalSeconds +=   val.Seconds;
    *(quint64*)(m_gameData) = qToBigEndian(TICKS_PER_SECOND * totalSeconds);

    emit modified();
}

QDateTime SkywardSwordQuestEditorForm::savetime() const
{
    if (!m_gameData)
        return QDateTime::currentDateTime();

    return fromWiiTime(qFromBigEndian(*(quint64*)(m_gameData + 0x0008))).addSecs(-(60*60));
}

void SkywardSwordQuestEditorForm::setSavetime(const QDateTime& time)
{
    if (!m_gameData)
        return;
    if (time == savetime())
        return;

    *(qint64*)(m_gameData + 0x0008) = qToBigEndian<qint64>(toWiiTime(time.addSecs((60*60))));
    emit modified();
}

QString SkywardSwordQuestEditorForm::currentMap() const
{
    return QString::fromLatin1(m_gameData + 0x531C);
}

void SkywardSwordQuestEditorForm::setCurrentMap(const QString& map)
{
    if (map == currentMap())
        return;

    memcpy(m_gameData + 0x531C, map.toLatin1().data(), map.length());
    if (map.length() < 32)
        memset(m_gameData + (0x531C + map.length()), 0, 32 - map.length());

    emit modified();
}

QString SkywardSwordQuestEditorForm::currentArea() const
{
    return QString::fromLatin1(m_gameData + 0x533C);
}

void SkywardSwordQuestEditorForm::setCurrentArea(const QString& area)
{
    if (area == currentArea())
        return;

    memcpy(m_gameData + 0x533C, area.toLatin1().data(), area.length());
    if (area.length() < 32)
        memset(m_gameData + (0x533C + area.length()), 0, 32 - area.length());

    emit modified();
}

QString SkywardSwordQuestEditorForm::currentRoom() const
{
    return QString::fromLatin1(m_gameData + 0x535C);
}

void SkywardSwordQuestEditorForm::setCurrentRoom(const QString& room)
{
    if (room == currentRoom())
        return;

    memcpy(m_gameData + 0x535C, room.toLatin1().data(), room.length());
    if (room.length() < 32)
        memset(m_gameData + (0x535C + room.length()), 0, 32 - room.length());
    emit modified();
}

int SkywardSwordQuestEditorForm::roomID() const
{
    return (uint)(*(uchar*)(m_gameData + 0x5309));
}

void SkywardSwordQuestEditorForm::setRoomID(int id)
{
    if (id == roomID())
        return;

    (*(char*)(m_gameData + 0x5309)) = (char)id;
    emit modified();
}

bool SkywardSwordQuestEditorForm::watchedIntro()
{
    return flag(0x0941, 0x02);
}

void SkywardSwordQuestEditorForm::setIntroWatched(bool val)
{
    if (val == watchedIntro())
        return;

    setFlag(0x0941, 0x02, val);
}

bool SkywardSwordQuestEditorForm::isNight()
{
    return flag(0x53B3, 0x01);
}

void SkywardSwordQuestEditorForm::setNight(bool val)
{
    if (val == isNight())
        return;

    setFlag(0x53B3, 0x01, val);
}

bool SkywardSwordQuestEditorForm::isHeroMode()
{
    return flag(0x08FE, 0x08);
}

void SkywardSwordQuestEditorForm::setHeroMode(bool val)
{
    setFlag(0x08FE, 0x08, val);
}

QString SkywardSwordQuestEditorForm::playerName() const
{
    if (!m_gameData)
        return QString("");

    // tmpName has 8 characters
    // the ninth is for the null terminator
    // this prevents garbage data from being displayed
    ushort tmpName[9];
    tmpName[8] = 0;
    for (int i = 0, j=0; i < 8; ++i, j+= 2)
    {
        tmpName[i] = *(quint16*)(m_gameData + (0x08D4 + j));
        tmpName[i] = qFromBigEndian(tmpName[i]);
    }

    return QString(QString::fromUtf16(tmpName));
}

void SkywardSwordQuestEditorForm::setPlayerName(const QString &name)
{
    if (!m_gameData)
        return;


    if (name == playerName())
        return;

    for (int i = 0, j = 0; i < 8; ++i, ++j)
    {
        if (i > name.length())
        {
            *(ushort*)(m_gameData + (0x08D4 + j++)) = 0;
            continue;
        }
        *(ushort*)(m_gameData + (0x08D4 + j++)) = qToBigEndian<quint16>(name.utf16()[i]);
    }

    emit modified();
}

quint32 SkywardSwordQuestEditorForm::rupees() const
{
    if (!m_gameData)
        return 0;

    return qFromBigEndian(*(quint16*)(m_gameData + 0x0A5E));
}

void SkywardSwordQuestEditorForm::setRupees(int val)
{
    if (!m_gameData)
        return;

    if ((quint32)val == rupees())
        return;

    *(quint16*)(m_gameData + 0x0A5E) = qToBigEndian((quint16)val);
    emit modified();
}

int SkywardSwordQuestEditorForm::totalHP()
{
    return (int)qFromBigEndian(*(quint16*)(m_gameData + 0x5302));
}

void SkywardSwordQuestEditorForm::setTotalHP(int val)
{
    if (val == totalHP())
        return;

    *(quint16*)(m_gameData + 0x5302) = qToBigEndian((quint16)val);
    emit modified();
}

quint32 SkywardSwordQuestEditorForm::unkHP()
{
    return (int)qFromBigEndian(*(quint16*)(m_gameData + 0x5304));
}

void SkywardSwordQuestEditorForm::setUnkHP(int val)
{
    if (val == (int)unkHP())
        return;

    *(quint16*)(m_gameData + 0x5304) = qToBigEndian((quint16)val);
    emit modified();
}

quint32 SkywardSwordQuestEditorForm::currentHP()
{
    return (int)qFromBigEndian(*(quint16*)(m_gameData + 0x5306));
}

void SkywardSwordQuestEditorForm::setCurrentHP(int val)
{
    if (val == (int)currentHP())
        return;

    *(quint16*)(m_gameData + 0x5306) = qToBigEndian((quint16)val);
    emit modified();
}

float SkywardSwordQuestEditorForm::player(Coord coord)
{
    float x     = *(float*)(m_gameData + 0x0010);
    float y     = *(float*)(m_gameData + 0x0014);
    float z     = *(float*)(m_gameData + 0x0018);
    float roll  = *(float*)(m_gameData + 0x001C);
    float pitch = *(float*)(m_gameData + 0x0020);
    float yaw   = *(float*)(m_gameData + 0x0024);
    if (!Athena::utility::isSystemBigEndian())
    {
        x = Athena::utility::swapFloat(x);
        y = Athena::utility::swapFloat(y);
        z = Athena::utility::swapFloat(z);
        roll = Athena::utility::swapFloat(roll);
        pitch = Athena::utility::swapFloat(pitch);
        yaw = Athena::utility::swapFloat(yaw);
    }

    switch(coord)
    {
        case XCoord:    return x;
        case YCoord:    return y;
        case ZCoord:    return z;
        case RollCoord: return roll;
        case PitchCoord:return pitch;
        case YawCoord:  return yaw;
        default:        return 0.f;
    }
}

float SkywardSwordQuestEditorForm::camera(Coord coord)
{
    float x     = *(float*)(m_gameData + 0x0028);
    float y     = *(float*)(m_gameData + 0x002C);
    float z     = *(float*)(m_gameData + 0x0030);
    float roll  = *(float*)(m_gameData + 0x0034);
    float pitch = *(float*)(m_gameData + 0x0038);
    float yaw   = *(float*)(m_gameData + 0x003C);
    if (!Athena::utility::isSystemBigEndian())
    {
        x     = Athena::utility::swapFloat(x);
        y     = Athena::utility::swapFloat(y);
        z     = Athena::utility::swapFloat(z);
        roll  = Athena::utility::swapFloat(roll);
        pitch = Athena::utility::swapFloat(pitch);
        yaw   = Athena::utility::swapFloat(yaw);
    }

    switch(coord)
    {
        case XCoord:    return x;
        case YCoord:    return y;
        case ZCoord:    return z;
        case RollCoord: return roll;
        case PitchCoord:return pitch;
        case YawCoord:  return yaw;
        default:        return 0.f;
    }
}

void SkywardSwordQuestEditorForm::setCoord(double val)
{
    if (!updatesEnabled())
        return;

    float coord = (float)val;
    if (!Athena::utility::isSystemBigEndian())
        coord = Athena::utility::swapFloat(coord);

    if (sender() == ui->playerXSpinBox)
        *(float*)(m_gameData + 0x0010) = coord;
    if (sender() == ui->playerYSpinBox)
        *(float*)(m_gameData + 0x0014) = coord;
    if (sender() == ui->playerZSpinBox)
        *(float*)(m_gameData + 0x0018) = coord;
    if (sender() == ui->playerRollSpinBox)
        *(float*)(m_gameData + 0x001C) = coord;
    if (sender() == ui->playerPitchSpinBox)
        *(float*)(m_gameData + 0x0020) = coord;
    if (sender() == ui->playerYawSpinBox)
        *(float*)(m_gameData + 0x0024) = coord;
    if (sender() == ui->cameraXSpinBox)
        *(float*)(m_gameData + 0x0028) = coord;
    if (sender() == ui->cameraYSpinBox)
        *(float*)(m_gameData + 0x002C) = coord;
    if (sender() == ui->cameraZSpinBox)
        *(float*)(m_gameData + 0x0030) = coord;
    if (sender() == ui->cameraRollSpinBox)
        *(float*)(m_gameData + 0x0034) = coord;
    if (sender() == ui->cameraPitchSpinBox)
        *(float*)(m_gameData + 0x0038) = coord;
    if (sender() == ui->cameraYawSpinBox)
        *(float*)(m_gameData + 0x003C) = coord;

    emit modified();
}

bool SkywardSwordQuestEditorForm::beetle(BeetleType type)
{
    switch(type)
    {
        case NormalBeetle:return flag(0x09E6, 0x20);
        case HookBeetle:  return flag(0x09EB, 0x02);
        case QuickBeetle: return flag(0x09EB, 0x04);
        case ToughBeetle: return flag(0x09EB, 0x08);
        default:    return false;
    }
}

void SkywardSwordQuestEditorForm::setBeetle(bool val)
{
    if (!updatesEnabled())
        return;

    if (sender() == ui->beetleChkBox)
    {
        if (!val)
            ui->hookBeetleChkBox->setChecked(false);

        setFlag(0x09E6, 0x20, val);
    }
    else if (sender() == ui->hookBeetleChkBox)
    {
        if (val)
            ui->beetleChkBox->setChecked(true);

        if (!val)
            ui->quickBeetleChkBox->setChecked(false);

        setFlag(0x09EB, 0x02, val);
    }
    else if (sender() == ui->quickBeetleChkBox)
    {
        if (val)
            ui->hookBeetleChkBox->setChecked(true);
        if (!val)
            ui->toughBeetleChkBox->setChecked(false);

        setFlag(0x09EB, 0x04, val);
    }
    else if (sender() == ui->toughBeetleChkBox)
    {
        if (val)
            ui->quickBeetleChkBox->setChecked(true);

        setFlag(0x09EB, 0x08, val);
    }
}

bool SkywardSwordQuestEditorForm::bow(BowType bow)
{
    switch (bow)
    {
        case NormalBow:return flag(0x09E4, 0x10);
        case IronBow:  return flag(0x09ED, 0x01);
        case SacredBow:return flag(0x09ED, 0x02);
        default:       return false;
    }
}

void SkywardSwordQuestEditorForm::setBow(bool val)
{
    if (!updatesEnabled())
        return;

    if (sender() == ui->bowChkBox)
    {
        if (!val)
            ui->ironBowChkBox->setChecked(false);

        setFlag(0x09E4, 0x10, val);
    }
    else if (sender() == ui->ironBowChkBox)
    {
        if (val)
            ui->bowChkBox->setChecked(true);

        if (!val)
            ui->sacredBowChkBox->setChecked(false);

        setFlag(0x09ED, 0x01, val);
    }
    else if (sender() == ui->sacredBowChkBox)
    {
        if (val)
            ui->ironBowChkBox->setChecked(true);

        setFlag(0x09ED, 0x02, val);
    }
}

bool SkywardSwordQuestEditorForm::bugnet(SkywardSwordQuestEditorForm::BugnetType type)
{
    switch(type)
    {
        case Bugnet:    return flag(0x09E8, 0x01);
        case BigBugnet: return flag(0x09F2, 0x02);
        default:        return false;
    }
}

void SkywardSwordQuestEditorForm::setBugnet(bool val)
{
    if (!updatesEnabled())
        return;

    if (sender() == ui->bugNetChkBox)
    {
        if (!val)
            ui->bigBugNetChkBox->setChecked(false);

        setFlag(0x09E8, 0x01, val);
    }
    else if (sender() == ui->bigBugNetChkBox)
    {
        if (val)
            ui->bugNetChkBox->setChecked(true);

        setFlag(0x09F2, 0x02, val);
    }
}

bool SkywardSwordQuestEditorForm::gustBellows()
{
    return flag(0x09E6, 0x02);
}

void SkywardSwordQuestEditorForm::setGustBellows(bool val)
{
    if (!updatesEnabled())
        return;
    setFlag(0x09E6, 0x02, val);
}

bool SkywardSwordQuestEditorForm::goddessHarp()
{

    return flag(0x09F4, 0x02);
}

void SkywardSwordQuestEditorForm::setGoddessHarp(bool val)
{
    if (!updatesEnabled())
        return;
    setFlag(0x09F4, 0x02, val);
}

bool SkywardSwordQuestEditorForm::whip()
{
    return flag(0x09F3, 0x10);
}

void SkywardSwordQuestEditorForm::setWhip(bool val)
{
    if (!updatesEnabled())
        return;
    setFlag(0x09F3, 0x10, val);
}

bool SkywardSwordQuestEditorForm::clawshot()
{
    return flag(0x09E4, 0x20);
}

void SkywardSwordQuestEditorForm::setClawshot(bool val)
{
    if (!updatesEnabled())
        return;

    setFlag(0x09E4, 0x20, val);
}

bool SkywardSwordQuestEditorForm::diggingMitts()
{
    return flag(0x09E6, 0x40);
}

void SkywardSwordQuestEditorForm::setDiggingMitts(bool val)
{
    if (!updatesEnabled())
        return;

    if (!val)
        setMoleMitts(false);

    setFlag(0x09E6, 0x40, val);
}

bool SkywardSwordQuestEditorForm::moleMitts()
{
    return flag(0x09EC, 0x02);
}

void SkywardSwordQuestEditorForm::setMoleMitts(bool val)
{
    if (!updatesEnabled())
        return;

    if (val)
        setDiggingMitts(true);

    setFlag(0x09EC, 0x02, val);
}

bool SkywardSwordQuestEditorForm::sailCloth()
{
    return flag(0x09F4, 0x01);
}

void SkywardSwordQuestEditorForm::setSailCloth(bool val)
{
    setFlag(0x09F4, 0x01, val);
}

bool SkywardSwordQuestEditorForm::waterDragonScale()
{
    return flag(0x09E9, 0x20);
}

void SkywardSwordQuestEditorForm::setWaterDragonScale(bool val)
{
    setFlag(0x09E9, 0x20, val);
}

bool SkywardSwordQuestEditorForm::fireShieldEarings()
{
    return flag(0x09F3, 0x20);
}

void SkywardSwordQuestEditorForm::setFireShieldEarings(bool val)
{
    setFlag(0x09F3, 0x20, val);
}

bool SkywardSwordQuestEditorForm::bombs()
{
    return flag(0x09ED, 0x04);
}

void SkywardSwordQuestEditorForm::setBombs(bool val)
{
    setFlag(0x09ED, 0x04, val);
}

bool SkywardSwordQuestEditorForm::slingshot(SlingshotType type)
{
    switch(type)
    {
        case Slingshot:  return flag(0x09E6, 0x10);
        case Scattershot:return flag(0x09EC, 0x80);
        default: return false;
    }
}

void SkywardSwordQuestEditorForm::setSlingshot(bool val)
{
    if (!updatesEnabled())
        return;

    if (sender() == ui->slingShotChkBox)
    {
        if (!val)
            ui->scatterShotChkBox->setChecked(false);

        setFlag(0x09E6, 0x10, val);
    }
    else if (sender() == ui->scatterShotChkBox)
    {

        if (val)
            ui->slingShotChkBox->setChecked(true);

        setFlag(0x09EC, 0x80, val);
    }
}

int SkywardSwordQuestEditorForm::ammo(SkywardSwordQuestEditorForm::AmmoType type)
{
    // Nintendo uses an interesting encoding for the ammo counts
    // 11111111 00111111 10000000 00111111
    // | `----|   `----| `---------------|
    // |      `Arrows  |  Seeds
    // `---------------- Bombs
    // Why nintendo decided to use this kind of encoding is a mystery,
    // but fortunately it's easier to decode than it is to understand
    quint32 val = qFromBigEndian(*(quint32*)(m_gameData + 0x0A60));
    quint32 ret = 0;

    switch(type)
    {
        case Arrows: ret = (val >> 0 ) & 127; break;
        case Bombs:  ret = (val >> 7 ) & 127; break;
        case Seeds:  ret = (val >> 23) & 127; break;
    }

    return ret;
}

void SkywardSwordQuestEditorForm::setAmmo(int val)
{
    if (!updatesEnabled())
        return;

    // Nintendo uses an interesting encoding for the ammo counts
    // 11111111 00111111 10000000 00111111
    // | `----|   `----| `---------------|
    // |      `Arrows  |  Seeds
    // `---------------- Bombs
    // Why nintendo decided to use this kind of encoding is a mystery,
    // but fortunately it's easier to decode than it is to understand

    quint32 tmp = qFromBigEndian(*(quint32*)(m_gameData + 0x0A60));
    quint32 arrows = (tmp >> 0)  & 127;
    quint32 bombs  = (tmp >> 7)  & 127;
    quint32 seeds  = (tmp >> 23) & 127;

    if (sender() == ui->arrowAmmoSpinBox)
        arrows = val;
    else if (sender() == ui->bombAmmoSpinBox)
        bombs = val;
    else if (sender() == ui->seedAmmoSpinBox)
        seeds = val;

    *(quint32*)(m_gameData + 0x0A60) = qToBigEndian(arrows | (bombs << 7) | (seeds << 23));

    emit modified();
}

bool SkywardSwordQuestEditorForm::practiceSword()
{
    return flag(0x09F2, 0x01);
}

void SkywardSwordQuestEditorForm::setPracticeSword(bool val)
{
    if (!updatesEnabled())
        return;

    if (!val)
        setGoddessSword(false);

    setFlag(0x09F2, 0x01, val);
}

bool SkywardSwordQuestEditorForm::goddessSword()
{
    return flag(0x09E4, 0x01);
}

void SkywardSwordQuestEditorForm::setGoddessSword(bool val)
{
    if (!updatesEnabled())
        return;

    if (val)
        setPracticeSword(true);

    if (!val)
        setGoddessLongSword(false);

    setFlag(0x09E4, 0x01, val);
}

bool SkywardSwordQuestEditorForm::goddessLongSword()
{
    return flag(0x09E4, 0x02);
}

void SkywardSwordQuestEditorForm::setGoddessLongSword(bool val)
{
    if (!updatesEnabled())
        return;

    if (val)
        setGoddessSword(true);

    if (!val)
        setGoddessWhiteSword(false);
    setFlag(0x09E4, 0x02, val);
}

bool SkywardSwordQuestEditorForm::goddessWhiteSword()
{
    return flag(0x09FB, 0x10);
}

void SkywardSwordQuestEditorForm::setGoddessWhiteSword(bool val)
{
    if (!updatesEnabled())
        return;

    if (val)
        setGoddessLongSword(true);

    if (!val)
        setMasterSword(false);

    setFlag(0x09FB, 0x10, val);
}

bool SkywardSwordQuestEditorForm::masterSword()
{
    return flag(0x09E4, 0x04);
}

void SkywardSwordQuestEditorForm::setMasterSword(bool val)
{
    if (!updatesEnabled())
        return;

    if (val)
        setGoddessWhiteSword(true);
    if (!val)
        setTrueMasterSword(false);

    setFlag(0x09E4, 0x04, val);
}

bool SkywardSwordQuestEditorForm::trueMasterSword()
{
    return flag(0x09F3, 0x80);
}

void SkywardSwordQuestEditorForm::setTrueMasterSword(bool val)
{
    if (!updatesEnabled())
        return;

    if (val)
        setMasterSword(true);

    setFlag(0x09F3, 0x080, val);
}

bool SkywardSwordQuestEditorForm::wallet(WalletType type)
{
    switch(type)
    {
        case SmallWallet:  return true;
        case MediumWallet: return flag(0x09EF, 0x04);
        case BigWallet:    return flag(0x09EF, 0x08);
        case GiantWallet:  return flag(0x09EF, 0x10);
        case TycoonWallet: return flag(0x09EF, 0x20);
    }

    return false;
}

void SkywardSwordQuestEditorForm::setWallet(bool val)
{
    if (!updatesEnabled())
        return;

    if (sender() == ui->smallWalletChkBox)
        emit modified();
    if (sender() == ui->mediumWalletChkBox)
        setFlag(0x09EF, 0x04, val);
    else if (sender() == ui->bigWalletChkBox)
        setFlag(0x09EF, 0x08, val);
    else if (sender() == ui->giantWalletChkBox)
        setFlag(0x09EF, 0x10, val);
    else if (sender() == ui->tycoonWalletChkBox)
        setFlag(0x09Ef, 0x20, val);
}

bool SkywardSwordQuestEditorForm::triforce(TriforceWidget::TriforcePiece piece)
{
    switch(piece)
    {
        case TriforceWidget::Courage:return flag(0x09ED, 0x20);
        case TriforceWidget::Power:  return flag(0x09ED, 0x40);
        case TriforceWidget::Wisdom: return flag(0x09ED, 0x80);
        default: return false;
    }
}

void SkywardSwordQuestEditorForm::triforceClicked()
{
    if (!updatesEnabled())
        return;

    TriforceWidget* tw = qobject_cast<TriforceWidget*>(ui->triforceWidget);
    if (tw)
    {
        if (tw->isPowerChecked() != triforce(TriforceWidget::Power))
            setFlag(0x09ED, 0x40, tw->isPowerChecked());
        else if (tw->isCourageChecked() != triforce(TriforceWidget::Courage))
            setFlag(0x09ED, 0x20, tw->isCourageChecked());
        else if ((tw->isWisdomChecked() != triforce(TriforceWidget::Wisdom)))
            setFlag(0x09ED, 0x80, tw->isWisdomChecked());
    }
}

bool SkywardSwordQuestEditorForm::bug(SkywardSwordQuestEditorForm::BugType bug)
{
    switch(bug)
    {
        case DekuHornet:         return flag(0x08F6, 0x80);
        case BlessedButterfly:   return flag(0x09F2, 0x80);
        case GerudoDragonfly:    return flag(0x09F5, 0x04);
        case StarryFirefly:      return flag(0x09F5, 0x20);
        case WoodlandRhinoBeetle:return flag(0x09F2, 0x08);
        case VolcanicLadybug:    return flag(0x09F2, 0x40);
        case SandCicada:         return flag(0x09F5, 0x02);
        case SkyStagBeetle:      return flag(0x09F5, 0x10);
        case FaronGrasshopper:   return flag(0x09F2, 0x04);
        case SkyloftMantis:      return flag(0x09F2, 0x20);
        case LanayruAnt:         return flag(0x09F5, 0x01);
        case EldinRoller:        return flag(0x09F5, 0x08);
        default:                 return false;
    }
}

void SkywardSwordQuestEditorForm::setBug(bool val)
{
    if (!updatesEnabled())
        return;

    if (sender() == ui->hornetChkBox)
        setFlag(0x08F6, 0x80, val);
    if (sender() == ui->butterflyChkBox)
        setFlag(0x09F2, 0x80, val);
    if (sender() == ui->dragonflyChkBox)
        setFlag(0x09F5, 0x04, val);
    if (sender() == ui->fireflyChkBox)
        setFlag(0x09F5, 0x20, val);
    if (sender() == ui->rhinoBeetleChkBox)
        setFlag(0x09F2, 0x08, val);
    if (sender() == ui->ladybugChkBox)
        setFlag(0x09F2, 0x40, val);
    if (sender() == ui->sandCicadaChkBox)
        setFlag(0x09F5, 0x02, val);
    if (sender() == ui->stagBeetleChkBox)
        setFlag(0x09F5, 0x10, val);
    if (sender() == ui->grasshopperChkBox)
        setFlag(0x09F2, 0x04, val);
    if (sender() == ui->mantisChkBox)
        setFlag(0x09F2, 0x20, val);
    if (sender() == ui->antChkBox)
        setFlag(0x09F5, 0x01, val);
    if (sender() == ui->eldinRollerChkBox)
        setFlag(0x09F5, 0x08, val);
}

int SkywardSwordQuestEditorForm::bugAmount(SkywardSwordQuestEditorForm::BugType bug)
{
    switch(bug)
    {
        case DekuHornet:          return quantity(true,  0x0A4C);
        case BlessedButterfly:    return quantity(false, 0x0A4A);
        case GerudoDragonfly:     return quantity(true,  0x0A46);
        case StarryFirefly:       return quantity(false, 0x0A44);
        case WoodlandRhinoBeetle: return quantity(false, 0x0A4E);
        case VolcanicLadybug:     return quantity(true,  0x0A4A);
        case SandCicada:          return quantity(false, 0x0A48);
        case SkyStagBeetle:       return quantity(true,  0x0A44);
        case FaronGrasshopper:    return quantity(true,  0x0A4E);
        case SkyloftMantis:       return quantity(false, 0x0A4C);
        case LanayruAnt:          return quantity(true,  0x0A48);
        case EldinRoller:         return quantity(false, 0x0A46);
        default:                  return 0;
    }
}

void SkywardSwordQuestEditorForm::setBugAmount(int val)
{
    if (!updatesEnabled())
        return;

    if (val > 99)
        sender()->setProperty("valid", false);
    else
        sender()->setProperty("valid", true);

    QWidget* w = qobject_cast<QWidget*>(sender());
    if (w)
        style()->polish(w);

    if (sender() == ui->hornetSpinBox)
        setQuantity(true,  0x0A4C, val);
    if (sender() == ui->butterflySpinBox)
        setQuantity(false, 0x0A4A, val);
    if (sender() == ui->dragonflySpinBox)
        setQuantity(true,  0x0A46, val);
    if (sender() == ui->fireflySpinBox)
        setQuantity(false, 0x0A44, val);
    if (sender() == ui->rhinoBeetleSpinBox)
        setQuantity(false, 0x0A4E, val);
    if (sender() == ui->ladybugSpinBox)
        setQuantity(true,  0x0A4A, val);
    if (sender() == ui->sandCicadaSpinBox)
        setQuantity(false, 0x0A48, val);
    if (sender() == ui->stagBeetleSpinBox)
        setQuantity(true,  0x0A44, val);
    if (sender() == ui->grasshopperSpinBox)
        setQuantity(true,  0x0A4E, val);
    if (sender() == ui->mantisSpinBox)
        setQuantity(false, 0x0A4C, val);
    if (sender() == ui->antSpinBox)
        setQuantity(true,  0x0A48, val);
    if (sender() == ui->eldinRollerSpinBox)
        setQuantity(false, 0x0A46, val);
}

bool SkywardSwordQuestEditorForm::material(MaterialType material)
{
    switch(material)
    {
        case DekuHornetLarvae:return flag(0x0934, 0x02);
        case BirdFeather:     return flag(0x0934, 0x04);
        case TumbleWeed:      return flag(0x0934, 0x08);
        case LizardTail:      return flag(0x0934, 0x10);
        case EldinOre:        return flag(0x0934, 0x20);
        case AncientFlower:   return flag(0x0934, 0x40);
        case AmberRelic:      return flag(0x0934, 0x80);
        case DuskRelic:       return flag(0x0937, 0x01);
        case JellyBlob:       return flag(0x0937, 0x02);
        case MonsterClaw:     return flag(0x0937, 0x04);
        case MonsterHorn:     return flag(0x0937, 0x08);
        case OrnamentalSkull: return flag(0x0937, 0x10);
        case EvilCrystal:     return flag(0x0937, 0x20);
        case BlueBirdFeather: return flag(0x0937, 0x40);
        case GoldenSkull:     return flag(0x0937, 0x80);
        case GoddessPlume:    return flag(0x0936, 0x01);
        default:              return false;
    }
}

void SkywardSwordQuestEditorForm::setMaterial(bool val)
{
    if (!updatesEnabled())
        return;

    if (sender() == ui->hornetLarvaeChkBox)
        setFlag(0x0934, 0x02, val);
    else if (sender() == ui->birdFeatherChkBox)
        setFlag(0x0934, 0x04, val);
    else if (sender() == ui->tumbleWeedChkBox)
        setFlag(0x0934, 0x08, val);
    else if (sender() == ui->lizardTailChkBox)
        setFlag(0x0934, 0x10, val);
    else if (sender() == ui->eldinOreChkBox)
        setFlag(0x0934, 0x20, val);
    else if (sender() == ui->ancientFlowerChkBox)
        setFlag(0x0934, 0x40, val);
    else if (sender() == ui->amberRelicChkBox)
        setFlag(0x0934, 0x80, val);
    else if (sender() == ui->duskRelicChkBox)
        setFlag(0x0937, 0x01, val);
    else if (sender() == ui->jellyBlobChkBox)
        setFlag(0x0937, 0x02, val);
    else if (sender() == ui->monsterClawChkBox)
        setFlag(0x0937, 0x04, val);
    else if (sender() == ui->monsterHornChkBox)
        setFlag(0x0937, 0x08, val);
    else if (sender() == ui->decoSkullChkBox)
        setFlag(0x0937, 0x10, val);
    else if (sender() == ui->evilCrystalChkBox)
        setFlag(0x0937, 0x20, val);
    else if (sender() == ui->blueBirdFeatherChkBox)
        setFlag(0x0937, 0x40, val);
    else if (sender() == ui->goldenSkullChkBox)
        setFlag(0x0937, 0x80, val);
    else if (sender() == ui->goddessPlumeChkBox)
        setFlag(0x0936, 0x01, val);
}

int SkywardSwordQuestEditorForm::materialAmount(MaterialType material)
{
    switch(material)
    {
        case DekuHornetLarvae: return quantity(true, 0x0A42);
        case BirdFeather:      return quantity(false,0x0A42);
        case TumbleWeed:       return quantity(true, 0x0A40);
        case LizardTail:       return quantity(false,0x0A40);
        case EldinOre:         return quantity(true, 0x0A3E);
        case AncientFlower:    return quantity(false,0x0A3E);
        case AmberRelic:       return quantity(true, 0x0A3C);
        case DuskRelic:        return quantity(false,0x0A3C);
        case JellyBlob:        return quantity(true, 0x0A3A);
        case MonsterClaw:      return quantity(false,0x0A3A);
        case MonsterHorn:      return quantity(true, 0x0A38);
        case OrnamentalSkull:  return quantity(false,0x0A38);
        case EvilCrystal:      return quantity(true, 0x0A36);
        case BlueBirdFeather:  return quantity(false,0x0A36);
        case GoldenSkull:      return quantity(true, 0x0A34);
        case GoddessPlume:     return quantity(false,0x0A34);
        default:               return 0;
    }
}

void SkywardSwordQuestEditorForm::setMaterialAmount(int val)
{
    if (!updatesEnabled())
        return;

    if (val > 99)
        sender()->setProperty("valid", false);
    else
        sender()->setProperty("valid", true);

    QWidget* w = qobject_cast<QWidget*>(sender());
    if (w)
        style()->polish(w);

    if (sender() == ui->hornetLarvaeSpinBox)
        setQuantity(true,  0x0A42, val);
    else if (sender() == ui->birdFeatherSpinBox)
        setQuantity(false, 0x0A42, val);
    else if (sender() == ui->tumbleWeedSpinBox)
        setQuantity(true,  0x0A40, val);
    else if (sender() == ui->lizardTailSpinBox)
        setQuantity(false, 0x0A40, val);
    else if (sender() == ui->eldinOreSpinBox)
        setQuantity(true,  0x0A3E, val);
    else if (sender() == ui->ancientFlowerSpinBox)
        setQuantity(false, 0x0A3E, val);
    else if (sender() == ui->amberRelicSpinBox)
        setQuantity(true,  0x0A3C, val);
    else if (sender() == ui->duskRelicSpinBox)
        setQuantity(false, 0x0A3C, val);
    else if (sender() == ui->jellyBlobSpinBox)
        setQuantity(true,  0x0A3A, val);
    else if (sender() == ui->monsterClawSpinBox)
        setQuantity(false, 0x0A3A, val);
    else if (sender() == ui->monsterHornSpinBox)
        setQuantity(true,  0x0A38, val);
    else if (sender() == ui->decoSkullSpinBox)
        setQuantity(false, 0x0A38, val);
    else if (sender() == ui->evilCrystalSpinBox)
        setQuantity(true,  0x0A36, val);
    else if (sender() == ui->blueBirdFeatherSpinBox)
        setQuantity(false, 0x0A36, val);
    else if (sender() == ui->goldenSkullSpinBox)
        setQuantity(true,  0x0A34, val);
    else if (sender() == ui->goddessPlumeSpinBox)
        setQuantity(false, 0x0A34, val);
}

int SkywardSwordQuestEditorForm::gratitudeCrystals()
{
    return (int)((qFromBigEndian<quint16>(*(quint16*)(m_gameData + 0x0A50)) >> 3) & 127);
}

void SkywardSwordQuestEditorForm::setGratitudeCrystals(int val)
{
    if (!updatesEnabled())
        return;

    if (val > 127)
    {
        ui->gratitudeCrystalsSpinBox->setProperty("valid", false);
        style()->polish(ui->gratitudeCrystalsSpinBox);
        return;
    }
    else
    {
        ui->gratitudeCrystalsSpinBox->setProperty("valid", true);
        style()->polish(ui->gratitudeCrystalsSpinBox);
    }

    quint16 oldVal = qFromBigEndian(*(quint16*)(m_gameData + 0x0A50)) & 0xFC00;
    *(quint16*)(m_gameData + 0x0A50) = qToBigEndian((quint16)(oldVal | (((quint16)val << 3) & 0x03FF)));
    emit modified();
}

bool SkywardSwordQuestEditorForm::isCutsceneMode()
{
    if (!m_gameData)
        return true;

    return flag(0x53A9, 0x01);
}

void SkywardSwordQuestEditorForm::setCutsceneMode(bool isCutsceneMode)
{
    if (!m_gameData)
        return;

    setFlag(0x53A9, 0x01, isCutsceneMode);
}

bool SkywardSwordQuestEditorForm::isNew()
{
    if (!m_gameData)
        return true;

    return flag(0x53AD, 0x01);
}

void SkywardSwordQuestEditorForm::setNew(bool isNew)
{
    if (!m_gameData)
        return;

    setFlag(0x53AD, 0x01, isNew);
}

int SkywardSwordQuestEditorForm::checksum()
{
    return qFromBigEndian(*(quint32*)(m_gameData + 0x53BC));
}

void SkywardSwordQuestEditorForm::updateQuestChecksum()
{
    int oldChecksum = checksum();
    *(quint32*)(m_gameData + 0x53BC) = qToBigEndian((quint32)Athena::Checksums::crc32((atUint8*)m_gameData, 0x53BC));

    if (checksum() != oldChecksum)
        emit modified();
}

int SkywardSwordQuestEditorForm::skipChecksum()
{
    return qFromBigEndian(*(quint32*)(m_skipData + 0x20));
}

void SkywardSwordQuestEditorForm::updateSkipChecksum()
{
    int oldChecksum = skipChecksum();
    *(quint32*)(m_skipData + 0x20) = qToBigEndian((quint32)Athena::Checksums::crc32((atUint8*)m_skipData, 0x20));

    if (skipChecksum() != oldChecksum)
        emit modified();
}

void SkywardSwordQuestEditorForm::setAllSkips()
{
    SettingsDialog* settingsDialog = qobject_cast<SettingsDialog*>(SkywardSwordPlugin::instance()->settingsDialog());

    for (int i = 0; i <  settingsDialog->skipDatabase().count(); i++)
    {
        if (skipBit(settingsDialog->skipDatabase().at(i).offset, settingsDialog->skipDatabase().at(i).bit))
            continue;
        setSkipBit(settingsDialog->skipDatabase().at(i).offset, settingsDialog->skipDatabase().at(i).bit, true);
    }
}

void SkywardSwordQuestEditorForm::clearAllSkips()
{
    SettingsDialog* settingsDialog = qobject_cast<SettingsDialog*>(SkywardSwordPlugin::instance()->settingsDialog());

    for (int i = 0; i <  settingsDialog->skipDatabase().count(); i++)
    {
        if (!skipBit(settingsDialog->skipDatabase().at(i).offset, settingsDialog->skipDatabase().at(i).bit))
            continue;
        setSkipBit(settingsDialog->skipDatabase().at(i).offset, settingsDialog->skipDatabase().at(i).bit, false);
    }
}

void SkywardSwordQuestEditorForm::buildSkipTab()
{
    SettingsDialog* settingsDialog = qobject_cast<SettingsDialog*>(SkywardSwordPlugin::instance()->settingsDialog());
    QList<SkipElement> database = settingsDialog->skipDatabase();

    this->setUpdatesEnabled(false);
    while (m_skipChkBoxes.count() > database.count())
    {
        delete m_skipChkBoxes.takeLast();
        m_skipChkColumn--;
        if (m_skipChkColumn <= 0)
        {
            if (m_skipChkRow > 0)
            {
                m_skipChkColumn = 8;
                m_skipChkRow--;
            }
            else
                m_skipChkColumn = 0;
        }
    }


    for (int i = 0; i < database.count(); i++)
    {
        SkipElement elem = database.at(i);
        if (i >= m_skipChkBoxes.count())
            addSkipChkBox(elem.objectName, elem.text, elem.offset, elem.bit, elem.visible);
        else
            updateChkBox(elem.objectName, elem.text, elem.offset, elem.bit, elem.visible, m_skipChkBoxes.at(i));
    }
    this->setUpdatesEnabled(true);
}

bool SkywardSwordQuestEditorForm::skipBit(quint32 offset, quint32 bit)
{
    return (bool)(*(atUint8*)(m_skipData + offset) & (1 << bit));
}

void SkywardSwordQuestEditorForm::setSkipBit(quint32 offset, quint32 bit, bool val)
{
    if (val)
        *(atUint8*)(m_skipData + offset) |= (1 << bit);
    else
        *(atUint8*)(m_skipData + offset) &= ~(1 << bit);
    emit modified();
}

void SkywardSwordQuestEditorForm::addSkipChkBox(const QString& name, const QString& title, quint32 offset, quint32 bit, bool visible)
{   
    if (name.isNull())
    {
        qWarning() << "Unable to add checkbox without an object name";
        return;
    }
    if (bit > 7)
    {
        qWarning() << "Bit out of range, expected 0-7 got " << bit;
    }

    QGridLayout* layout = qobject_cast<QGridLayout*>(ui->skipTab->layout());
    QCheckBox* chkBox = new QCheckBox(ui->skipTab);
    layout->addWidget(chkBox, m_skipChkRow, m_skipChkColumn);
    m_skipChkColumn++;
    if (m_skipChkColumn >= 6)
    {
        m_skipChkColumn = 0;
        m_skipChkRow++;
    }

    // Remove from layout first, to avoid weird crashes.
    layout->removeItem(ui->skipDataHorizontalSpacer);
    layout->removeItem(ui->skipDataVerticalSpacer);
    layout->addItem(ui->skipDataHorizontalSpacer, layout->rowCount(), layout->columnCount());
    layout->addItem(ui->skipDataVerticalSpacer, layout->rowCount(), layout->columnCount());
    layout->removeWidget(ui->setAllSkipsBtn);
    layout->removeWidget(ui->clearAllSkipsBtn);
    layout->addWidget(ui->setAllSkipsBtn, layout->rowCount(), 0);
    layout->addWidget(ui->clearAllSkipsBtn, layout->rowCount() - 1, 1);

    updateChkBox(name, title, offset, bit, visible, chkBox);

    connect(chkBox, SIGNAL(toggled(bool)), this, SLOT(onCheckboxToggled()));

    m_skipChkBoxes << chkBox;
}

void SkywardSwordQuestEditorForm::setQuantity(bool isRight, int offset, quint32 val)
{
    quint16 oldVal = qFromBigEndian(*(quint16*)(m_gameData + offset));
    if (!isRight)
    {
        quint16 newVal = (oldVal&127)|(((quint16)val << 7));
        *(quint16*)(m_gameData + offset) = qToBigEndian(newVal);
    }

    if (isRight)
    {
        oldVal = (oldVal >> 7) & 127;
        quint16 newVal = (val|(oldVal << 7));
        *(quint16*)(m_gameData + offset) = qToBigEndian(newVal);
    }

    emit modified();
}

quint32 SkywardSwordQuestEditorForm::quantity(bool isRight, int offset) const
{
    if (!m_gameData)
        return 0;

    return (quint32)(qFromBigEndian((*(quint16*)(m_gameData + offset))) >> (isRight ? 0 : 7)) & 127;
}

bool SkywardSwordQuestEditorForm::flag(quint32 offset, quint32 flag)
{
    return (*(char*)(m_gameData + offset) & flag) == flag;
}

void SkywardSwordQuestEditorForm::setFlag(quint32 offset, quint32 flag, bool val)
{
    if (this->flag(offset, flag) == val)
        return;

    if (val)
        *(char*)(m_gameData + offset) |= flag;
    else
        *(char*)(m_gameData + offset) &= ~flag;

    emit modified();
}

void SkywardSwordQuestEditorForm::updateData()
{
    this->setUpdatesEnabled(false);
    // "Toolbar" Items
    ui->deletePushButton->setEnabled(!isNew());
    ui->createPushButton->setEnabled(isNew());
    ui->checksumValueLbl->setText("0x" + QString("%1").arg((uint)checksum(), 8, 16, QChar('0')).toUpper());
    ui->skipChecksumValueLbl->setText("0x" + QString("%1").arg((uint)skipChecksum(), 8, 16, QChar('0')).toUpper());
    ui->tabWidget->setEnabled(!isNew());
    ui->copyPushButton->setEnabled(!isNew());
    ui->exportPushButton->setEnabled(!isNew());

    ui->cutsceneModeChkBox->setChecked(isCutsceneMode());
    // Play Stats
    PlaytimeWidget* ptw = qobject_cast<PlaytimeWidget*>(ui->playtime);
    if (ptw)
        ptw->setPlaytime(playtime());

    ui->saveTimeEdit->setDateTime(savetime());
    ui->curMapLineEdit->setText(currentMap());
    ui->curAreaLineEdit->setText(currentArea());
    ui->curRoomLineEdit->setText(currentRoom());
    ui->roomIDSpinBox->setValue(roomID());
    ui->introViewedChkBox->setChecked(watchedIntro());
    ui->nightChkbox->setChecked(isNight());
    ui->heroModeChkBox->setChecked(isHeroMode());

    // Player
    ui->nameLineEdit->setText(playerName());
    ui->rupeeSpinBox->setValue(rupees());
    ui->totalHPSpinBox->setValue(totalHP());
    ui->unkHPSpinBox->setValue(unkHP());
    ui->curHPSpinBox->setValue(currentHP());

    // Player Location
    ui->playerXSpinBox->setValue(player(XCoord));
    ui->playerYSpinBox->setValue(player(YCoord));
    ui->playerZSpinBox->setValue(player(ZCoord));
    ui->playerRollSpinBox->setValue(player(RollCoord));
    ui->playerPitchSpinBox->setValue(player(PitchCoord));
    ui->playerYawSpinBox->setValue(player(YawCoord));

    // Camera Location
    ui->cameraXSpinBox->setValue(camera(XCoord));
    ui->cameraYSpinBox->setValue(camera(YCoord));
    ui->cameraZSpinBox->setValue(camera(ZCoord));
    ui->cameraRollSpinBox->setValue(camera(RollCoord));
    ui->cameraPitchSpinBox->setValue(camera(PitchCoord));
    ui->cameraYawSpinBox->setValue(camera(YawCoord));

    // Equipment
    ui->beetleChkBox->setChecked(beetle(NormalBeetle));
    ui->hookBeetleChkBox->setChecked(beetle(HookBeetle));
    ui->quickBeetleChkBox->setChecked(beetle(QuickBeetle));
    ui->toughBeetleChkBox->setChecked(beetle(ToughBeetle));
    ui->bowChkBox->setChecked(bow(NormalBow));
    ui->ironBowChkBox->setChecked(bow(IronBow));
    ui->sacredBowChkBox->setChecked(bow(SacredBow));
    ui->arrowAmmoSpinBox->setValue(ammo(Arrows));
    ui->bugNetChkBox->setChecked(bugnet(Bugnet));
    ui->bigBugNetChkBox->setChecked(bugnet(BigBugnet));
    ui->gustBellowsChkBox->setChecked(gustBellows());
    ui->harpChkBox->setChecked(goddessHarp());
    ui->whipChkBox->setChecked(whip());
    ui->clawShotChkBox->setChecked(clawshot());
    ui->diggingMittsChkBox->setChecked(diggingMitts());
    ui->moleMittsChkBox->setChecked(moleMitts());
    ui->sailClothChkBox->setChecked(sailCloth());
    ui->dragonScaleChkBox->setChecked(waterDragonScale());
    ui->fireEaringsChkBox->setChecked(fireShieldEarings());
    ui->bombChkBox->setChecked(bombs());
    ui->bombAmmoSpinBox->setValue(ammo(Bombs));
    ui->slingShotChkBox->setChecked(slingshot(Slingshot));
    ui->scatterShotChkBox->setChecked(slingshot(Scattershot));
    ui->seedAmmoSpinBox->setValue(ammo(Seeds));

    // Wallets
    ui->smallWalletChkBox->setChecked(wallet(SmallWallet));
    ui->mediumWalletChkBox->setChecked(wallet(MediumWallet));
    ui->bigWalletChkBox->setChecked(wallet(BigWallet));
    ui->giantWalletChkBox->setChecked(wallet(GiantWallet));
    ui->tycoonWalletChkBox->setChecked(wallet(TycoonWallet));

    // Swords
    ui->practiceSwdChkBox->setChecked(practiceSword());
    ui->goddessSwdChkBox->setChecked(goddessSword());
    ui->longSwdChkBox->setChecked(goddessLongSword());
    ui->whiteSwdChkBox->setChecked(goddessWhiteSword());
    ui->masterSwdChkBox->setChecked(masterSword());
    ui->trueMasterSwdChkBox->setChecked(trueMasterSword());

    // Triforce
    TriforceWidget* tri = qobject_cast<TriforceWidget*>(ui->triforceWidget);
    if (tri)
    {
        tri->setCourageChecked(triforce(TriforceWidget::Courage));
        tri->setWisdomChecked(triforce(TriforceWidget::Wisdom));
        tri->setPowerChecked(triforce(TriforceWidget::Power));
    }

    // Bugs
    updateBugs();

    // Materials
    updateMaterials();

    // Gratitude crystals
    ui->gratitudeCrystalsLabel->setEnabled(gratitudeCrystals() > 0);
    ui->gratitudeCrystalsSpinBox->setValue(gratitudeCrystals());

    // Cutscenes
    foreach (QCheckBox* chkBox, m_skipChkBoxes)
    {
        if (chkBox->property("isCutscene").isValid() && chkBox->property("isCutscene").toBool())
        {
            bool ok = false;
            quint32 offset = chkBox->property("offset").toInt(&ok);
            if (ok)
            {
                quint32 bit = chkBox->property("bit").toInt(&ok);
                if (ok)
                    chkBox->setChecked(skipBit(offset, bit));
            }
        }
    }

    this->setUpdatesEnabled(true);
    //this->update();
}

void SkywardSwordQuestEditorForm::updateBugs()
{
    ui->hornetChkBox      ->setChecked(bug(DekuHornet));
    ui->hornetSpinBox     ->setValue(bugAmount(DekuHornet));
    ui->butterflyChkBox   ->setChecked(bug(BlessedButterfly));
    ui->butterflySpinBox  ->setValue(bugAmount(BlessedButterfly));
    ui->dragonflyChkBox   ->setChecked(bug(GerudoDragonfly));
    ui->dragonflySpinBox  ->setValue(bugAmount(GerudoDragonfly));
    ui->fireflyChkBox     ->setChecked(bug(StarryFirefly));
    ui->fireflySpinBox    ->setValue(bugAmount(StarryFirefly));
    ui->rhinoBeetleChkBox ->setChecked(bug(WoodlandRhinoBeetle));
    ui->rhinoBeetleSpinBox->setValue(bugAmount(WoodlandRhinoBeetle));
    ui->ladybugChkBox     ->setChecked(bug(VolcanicLadybug));
    ui->ladybugSpinBox    ->setValue(bugAmount(VolcanicLadybug));
    ui->sandCicadaChkBox  ->setChecked(bug(SandCicada));
    ui->sandCicadaSpinBox ->setValue(bugAmount(SandCicada));
    ui->stagBeetleChkBox  ->setChecked(bug(SkyStagBeetle));
    ui->stagBeetleSpinBox ->setValue(bugAmount(SkyStagBeetle));
    ui->grasshopperChkBox ->setChecked(bug(FaronGrasshopper));
    ui->grasshopperSpinBox->setValue(bugAmount(FaronGrasshopper));
    ui->mantisChkBox      ->setChecked(bug(SkyloftMantis));
    ui->mantisSpinBox     ->setValue(bugAmount(SkyloftMantis));
    ui->antChkBox         ->setChecked(bug(LanayruAnt));
    ui->antSpinBox        ->setValue(bugAmount(LanayruAnt));
    ui->eldinRollerChkBox ->setChecked(bug(EldinRoller));
    ui->eldinRollerSpinBox->setValue(bugAmount(EldinRoller));
}


void SkywardSwordQuestEditorForm::updateMaterials()
{
    ui->hornetLarvaeChkBox->setChecked(material(DekuHornetLarvae));
    ui->hornetLarvaeSpinBox->setValue(materialAmount(DekuHornetLarvae));
    ui->birdFeatherChkBox->setChecked(material(BirdFeather));
    ui->birdFeatherSpinBox->setValue(materialAmount(BirdFeather));
    ui->tumbleWeedChkBox->setChecked(material(TumbleWeed));
    ui->tumbleWeedSpinBox->setValue(materialAmount(TumbleWeed));
    ui->lizardTailChkBox->setChecked(material(LizardTail));
    ui->lizardTailSpinBox->setValue(materialAmount(LizardTail));
    ui->eldinOreChkBox->setChecked(material(EldinOre));
    ui->eldinOreSpinBox->setValue(materialAmount(EldinOre));
    ui->ancientFlowerChkBox->setChecked(material(AncientFlower));
    ui->ancientFlowerSpinBox->setValue(materialAmount(AncientFlower));
    ui->amberRelicChkBox->setChecked(material(AmberRelic));
    ui->amberRelicSpinBox->setValue(materialAmount(AmberRelic));
    ui->duskRelicChkBox->setChecked(material(DuskRelic));
    ui->duskRelicSpinBox->setValue(materialAmount(DuskRelic));
    ui->jellyBlobChkBox->setChecked(material(JellyBlob));
    ui->jellyBlobSpinBox->setValue(materialAmount(JellyBlob));
    ui->monsterClawChkBox->setChecked(material(MonsterClaw));
    ui->monsterClawSpinBox->setValue(materialAmount(MonsterClaw));
    ui->monsterHornChkBox->setChecked(material(MonsterHorn));
    ui->monsterHornSpinBox->setValue(materialAmount(MonsterHorn));
    ui->decoSkullChkBox->setChecked(material(OrnamentalSkull));
    ui->decoSkullSpinBox->setValue(materialAmount(OrnamentalSkull));
    ui->evilCrystalChkBox->setChecked(material(EvilCrystal));
    ui->evilCrystalSpinBox->setValue(materialAmount(EvilCrystal));
    ui->blueBirdFeatherChkBox->setChecked(material(BlueBirdFeather));
    ui->blueBirdFeatherSpinBox->setValue(materialAmount(BlueBirdFeather));
    ui->goldenSkullChkBox->setChecked(material(GoldenSkull));
    ui->goldenSkullSpinBox->setValue(materialAmount(GoldenSkull));
    ui->goddessPlumeChkBox->setChecked(material(GoddessPlume));
    ui->goddessPlumeSpinBox->setValue(materialAmount(GoddessPlume));
}

void SkywardSwordQuestEditorForm::updateChkBox(const QString& name, const QString& title, quint32 offset, quint32 bit, bool visible, QCheckBox* chkBox)
{
    chkBox->setObjectName(name);
    chkBox->setProperty("isCutscene", true);
    chkBox->setProperty("offset", offset);
    chkBox->setProperty("bit", bit);
    chkBox->setChecked(skipBit(offset, bit));
    chkBox->setVisible(visible);
    // If the title is empty use the object name;
    chkBox->setText((title.isEmpty() ? name : title));
}
