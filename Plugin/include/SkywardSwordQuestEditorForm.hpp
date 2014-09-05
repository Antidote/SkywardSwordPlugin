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

#ifndef SKYWARDSWORDQUESTEDITORFORM_HPP
#define SKYWARDSWORDQUESTEDITORFORM_HPP

#include <QWidget>
#include <QDateTime>
#include "Common.hpp"
#include "TriforceWidget.hpp"

class SkywardSwordGameDocument;
class SettingsManager;
class QCheckBox;

namespace Ui {
class SkywardSwordEditorForm;
}

class SkywardSwordQuestEditorForm : public QWidget
{
    Q_OBJECT
public:

    enum Coord
    {
        XCoord,
        YCoord,
        ZCoord,
        RollCoord,
        PitchCoord,
        YawCoord
    };

    enum BeetleType
    {
        NormalBeetle,
        HookBeetle,
        QuickBeetle,
        ToughBeetle
    };

    enum BowType
    {
        NormalBow,
        IronBow,
        SacredBow
    };

    enum SlingshotType
    {
        Slingshot,
        Scattershot
    };

    enum BugnetType
    {
        Bugnet,
        BigBugnet
    };

    enum BugType
    {
        DekuHornet,
        BlessedButterfly,
        GerudoDragonfly,
        StarryFirefly,
        WoodlandRhinoBeetle,
        VolcanicLadybug,
        SandCicada,
        SkyStagBeetle,
        FaronGrasshopper,
        SkyloftMantis,
        LanayruAnt,
        EldinRoller
    };

    enum AmmoType
    {
        Arrows,
        Bombs,
        Seeds
    };

    enum WalletType
    {
        SmallWallet,
        MediumWallet,
        BigWallet,
        GiantWallet,
        TycoonWallet
    };

    enum MaterialType
    {
        DekuHornetLarvae,
        BirdFeather,
        TumbleWeed,
        LizardTail,
        EldinOre,
        AncientFlower,
        AmberRelic,
        DuskRelic,
        JellyBlob,
        MonsterClaw,
        MonsterHorn,
        OrnamentalSkull,
        EvilCrystal,
        BlueBirdFeather,
        GoldenSkull,
        GoddessPlume
    };

    explicit SkywardSwordQuestEditorForm(const char* data, const char* skipData, Region region, QWidget *paren=0);
    ~SkywardSwordQuestEditorForm();

    void setGameData(const QByteArray& data);
    char* gameData() const;

    void setSkipData(const QByteArray& data);
    char* skipData() const;
    int currentTab();
    void setCurrentTab(int index);

public slots:
    // Actions
    void onDelete();
    void onCreate();
    void onCopy();
    void onModified();
    void onCheckboxToggled();
    void onImportExport();
    void setRegion(Region region);

    // Data
    // Play Stats
    Playtime playtime() const;
    void setPlaytime(Playtime val);
    QDateTime savetime() const;
    void setSavetime(const QDateTime& time);
    QString currentMap() const;
    void setCurrentMap(const QString& map);
    QString currentArea() const;
    void setCurrentArea(const QString& area);
    QString currentRoom() const;
    void setCurrentRoom(const QString& room);
    int roomID() const;
    void setRoomID(int id);
    bool watchedIntro();
    void setIntroWatched(bool val);
    bool isNight();
    void setNight(bool val);
    bool isHeroMode();
    void setHeroMode(bool val);

    // Player
    QString playerName() const;
    void setPlayerName(const QString& name);
    quint32 rupees() const;
    void setRupees(int val);
    int totalHP();
    void setTotalHP(int val);
    quint32 unkHP();
    void setUnkHP(int val);
    quint32 currentHP();
    void setCurrentHP(int val);

    // Player Location
    float player(Coord coord);
    void setCoord(double x);
    float camera(Coord coord);

    // Equipment
    bool beetle(BeetleType type);
    void setBeetle(bool val);
    bool bow(BowType bow);
    void setBow(bool val);
    bool bugnet(BugnetType type);
    void setBugnet(bool val);
    bool gustBellows();
    void setGustBellows(bool val);
    bool goddessHarp();
    void setGoddessHarp(bool val);
    bool whip();
    void setWhip(bool val);
    bool clawshot();
    void setClawshot(bool val);
    bool diggingMitts();
    void setDiggingMitts(bool val);
    bool moleMitts();
    void setMoleMitts(bool val);
    bool sailCloth();
    void setSailCloth(bool val);
    bool waterDragonScale();
    void setWaterDragonScale(bool val);
    bool fireShieldEarings();
    void setFireShieldEarings(bool val);
    bool bombs();
    void setBombs(bool val);
    bool slingshot(SlingshotType type);
    void setSlingshot(bool val);

    int ammo(AmmoType type);
    void setAmmo(int val);
    // Swords
    bool practiceSword();
    void setPracticeSword(bool val);
    bool goddessSword();
    void setGoddessSword(bool val);
    bool goddessLongSword();
    void setGoddessLongSword(bool val);
    bool goddessWhiteSword();
    void setGoddessWhiteSword(bool val);
    bool masterSword();
    void setMasterSword(bool val);
    bool trueMasterSword();
    void setTrueMasterSword(bool val);

    // Wallet
    bool wallet(WalletType type);
    void setWallet(bool val);

    // Triforce
    bool triforce(TriforceWidget::TriforcePiece piece);
    void triforceClicked();
    bool bug(BugType bug);
    void setBug(bool val);
    int bugAmount(BugType bug);
    void setBugAmount(int val);

    bool material(MaterialType material);
    void setMaterial(bool val);
    int materialAmount(MaterialType material);
    void setMaterialAmount(int val);

    int gratitudeCrystals();
    void setGratitudeCrystals(int val);

    bool isCutsceneMode();
    void setCutsceneMode(bool isCutsceneMode);
    bool isNew();
    void setNew(bool isNew);
    int checksum();
    void updateQuestChecksum();
    int skipChecksum();
    void updateSkipChecksum();

    void setAllSkips();
    void clearAllSkips();
signals:
    void modified();
    void copy(SkywardSwordQuestEditorForm*);

private slots:
    void buildSkipTab();
private:
    bool skipBit(quint32 offset, quint32 bit);
    void setSkipBit(quint32 offset, quint32 bit, bool val);
    void addSkipChkBox(const QString& name, const QString& title, quint32 offset, quint32 bit, bool visible);
    void setQuantity(bool isRight, int offset, quint32 val);
    quint32 quantity(bool isRight, int offset);
    bool flag(quint32 offset, quint32 flag);
    void setFlag(quint32 offset, quint32 flag, bool val);
    void updateData();
    void updateBugs();
    void updateMaterials();
    void updateChkBox(const QString& name, const QString& title, quint32 offset, quint32 bit, bool visible, QCheckBox* chkBox);
    Ui::SkywardSwordEditorForm *ui;
    Region m_region;
    char* m_gameData;
    char* m_skipData;
    quint32 m_skipChkColumn;
    quint32 m_skipChkRow;
    QList<QCheckBox*> m_skipChkBoxes;
};

#endif // SKYWARDSWORDQUESTEDITORFORM_HPP

