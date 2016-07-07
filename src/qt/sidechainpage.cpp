#include "sidechainpage.h"
#include "ui_sidechainpage.h"

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
