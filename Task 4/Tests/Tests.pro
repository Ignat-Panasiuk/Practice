QT += core gui testlib
QT -= widgets

CONFIG += qt console warn_on depend_includepath testcase
TEMPLATE = app

INCLUDEPATH += ../Source

HEADERS += \
    ../Source/shape.h \
    ../Source/paintcanvas.h

SOURCES += \
    ../Source/shape.cpp \
    tst_unit.cpp \
    ../Source/paintcanvas.h