#ifndef DRIVECHAINHISTORYTABLEMODEL_H
#define DRIVECHAINHISTORYTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>

/** Model provides list of drivechain WT / WT^ transaction requests.
 * Filtered by height, ownership (are any wt(s) mine?), type (in / out).
 */
class DrivechainHistoryTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /* Display filtered list of drivechain WT requests */
    explicit DrivechainHistoryTableModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    QList<QVariant> filteredObjects;
};

#endif // DRIVECHAINHISTORYTABLEMODEL_H
