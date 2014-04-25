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

#ifndef SKIPDATABASEDIALOG_HPP
#define SKIPDATABASEDIALOG_HPP

#include <QDialog>
#include <QXmlStreamWriter>
#include <QHash>
#include <SkipDatabaseElementEditor.hpp>

namespace Ui {
class SkipDatabaseWidget;
}

class QTreeWidgetItem;


class SkipDatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SkipDatabaseWidget(QWidget *parent = 0);
    ~SkipDatabaseWidget();

    QList<SkipElement> skipDatabase() const;

public slots:
    void saveDatabase();
    void loadDatabase();
signals:
    void skipDatabaseChanged();
    void skipElementAdded();
    void skipElementRemoved();
    void skipElementEdited();

protected slots:
    void onAdd();
    void onEdit();
    void onRemove();
    void onItemClicked(QTreeWidgetItem* item, int row);
    void onMoveItem();
    void onCurrentItemChanged();
protected:
    void showEvent(QShowEvent *se);
private slots:
    void buildTree();
private:
    enum Rows
    {
        ObjectNameRow,
        TextRow,
        OffsetRow,
        BitRow,
        VisibleRow
    };

    Ui::SkipDatabaseWidget *ui;
    QList<SkipElement> m_skipDatabase;
    int                m_newElementCount;
};

#endif // SKIPDATABASEDIALOG_HPP
