QT += widgets serialport

msvc: QMAKE_CXXFLAGS += /utf-8

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    # formset.cpp \
    icd.cpp \
    main.cpp \
    mainwindow.cpp \
    # parse.cpp \
    ringbuffer.cpp \
    # serialportset.cpp \
    work\devicework.cpp\
    # protocol.cpp

HEADERS += \
    # formset.h \
    icd.h \
    mainwindow.h \
    # parse.h \
    ringbuffer.h \
    # serialportset.h \
    work\devicework.h\
    # protocol.h

FORMS += \
    # form.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
