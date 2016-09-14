#include "sidechainpage.h"
#include "ui_sidechainpage.h"

#include "base58.h"
#include "coins.h"
#include "guiutil.h"
#include "init.h"
#include "main.h"
#include "primitives/drivechain.h"
#include "txdb.h"
#include "wallet/wallet.h"

#include <QApplication>
#include <QClipboard>
#include <QDialog>
#include <QStackedWidget>

SidechainPage::SidechainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SidechainPage)
{
    ui->setupUi(this);

    // Initialize models and table views
    incomingTableView = new QTableView(this);
    outgoingTableView = new QTableView(this);
    incomingTableModel = new DrivechainHistoryTableModel(this);
    outgoingTableModel = new DrivechainHistoryTableModel(this);

    // Set table models
    incomingTableView->setModel(incomingTableModel);
    outgoingTableView->setModel(outgoingTableModel);

    // Table style
#if QT_VERSION < 0x050000
    incomingTableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    incomingTableView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    outgoingTableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    outgoingTableView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#else
    incomingTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    incomingTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    outgoingTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    outgoingTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#endif

    // Display tables
    ui->frameIncoming->layout()->addWidget(incomingTableView);
    ui->frameOutgoing->layout()->addWidget(outgoingTableView);
}

SidechainPage::~SidechainPage()
{
    delete ui;
}

void SidechainPage::on_pushButtonWithdraw_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageWithdraw);
}

void SidechainPage::on_pushButtonDeposit_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageDeposit);
}

void SidechainPage::on_pushButtonCopy_clicked()
{

}

void SidechainPage::on_pushButtonNew_clicked()
{

}

void SidechainPage::on_pushButtonWT_clicked()
{
    if (!validate()) return;
    if (!pdrivechaintree) return;
    if (pwalletMain->IsLocked()) return;

    // WT
    drivechainWithdraw wt;

    // Set wt KeyID
    CBitcoinAddress address(ui->payTo->text().toStdString());
    if (!address.GetKeyID(wt.keyID)) {
        // TODO make pretty
        QDialog error;
        error.setWindowTitle("Invalid address");
        error.exec();
        return;
    }

    // Set wt sidechain ID
    wt.sidechainid = SIDECHAIN_ID;

    // Send payment to the wt script
    std::vector<CRecipient> vecSend;
    CRecipient recipient = {wt.GetScript(), ui->payAmount->value(), false};
    vecSend.push_back(recipient);

    CWalletTx wtx;
    CReserveKey reserveKey(pwalletMain);
    CAmount nFee;
    int nChangePos = -1;
    std::string strError;
    if (!pwalletMain->CreateTransaction(vecSend, wtx, reserveKey, nFee, nChangePos, strError))
    {
        // TODO make pretty
        QDialog error;
        error.setWindowTitle(QString::fromStdString(strError));
        error.exec();
        return;
    }

    if (!pwalletMain->CommitTransaction(wtx, reserveKey))
    {
        QDialog error;
        error.setWindowTitle("Failed to commit wt transaction");
        error.exec();
        return;
    }
}

void SidechainPage::on_addressBookButton_clicked()
{

}

void SidechainPage::on_pasteButton_clicked()
{
    // Paste text from clipboard into recipient field
    ui->payTo->setText(QApplication::clipboard()->text());
}

void SidechainPage::on_deleteButton_clicked()
{
    ui->payTo->clear();
}

bool SidechainPage::validate()
{
    // Check drivechain index
    if (!pdrivechaintree) return false;

    // Check address
    CBitcoinAddress address(ui->payTo->text().toStdString());
    if (!address.IsValid()) return false;

    if (!ui->payAmount->validate()) return false;

    // Sending a zero amount is invalid
    if (ui->payAmount->value(0) <= 0)
    {
        ui->payAmount->setValid(false);
        return false;
    }

    // Reject dust outputs:
    if (GUIUtil::isDust(ui->payTo->text(), ui->payAmount->value())) {
        ui->payAmount->setValid(false);
        return false;
    }

    return true;
}
