#include "drivechainhistorytablemodel.h"

DrivechainHistoryTableModel::DrivechainHistoryTableModel(CWallet *wallet, WalletModel *parent) :
    walletModel(parent)
{
    Q_UNUSED(wallet);

    /*
     * Table Columns:
     * 1. Type (incoming, outgoing)
     * 2. WT / WT^ ID
     * 3. Height
     */
}

int DrivechainHistoryTableModel::rowCount(const QModelIndex &parent) const
{
    return filteredObjects.length();
}

int DrivechainHistoryTableModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant DrivechainHistoryTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return false;
    }

    int row = index.row();
    int col = index.column();

    // Get the current object
    QVariant filteredObject = filteredObjects.at(row);

    switch (role) {
    case Qt::DecorationRole:
    {
        // Type icon
        if (col == 0) {
//            // Incoming icon
//            if (filteredObject.canConvert<drivechainIncoming *>()) {
//                return QPixmap(":/icons/warning").scaled(80, 80);
//            }
//            // Outgoing icon
//            if (filteredObject.canConvert<drivechainOutgoing *>()) {
//                return QPixmap(":/icons/warning").scaled(80, 80);
//            }
        }
    }
    case Qt::DisplayRole:
    {
        // WT ID
        if (col == 1) {
//            // Incoming
//            if (filteredObject.canConvert<drivechainIncoming *>()) {

//            }
//            // Outgoing
//            if (filteredObject.canConvert<drivechainOutgoing *>()) {

//            }
        }
        // Height
        if (col == 2) {
            //            // Incoming
            //            if (filteredObject.canConvert<drivechainIncoming *>()) {

            //            }
            //            // Outgoing
            //            if (filteredObject.canConvert<drivechainOutgoing *>()) {

            //            }
        }
    }
    }
    return QVariant();
}

QVariant DrivechainHistoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QString("Type/Icon");
            case 1:
                return QString("WT ID");
            case 2:
                return QString("Height");
            }
        }
    }
    return QVariant();
}
