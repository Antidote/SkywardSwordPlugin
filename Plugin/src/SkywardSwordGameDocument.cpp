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

#include "SkywardSwordGameDocument.hpp"
#include "SkywardSwordQuestEditorForm.hpp"
#include "SkywardSwordWidget.hpp"
#include "SkywardSwordTabWidget.hpp"
#include "SkywardSwordPlugin.hpp"
#include "CopyWidget.hpp"
#include "SettingsManager.hpp"
#include "SaveInfoDialog.hpp"

#include <Athena/WiiSaveReader.hpp>
#include <Athena/WiiSaveWriter.hpp>
#include <Athena/WiiSave.hpp>
#include <Athena/WiiFile.hpp>
#include <Athena/WiiBanner.hpp>
#include <Athena/WiiImage.hpp>
#include <Athena/BinaryReader.hpp>
#include <Athena/BinaryWriter.hpp>
#include <Athena/Exception.hpp>
#include <Athena/FileNotFoundException.hpp>
#include <Athena/InvalidOperationException.hpp>
#include <Athena/InvalidDataException.hpp>

#include <WiiKeyManagerBase.hpp>
#include <MainWindowBase.hpp>

#include <QMessageBox>
#include <QtEndian>
#include <QApplication>
#include <QMainWindow>
#include <QFileInfo>
#include <QTabBar>
#include <QDebug>
#include <QFileDialog>

SkywardSwordGameDocument::SkywardSwordGameDocument(const PluginInterface *loader, const QString &file)
    : GameDocument(loader, file),
      m_skipData(NULL)
{
    m_widget = new SkywardSwordWidget;
    m_copyWidget = new CopyWidget(m_widget);
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    SkywardSwordTabWidget* tw = mainWidget->tabWidget();

    connect(mainWidget, SIGNAL(infoButtonClicked()), this, SLOT(onInfoButtonClicked()));
    tw->setIconSize(QSize(32, 32));
    tw->setDocumentMode(true);
    tw->setMovable(true);
    connect(tw->tabBar(), SIGNAL(tabMoved(int,int)), this, SLOT(onTabMoved(int, int)));

    if (file.isEmpty())
    {
        m_skipData = new atUint8[0x80];
        memset(m_skipData, 0, 0x80);
        mainWidget->setRegion(SettingsManager::instance()->defaultRegion());

        for (int i = 0; i < 3; i++)
        {
            char* data = new char[0x53C0];
            memset(data, 0, 0x53C0);
            char* skipData = new char[0x24];
            memcpy(skipData, (m_skipData + (0x24 * i)), 0x24);
            SkywardSwordQuestEditorForm* sw = new SkywardSwordQuestEditorForm(data, skipData, mainWidget->region());
            sw->setNew(true);
            tw->addTab(sw, QIcon(QString(":/icons/Game%1").arg(i+1)), tr("&%1 New Game").arg(i + 1));
            connect(sw, SIGNAL(modified()), this, SLOT(onModified()));
            connect(mainWidget, SIGNAL(regionChanged(Region)), sw, SLOT(setRegion(Region)));
            connect(sw, SIGNAL(copy(SkywardSwordQuestEditorForm*)), this, SLOT(onCopy(SkywardSwordQuestEditorForm*)));
        }
        setDirty(true);
    }
}

SkywardSwordGameDocument::~SkywardSwordGameDocument()
{
    if (m_skipData)
    {
        delete[] m_skipData;
        m_skipData = NULL;
    }
}

bool SkywardSwordGameDocument::loadFile()
{
    if (QFileInfo(filePath()).suffix() == "bin")
    {
        try
        {
            QMessageBox msg(SkywardSwordPlugin::instance()->mainWindow()->mainWindow());
            msg.setWindowTitle("Loading WiiSave...");
            msg.setText(tr("Loading WiiSave please wait...."));
            msg.setStandardButtons(QMessageBox::NoButton);
            msg.show();
            qApp->setOverrideCursor(Qt::WaitCursor);
            Athena::io::WiiSaveReader reader(filePath().toStdString());
            Athena::WiiSave* file = reader.readSave();
            qApp->restoreOverrideCursor();
            msg.hide();
            if (file->file("/wiiking2.sav"))
            {
                atUint8* data = file->file("/wiiking2.sav")->data();
                m_isWiiSave = true;
                bool ret = loadData(Athena::io::BinaryReader(data, (atUint64)file->file("/wiiking2.sav")->length()));
                delete file;
                return ret;
            }
            delete file;
        }
        catch (Athena::error::Exception e)
        {
            qWarning() << e.message().c_str() << " -> " << e.file().c_str() << "(" << e.line() << "): " << e.function().c_str();
            qApp->restoreOverrideCursor();
        }
    }

    try
    {
        return loadData(Athena::io::BinaryReader(filePath().toStdString()));
    }
    catch (Athena::error::Exception e)
    {
        qWarning() << e.message().c_str() << " -> " << e.file().c_str() << "(" << e.line() << "): " << e.function().c_str();
        return false;
    }
}

