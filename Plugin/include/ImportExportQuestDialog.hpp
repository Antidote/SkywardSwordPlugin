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

#ifndef EXPORTQUESTDIALOG_H
#define EXPORTQUESTDIALOG_H

#include <QDialog>

namespace Ui {
class ImportExportQuestDialog;
}

namespace Athena
{
class ZQuestFile;
}

class ImportExportQuestDialog : public QDialog
{
    Q_OBJECT
    
public:
    enum Mode
    {
        Import,
        Export
    };

    explicit ImportExportQuestDialog(QWidget *parent, Mode mode = Export);
    ~ImportExportQuestDialog();
public slots:
    void onPathPressed();
private slots:
    void accept();

private:
    Ui::ImportExportQuestDialog *ui;

    Mode m_mode;
    Athena::ZQuestFile* m_quest;
};

#endif // EXPORTQUESTDIALOG_H
