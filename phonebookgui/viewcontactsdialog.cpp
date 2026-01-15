#include "viewcontactsdialog.h"
#include "contactdetailsdialog.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

#include <vector>
#include <algorithm>

static QString qs(const std::string& s) { return QString::fromStdString(s); }

ViewContactsDialog::ViewContactsDialog(PhoneBook* book, QWidget* parent)
    : QDialog(parent), m_book(book)
{
    setWindowTitle("View Contacts");
    setModal(true);

    auto* root = new QVBoxLayout(this);

    // Top controls: sort
    auto* controls = new QHBoxLayout();
    root->addLayout(controls);

    controls->addWidget(new QLabel("Sort by:", this));
    m_sortField = new QComboBox(this);
    m_sortField->addItem("ID");
    m_sortField->addItem("First name");
    m_sortField->addItem("Last name");
    m_sortField->addItem("Email");
    controls->addWidget(m_sortField);

    controls->addWidget(new QLabel("Order:", this));
    m_sortOrder = new QComboBox(this);
    m_sortOrder->addItem("Ascending");
    m_sortOrder->addItem("Descending");
    controls->addWidget(m_sortOrder);

    m_btnApplySort = new QPushButton("Apply", this);
    controls->addWidget(m_btnApplySort);

    m_btnRefresh = new QPushButton("Refresh", this);
    controls->addWidget(m_btnRefresh);

    controls->addStretch();

    m_countLabel = new QLabel(this);
    controls->addWidget(m_countLabel);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(10);
    m_table->setHorizontalHeaderLabels({
        "ID", "First", "Middle", "Last", "Email",
        "Work", "Home", "Office", "Address", "Birthday"
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    root->addWidget(m_table);

    // Bottom buttons
    auto* bottom = new QHBoxLayout();
    root->addLayout(bottom);

    m_btnViewSelected = new QPushButton("View Selected", this);
    bottom->addWidget(m_btnViewSelected);

    auto* closeBtn = new QPushButton("Close", this);
    bottom->addWidget(closeBtn);
    bottom->addStretch();

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_btnRefresh, &QPushButton::clicked, this, &ViewContactsDialog::refreshTable);
    connect(m_btnApplySort, &QPushButton::clicked, this, &ViewContactsDialog::applySortAndRefresh);
    connect(m_btnViewSelected, &QPushButton::clicked, this, &ViewContactsDialog::viewSelected);

    // Double-click row to view details
    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int, int){ viewSelected(); });

    resize(980, 520);
    refreshTable();
}

void ViewContactsDialog::applySortAndRefresh()
{
    refreshTable();
}

void ViewContactsDialog::refreshTable()
{
    if (!m_book) {
        QMessageBox::critical(this, "Error", "Internal error: PhoneBook is not available.");
        return;
    }

    // Copy unordered_map into sortable vector
    std::vector<std::pair<unsigned int, Contact>> rows;
    rows.reserve(m_book->mainStorage.size());
    for (const auto& p : m_book->mainStorage) {
        rows.push_back(p);
    }

    const int fieldIdx = m_sortField->currentIndex();
    const bool desc = (m_sortOrder->currentIndex() == 1);

    auto cmp = [&](const auto& a, const auto& b) {
        const unsigned int ida = a.first, idb = b.first;
        const Contact& ca = a.second;
        const Contact& cb = b.second;

        auto lessStr = [&](const std::string& sa, const std::string& sb) {
            // Simple lexical comparison; can be improved later (case-insensitive).
            return sa < sb;
        };

        bool res = false;
        switch (fieldIdx) {
        case 0: res = ida < idb; break;
        case 1: res = lessStr(ca.firstName, cb.firstName); break;
        case 2: res = lessStr(ca.lastName, cb.lastName); break;
        case 3: res = lessStr(ca.email, cb.email); break;
        default: res = ida < idb; break;
        }
        return desc ? !res : res;
    };

    std::sort(rows.begin(), rows.end(), cmp);

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
        set(2, qs(c.middleName));
        set(3, qs(c.lastName));
        set(4, qs(c.email));
        set(5, qs(c.numbers.number1));
        set(6, qs(c.numbers.number2));
        set(7, qs(c.numbers.number3));
        set(8, qs(c.address));
        set(9, qs(c.birthday));
    }

    m_countLabel->setText(QString("Count: %1").arg(rows.size()));
    m_table->resizeColumnsToContents();
}

void ViewContactsDialog::viewSelected()
{
    auto ranges = m_table->selectedRanges();
    if (ranges.isEmpty()) {
        QMessageBox::information(this, "View Contact", "Select a contact row first.");
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

    ContactDetailsDialog dlg(id, it->second, this);
    dlg.exec();
}
