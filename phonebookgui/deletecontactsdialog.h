#ifndef DELETECONTACTSDIALOG_H
#define DELETECONTACTSDIALOG_H


#include <QDialog>
#include "PhoneBookgui.h"

class QTableWidget;
class QPushButton;

class DeleteContactsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeleteContactsDialog(PhoneBook* book, QWidget* parent = nullptr);

private slots:
    void refreshTable();
    void deleteSelected();

private:
    PhoneBook* m_book;
    QTableWidget* m_table;
    QPushButton* m_btnDelete;
    QPushButton* m_btnRefresh;
};



#endif // DELETECONTACTSDIALOG_H
