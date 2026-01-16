#include "migrationdialog.h"
#include "PhoneBookgui.h"
#include "DatabaseManager.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>

MigrationDialog::MigrationDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Data Migration Tool");
    setModal(true);

    auto* layout = new QVBoxLayout(this);

    // Title
    auto* titleLabel = new QLabel("Data Migration", this);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // Description
    auto* descLabel = new QLabel(
        "Migrate data between JSON file storage and PostgreSQL database.\n"
        "This tool helps you switch between storage backends without losing data.",
        this
        );
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(descLabel);

    layout->addSpacing(20);

    // Buttons
    auto* buttonsLayout = new QHBoxLayout();

    m_btnJsonToDb = new QPushButton("JSON → Database", this);
    m_btnJsonToDb->setToolTip("Migrate all contacts from JSON file to PostgreSQL database");
    m_btnJsonToDb->setMinimumSize(150, 50);

    m_btnDbToJson = new QPushButton("Database → JSON", this);
    m_btnDbToJson->setToolTip("Export all contacts from database to JSON file");
    m_btnDbToJson->setMinimumSize(150, 50);

    buttonsLayout->addWidget(m_btnJsonToDb);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_btnDbToJson);

    layout->addLayout(buttonsLayout);

    layout->addSpacing(20);

    // Progress bar
    m_progress = new QProgressBar(this);
    m_progress->setRange(0, 100);
    m_progress->setValue(0);
    m_progress->setVisible(false);
    layout->addWidget(m_progress);

    // Status label
    m_status = new QLabel("", this);
    m_status->setAlignment(Qt::AlignCenter);
    m_status->setWordWrap(true);
    layout->addWidget(m_status);

    layout->addSpacing(20);

    // Close button
    auto* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn);

    // Connect signals
    connect(m_btnJsonToDb, &QPushButton::clicked, this, &MigrationDialog::migrateJsonToDatabase);
    connect(m_btnDbToJson, &QPushButton::clicked, this, &MigrationDialog::migrateDatabaseToJson);

    resize(500, 350);
}

void MigrationDialog::migrateJsonToDatabase()
{
    if (!DatabaseManager::instance().isConnected()) {
        QMessageBox::warning(this, "No Database Connection",
                             "Please connect to PostgreSQL database first.");
        return;
    }

    // Ask for JSON file
    QString jsonPath = QFileDialog::getOpenFileName(
        this,
        "Select JSON File",
        QDir::homePath(),
        "JSON Files (*.json *.db);;All Files (*.*)"
        );

    if (jsonPath.isEmpty()) {
        return;
    }

    QFile jsonFile(jsonPath);
    if (!jsonFile.exists()) {
        QMessageBox::warning(this, "File Not Found", "Selected file does not exist.");
        return;
    }

    // Load contacts from JSON
    PhoneBook tempBook;
    if (!tempBook.load_from_file(jsonPath.toStdString())) {
        QMessageBox::warning(this, "Load Failed", "Could not load contacts from JSON file.");
        return;
    }

    // Count contacts
    int totalContacts = static_cast<int>(tempBook.mainStorage.size());
    if (totalContacts == 0) {
        QMessageBox::information(this, "No Data", "JSON file contains no contacts.");
        return;
    }

    // Ask for confirmation
    QString confirmMsg = QString("Found %1 contacts in the JSON file.\n\n"
                                 "This will import all contacts into the PostgreSQL database.\n"
                                 "Existing database contacts will remain unchanged.\n\n"
                                 "Continue?").arg(totalContacts);

    if (QMessageBox::question(this, "Confirm Import", confirmMsg,
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    // Start migration
    m_progress->setVisible(true);
    m_progress->setValue(0);
    m_status->setText("Starting migration...");
    qApp->processEvents(); // Update UI

    int migrated = 0;
    int failed = 0;

    for (const auto& pair : tempBook.mainStorage) {
        const Contact& contact = pair.second;

        // Try to add to database
        std::string error;
        unsigned int newId = 0;

        if (DatabaseManager::instance().createContact(contact, &newId)) {
            migrated++;
        } else {
            failed++;
            qDebug() << "Failed to migrate contact:" << QString::fromStdString(error);
        }

        // Update progress
        int progress = static_cast<int>((migrated + failed) * 100 / totalContacts);
        m_progress->setValue(progress);
        m_status->setText(QString("Migrating... %1/%2 contacts").arg(migrated + failed).arg(totalContacts));
        qApp->processEvents();
    }

    m_progress->setValue(100);

    // Show results
    QString resultMsg = QString("Migration completed!\n\n"
                                "Successfully migrated: %1 contacts\n"
                                "Failed: %2 contacts").arg(migrated).arg(failed);

    if (failed > 0) {
        resultMsg += "\n\nSome contacts may have duplicate emails or validation errors.";
    }

    m_status->setText(resultMsg);

    if (migrated > 0) {
        QMessageBox::information(this, "Migration Complete", resultMsg);
    } else {
        QMessageBox::warning(this, "Migration Failed", "No contacts were migrated.");
    }
}

void MigrationDialog::migrateDatabaseToJson()
{
    if (!DatabaseManager::instance().isConnected()) {
        QMessageBox::warning(this, "No Database Connection",
                             "Please connect to PostgreSQL database first.");
        return;
    }

    // Get all contacts from database
    auto contacts = DatabaseManager::instance().getAllContacts();
    int totalContacts = contacts.size();

    if (totalContacts == 0) {
        QMessageBox::information(this, "No Data", "Database contains no contacts.");
        return;
    }

    // Ask for save location
    QString savePath = QFileDialog::getSaveFileName(
        this,
        "Save JSON File",
        QDir::homePath() + "/phonebook_export.json",
        "JSON Files (*.json);;All Files (*.*)"
        );

    if (savePath.isEmpty()) {
        return;
    }

    // Add .json extension if not present
    if (!savePath.endsWith(".json", Qt::CaseInsensitive)) {
        savePath += ".json";
    }

    // Ask for confirmation
    QString confirmMsg = QString("Found %1 contacts in the database.\n\n"
                                 "This will export all contacts to a JSON file.\n\n"
                                 "Continue?").arg(totalContacts);

    if (QMessageBox::question(this, "Confirm Export", confirmMsg,
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    // Create temporary PhoneBook with database contacts
    PhoneBook tempBook;
    tempBook.set_index(0);

    m_progress->setVisible(true);
    m_progress->setValue(0);
    m_status->setText("Preparing export...");
    qApp->processEvents();

    int exported = 0;

    for (const auto& pair : contacts) {
        unsigned int id = pair.first;
        const Contact& contact = pair.second;

        // Add to temporary PhoneBook (bypass validation since data already valid)
        tempBook.mainStorage[id] = contact;

        exported++;

        // Update progress
        int progress = static_cast<int>(exported * 100 / totalContacts);
        m_progress->setValue(progress);
        m_status->setText(QString("Exporting... %1/%2 contacts").arg(exported).arg(totalContacts));
        qApp->processEvents();
    }

    // Save to JSON
    if (tempBook.save_to_file(savePath.toStdString())) {
        m_progress->setValue(100);
        m_status->setText(QString("Export completed!\nSaved to: %1").arg(savePath));

        QMessageBox::information(this, "Export Complete",
                                 QString("Successfully exported %1 contacts to:\n%2")
                                     .arg(exported)
                                     .arg(savePath));
    } else {
        m_status->setText("Export failed!");
        QMessageBox::warning(this, "Export Failed",
                             "Could not save JSON file. Check write permissions.");
    }
}
