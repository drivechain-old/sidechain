#ifndef SIDECHAINPAGE_H
#define SIDECHAINPAGE_H

#include <QWidget>

namespace Ui {
class SidechainPage;
}

class SidechainPage : public QWidget
{
    Q_OBJECT

public:
    explicit SidechainPage(QWidget *parent = 0);
    ~SidechainPage();

private:
    Ui::SidechainPage *ui;
};

#endif // SIDECHAINPAGE_H
