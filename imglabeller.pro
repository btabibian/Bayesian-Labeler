#-------------------------------------------------
#
# Project created by QtCreator 2011-09-23T21:30:41
#
#-------------------------------------------------

QT       += core gui

TARGET = imglabeller
TEMPLATE = app
RESOURCES = rsc.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    img_label.cpp \
    state_variable.cpp \
    infer_engine.cpp \
    act_predict.cpp

HEADERS  += mainwindow.h \
    img_label.h \
    state_variable.h \
    infer_engine.h \
    act_predict.h

FORMS    += mainwindow.ui
