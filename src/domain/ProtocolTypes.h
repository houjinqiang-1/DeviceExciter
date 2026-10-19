#ifndef PROTOCOLTYPES_H
#define PROTOCOLTYPES_H

enum class FrameDirection{
    Tx,
    Rx
};

enum class FieldType{
    UInt8,
    Int8,
    UInt16,
    Int16,
    UInt24,
    Int24,
    UInt32,
    Int32,
    Ascii,
    Enumeration,
    BitField,
    Reserved
};

enum class ByteOrder
{
    BigEndian,
    LittleEndian
};

enum class SendMode
{
    Trigger,
    Periodic
};

enum class SendState
{
    Idle,
    Starting,
    Sending,
    Stopping,
    Stopped,
    Completed,
    Failed
};

enum class PortOpenState
{
    Closed,
    Open,
    Error
};

enum class CommunicationState
{
    NotStarted,
    Waiting,
    Normal,
    Timeout,
    Fault
};

enum class ParseStatus
{
    Normal,
    ChecksumError,
    LengthError,
    UnknownEnum,
    Expired,
    UnknownField
};

enum class ProbeStatus
{
    NotMatched,
    NeedMoreData,
    Matched,
    Invalid
};


#endif