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
