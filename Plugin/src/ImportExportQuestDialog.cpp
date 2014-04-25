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

#include "ImportExportQuestDialog.hpp"
#include "ui_ImportExportQuestDialog.h"
#include "SkywardSwordEditorForm.hpp"
#include "SkywardSwordPlugin.hpp"
#include <MainWindowBase.hpp>
#include <Athena/ZQuestFile.hpp>
#include <Athena/ZQuestFileWriter.hpp>
#include <Athena/ZQuestFileReader.hpp>
#include <Athena/Exception.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
struct ExportFmt
{
    char GameData[0x53C0];
    char SkipData[0x24];
};

ImportExportQuestDialog::ImportExportQuestDialog(QWidget *parent, Mode mode) :
    QDialog(parent),
    ui(new Ui::ImportExportQuestDialog),
    m_mode(mode),
    m_quest(NULL)
{
    ui->setupUi(this);
    SkywardSwordQuestEditorForm* editorForm = qobject_cast<SkywardSwordQuestEditorForm*>(this->parent());
    ui->statusLabel->clear();
    if (editorForm)
    {
        if (mode == Export)
        {
            setWindowTitle("Export Quest");
            ui->nameLineEdit->setText(editorForm->playerName());
            ui->rupeeSpinBox->setValue(editorForm->rupees());
            ui->hpCurrentSpinBox->setValue(editorForm->currentHP());
            ui->hpMaxSpinBox->setValue(editorForm->totalHP());

            connect(ui->nameLineEdit, SIGNAL(textChanged(QString)), editorForm, SLOT(setPlayerName(QString)));
            connect(ui->rupeeSpinBox, SIGNAL(valueChanged(int)), editorForm, SLOT(setRupees(int)));
            connect(ui->hpCurrentSpinBox, SIGNAL(valueChanged(int)), editorForm, SLOT(setCurrentHP(int)));
            connect(ui->hpMaxSpinBox, SIGNAL(valueChanged(int)), editorForm, SLOT(setTotalHP(int)));
            ui->compressChkBox->setVisible(true);
            ui->importPathgroup->setVisible(false);
            // Resize the window to hide the gap
            this->resize(this->width(), this->height() - (ui->importPathgroup->height() * 2));
        }
        else
        {
            setWindowTitle("Import Quest");
            ui->compressChkBox->setVisible(false);
            ui->importPathgroup->setVisible(true);

            connect(ui->importPushBtn, SIGNAL(clicked()), this, SLOT(onPathPressed()));
        }
    }
}

ImportExportQuestDialog::~ImportExportQuestDialog()
{
    delete m_quest;
    delete ui;
}

void ImportExportQuestDialog::onPathPressed()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Import Quest"), QString(), "ZQuest files (*.zquest)");

    if (!path.isEmpty())
    {
        ui->importLineEdit->setText(path);
        try
        {
            Athena::io::ZQuestFileReader reader(path.toStdString());
            m_quest = reader.read();
            qDebug() << m_quest->gameString().c_str();
            if (m_quest->data() && m_quest->gameString() == "Skyward Sword")
            {
                if (m_quest->length() < 0x53C0)
                {
                    qWarning() << "Not a valid Skyward Sword quest export";
                    ui->statusLabel->setText("Failed to load, check the application log");
                    return;
                }

                ExportFmt* data = (ExportFmt*)m_quest->data();
                char* gameData = new char[0x53C0];
                char* skipData = new char[0x24];

                memcpy(gameData, data->GameData, 0x53C0);
                Region region = (Region)(qFromBigEndian(*(quint32*)gameData) & 0x000000FF);

                if (m_quest->length() == 0x53E4)
                    memcpy(skipData, data->SkipData, 0x24);
                else
                    memset(skipData, 0, 0x24);

                SkywardSwordQuestEditorForm game(gameData, skipData, region);
                ui->nameLineEdit->setText(game.playerName());
                ui->rupeeSpinBox->setValue(game.rupees());
                ui->hpCurrentSpinBox->setValue(game.currentHP());
                ui->hpMaxSpinBox->setValue(game.totalHP());
                ui->statusLabel->setText("Loaded");
            }
            else
            {
                delete m_quest;
                m_quest = NULL;
                qWarning() << "Not a valid Skyward Sword quest export";
                ui->statusLabel->setText("Failed to load, check the application log");
            }
        }
        catch(Athena::error::Exception e)
        {
            m_quest = NULL;
            ui->statusLabel->setText("Failed to load, check the application log");
            qWarning() << QString::fromStdString(e.message());
        }
    }
}

void ImportExportQuestDialog::accept()
{
    SkywardSwordQuestEditorForm* editorForm = qobject_cast<SkywardSwordQuestEditorForm*>(parent());

    if (editorForm)
    {
        try
        {
            if (m_mode == Export)
            {
                QString filename = QFileDialog::getSaveFileName(this, tr("Export Quest"), QString(), "ZQuest files (*.zquest)");
                if (!filename.isEmpty())
                {
                    Athena::ZQuestFile* zquest = new Athena::ZQuestFile();
                    ExportFmt* exportData = new ExportFmt;
                    memcpy(exportData->GameData, editorForm->gameData(), 0x53C0);
                    memcpy(exportData->SkipData, editorForm->skipData(), 0x24);

                    zquest->setData((Uint8*)exportData, sizeof(ExportFmt));

                    // LOZ:Skyward Sword
                    zquest->setGameString("Skyward Sword");
                    // LOZ:Skyward sword is always big endian
                    zquest->setEndian(Athena::Endian::BigEndian);

                    Athena::io::ZQuestFileWriter writer(filename.toStdString());
                    writer.write(zquest, ui->compressChkBox->isChecked());
                    delete zquest;
                }
            }
            else if (m_quest)
            {
                if (!ui->importLineEdit->text().isEmpty())
                {
                    ExportFmt* data = (ExportFmt*)m_quest->data();
                    char* gameData = new char[0x53C0];
                    char* skipData = new char[0x24];
                    memcpy(gameData, data->GameData, 0x53C0);

                    if (m_quest->length() == 0x53E4)
                        memcpy(skipData, data->SkipData, 0x24);
                    else
                        memset(skipData, 0, 0x24);

                    editorForm->setGameData(QByteArray(gameData, 0x53C0));
                    editorForm->setSkipData(QByteArray(skipData, 0x24));
                    editorForm->setPlayerName(ui->nameLineEdit->text());
                    editorForm->setRupees(ui->rupeeSpinBox->value());
                    editorForm->setCurrentHP(ui->hpCurrentSpinBox->value());
                    editorForm->setTotalHP(ui->hpMaxSpinBox->value());
                }
                else
                {
                    QMessageBox mbox(QMessageBox::Warning, tr("No file specified"), tr("No file was selected to import"));
                    mbox.exec();
                }
            }
        }
        catch(Athena::error::Exception e)
        {
            qCritical() << QString::fromStdString(e.message());
        }

    }
    QDialog::accept();
}

