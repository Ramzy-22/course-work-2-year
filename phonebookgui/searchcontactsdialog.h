#ifndef SEARCHCONTACTSDIALOG_H
#define SEARCHCONTACTSDIALOG_H

#include <QDialog>
#include "PhoneBookgui.h"

class QLineEdit;
class QComboBox;
class QCheckBox;
class QTableWidget;
class QLabel;
class QPushButton;

class SearchContactsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SearchContactsDialog(PhoneBook* book, QWidget* parent = nullptr);

private slots:
    void runSearch();
    void clearFilters();
    void viewSelected();

private:
    PhoneBook* m_book;

    // Filters
    QLineEdit* m_firstName;
    QLineEdit* m_lastName;
    QLineEdit* m_email;
    QLineEdit* m_phoneAny;   // checks work/home/office
    QLineEdit* m_address;

    QComboBox* m_matchMode;  // Contains / Exact
    QCheckBox* m_caseSensitive;

    // Results
    QTableWidget* m_table;
    QLabel* m_status;

    QPushButton* m_btnSearch;
    QPushButton* m_btnClear;
    QPushButton* m_btnView;
};

#endif // SEARCHCONTACTSDIALOG_H
