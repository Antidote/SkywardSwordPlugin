﻿// This file is part of Sakura Suite.
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

#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>
#include <PluginSettingsDialog.hpp>
#include "Common.hpp"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public PluginSettingsDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    QWidget* centralWidget() const;
    QList<SkipElement> skipDatabase() const;
    void reloadSkipDatabase();
    void restoreOwnership();
    void loadSettings();
signals:
    void skipDatabaseChanged();

public slots:
    void accept();
    void onTextChanged(QString text);
protected:
    void showEvent(QShowEvent *se);
private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_HPP
