QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DatabaseManager.cpp \
    MigrationDialog.cpp \
    actionwindow.cpp \
    checkersgui.cpp \
    contactdetailsdialog.cpp \
    contactgui.cpp \
    createcontactdialog.cpp \
    definitionsgui.cpp \
    deletecontactsdialog.cpp \
    editcontactdialog.cpp \
    editcontactsdialog.cpp \
    maingui.cpp \
    mainwindow.cpp \
    searchcontactsdialog.cpp \
    viewcontactsdialog.cpp

HEADERS += \
    Checkersgui.h \
    Contactgui.h \
    DatabaseManager.h \
    MigrationDialog.h \
    PhoneBookgui.h \
    actionwindow.h \
    contactdetailsdialog.h \
    createcontactdialog.h \
    deletecontactsdialog.h \
    editcontactdialog.h \
    editcontactsdialog.h \
    mainwindow.h \
    searchcontactsdialog.h \
    viewcontactsdialog.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    schema.sql
