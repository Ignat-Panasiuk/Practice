QT += core gui widgets multimedia

CONFIG += qt warn_on
CONFIG += c++17

TEMPLATE = app

INCLUDEPATH += ../Source_code

SOURCES += main.cpp \
           gamewindow.cpp

HEADERS += gamewindow.h

SOURCES += ../Source_code/game_engine.cpp \
           ../Source_code/game_objects.cpp

HEADERS += ../Source_code/game_engine.h \
           ../Source_code/game_objects.h

RESOURCES += resources.qrc

DISTFILES += \
    dsgldth.wav \
    dstelept.wav \
    music_game.wav \
    music_gameover.wav \
    music_menu.wav \
    music_pause.wav

