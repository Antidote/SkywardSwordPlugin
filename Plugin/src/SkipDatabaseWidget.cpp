#include "include/SkipDatabaseDialog.hpp"
#include "ui_SkipDatabaseDialog.h"

SkipDatabaseDialog::SkipDatabaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SkipDatabaseDialog)
{
    ui->setupUi(this);
}

SkipDatabaseDialog::~SkipDatabaseDialog()
{
    delete ui;
}
