#-------------------------------------------------
#
# Project created by QtCreator 2011-11-15T22:08:10
#
#-------------------------------------------------

QT       += core gui

TARGET = ConnectFourModerator
TEMPLATE = app
RESOURCES = resources.qrc
RC_FILE = icon.rc
ICON = icon.icns

SOURCES += main.cpp\
    Board.cpp \
    Game.cpp \
    Piece.cpp \
    ControlPanel.cpp \
    Moderator.cpp \
    Player.cpp \
    Trainer.cpp \
    Tournament.cpp

HEADERS  += \
    Board.h \
    Game.h \
    Piece.h \
    ControlPanel.h \
    Moderator.h \
    Player.h \
    Trainer.h \
    Tournament.h

FORMS += \
    trainerdialog.ui

OTHER_FILES += \
    icon.rc
