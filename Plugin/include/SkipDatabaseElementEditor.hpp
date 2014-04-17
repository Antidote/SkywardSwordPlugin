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
