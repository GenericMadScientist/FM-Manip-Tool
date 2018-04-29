QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FM_Manip_Tool
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += src/main.cpp \
    src/easymodefinder.cpp \
    src/decks.cpp \
    src/pools.cpp \
    src/seedfuns.cpp \
    src/shiftfuns.cpp \
    src/cards.cpp \
    src/manipfinder.cpp \
    src/mainwindow.cpp \
    src/starterdecks.cpp \
    src/startsearcher.cpp \
    src/fmdata.cpp \
    lib/sqlite3.c \
    src/pathfinder.cpp \
    src/manipanims.cpp \
    src/customlistitems.cpp \
    src/seedindexvalidator.cpp \
    src/optschanger.cpp

INCLUDEPATH += $$PWD/include $$PWD/lib
HEADERS += include/easymodefinder.hpp \
    include/constants.hpp \
    include/decks.hpp \
    include/pools.hpp \
    include/seedfuns.hpp \
    include/shiftfuns.hpp \
    include/duelists.hpp \
    include/cards.hpp \
    include/manipfinder.hpp \
    include/mainwindow.hpp \
    include/starterdecks.hpp \
    include/startsearcher.hpp \
    include/manipanims.hpp \
    include/fmdata.hpp \
    lib/sqlite3.h \
    include/pathfinder.hpp \
    include/customlistitems.hpp \
    include/seedindexvalidator.hpp \
    include/optschanger.hpp

FORMS += forms/easymodefinder.ui \
    forms/startsearchform.ui \
    forms/manipfinderform.ui

RC_ICONS = Necklace.ico

CONFIG += c++14

QMAKE_CXXFLAGS += -Wall -Wextra
QMAKE_CXXFLAGS += -O3

QMAKE_CXXFLAGS_DEBUG += -pg
QMAKE_LFLAGS_DEBUG += -pg
