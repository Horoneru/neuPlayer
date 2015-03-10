#-------------------------------------------------
#
# Project created by QtCreator 2015-01-19T15:05:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia
RC_FILE = win-icon.rc
TARGET = NeuPlayer
TEMPLATE = app


SOURCES += main.cpp\
        player.cpp \
    playlist.cpp \
    settings.cpp \
    tagviewer.cpp \
    initialconfig.cpp \
    scrollinginfolabel.cpp

HEADERS  += player.h \
    playlist.h \
    settings.h \
    tagviewer.h \
    initialconfig.h \
    scrollinginfolabel.h

FORMS    += player.ui \
    playlist.ui \
    settings.ui \
    tagviewer.ui \
    initialconfig.ui

RESOURCES += \
    assets.qrc

TRANSLATIONS = neuplayer_en.ts neuplayer_fr.ts
OTHER_FILES +=
