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

#include "SkipDatabaseWidget.hpp"
#include "ui_SkipDatabaseWidget.h"
#include <QTreeWidgetItem>
#include <QDebug>
#include <QFile>
#include <QDomDocument>
#include <SkywardSwordPlugin.hpp>
#include <MainWindowBase.hpp>

SkipDatabaseWidget::SkipDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkipDatabaseWidget),
    m_newElementCount(0)
{
    ui->setupUi(this);
    loadDatabase();
    ui->treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);
}

SkipDatabaseWidget::~SkipDatabaseWidget()
{
    delete ui;
}

QList<SkipElement> SkipDatabaseWidget::skipDatabase() const
{
    return m_skipDatabase;
}

void SkipDatabaseWidget::saveDatabase()
{
    SkywardSwordPlugin* plugin = SkywardSwordPlugin::instance();
    QFile file(QString(plugin->mainWindow()->homePath().absolutePath() + QDir::separator() + plugin->name() + QDir::separator() + "skipdb.xml"));
    if (file.open(QFile::WriteOnly))
    {
        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.writeStartDocument("1.0", true);
        writer.writeStartElement("SkipDatabase");
        for (int i = 0; i < m_skipDatabase.count(); i++)
        {
            const SkipElement& data = m_skipDatabase.at(i);
            writer.writeStartElement("SkipData");
            writer.writeAttribute("objectName", data.objectName);
            writer.writeAttribute("text", data.text);
            writer.writeAttribute("offset", QString("0x%1").arg(data.offset, 2, 16, QLatin1Char('0')));
            writer.writeAttribute("bit", QVariant(data.bit).toString());
            writer.writeAttribute("visible", QVariant(data.visible).toString());
            writer.writeEndElement();
        }
        writer.writeEndElement();
        writer.writeEndDocument();
    }
}

void SkipDatabaseWidget::loadDatabase()
{
    m_skipDatabase.clear();
    SkywardSwordPlugin* plugin = SkywardSwordPlugin::instance();
    QFile file(QString(plugin->mainWindow()->homePath().absolutePath() + QDir::separator() + plugin->name() + QDir::separator() + "skipdb.xml"));
    if (file.open(QFile::ReadOnly))
    {
        qDebug() << "Parsing skip database file";
        QDomDocument document;
        if (!document.setContent(&file))
        {
            file.close();
            return;
        }

        QDomElement rootElement = document.documentElement();
        QDomElement elem = rootElement.firstChildElement("SkipData");
        while (!elem.isNull())
        {
            QString objName = elem.attribute("objectName");
            QString text = elem.attribute("text");
            QString offsetHex = elem.attribute("offset");
            bool ok = 0;
            quint32 offset = offsetHex.remove("0x", Qt::CaseInsensitive).toUInt(&ok, 16);
            if (!ok)
                qWarning() << "SkipElement offset invalid";
            quint32 bit = elem.attribute("bit").toUInt(&ok);

            if (!ok)
                qWarning() << "SkipElement bit invalid";

            bool visible = QVariant(elem.attribute("visible")).toBool();
            m_skipDatabase <<  SkipElement{objName,  text, offset, bit, visible};
            elem = elem.nextSiblingElement("SkipData");
        }

        emit skipDatabaseChanged();
        buildTree();
    }
}

void SkipDatabaseWidget::onAdd()
{
    m_skipDatabase << SkipElement{QString("newSkipElement%1").arg(m_newElementCount++), QString("newSkipElement%1").arg(m_newElementCount-1), 0x00, 0, true};
    SkipDatabaseElementEditor sde(this);
    sde.setSkipElement((SkipElement*)&m_skipDatabase.at(m_skipDatabase.count() - 1));
    sde.exec();

    if (sde.result() == QDialog::Rejected)
    {
        m_skipDatabase.removeLast();
        return;
    }

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, sde.skipElement()->objectName);
    item->setText(1, sde.skipElement()->text);
    item->setText(2, QString("0x%1").arg(sde.skipElement()->offset, 2, 16, QLatin1Char('0')));
    item->setText(3, QVariant(sde.skipElement()->bit).toString());
    item->setCheckState(4, (sde.skipElement()->visible ? Qt::Checked : Qt::Unchecked));

    ui->treeWidget->addTopLevelItem(item);
    emit skipDatabaseChanged();
}

