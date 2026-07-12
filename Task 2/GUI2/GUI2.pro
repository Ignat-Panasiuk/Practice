QT += widgets

CONFIG += c++17

INCLUDEPATH += ../Source

SOURCES += main.cpp \
           mainwindow.cpp \
           ../Source/clientmodel.cpp

HEADERS += mainwindow.h \
           ../Source/client.h \
           ../Source/clientmodel.h

FORMS += mainwindow.ui

# Стандартные правила деплоя
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target