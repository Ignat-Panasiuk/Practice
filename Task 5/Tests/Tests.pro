QT += core testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
TEMPLATE = app

INCLUDEPATH += ../Source_code

SOURCES += tst_unit.cpp

SOURCES += ../Source_code/game_objects.cpp \
           ../Source_code/game_engine.cpp

HEADERS += ../Source_code/game_objects.h \
           ../Source_code/game_engine.h