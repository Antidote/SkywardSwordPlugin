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

#ifndef SKYWARDSWORDGAMEFILE_HPP
#define SKYWARDSWORDGAMEFILE_HPP

#include <GameDocument.hpp>
#include <QObject>
#include "Common.hpp"
#include <memory>
#include <Athena/Types.hpp>

class CopyWidget;
class SkywardSwordQuestEditorForm;
namespace Athena
{
namespace io
{
class IStreamReader;
}
}

class SkywardSwordGameDocument : public GameDocument
{
    Q_OBJECT
public:
    SkywardSwordGameDocument(const PluginInterface* loader, const QString& file = QString());
    ~SkywardSwordGameDocument();
    QString game() const;
    bool save(const QString &filename = QString());

    bool loadFile();

public slots:
    bool reload();
    bool supportsWiiSave() const;
    bool exportWiiSave();

    Region region();
    void setRegion(Region region);

    SkywardSwordQuestEditorForm* quest(quint32 index);
    int currentQuestIndex();
    SkywardSwordQuestEditorForm* currentQuest();
private slots:
    void onInfoButtonClicked();
    void onModified();
    void onCopy(SkywardSwordQuestEditorForm* source);
    void onTabMoved(int left, int right);
private:
    Region m_region;
    bool loadData(Athena::io::IStreamReader& reader);
    std::unique_ptr<atUint8[]> m_skipData;
    CopyWidget* m_copyWidget;
};

#endif // SKYWARDSWORDGAMEFILE_HPP
