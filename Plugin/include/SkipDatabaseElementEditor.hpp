#ifndef SKIPDATABASEELEMENTEDITOR_HPP
#define SKIPDATABASEELEMENTEDITOR_HPP

#include <QDialog>

namespace Ui {
class SkipDatabaseElementEditor;
}

class SkipDatabaseElementEditor : public QDialog
{
    Q_OBJECT

public:
    explicit SkipDatabaseElementEditor(QWidget *parent = 0);
    ~SkipDatabaseElementEditor();

private:
    Ui::SkipDatabaseElementEditor *ui;
};

#endif // SKIPDATABASEELEMENTEDITOR_HPP
