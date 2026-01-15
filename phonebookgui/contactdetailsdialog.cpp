#include "contactdetailsdialog.h"

#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

static QLabel* makeLabel(const QString& s, QWidget* parent)
{
    auto* l = new QLabel(s, parent);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    l->setWordWrap(true);
    return l;
}

ContactDetailsDialog::ContactDetailsDialog(unsigned int id, const Contact& c, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QString("Contact Details (ID: %1)").arg(id));
    setModal(true);

    auto* root = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    root->addLayout(form);

    form->addRow("First name:",  makeLabel(QString::fromStdString(c.firstName), this));
    form->addRow("Middle name:", makeLabel(QString::fromStdString(c.middleName), this));
    form->addRow("Last name:",   makeLabel(QString::fromStdString(c.lastName), this));

    form->addRow("Email:",   makeLabel(QString::fromStdString(c.email), this));
    form->addRow("Address:", makeLabel(QString::fromStdString(c.address), this));
    form->addRow("Birthday:",makeLabel(QString::fromStdString(c.birthday), this));

    form->addRow("Work phone:",   makeLabel(QString::fromStdString(c.numbers.number1), this));
    form->addRow("Home phone:",   makeLabel(QString::fromStdString(c.numbers.number2), this));
    form->addRow("Office phone:", makeLabel(QString::fromStdString(c.numbers.number3), this));

    auto* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    root->addWidget(closeBtn);

    resize(520, 360);
}
