#ifndef VARIANTMAPTABLEMODEL_H
#define VARIANTMAPTABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>

class AbstractColumn
{
public:
    AbstractColumn(QString name);
    QString name() { return _name; }
    virtual QVariant colData(const QVariantMap &rowData, int role = Qt::DisplayRole) = 0;
private:
    QString _name;
};

class SimpleColumn : public AbstractColumn
{
public:
    SimpleColumn(QString name);

public:
    virtual QVariant colData(const QVariantMap &rowData, int role) override;
};

class VariantMapTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    VariantMapTableModel(QObject *parent = nullptr);
    void registerColumn(AbstractColumn* column);
    void addRow(QVariantMap rowData);

    int idByRow(int row) const;
    int colByName(QString name) const;
    QString nameByCol(int col) const;
private:
    QList<int> _rowIndex;
    QHash<int, QVariantMap> _dataHash;
    QList<AbstractColumn*> _columns;
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
};

#endif // VARIANTMAPTABLEMODEL_H
