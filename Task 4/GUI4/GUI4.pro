QT       += core gui widgets

CONFIG += c++17

INCLUDEPATH += ../Source

SOURCES += main.cpp \
           mainwindow.cpp \
           ../Source/paintcanvas.cpp \
           ../Source/shape.cpp

HEADERS += mainwindow.h \
           ../Source/paintcanvas.h \
           ../Source/shape.h

FORMS += mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target