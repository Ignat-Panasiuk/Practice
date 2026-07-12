QT += core testlib
QT -= gui
CONFIG += qt console warn_on depend_includepath testcase
TEMPLATE = app

INCLUDEPATH += ../Source
SOURCES += tst_unit.cpp ../Source/client.cpp
HEADERS += ../Source/client.h