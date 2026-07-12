QT += widgets

CONFIG += c++17

INCLUDEPATH += ../Source

SOURCES += main.cpp \
           mainwindow.cpp \
           ../Source/client.cpp

HEADERS += mainwindow.h \
           ../Source/client.h

FORMS += mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target