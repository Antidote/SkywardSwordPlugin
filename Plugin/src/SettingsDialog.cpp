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

#include "SettingsDialog.hpp"
#include "SettingsManager.hpp"
#include "Constants.hpp"
#include "ui_SettingsDialog.h"
#include "SkywardSwordPlugin.hpp"
#include "Common.hpp"
#include <QShowEvent>

#include <QSettings>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    PluginSettingsDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    SkipDatabaseWidget* sdw = qobject_cast<SkipDatabaseWidget*>(ui->skipDatabaseTab);
    connect(sdw, SIGNAL(skipDatabaseChanged()), this, SIGNAL(skipDatabaseChanged()));
    restoreWidgetGeom(this, "settingsDialogGeom");
}

SettingsDialog::~SettingsDialog()
{
    saveWidgetGeom(this, "settingsDialogGeom");
    delete ui;
}

QWidget* SettingsDialog::centralWidget() const
{
    return ui->settingsTab;
}

QList<SkipElement> SettingsDialog::skipDatabase() const
{
    SkipDatabaseWidget* sdw = qobject_cast<SkipDatabaseWidget*>(ui->skipDatabaseTab);
    return sdw->skipDatabase();
}

void SettingsDialog::reloadSkipDatabase()
{
    SkipDatabaseWidget* sdw = qobject_cast<SkipDatabaseWidget*>(ui->skipDatabaseTab);
    sdw->loadDatabase();
}

void SettingsDialog::restoreOwnership()
{
    ui->settingsTab->setParent(NULL);
    ui->gridLayout->addWidget(ui->settingsTab, 0, 0, 2, 2);
    ui->settingsTab->show();
}

void SettingsDialog::loadSettings()
{
    // Region Settings
    SettingsManager* settings = SettingsManager::instance();
    ui->settingsTab->setCurrentIndex(0);
    switch(settings->defaultRegion())
    {
    case Region::NTSCU: ui->ntscURB->setChecked(true); break;
    case Region::NTSCK: ui->ntscKRB->setChecked(true); break;
    case Region::NTSCJ: ui->ntscJRB->setChecked(true); break;
    case Region::PAL:   ui->palRB  ->setChecked(true); break;
    default: break;
    }

    ui->ntscUNameLE      ->setText(settings->defaultPlayerNameForRegion(Region::NTSCU));
    ui->ntscUNameLE      ->setModified(false);
    ui->ntscKNameLE      ->setText(settings->defaultPlayerNameForRegion(Region::NTSCK));
    ui->ntscKNameLE      ->setModified(false);
    ui->ntscJNameLE      ->setText(settings->defaultPlayerNameForRegion(Region::NTSCJ));
    ui->ntscJNameLE      ->setModified(false);
    ui->palNameLE        ->setText(settings->defaultPlayerNameForRegion(Region::PAL  ));
    ui->palNameLE        ->setModified(false);
    ui->updateUrlLineEdit->setText(settings->updateUrl());
    ui->updateUrlLineEdit->setModified(false);
    ui->checkOnStart     ->setChecked(settings->updateCheckOnStart());
}

void SettingsDialog::accept()
{
    SettingsManager* settings = SettingsManager::instance();

    const QAbstractButton* regionBtn = ui->regionBtnGrp->checkedButton();
    if (regionBtn == ui->ntscURB)
        settings->setDefaultRegion(Region::NTSCU);
    else if (regionBtn == ui->ntscKRB)
        settings->setDefaultRegion(Region::NTSCK);
    else if (regionBtn == ui->ntscJRB)
        settings->setDefaultRegion(Region::NTSCJ);
    else if (regionBtn == ui->palRB)
        settings->setDefaultRegion(Region::PAL);

    if (ui->ntscUNameLE->isModified() && !ui->ntscUNameLE->text().isEmpty())
        settings->setDefaultPlayerNameForRegion(Region::NTSCU, ui->ntscUNameLE->text());
    if (ui->ntscKNameLE->isModified() && !ui->ntscKNameLE->text().isEmpty())
        settings->setDefaultPlayerNameForRegion(Region::NTSCK, ui->ntscKNameLE->text());
    if (ui->ntscJNameLE->isModified() && !ui->ntscJNameLE->text().isEmpty())
        settings->setDefaultPlayerNameForRegion(Region::NTSCJ, ui->ntscJNameLE->text());
    if (ui->palNameLE->isModified() && !ui->palNameLE->text().isEmpty())
        settings->setDefaultPlayerNameForRegion(Region::PAL, ui->palNameLE->text());
    if (ui->updateUrlLineEdit->isModified() && !ui->updateUrlLineEdit->text().isEmpty())
        settings->setUpdateUrl(ui->updateUrlLineEdit->text());
    settings->setUpdateCheckOnStart(ui->checkOnStart->isChecked());
    SkipDatabaseWidget* sdw = qobject_cast<SkipDatabaseWidget*>(ui->skipDatabaseTab);
    sdw->saveDatabase();
    settings->saveSettings();
    QDialog::accept();
}

void SettingsDialog::onTextChanged(QString text)
{
    QRegExp http("^(http|https)://", Qt::CaseInsensitive);
    if (sender() == ui->updateUrlLineEdit)
    {
        if (text.isEmpty() || !text.contains(http))
            ui->updateUrlLineEdit->setProperty("valid", false);
        else
            ui->updateUrlLineEdit->setProperty("valid", true);

        style()->polish(ui->updateUrlLineEdit);

        // If the text matches what is currently stored
        // Set the line edit is unmodified
        if (text == QSettings().value(Constants::Settings::SKYWARDSWORD_UPDATE_URL, Constants::Settings::SKYWARDSWORD_UPDATE_URL_DEFAULT).toString())
            ui->updateUrlLineEdit->setModified(false);
    }
}

void SettingsDialog::showEvent(QShowEvent* se)
{
    QDialog::showEvent(se);
    loadSettings();
}
