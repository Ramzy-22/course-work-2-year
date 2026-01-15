#ifndef VIEWCONTACTSDIALOG_H
#define VIEWCONTACTSDIALOG_H

#include <QDialog>
#include "PhoneBookgui.h"

class QTableWidget;
class QComboBox;
class QPushButton;
class QLabel;

class ViewContactsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ViewContactsDialog(PhoneBook* book, QWidget* parent = nullptr);

private slots:
    void refreshTable();
    void applySortAndRefresh();
    void viewSelected();

private:
    enum class SortField { Id, FirstName, LastName, Email };

    PhoneBook* m_book;

    QTableWidget* m_table;
    QComboBox* m_sortField;
    QComboBox* m_sortOrder;
    QLabel* m_countLabel;

    QPushButton* m_btnRefresh;
    QPushButton* m_btnApplySort;
    QPushButton* m_btnViewSelected;
};

#endif // VIEWCONTACTSDIALOG_H
