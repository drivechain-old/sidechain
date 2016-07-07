#include "sidechainpage.h"
#include "ui_sidechainpage.h"

#include <QStackedWidget>

SidechainPage::SidechainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SidechainPage)
{
    ui->setupUi(this);
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
