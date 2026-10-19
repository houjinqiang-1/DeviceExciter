#ifndef RUNTIMEMODELS_H
#define RUNTIMEMODELS_H

#include "domain/ProtocolTypes.h"
#include "domain/Result.h"

#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QSerialPort>
#include <QString>
#include <QVariant>

struct FieldValue
{
    QString fieldId;
    QVariant inputValue;
    QByteArray encodedValue;
    bool valid{true};
    QString error;
};

struct ParsedField
{
    QString fieldId;
    QString name;
    QString category;
    QVariant parsedValue;
    QByteArray rawBytes;
    QString unitOrEnumText;
    ParseStatus status{ParseStatus::Normal};
};

struct ParseResult
{
    bool success{false};
    QString frameId;
    QString frameName;
    QList<ParsedField> fields;
    ErrorInfo error;
};

struct SerialPortConfig
{
    QString portName;
    qint32 baudRate{115200};
    QSerialPort::DataBits dataBits{QSerialPort::Data8};
    QSerialPort::StopBits stopBits{QSerialPort::OneStop};
    QSerialPort::Parity parity{QSerialPort::NoParity};
    QSerialPort::FlowControl flowControl{QSerialPort::NoFlowControl};
    int readTimeoutMs{1000};
    int writeTimeoutMs{1000};
};

struct PortRuntimeState
{
    PortOpenState openState{PortOpenState::Closed};
    CommunicationState communicationState{CommunicationState::NotStarted};
    QDateTime lastTxTime;
    QDateTime lastValidRxTime;
    QString lastError;
};

struct SendTask
{
    QString deviceId;
    QString frameId;
    SendMode mode{SendMode::Trigger};
    int periodMs{};
    SendState state{SendState::Idle};
    bool running{false};
    QDateTime lastSendTime;
    QString lastError;
};

struct MessageRecord
{
    QDateTime timestamp;
    FrameDirection direction{FrameDirection::Tx};
    QString deviceId;
    QString deviceName;
    QString portName;
    QString frameId;
    QString frameName;
    int length{};
    QByteArray rawBytes;
    QString processResult;
    QString errorReason;
    QString correlationId;
};

#endif // RUNTIMEMODELS_H
