#ifndef SIDECHAINPAGE_H
#define SIDECHAINPAGE_H

#include "drivechainhistorytablemodel.h"

#include "amount.h"

#include <QTableView>
#include <QWidget>

class WalletModel;

namespace Ui {
class SidechainPage;
}

class SidechainPage : public QWidget
{
    Q_OBJECT

public:
    explicit SidechainPage(QWidget *parent = 0);
    ~SidechainPage();

    void generateQR(QString data);

    void setWalletModel(WalletModel *model);

public Q_SLOTS:
    // Passthrough to use existing signal
    void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance,
                    const CAmount& immatureBalance, const CAmount& watchOnlyBalance,
                    const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);

    void displayBalance(const CAmount& balance, const CAmount& pending);

private Q_SLOTS:
    void on_pushButtonWithdraw_clicked();

    void on_pushButtonDeposit_clicked();

    void on_pushButtonCopy_clicked();

    void on_pushButtonNew_clicked();

    void on_pushButtonWT_clicked();

    void on_addressBookButton_clicked();

    void on_pasteButton_clicked();

    void on_deleteButton_clicked();

private:
    Ui::SidechainPage *ui;

    WalletModel *walletModel;

    QTableView *incomingTableView;
    QTableView *outgoingTableView;

    DrivechainHistoryTableModel *incomingTableModel;
    DrivechainHistoryTableModel *outgoingTableModel;

    bool validate();

    void generateAddress();
};

#endif // SIDECHAINPAGE_H
