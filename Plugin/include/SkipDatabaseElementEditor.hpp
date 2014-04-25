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

#ifndef SKIPDATABASEELEMENTEDITOR_HPP
#define SKIPDATABASEELEMENTEDITOR_HPP

#include <QDialog>
#include "Common.hpp"

namespace Ui {
class SkipDatabaseElementEditor;
}

class QAbstractButton;

class SkipDatabaseElementEditor : public QDialog
{
    Q_OBJECT

public:
    explicit SkipDatabaseElementEditor(QWidget *parent = 0);
    ~SkipDatabaseElementEditor();

    void setSkipElement(SkipElement* skipData);
    SkipElement* skipElement() const;

protected slots:
    void onTextChanged(QString text);
    void onValueChanged(int value);
    void onToggled(bool value);
    void accept();
    void applyInfo();

protected:
private:
    void updateInfo();
    Ui::SkipDatabaseElementEditor *ui;
    QString           m_objectName;
    QString           m_labelText;
    quint32           m_offset;
    quint32           m_bit;
    bool              m_visible;
    SkipElement*      m_currentElement;
};

#endif // SKIPDATABASEELEMENTEDITOR_HPP
