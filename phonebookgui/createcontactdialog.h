#ifndef CREATECONTACTDIALOG_H
#define CREATECONTACTDIALOG_H

#include <QDialog>

class QLineEdit;
class QDateEdit;
class QCheckBox;
class QDialogButtonBox;

#include "PhoneBookgui.h"

class CreateContactDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CreateContactDialog(PhoneBook* book, QWidget* parent = nullptr);

private slots:
    void onTryCreate();

private:
    PhoneBook* m_book;

    QLineEdit* m_firstName;
    QLineEdit* m_middleName;
    QLineEdit* m_lastName;

    QLineEdit* m_email;
    QLineEdit* m_address;

    QLineEdit* m_workPhone;
    QLineEdit* m_homePhone;
    QLineEdit* m_officePhone;

    QCheckBox* m_hasBirthday;
    QDateEdit* m_birthday;

    QDialogButtonBox* m_buttons;
};

#endif // CREATECONTACTDIALOG_H