void SkipDatabaseWidget::onEdit()
{
    if (ui->treeWidget->currentItem())
    {
        QTreeWidgetItem* item = ui->treeWidget->currentItem();
        SkipElement* elem = NULL;
        for (QList<SkipElement>::iterator i = m_skipDatabase.begin(); i != m_skipDatabase.end(); ++i)
        {
            if ((*i).objectName == item->text(ObjectNameRow))
            {
                elem = &(*i);
                break;
            }
        }

        if (!elem)
            return;

        SkipDatabaseElementEditor sde(this);
        sde.setSkipElement(elem);
        sde.exec();

        item->setText(0, sde.skipElement()->objectName);
        item->setText(1, sde.skipElement()->text);
        item->setText(2, QString("0x%1").arg(sde.skipElement()->offset, 2, 16, QLatin1Char('0')));
        item->setText(3, QVariant(sde.skipElement()->bit).toString());
        item->setCheckState(4, (sde.skipElement()->visible ? Qt::Checked : Qt::Unchecked));
        emit skipDatabaseChanged();
    }
}

void SkipDatabaseWidget::onRemove()
{
    if (ui->treeWidget->currentItem())
    {
        QString objectName = ui->treeWidget->currentItem()->text(0);
        int index = -1;
        foreach (SkipElement element, m_skipDatabase)
        {
            if (element.objectName == objectName)
            {
                index++;
                break;
            }
            index++;
        }

        m_skipDatabase.removeAt(index);
        delete ui->treeWidget->currentItem();
        emit skipDatabaseChanged();
    }
}

void SkipDatabaseWidget::onItemClicked(QTreeWidgetItem* item, int row)
{
    if (row == VisibleRow)
    {
        for (QList<SkipElement>::iterator i = m_skipDatabase.begin(); i != m_skipDatabase.end(); ++i)
        {
            SkipElement& elem = (*i);
            if (elem.objectName == item->text(ObjectNameRow))
            {
                if (elem.visible == (item->checkState(VisibleRow) == Qt::Checked ? true : false))
                    break;

                elem.visible = (item->checkState(VisibleRow) == Qt::Checked ? true : false);
                emit skipDatabaseChanged();
                break;
            }
        }
    }
}

void SkipDatabaseWidget::onMoveItem()
{
    if (!ui->treeWidget->currentItem())
        return;
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    int row  = ui->treeWidget->currentIndex().row();

    if (sender() == ui->itemUpPushButton)
    {
        if (item && row > 0)
        {
            ui->treeWidget->takeTopLevelItem(row);
            ui->treeWidget->insertTopLevelItem(row - 1, item);
            ui->treeWidget->setCurrentItem(item);
            SkipElement elem = m_skipDatabase.takeAt(row);
            m_skipDatabase.insert(row - 1, elem);
        }
    }
    else if (sender() == ui->itemDownPushButton)
    {
        if (item && row < (ui->treeWidget->topLevelItemCount() - 1))
        {
            ui->treeWidget->takeTopLevelItem(row);
            ui->treeWidget->insertTopLevelItem(row + 1, item);
            ui->treeWidget->setCurrentItem(item);
            SkipElement elem = m_skipDatabase.takeAt(row);
            m_skipDatabase.insert(row + 1, elem);
        }
    }

    emit skipDatabaseChanged();
}

void SkipDatabaseWidget::onCurrentItemChanged()
{
    bool enable = ui->treeWidget->currentItem() != NULL;
    ui->editItemPushButton->setEnabled(enable);
    ui->removeItemPushButton->setEnabled(enable);
    ui->itemUpPushButton->setEnabled((enable && ui->treeWidget->currentIndex().row() > 0));
    ui->itemDownPushButton->setEnabled((enable && ui->treeWidget->currentIndex().row() < m_skipDatabase.count() - 1));
}

void SkipDatabaseWidget::showEvent(QShowEvent* se)
{
    QWidget::showEvent(se);
}

void SkipDatabaseWidget::buildTree()
{
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        delete ui->treeWidget->takeTopLevelItem(i);

    ui->treeWidget->clear();

    foreach (SkipElement elem, m_skipDatabase)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem;
        item->setText(0, elem.objectName);
        item->setText(1, elem.text);
        item->setText(2, QString("0x%1").arg(elem.offset, 2, 16, QLatin1Char('0')));
        item->setText(3, QVariant(elem.bit).toString());
        item->setCheckState(4, (elem.visible ? Qt::Checked : Qt::Unchecked));

        ui->treeWidget->addTopLevelItem(item);
    }
}