bool SkywardSwordGameDocument::reload()
{
    // First store the current tabs
    QList<int> swCurTabs;
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    SkywardSwordTabWidget* tw = mainWidget->tabWidget();
    int twCurrentTab = tw->currentIndex();

    for (int i = 0; i < tw->count(); i++)
    {
        SkywardSwordQuestEditorForm* sw = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(i));
        swCurTabs.push_back(sw->currentTab());
    }

    bool ret = loadFile();
    // Now set them back
    if (ret)
    {
        for (int i = 0; i < tw->count(); i++)
        {
            SkywardSwordQuestEditorForm* sw = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(i));
            sw->setCurrentTab(swCurTabs[i]);
        }
        tw->setCurrentIndex(twCurrentTab);
    }


    // Finally inform Main that we're done
    return ret;
}

bool SkywardSwordGameDocument::supportsWiiSave() const
{
    return true;
}

bool SkywardSwordGameDocument::exportWiiSave()
{
    if (m_keyManager == nullptr || !m_keyManager->isValid())
        return false;
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    QTabWidget* tw = mainWidget->tabWidget();

    QString filename = QFileDialog::getSaveFileName(SkywardSwordPlugin::instance()->mainWindow()->mainWindow(), "Export wii save", QString(), "WiiSave *.bin (*.bin)");
    if (filename.isNull())
        return false;

    if (QFileInfo(filename).suffix() != "bin")
        filename += ".bin";


    Athena::WiiSave* save = new Athena::WiiSave;

    Region region = mainWidget->region();
    int gameId = ('S' << 24) | ('O' << 16) | ('U' << 8) | (int)region;
    quint64 titleId = 0x00010000;
    titleId = (qToBigEndian((quint64)gameId)) | (qToBigEndian(titleId) >> 32);

    qDebug() << hex << qFromBigEndian(titleId);
    Athena::WiiBanner* banner = new Athena::WiiBanner();
    banner->setGameID(qFromBigEndian(titleId));
    QFile tmp(":/BannerData/banner.tpl");
    if (tmp.open(QFile::ReadOnly))
    {
        QDataStream dataStream(&tmp);
        char* bannerData = new char[192*64*2];
        dataStream.readRawData(bannerData, 192*64*2);
        banner->setBannerImage(new Athena::WiiImage(192, 64, (atUint8*)bannerData));
        tmp.close();
    }
    else
    {
        delete save;
        return false;
    }

    tmp.setFileName(":/BannerData/icon.tpl");

    if (tmp.open(QFile::ReadOnly))
    {
        QDataStream dataStream(&tmp);
        char* iconData = new char[48*48*2];
        dataStream.readRawData(iconData, 48*48*2);
        banner->addIcon(new Athena::WiiImage(48, 48, (atUint8*)iconData));
        tmp.close();
    }
    else
    {
        delete save;
        return false;
    }

    tmp.setFileName(QString(":/BannerData/%1/title.bin").arg((char)region));
    if (tmp.open(QFile::ReadOnly))
    {
        QString titleString = QString::fromUtf16((ushort*)tmp.readAll().data());
        banner->setTitle(titleString.toUtf8().data());
        tmp.close();
    }
    else
    {
        delete save;
        return false;
    }

    tmp.setFileName(QString(":/BannerData/%1/subtitle.bin").arg((char)region));
    if (tmp.open(QFile::ReadOnly))
    {
        QString subtitleString = QString::fromUtf16((ushort*)tmp.readAll().data());
        banner->setSubtitle(subtitleString.toUtf8().data());
        tmp.close();
    }
    else
    {
        delete save;
        return false;
    }
    banner->setPermissions(Athena::WiiFile::GroupRW | Athena::WiiFile::OwnerRW);
    banner->setAnimationSpeed(0);
    save->setBanner(banner);

    try
    {
        Athena::io::BinaryWriter writer(QString(QDir::temp().tempPath() + "/tmp.sav").toStdString());
        writer.setEndian(Athena::Endian::BigEndian);
        writer.writeUint32(0x534F5500);
        writer.seek(-1);
        writer.writeByte((char)region);
        writer.seek(0x1C, Athena::SeekOrigin::Begin);
        writer.writeUint32(0x1D);

        for (int i = 0; i < 3; i++)
        {
            SkywardSwordQuestEditorForm* ef = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(i));
            if (!ef)
                return false;
            else
            {
                // Let's be sure we have a proper checksum
                ef->updateQuestChecksum();
                writer.writeBytes((atInt8*)ef->gameData(), 0x53C0);
            }
        }
        writer.writeBytes((atInt8*)m_skipData, 0x80);
        writer.save();

        save->addFile(new Athena::WiiFile("wiiking2.sav", Athena::WiiFile::GroupRW | Athena::WiiFile::OwnerRW, writer.data(), writer.length()));
        save->addFile(new Athena::WiiFile("skip.dat", Athena::WiiFile::GroupRW | Athena::WiiFile::OwnerRW, (atUint8*)m_skipData, 0x80));
    }
    catch(...)
    {
        delete save;
        return false;
    }

    try
    {
        Athena::io::WiiSaveWriter writer(filename.toStdString());
        if (writer.writeSave(save, (atUint8*)keyManager()->macAddr().data(), keyManager()->ngId(), (atUint8*)keyManager()->ngPriv().data(), (atUint8*)keyManager()->ngSig().data(), keyManager()->ngKeyId()))
            qDebug() << "export successful";
    }
    catch(...)
    {
        delete save;
        return false;
    }

    delete save;
    return true;
}

