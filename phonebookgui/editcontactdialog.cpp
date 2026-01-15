#include "editcontactdialog.h"
#include "Checkersgui.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDate>
#include <QPushButton>
#include <QLabel>

static std::string toStdTrimmed(const QString& s)
{
    return s.trimmed().toStdString();
}

static std::string toStdEmailNormalized(QString s)
{
    s.remove(QRegularExpression("\\s+"));
    return s.toStdString();
}

EditContactDialog::EditContactDialog(PhoneBook* book, unsigned int id, QWidget* parent)
    : QDialog(parent), m_book(book), m_id(id)
{
    setWindowTitle(QString("Edit Contact (ID: %1)").arg(id));
    setModal(true);

    if (!m_book) {
        QMessageBox::critical(this, "Error", "Internal error: PhoneBook is not available.");
        reject();
        return;
    }

    Contact c;
    if (!m_book->get_contact(id, &c)) {
        QMessageBox::warning(this, "Not Found", "Contact not found.");
        reject();
        return;
    }

    auto* root = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    root->addLayout(form);

    m_firstName = new QLineEdit(this);
    m_middleName = new QLineEdit(this);
    m_lastName = new QLineEdit(this);

    m_email = new QLineEdit(this);
    m_autoEmailLabel = new QLabel(this);
    m_autoEmailLabel->setStyleSheet("color: #0066cc; font-style: italic;");
    m_btnAutoEmail = new QPushButton("Use Auto-Generated Email", this);

    m_address = new QLineEdit(this);

    m_workPhone = new QLineEdit(this);
    m_homePhone = new QLineEdit(this);
    m_officePhone = new QLineEdit(this);

    m_hasBirthday = new QCheckBox("Set birthday", this);
    m_birthday = new QDateEdit(this);
    m_birthday->setCalendarPopup(true);
    m_birthday->setEnabled(false);

    connect(m_hasBirthday, &QCheckBox::toggled, m_birthday, &QWidget::setEnabled);

    // Prefill
    m_firstName->setText(QString::fromStdString(c.firstName));
    m_middleName->setText(QString::fromStdString(c.middleName));
    m_lastName->setText(QString::fromStdString(c.lastName));

    m_email->setText(QString::fromStdString(c.email));
    m_address->setText(QString::fromStdString(c.address));

    m_workPhone->setText(QString::fromStdString(c.numbers.number1));
    m_homePhone->setText(QString::fromStdString(c.numbers.number2));
    m_officePhone->setText(QString::fromStdString(c.numbers.number3));

    if (!c.birthday.empty()) {
        const QDate d = QDate::fromString(QString::fromStdString(c.birthday), "dd-MM-yyyy");
        if (d.isValid()) {
            m_hasBirthday->setChecked(true);
            m_birthday->setEnabled(true);
            m_birthday->setDate(d);
        }
    }

    form->addRow("First name *", m_firstName);
    form->addRow("Middle name", m_middleName);
    form->addRow("Last name *", m_lastName);

    // Email section with auto-generation
    auto* emailLayout = new QVBoxLayout();
    emailLayout->addWidget(m_email);
    emailLayout->addWidget(m_autoEmailLabel);
    emailLayout->addWidget(m_btnAutoEmail);
    form->addRow("Email *", emailLayout);

    form->addRow("Work phone", m_workPhone);
    form->addRow("Home phone", m_homePhone);
    form->addRow("Office phone", m_officePhone);

    form->addRow("Address", m_address);
    form->addRow(m_hasBirthday, m_birthday);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    root->addWidget(m_buttons);

    // Connect signals for auto-email generation
    connect(m_firstName, &QLineEdit::textChanged, this, &EditContactDialog::updateAutoEmail);
    connect(m_lastName, &QLineEdit::textChanged, this, &EditContactDialog::updateAutoEmail);
    connect(m_btnAutoEmail, &QPushButton::clicked, this, &EditContactDialog::useAutoEmail);

    connect(m_buttons, &QDialogButtonBox::accepted, this, &EditContactDialog::onTryUpdate);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    resize(580, 500);

    // Initialize auto-email display
    updateAutoEmail();
}

void EditContactDialog::updateAutoEmail()
{
    std::string firstName = toStdTrimmed(m_firstName->text());
    std::string lastName = toStdTrimmed(m_lastName->text());

    if (isValidName(firstName) && isValidName(lastName)) {
        std::string autoEmail = generateEmail(firstName, lastName);
        m_autoEmailLabel->setText(QString("Auto-generated option: %1")
                                      .arg(QString::fromStdString(autoEmail)));
        m_btnAutoEmail->setEnabled(true);
        m_currentAutoEmail = autoEmail;
    } else {
        m_autoEmailLabel->setText("");
        m_btnAutoEmail->setEnabled(false);
        m_currentAutoEmail.clear();
    }
}

void EditContactDialog::useAutoEmail()
{
    if (!m_currentAutoEmail.empty()) {
        m_email->setText(QString::fromStdString(m_currentAutoEmail));
    }
}

void EditContactDialog::onTryUpdate()
{
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
        c.birthday = m_birthday->date().toString("dd-MM-yyyy").toStdString();
    } else {
        c.birthday.clear();
    }

    std::string err;
    if (!m_book->update_contact(m_id, c, &err)) {
        QMessageBox::warning(this, "Update Failed", QString::fromStdString(err));
        return;
    }

    QMessageBox::information(this, "Success", "Contact updated successfully!");
    accept();
}
