#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "actionwindow.h"
#include "createcontactdialog.h"
#include "viewcontactsdialog.h"
#include "deletecontactsdialog.h"
#include "searchcontactsdialog.h"
#include "editcontactsdialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnCreate_clicked()
{
    CreateContactDialog dlg(&m_book, this);
    dlg.exec();
}

void MainWindow::on_btnView_clicked()
{
    ViewContactsDialog dlg(&m_book, this);
    dlg.exec();
}

void MainWindow::on_btnSearch_clicked()
{
    SearchContactsDialog dlg(&m_book, this);
    dlg.exec();
}

void MainWindow::on_btnEdit_clicked()
{
    EditContactsDialog dlg(&m_book, this);
    dlg.exec();
}


void MainWindow::on_btnDelete_clicked()
{
    DeleteContactsDialog dlg(&m_book, this);
    dlg.exec();
}


void MainWindow::on_btnSort_clicked()
{
    // Sorting is handled inside the View Contacts dialog.
    ViewContactsDialog dlg(&m_book, this);
    dlg.exec();
}
