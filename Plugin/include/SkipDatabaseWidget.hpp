#ifndef SKIPDATABASEDIALOG_HPP
#define SKIPDATABASEDIALOG_HPP

#include <QDialog>

namespace Ui {
class SkipDatabaseDialog;
}

class SkipDatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SkipDatabaseDialog(QWidget *parent = 0);
    ~SkipDatabaseDialog();

private:
    Ui::SkipDatabaseDialog *ui;
};

#endif // SKIPDATABASEDIALOG_HPP
