#include "createcontactdialog.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QRegularExpression>

static std::string toStdTrimmed(const QString& s)
{
    return s.trimmed().toStdString();
}

static std::string toStdEmailNormalized(QString s)
{
    // remove ALL whitespace (also around '@')
    s.remove(QRegularExpression("\\s+"));
    return s.toStdString();
}

CreateContactDialog::CreateContactDialog(PhoneBook* book, QWidget* parent)
    : QDialog(parent), m_book(book)
{
    setWindowTitle("Create Contact");
    setModal(true);

    auto* root = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    root->addLayout(form);

    m_firstName = new QLineEdit(this);
    m_middleName = new QLineEdit(this);
    m_lastName = new QLineEdit(this);

    m_email = new QLineEdit(this);
    m_address = new QLineEdit(this);

    m_workPhone = new QLineEdit(this);
    m_homePhone = new QLineEdit(this);
    m_officePhone = new QLineEdit(this);

    m_hasBirthday = new QCheckBox("Set birthday", this);
    m_birthday = new QDateEdit(this);
    m_birthday->setCalendarPopup(true);
    m_birthday->setEnabled(false);

    connect(m_hasBirthday, &QCheckBox::toggled, m_birthday, &QWidget::setEnabled);

    // Placeholders help reduce user error
    m_workPhone->setPlaceholderText("+7(812)123-45-67 or 88121234567");
    m_homePhone->setPlaceholderText("+7(812)123-45-67 or 88121234567");
    m_officePhone->setPlaceholderText("+7(812)123-45-67 or 88121234567");

    form->addRow("First name *", m_firstName);
    form->addRow("Middle name", m_middleName);
    form->addRow("Last name *", m_lastName);

    form->addRow("Email *", m_email);

    form->addRow("Work phone", m_workPhone);
    form->addRow("Home phone", m_homePhone);
    form->addRow("Office phone", m_officePhone);

    form->addRow("Address", m_address);
    form->addRow(m_hasBirthday, m_birthday);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    root->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted, this, &CreateContactDialog::onTryCreate);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    resize(520, 420);
}

void CreateContactDialog::onTryCreate()
{
    if (!m_book) {
        QMessageBox::critical(this, "Error", "Internal error: PhoneBook is not available.");
        return;
    }

    Contact c;
    c.firstName  = toStdTrimmed(m_firstName->text());
    c.middleName = toStdTrimmed(m_middleName->text());
    c.lastName   = toStdTrimmed(m_lastName->text());

    c.email      = toStdEmailNormalized(m_email->text());
    c.address    = toStdTrimmed(m_address->text());

    c.numbers.number1 = toStdTrimmed(m_workPhone->text());
    c.numbers.number2 = toStdTrimmed(m_homePhone->text());
    c.numbers.number3 = toStdTrimmed(m_officePhone->text());

    if (m_hasBirthday->isChecked()) {
        // Your checker expects dd-mm-yyyy
        c.birthday = m_birthday->date().toString("dd-MM-yyyy").toStdString();
    } else {
        c.birthday.clear();
    }

    std::string err;
    if (!m_book->add_contact(c, &err)) {
        QMessageBox::warning(this, "Creation Failed", QString::fromStdString(err));
        return; // keep dialog open
    }

    QMessageBox::information(this, "Success", "Creation Successful.");
    accept();
}
