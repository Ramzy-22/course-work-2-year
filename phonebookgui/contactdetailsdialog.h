#ifndef CONTACTDETAILSDIALOG_H
#define CONTACTDETAILSDIALOG_H

#include <QDialog>
#include "Contactgui.h"

class ContactDetailsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ContactDetailsDialog(unsigned int id, const Contact& c, QWidget* parent = nullptr);
};

#endif // CONTACTDETAILSDIALOG_H
