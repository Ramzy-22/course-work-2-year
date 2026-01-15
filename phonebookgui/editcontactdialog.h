#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include <QDialog>
#include "PhoneBookgui.h"


class QLineEdit;
class QDateEdit;
class QCheckBox;
class QDialogButtonBox;
class QPushButton;
class QLabel;

class EditContactDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditContactDialog(PhoneBook* book, unsigned int id, QWidget* parent = nullptr);

private slots:
    void onTryUpdate();
    void updateAutoEmail();    // ADDED
    void useAutoEmail();       // ADDED

private:
    PhoneBook* m_book;
    unsigned int m_id;

    QLineEdit* m_firstName;
    QLineEdit* m_middleName;
    QLineEdit* m_lastName;

    QLineEdit* m_email;
    QLabel* m_autoEmailLabel;
    QPushButton* m_btnAutoEmail;
    std::string m_currentAutoEmail;

    QLineEdit* m_address;

    QLineEdit* m_workPhone;
    QLineEdit* m_homePhone;
    QLineEdit* m_officePhone;

    QCheckBox* m_hasBirthday;
    QDateEdit* m_birthday;

    QDialogButtonBox* m_buttons;
};

#endif // EDITCONTACTDIALOG_H
