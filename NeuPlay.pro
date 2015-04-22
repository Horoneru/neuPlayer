#-------------------------------------------------
#
# Project created by QtCreator 2015-01-19T15:05:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia
RC_FILE = win-icon.rc
TARGET = neuPlayer
TEMPLATE = app


SOURCES += main.cpp\
        player.cpp \
    playlist.cpp \
    settings.cpp \
    tagviewer.cpp \
    initialconfig.cpp \
    scrollinginfolabel.cpp \
    neuplaylist.cpp \
    skin.cpp \
    fadeanimmanager.cpp \
    slider.cpp \
    framelesshelper.cpp \
    fadewindow.cpp \
    updaterhandler.cpp

HEADERS  += player.h \
    playlist.h \
    settings.h \
    tagviewer.h \
    initialconfig.h \
    scrollinginfolabel.h \
    neuplaylist.h \
    skin.h \
    fadeanimmanager.h \
    slider.h \
    framelesshelper.h \
    fadewindow.h \
    updaterhandler.h

FORMS    += player.ui \
    playlist.ui \
    settings.ui \
    tagviewer.ui \
    initialconfig.ui

RESOURCES += \
    assets.qrc

TRANSLATIONS = neuplayer_en.ts neuplayer_fr.ts
