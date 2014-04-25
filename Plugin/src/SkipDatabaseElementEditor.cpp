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

#include "SkipDatabaseElementEditor.hpp"
#include "ui_SkipDatabaseElementEditor.h"
#include <QDebug>

class HexValidator : public QValidator
{
public:
    HexValidator(QObject* parent = 0) :
        QValidator(parent)
    {}
protected:
    QValidator::State validate(QString &input, int &pos) const
    {
        Q_UNUSED(pos)
        input = input.toUpper();
        // match against needed regexp
        QRegExp rx("(?:[0-9a-fA-F]{2})*[0-9a-fA-F]{0,2}");
        if (rx.exactMatch(input)) {
            return QValidator::Acceptable;
        }
        return QValidator::Invalid;
    }
};

SkipDatabaseElementEditor::SkipDatabaseElementEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SkipDatabaseElementEditor)
{
    ui->setupUi(this);
    ui->offsetLineEdit->setValidator(new HexValidator);
    restoreWidgetGeom(this, "skipElementEditorGeom");
}

SkipDatabaseElementEditor::~SkipDatabaseElementEditor()
{
    saveWidgetGeom(this, "skipElementEditorGeom");
    delete ui;
}

void SkipDatabaseElementEditor::setSkipElement(SkipElement* skipData)
{
    m_currentElement = skipData;
    updateInfo();
}

SkipElement* SkipDatabaseElementEditor::skipElement() const
{
    return m_currentElement;
}

void SkipDatabaseElementEditor::onTextChanged(QString text)
{
    Q_UNUSED(text)
    if (!m_currentElement)
        return;

    if (sender() == ui->objectNameLineEdit)
        m_objectName = ui->objectNameLineEdit->text();
    else if (sender() == ui->textLineEdit)
        m_labelText = ui->textLineEdit->text();
    else if (sender() == ui->offsetLineEdit)
    {
        if (!ui->offsetLineEdit->hasAcceptableInput())
        {
            ui->offsetLineEdit->setProperty("valid", false);
        }
        else
        {
            ui->offsetLineEdit->setProperty("valid", true);
            m_offset = ui->offsetLineEdit->text().remove("0x", Qt::CaseInsensitive).toUInt(NULL, 16);
        }
        style()->polish(ui->offsetLineEdit);
    }
}

void SkipDatabaseElementEditor::onValueChanged(int value)
{
    if (!m_currentElement)
        return;

    m_bit = value;
}

void SkipDatabaseElementEditor::onToggled(bool value)
{
    m_visible = value;
}

void SkipDatabaseElementEditor::accept()
{
    applyInfo();

    QDialog::accept();
}


void SkipDatabaseElementEditor::applyInfo()
{
    m_currentElement->objectName = m_objectName;
    m_currentElement->text = m_labelText;
    m_currentElement->offset = m_offset;
    m_currentElement->bit = m_bit;
    m_currentElement->visible = m_visible;
}


void SkipDatabaseElementEditor::updateInfo()
{
    if (!m_currentElement)
        return;

    ui->objectNameLineEdit->setText((m_objectName = m_currentElement->objectName));
    ui->textLineEdit->setText((m_labelText = m_currentElement->text));
    ui->offsetLineEdit->setText(QString("%1").arg((m_offset = m_currentElement->offset), 2, 16, QLatin1Char('0')));
    ui->bitSpinBox->setValue((m_bit = m_currentElement->bit));
    ui->visibleChkBox->setChecked((m_visible = m_currentElement->visible));
}
