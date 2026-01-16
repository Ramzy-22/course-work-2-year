#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "PhoneBookgui.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class QDateEdit;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_btnCreate_clicked();
    void on_btnView_clicked();
    void on_btnSearch_clicked();
    void on_btnEdit_clicked();
    void on_btnDelete_clicked();
    void on_btnSort_clicked();
    void on_btnMigration_clicked();

private:
    Ui::MainWindow *ui;
    PhoneBook m_book;
};
#endif // MAINWINDOW_H
