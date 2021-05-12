#-------------------------------------------------
#
# Project created by QtCreator 2019-06-03T13:54:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt_v8u_grav
TEMPLATE = app
RC_ICONS = tng.ico

RESOURCES += \
    app_resources.qrc

RC_FILE  = resources.rc


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    qlas.cpp \
    qt_vak_8u_file.cpp \
    qgravity.cpp

HEADERS += \
        mainwindow.h \
    qlas.h \
    qt_vak_8u_file.h \
    vak_8.h \
    tool_type.h \
    qgravity.h \
    resources.rc

FORMS += \
        mainwindow.ui

# The application version
VERSION = 0.0.1.0

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
