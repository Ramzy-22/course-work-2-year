#ifndef MIGRATIONDIALOG_H
#define MIGRATIONDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include "PhoneBookgui.h"

class MigrationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MigrationDialog(QWidget* parent = nullptr);

private slots:
    void migrateJsonToDatabase();
    void migrateDatabaseToJson();

private:
    QPushButton* m_btnJsonToDb;
    QPushButton* m_btnDbToJson;
    QProgressBar* m_progress;
    QLabel* m_status;
};

#endif
