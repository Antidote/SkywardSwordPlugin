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

#include "SettingsManager.hpp"
#include "SkywardSwordPlugin.hpp"
#include "Constants.hpp"
#include <QSettings>
#include <QStringList>
#include <QApplication>

SettingsManager* SettingsManager::m_instance = NULL;

SettingsManager::SettingsManager()
    : QObject(NULL)
{
    m_defaultNameList = QStringList() << "Link" << QString::fromUtf8("링크") << QString::fromUtf8("リンク") << "Link";
    m_defaultRegion   = Region::NTSCU;

    QSettings settings;
    settings.beginGroup(SkywardSwordPlugin::instance()->name());
    QStringList tmpNameList = settings.value("defaultPlayerNameForRegion", m_defaultNameList).toStringList();
    if (tmpNameList.count() == (int)Region::Count)
        m_defaultNameList = tmpNameList;

    m_defaultRegion = (Region)settings.value("defaultRegion", "U").toChar().toLatin1();

#ifdef SS_INTERNAL
    m_updateUrl = Constants::Settings::SKYWARDSWORD_UPDATE_URL_DEFAULT;
#else
    m_updateUrl = settings.value(Constants::Settings::SKYWARDSWORD_UPDATE_URL, Constants::Settings::SKYWARDSWORD_UPDATE_URL_DEFAULT).toString();
#endif

    m_updateCheckOnStart = settings.value(Constants::Settings::SKYWARDSWORD_CHECK_ON_START, false).toBool();
    settings.endGroup();
}

SettingsManager::~SettingsManager()
{
    saveSettings();
}


QString SettingsManager::defaultPlayerNameForRegion(Region region) const
{
    switch (region)
    {
    case Region::NTSCU:
        return m_defaultNameList[0];
    case Region::NTSCK:
        return m_defaultNameList[1];
    case Region::NTSCJ:
        return m_defaultNameList[2];
    case Region::PAL:
        return m_defaultNameList[3];
    default:
        return QString();
    }
}

void SettingsManager::setDefaultPlayerNameForRegion(Region region, const QString &name)
{
    int regionId;

    switch (region)
    {
    case Region::NTSCU: regionId = 0; break;
    case Region::NTSCK: regionId = 1; break;
    case Region::NTSCJ: regionId = 2; break;
    case Region::PAL:   regionId = 3; break;
    default:
        return;
    }

    m_defaultNameList[regionId] = name;
    saveSettings();
}

QString SettingsManager::defaultPlayerName() const
{
    switch (m_defaultRegion)
    {
    case Region::NTSCU:
        return m_defaultNameList[0];
    case Region::NTSCK:
        return m_defaultNameList[1];
    case Region::NTSCJ:
        return m_defaultNameList[2];
    case Region::PAL:
        return m_defaultNameList[3];
    default:
        return QString();
    }
}

Region SettingsManager::defaultRegion() const
{
    return m_defaultRegion;
}

void SettingsManager::setDefaultRegion(const Region region)
{
    m_defaultRegion = region;
    saveSettings();
}

QString SettingsManager::updateUrl() const
{
    return m_updateUrl;
}

void SettingsManager::setUpdateUrl(const QString& updateUrl)
{
    m_updateUrl = updateUrl;
    saveSettings();
}

bool SettingsManager::updateCheckOnStart() const
{
    return m_updateCheckOnStart;
}

void SettingsManager::setUpdateCheckOnStart(bool updateOnStart)
{
    m_updateCheckOnStart = updateOnStart;
    saveSettings();
}

void SettingsManager::saveSettings()
{
    QSettings settings;
    settings.beginGroup(SkywardSwordPlugin::instance()->name());
    settings.setValue("defaultPlayerNameForRegion", m_defaultNameList);
    settings.setValue("defaultRegion", QChar((char)m_defaultRegion));
#ifndef SS_INTERNAL
    settings.setValue(Constants::Settings::SKYWARDSWORD_UPDATE_URL, m_updateUrl);
#endif

    settings.setValue(Constants::Settings::SKYWARDSWORD_CHECK_ON_START, m_updateCheckOnStart);
    settings.endGroup();
}


SettingsManager *SettingsManager::instance()
{
    if (!m_instance)
        m_instance = new SettingsManager;

    return m_instance;
}