Region SkywardSwordGameDocument::region()
{
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    return mainWidget->region();
}

void SkywardSwordGameDocument::setRegion(Region region)
{
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    mainWidget->setRegion(region);
}

SkywardSwordQuestEditorForm*SkywardSwordGameDocument::quest(quint32 index)
{
    if (index > 3)
        return NULL;

    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    QTabWidget* tw = mainWidget->tabWidget();
    SkywardSwordQuestEditorForm* sw = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(index));
    return sw;
}

int SkywardSwordGameDocument::currentQuestIndex()
{
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    QTabWidget* tw = mainWidget->tabWidget();
    return tw->currentIndex();
}

SkywardSwordQuestEditorForm* SkywardSwordGameDocument::currentQuest()
{
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    QTabWidget* tw = mainWidget->tabWidget();
    SkywardSwordQuestEditorForm* sw = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(tw->currentIndex()));
    return sw;
}

void SkywardSwordGameDocument::onInfoButtonClicked()
{
    SaveInfoDialog sid(this, m_widget);
    sid.exec();
}

QString SkywardSwordGameDocument::game() const
{
    return "SkywardSword";
}

bool SkywardSwordGameDocument::save(const QString& filename)
{
    if (!filename.isEmpty())
    {
        m_file = QFileInfo(filename).fileName();
        m_path = QFileInfo(filename).absolutePath();
    }

    try
    {
        SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
        Q_ASSERT(mainWidget != NULL);
        QTabWidget* tw = mainWidget->tabWidget();

        Region region = mainWidget->region();
        Athena::io::BinaryWriter saveWriter(filePath().toStdString());
        Athena::io::BinaryWriter skipWriter(QString("%1/skip.dat").arg(m_path).toStdString());
        saveWriter.setEndian(Athena::Endian::BigEndian);
        saveWriter.writeUint32(0x534F5500);
        saveWriter.seek(-1);
        saveWriter.writeByte((char)region);
        saveWriter.seek(0x1C, Athena::SeekOrigin::Begin);
        saveWriter.writeUint32(0x1D);

        for (int i = 0; i < 3; i++)
        {
            SkywardSwordQuestEditorForm* ef = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(i));
            if (!ef)
                return false;
            else
            {
                // Let's be sure we have a proper checksum
                ef->updateQuestChecksum();
                saveWriter.writeBytes((atInt8*)ef->gameData(), 0x53C0);
                memcpy((m_skipData + (0x24 * i)), ef->skipData(), 0x24);
            }
        }

        saveWriter.writeUBytes(m_skipData, 0x80);
        skipWriter.writeUBytes(m_skipData, 0x80);
        saveWriter.save();
        skipWriter.save();
        m_dirty = false;
        m_isWiiSave = false;
        return true;
    }
    catch (...)
    {
    }

    return false;
}

void SkywardSwordGameDocument::onModified()
{
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    QTabWidget* tw = mainWidget->tabWidget();
    for (int i = 0; i < tw->count(); i++)
    {
        SkywardSwordQuestEditorForm* sw = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(i));
        tw->setTabIcon(i, QIcon(QString(":/icons/Game%1").arg(i + 1)));
        if (!sw->isNew())
            tw->setTabText(i, QString("&%1 %2").arg(i+1).arg(sw->playerName()));
        else
            tw->setTabText(i, tr("&%1 New Game").arg(i + 1));
    }
    this->setDirty(true);
    emit modified();
}

