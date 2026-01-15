#ifndef EDITCONTACTSDIALOG_H
#define EDITCONTACTSDIALOG_H

#include <QDialog>
#include "PhoneBookgui.h"

class QTableWidget;
class QPushButton;

class EditContactsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditContactsDialog(PhoneBook* book, QWidget* parent = nullptr);

private slots:
    void refreshTable();
    void editSelected();

private:
    PhoneBook* m_book;
    QTableWidget* m_table;
    QPushButton* m_btnEdit;
    QPushButton* m_btnRefresh;
};

#endif // EDITCONTACTSDIALOG_H
