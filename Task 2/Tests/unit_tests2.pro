QT += core gui widgets testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../Source

SOURCES += tst_unit.cpp \
    ../Source/clientmodel.cpp

HEADERS += \
    ../Source/client.h \
    ../Source/clientmodel.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target