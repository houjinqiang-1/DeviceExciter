#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "domain/ProtocolTypes.h"

#include <QByteArray>
#include <QList>
#include <QString>

struct EnumOption
{
    int value{};
    QString label;
};

struct BitDefinition
{
    QString name;
    int bitIndex{};
    QString meaning0;
    QString meaning1;
};

struct FieldDefinition
{
    QString id;
    QString name;
    FieldType type{FieldType::UInt8};
    int byteOffset{};
    int byteLength{};
    ByteOrder byteOrder{ByteOrder::BigEndian};
    double resolution{1.0};
    double offset{0.0};
    QString unit;
    bool editable{true};
    QString groupName;
    QList<EnumOption> enumOptions;
    QList<BitDefinition> bitDefinitions;
    QByteArray fixedRawValue;
};

struct FrameDefinition
{
    QString id;
    QString name;
    FrameDirection direction{FrameDirection::Tx};
    int frameLength{};
    QByteArray header;
    QByteArray footer;
    QList<FieldDefinition> fields;
    SendMode defaultMode{SendMode::Trigger};
    int defaultPeriodMs{};
    bool periodLocked{false};
};

struct DeviceDefinition
{
    QString id;
    QString name;
    QString category;
    QString icdName;
    QList<FrameDefinition> txFrames;
    QList<FrameDefinition> rxFrames;
};

#endif // DEFINITIONS_H
