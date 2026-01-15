#include "searchcontactsdialog.h"
#include "contactdetailsdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>

#include <vector>
#include <algorithm>

static QString qs(const std::string& s) { return QString::fromStdString(s); }

static bool matchText(const QString& field,
                      const QString& needle,
                      bool exact,
                      Qt::CaseSensitivity cs)
{
    if (needle.isEmpty()) return true;              // no filter
    if (exact) return field.compare(needle, cs) == 0;
    return field.contains(needle, cs);
}

SearchContactsDialog::SearchContactsDialog(PhoneBook* book, QWidget* parent)
    : QDialog(parent), m_book(book)
{
    setWindowTitle("Search Contacts");
    setModal(true);

    auto* root = new QVBoxLayout(this);

    // Filters panel
    auto* filtersBox = new QHBoxLayout();
    root->addLayout(filtersBox);

    auto* form = new QFormLayout();
    filtersBox->addLayout(form, 1);

    m_firstName = new QLineEdit(this);
    m_lastName  = new QLineEdit(this);
    m_email     = new QLineEdit(this);
    m_phoneAny  = new QLineEdit(this);
    m_address   = new QLineEdit(this);

    m_phoneAny->setPlaceholderText("Matches work/home/office");

    form->addRow("First name:", m_firstName);
    form->addRow("Last name:",  m_lastName);
    form->addRow("Email:",      m_email);
    form->addRow("Phone (any):",m_phoneAny);
    form->addRow("Address:",    m_address);

    auto* options = new QVBoxLayout();
    filtersBox->addLayout(options);

    m_matchMode = new QComboBox(this);
    m_matchMode->addItem("Contains");
    m_matchMode->addItem("Exact");
    options->addWidget(new QLabel("Match mode:", this));
    options->addWidget(m_matchMode);

    m_caseSensitive = new QCheckBox("Case sensitive", this);
    options->addWidget(m_caseSensitive);

    options->addStretch();

    // Buttons row
    auto* btnRow = new QHBoxLayout();
    root->addLayout(btnRow);

    m_btnSearch = new QPushButton("Search", this);
    m_btnClear  = new QPushButton("Clear", this);
    m_btnView   = new QPushButton("View Selected", this);
    auto* closeBtn = new QPushButton("Close", this);

    btnRow->addWidget(m_btnSearch);
    btnRow->addWidget(m_btnClear);
    btnRow->addWidget(m_btnView);
    btnRow->addStretch();
    btnRow->addWidget(closeBtn);

    // Status label
    m_status = new QLabel(this);
    root->addWidget(m_status);

    // Results table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"ID", "First", "Last", "Email", "Phone", "Address"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSortingEnabled(true); // “free” sorting on columns
    root->addWidget(m_table);

    connect(closeBtn,  &QPushButton::clicked, this, &QDialog::accept);
    connect(m_btnSearch, &QPushButton::clicked, this, &SearchContactsDialog::runSearch);
    connect(m_btnClear,  &QPushButton::clicked, this, &SearchContactsDialog::clearFilters);
    connect(m_btnView,   &QPushButton::clicked, this, &SearchContactsDialog::viewSelected);

    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int, int){ viewSelected(); });

    resize(980, 560);

    // Optional: show all results initially
    runSearch();
}

void SearchContactsDialog::clearFilters()
{
    m_firstName->clear();
    m_lastName->clear();
    m_email->clear();
    m_phoneAny->clear();
    m_address->clear();
    m_matchMode->setCurrentIndex(0);
    m_caseSensitive->setChecked(false);

    runSearch();
}

void SearchContactsDialog::runSearch()
{
    if (!m_book) {
        QMessageBox::critical(this, "Error", "Internal error: PhoneBook is not available.");
        return;
    }

    const bool exact = (m_matchMode->currentIndex() == 1);
    const Qt::CaseSensitivity cs = m_caseSensitive->isChecked()
                                       ? Qt::CaseSensitive
                                       : Qt::CaseInsensitive;

    const QString fn = m_firstName->text().trimmed();
    const QString ln = m_lastName->text().trimmed();
    const QString em = m_email->text().trimmed();
    const QString ph = m_phoneAny->text().trimmed();
    const QString ad = m_address->text().trimmed();

    std::vector<std::pair<unsigned int, Contact>> hits;
    hits.reserve(m_book->mainStorage.size());

    for (const auto& p : m_book->mainStorage) {
        const unsigned int id = p.first;
        const Contact& c = p.second;

        const QString cFirst = qs(c.firstName).trimmed();
        const QString cLast  = qs(c.lastName).trimmed();
        const QString cEmail = qs(c.email).trimmed();
        const QString cAddr  = qs(c.address).trimmed();

        const QString w = qs(c.numbers.number1).trimmed();
        const QString h = qs(c.numbers.number2).trimmed();
        const QString o = qs(c.numbers.number3).trimmed();

        bool ok = true;

        ok = ok && matchText(cFirst, fn, exact, cs);
        ok = ok && matchText(cLast,  ln, exact, cs);
        ok = ok && matchText(cEmail, em, exact, cs);
        ok = ok && matchText(cAddr,  ad, exact, cs);

        if (!ph.isEmpty()) {
            const bool phoneOk =
                matchText(w, ph, exact, cs) ||
                matchText(h, ph, exact, cs) ||
                matchText(o, ph, exact, cs);
            ok = ok && phoneOk;
        }

        if (ok) hits.push_back({id, c});
    }

    // deterministic ordering by ID
    std::sort(hits.begin(), hits.end(), [](const auto& a, const auto& b){ return a.first < b.first; });

    m_table->setRowCount(static_cast<int>(hits.size()));

    for (int r = 0; r < static_cast<int>(hits.size()); ++r) {
        const unsigned int id = hits[r].first;
        const Contact& c = hits[r].second;

        auto set = [&](int col, const QString& text) {
            auto* it = new QTableWidgetItem(text);
            m_table->setItem(r, col, it);
        };

        set(0, QString::number(id));
        set(1, qs(c.firstName));
        set(2, qs(c.lastName));
        set(3, qs(c.email));

        // Show one “best” phone for the row (first non-empty)
        QString phoneShown = qs(c.numbers.number1);
        if (phoneShown.trimmed().isEmpty()) phoneShown = qs(c.numbers.number2);
        if (phoneShown.trimmed().isEmpty()) phoneShown = qs(c.numbers.number3);
        set(4, phoneShown);

        set(5, qs(c.address));
    }

    m_table->resizeColumnsToContents();

    const bool anyFilter =
        !fn.isEmpty() || !ln.isEmpty() || !em.isEmpty() || !ph.isEmpty() || !ad.isEmpty();

    m_status->setText(QString("Matches: %1%2")
                          .arg(hits.size())
                          .arg(anyFilter ? "" : " (no filters applied; showing all)"));
}

void SearchContactsDialog::viewSelected()
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
        runSearch();
        return;
    }

    ContactDetailsDialog dlg(id, it->second, this);
    dlg.exec();
}
