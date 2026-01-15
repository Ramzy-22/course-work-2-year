#include "deletecontactsdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>

#include <vector>

static QString qs(const std::string& s) { return QString::fromStdString(s); }

DeleteContactsDialog::DeleteContactsDialog(PhoneBook* book, QWidget* parent)
    : QDialog(parent), m_book(book)
{
    setWindowTitle("Delete Contact");
    setModal(true);

    auto* root = new QVBoxLayout(this);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"ID", "First", "Last", "Email", "Work phone"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    root->addWidget(m_table);

    auto* bottom = new QHBoxLayout();
    root->addLayout(bottom);

    m_btnDelete = new QPushButton("Delete Selected", this);
    m_btnRefresh = new QPushButton("Refresh", this);
    auto* closeBtn = new QPushButton("Close", this);

    bottom->addWidget(m_btnDelete);
    bottom->addWidget(m_btnRefresh);
    bottom->addStretch();
    bottom->addWidget(closeBtn);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_btnRefresh, &QPushButton::clicked, this, &DeleteContactsDialog::refreshTable);
    connect(m_btnDelete, &QPushButton::clicked, this, &DeleteContactsDialog::deleteSelected);

    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int, int){ deleteSelected(); });

    resize(760, 420);
    refreshTable();
}

void DeleteContactsDialog::refreshTable()
{
    if (!m_book) {
        QMessageBox::critical(this, "Error", "Internal error: PhoneBook is not available.");
        return;
    }

    std::vector<std::pair<unsigned int, Contact>> rows;
    rows.reserve(m_book->mainStorage.size());
    for (const auto& p : m_book->mainStorage) rows.push_back(p);

    m_table->setRowCount(static_cast<int>(rows.size()));

    for (int r = 0; r < static_cast<int>(rows.size()); ++r) {
        const unsigned int id = rows[r].first;
        const Contact& c = rows[r].second;

        auto set = [&](int col, const QString& text) {
            auto* it = new QTableWidgetItem(text);
            m_table->setItem(r, col, it);
        };

        set(0, QString::number(id));
        set(1, qs(c.firstName));
        set(2, qs(c.lastName));
        set(3, qs(c.email));
        set(4, qs(c.numbers.number1));
    }

    m_table->resizeColumnsToContents();
}

void DeleteContactsDialog::deleteSelected()
{
    auto ranges = m_table->selectedRanges();
    if (ranges.isEmpty()) {
        QMessageBox::information(this, "Delete Contact", "Select a contact row first.");
        return;
    }

    const int row = ranges.first().topRow();
    auto* idItem = m_table->item(row, 0);
    if (!idItem) return;

    bool ok = false;
    const unsigned int id = idItem->text().toUInt(&ok);
    if (!ok) return;

    auto it = m_book->mainStorage.find(id);
    if (it == m_book->mainStorage.end()) {
        QMessageBox::warning(this, "Not Found", "The selected contact no longer exists.");
        refreshTable();
        return;
    }

    const Contact& c = it->second;

    const QString msg = QString("Delete this contact?\n\nID: %1\nName: %2 %3\nEmail: %4")
                            .arg(id)
                            .arg(qs(c.firstName))
                            .arg(qs(c.lastName))
                            .arg(qs(c.email));

    if (QMessageBox::question(this, "Confirm Delete", msg,
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    std::string err;
    if (!m_book->remove_contact(id, &err)) {
        QMessageBox::warning(this, "Delete Failed", QString::fromStdString(err));
        return;
    }

    QMessageBox::information(this, "Success", "Deletion Successful.");
    refreshTable();
}
