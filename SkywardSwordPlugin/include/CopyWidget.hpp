// This file is part of WiiKing2 Editor.
//
// WiiKing2 Editor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Wiiking2 Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with WiiKing2 Editor.  If not, see <http://www.gnu.org/licenses/>

#ifndef COPYWIDGET_HPP
#define COPYWIDGET_HPP

#include <QDialog>

namespace Ui {
class CopyWidget;
}

class CopyWidget : public QDialog
{
    Q_OBJECT

public:
    explicit CopyWidget(QWidget *parent = 0);
    ~CopyWidget();

    enum Quest
    {
        Quest1,
        Quest2,
        Quest3
    };

    void setQuestEnabled(Quest quest, bool enabled);
    bool questChecked(Quest quest);

protected:
    void showEvent(QShowEvent* e);
private:
    Ui::CopyWidget *ui;
};

#endif // COPYWIDGET_HPP
