#include "actionwindow.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

ActionWindow::ActionWindow(const QString& title,
                           const QString& message,
                           QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setModal(true);

    auto* layout = new QVBoxLayout(this);

    auto* label = new QLabel(message, this);
    label->setWordWrap(true);
    layout->addWidget(label);

    auto* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn);

    resize(420, 200);
}
