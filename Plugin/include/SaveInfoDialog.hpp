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

#ifndef FILEINFODIALOG_H
#define FILEINFODIALOG_H

#include <QDialog>
#include "Common.hpp"
class SkywardSwordGameDocument;
class QAbstractButton;

namespace Ui {
    class SaveInfoDialog;
}

class SaveInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveInfoDialog(SkywardSwordGameDocument* document, QWidget *parent);
    ~SaveInfoDialog();

private slots:
    void showEvent(QShowEvent *);
    void onRegionChanged(QAbstractButton* btn, bool val);
    void accept();
private:
    enum StringType {Title, Subtitle};
    QString regionString(Region region, StringType type) const;
    Ui::SaveInfoDialog *ui;
    SkywardSwordGameDocument* m_document;
    Region               m_region;
};

#endif // FILEINFODIALOG_H
