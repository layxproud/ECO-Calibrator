#include "variantmaptablemodel.h"

VariantMapTableModel::VariantMapTableModel(QObject *parent) : QAbstractTableModel(parent)
{

}

void VariantMapTableModel::registerColumn(AbstractColumn *column)
{
    _columns.append(column);
}

void VariantMapTableModel::addRow(QVariantMap rowData)
{
    int id = rowData.value("id").toInt();
    beginInsertRows(QModelIndex(), _rowIndex.count(), _rowIndex.count());
    _rowIndex.append(id);
    _dataHash.insert(id, rowData);
    endInsertRows();
}

int VariantMapTableModel::idByRow(int row) const
{
    return _rowIndex.at(row);
}

int VariantMapTableModel::colByName(QString name) const
{
    for (int col = 0; col < _columns.count(); col++) {
        if (nameByCol(col) == name)
            return col;
    }
    return -1;
}

QString VariantMapTableModel::nameByCol(int col) const
{
    return _columns.at(col)->name();
}

int VariantMapTableModel::rowCount(const QModelIndex &parent) const
{
    return _rowIndex.count();
}

int VariantMapTableModel::columnCount(const QModelIndex &parent) const
{
    return _columns.count();
}

QVariant VariantMapTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int id = idByRow(index.row());
    QVariantMap rowData = _dataHash.value(id);
    return _columns.at(index.column())->colData(rowData, role);
}

SimpleColumn::SimpleColumn(QString name) : AbstractColumn(name)
{

}

QVariant SimpleColumn::colData(const QVariantMap &rowData, int role)
{
    if(role == Qt::DisplayRole)
        return QVariant();
    return rowData.value(name());
}

AbstractColumn::AbstractColumn(QString name) : _name(name)
{

}
