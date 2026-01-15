#ifndef ACTIONWINDOW_H
#define ACTIONWINDOW_H
#include <QDialog>

class ActionWindow : public QDialog
{
    Q_OBJECT
public:
    explicit ActionWindow(const QString& title,
                          const QString& message,
                          QWidget* parent = nullptr);
};

#endif // ACTIONWINDOW_H
