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

#ifndef SKYWARDSWORDPLUGGIN_HPP
#define SKYWARDSWORDPLUGGIN_HPP

#include "skywardswordplugin_global.hpp"
#include "PluginInterface.hpp"
#include <Updater.hpp>
#include <QFileSystemWatcher>

#include <QMessageBox>
#include <QIcon>


class SettingsDialog;
class SettingsManager;

class QMenu;

class SKYWARDSWORDPLUGGIN_EXPORT SkywardSwordPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.wiiking2.skywardswordplugin" FILE "skywardswordplugin.json")
#endif
public:
    SkywardSwordPlugin();
    ~SkywardSwordPlugin();
    void initialize(MainWindowBase *mainWindow);
    QString filter()      const;
    QString extension()   const;
    QString name()        const;
    QString author()      const;
    QString version()     const;
    QString website()     const;
    void setPath(const QString &path);
    QString path()        const;
    QString license()     const;
    QString description() const;
    bool enabled() const;
    void setEnabled(const bool enable);
    bool canLoad(const QString& filename);

    DocumentBase* loadFile(const QString& file) const;

    PluginSettingsDialog* settingsDialog();
    Updater* updater();
    QObject* object();

    QIcon icon() const;
    bool hasUpdater() const;
    void doUpdate();
    MainWindowBase* mainWindow() const;

    static SkywardSwordPlugin* instance();
    static SettingsManager* settings();
signals:
    void enabledChanged();
    void newDocument(DocumentBase*);
public slots:

private slots:
    void onNewDocument();
    void onUpdaterDone();
    void onUpdaterError(Updater::ErrorType error);
    void onUpdaterWarning(QString warning);
    void onNoUpdate();
private:
    QAction*                   m_actionNewDocument;
    bool                       m_enabled;
    QString                    m_path;
    QIcon                      m_icon;
    SettingsDialog*            m_settingsDialog;
    static SkywardSwordPlugin* m_instance;
    QMessageBox                m_updateMBox;
    Updater*                   m_updater;
    MainWindowBase*            m_mainWindow;
};

#endif // SKYWARDSWORDPLUGGIN_HPP
