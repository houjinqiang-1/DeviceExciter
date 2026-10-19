QT += core gui widgets serialport

CONFIG += c++17 warn_on

TEMPLATE = app
TARGET = DeviceExciter

INCLUDEPATH += $$PWD/src

SOURCES += \
    src/icd/IcdRegistry.cpp \
    src/main.cpp \
    src/ui/mainwindow.cpp

HEADERS += \
    src/icd/IcdDefinitions.h \
    src/icd/IcdRegistry.h \
    src/ui/mainwindow.h \
    src/domain/ProtocolTypes.h \
    src/domain/ErrorCode.h \
    src/domain/Result.h \
    src/domain/Definitions.h \
    src/domain/RuntimeModels.h
