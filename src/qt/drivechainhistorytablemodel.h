#ifndef DRIVECHAINHISTORYTABLEMODEL_H
#define DRIVECHAINHISTORYTABLEMODEL_H

#include "walletmodel.h"

#include <QAbstractTableModel>
#include <QList>

class CWallet;

/** Model provides list of drivechain WT / WT^ transaction requests.
 * Filtered by height, ownership (are any wt(s) mine?), type (in / out).
 *
 * Part of wallet model.
 */
class DrivechainHistoryTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /* Display filtered list of drivechain WT requests */
    explicit DrivechainHistoryTableModel(CWallet *wallet, WalletModel *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    WalletModel *walletModel;
    QList<QVariant> filteredObjects;
};

#endif // DRIVECHAINHISTORYTABLEMODEL_H
