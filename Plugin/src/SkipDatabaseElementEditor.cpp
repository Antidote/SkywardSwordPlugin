#include "SkipDatabaseElementEditor.hpp"
#include "ui_SkipDatabaseElementEditor.h"

SkipDatabaseElementEditor::SkipDatabaseElementEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SkipDatabaseElementEditor)
{
    ui->setupUi(this);
}

SkipDatabaseElementEditor::~SkipDatabaseElementEditor()
{
    delete ui;
}
