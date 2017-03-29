#-------------------------------------------------
#
# Project created by QtCreator 2015-01-19T15:05:00
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=c++11 #This reinforces c++11 on GCC/MinGW compilers

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia winextras
RC_FILE = win-icon.rc
TARGET = neuPlayer
TEMPLATE = app


SOURCES += main.cpp \
    ui\components\scrollinglabel.cpp \
    ui\components\slider.cpp \
    ui\logic\player.cpp \
    ui\logic\playlist.cpp \
    ui\logic\settings.cpp \
    ui\logic\tagviewer.cpp \
    ui\logic\initialconfig.cpp \
    util\neuplaylist.cpp \
    util\skin.cpp \
    util\updaterhandler.cpp \
    lib\fadewindow.cpp \
    lib\fademanager.cpp \
    lib\framelesshelper.cpp \
    lib\moveanimation.cpp

HEADERS  += ui\components\slider.h \
    ui\components\scrollinglabel.h \
    ui\logic\player.h \
    ui\logic\playlist.h \
    ui\logic\settings.h \
    ui\logic\tagviewer.h \
    ui\logic\initialconfig.h \
    util\neuplaylist.h \
    util\skin.h \
    util\updaterhandler.h \
    lib\framelesshelper.h \
    lib\fadewindow.h \
    lib\fademanager.h \
    lib\moveanimation.h

FORMS    += ui\player.ui \
    ui\playlist.ui \
    ui\settings.ui \
    ui\tagviewer.ui \
    ui\initialconfig.ui

RESOURCES += \
    Ressources\assets.qrc \

TRANSLATIONS = translations\neuplayer_en.ts translations\neuplayer_fr.ts