void SkywardSwordGameDocument::onCopy(SkywardSwordQuestEditorForm* source)
{
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    QTabWidget* tw = mainWidget->tabWidget();
    int index = 0;
    for (int i = 0; i < tw->count(); i++)
    {
        if (tw->widget(i) == source)
            index = i;

        m_copyWidget->setQuestEnabled((CopyWidget::Quest)i, true);
    }

    m_copyWidget->setQuestEnabled((CopyWidget::Quest)index, false);
    m_copyWidget->move(QCursor().pos());
    m_copyWidget->exec();

    if (m_copyWidget->result() == QDialog::Accepted)
    {
        QMessageBox mbox(source);
        mbox.setWindowTitle("Copy data...");
        mbox.setText("Are you sure you wish to copy this data?<br />"
                     "The data at the destination(s) you selected <b>will be lost</b>");
        mbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        mbox.exec();
        if (mbox.result() == QMessageBox::No)
            return;

        SkywardSwordQuestEditorForm* dest = NULL;
        if (m_copyWidget->questChecked(CopyWidget::Quest1) && source != tw->widget(0))
        {
            dest = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(CopyWidget::Quest1));
            if (dest)
            {
                dest->setGameData(QByteArray(source->gameData(), 0x53C0));
                dest->setSkipData(QByteArray(source->skipData(), 0x24));
            }
            dest = NULL;
        }
        if (m_copyWidget->questChecked(CopyWidget::Quest2) && source != tw->widget(CopyWidget::Quest2))
        {
            dest = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(CopyWidget::Quest2));
            if (dest)
            {
                dest->setGameData(QByteArray(source->gameData(), 0x53C0));
                dest->setSkipData(QByteArray(source->skipData(), 0x24));
            }
            dest = NULL;
        }
        if (m_copyWidget->questChecked(CopyWidget::Quest3) && source != tw->widget(CopyWidget::Quest3))
        {
            dest = qobject_cast<SkywardSwordQuestEditorForm*>(tw->widget(CopyWidget::Quest3));
            if (dest)
            {
                dest->setGameData(QByteArray(source->gameData(), 0x53C0));
                dest->setSkipData(QByteArray(source->skipData(), 0x24));
            }
            dest = NULL;
        }
    }
}

void SkywardSwordGameDocument::onTabMoved(int from, int to)
{
    Q_UNUSED(from);
    Q_UNUSED(to);
    onModified();
}

bool SkywardSwordGameDocument::loadData(Athena::io::BinaryReader reader)
{
    SkywardSwordWidget* mainWidget = qobject_cast<SkywardSwordWidget*>(m_widget);
    Q_ASSERT(mainWidget != NULL);
    QTabWidget* tw = mainWidget->tabWidget();
    tw->clear();
    if (filePath().isEmpty())
        return true;

    try
    {
        reader.setEndian(Athena::Endian::BigEndian);

        atUint32 magic = reader.readUint32();
        mainWidget->setRegion((Region)(magic & 0x000000FF));
        magic &= 0xFFFFFF00;

        if (magic != 0x534F5500)
            THROW_INVALID_DATA_EXCEPTION("Not a valid Skyward Sword file");

        reader.seek(0x1C, Athena::SeekOrigin::Begin);

        atUint32 headerSize = reader.readUint32();
        if (headerSize != 0x1D)
            THROW_INVALID_DATA_EXCEPTION("Invalid header size");

        atUint32 oldPos = reader.position();
        reader.seek(0x80, Athena::SeekOrigin::End);
        m_skipData = reader.readUBytes(0x80);
        reader.seek(oldPos, Athena::SeekOrigin::Begin);
        for (int i = 0; i < 3; i++)
        {
            char* data = (char*)reader.readBytes(0x53C0);
            char* skipData = new char[0x24];
            memcpy(skipData, (m_skipData + (0x24 * i)), 0x24);
            SkywardSwordQuestEditorForm* sw = new SkywardSwordQuestEditorForm(data, skipData, mainWidget->region());

            connect(sw, SIGNAL(modified()), this, SLOT(onModified()));
            connect(sw, SIGNAL(copy(SkywardSwordQuestEditorForm*)), this, SLOT(onCopy(SkywardSwordQuestEditorForm*)));
            connect(mainWidget, SIGNAL(regionChanged(Region)), sw, SLOT(setRegion(Region)));
            if (!sw->isNew())
                tw->addTab(sw, QIcon(QString(":/icons/Game%1").arg(i+1)), QString("&%1 %2").arg(i+1).arg(sw->playerName()));
            else
                tw->addTab(sw, QIcon(QString(":/icons/Game%1").arg(i+1)), tr("&%1 New Game").arg(i + 1));
        }

        setDirty(false);
        emit modified();
        return true;
    }
    catch (Athena::error::Exception e)
    {
        qWarning() << QString::fromStdString(e.message());
    }

    return false;
}

