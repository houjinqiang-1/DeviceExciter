#ifndef ERRORCODE_H
#define ERRORCODE_H

enum class ErrorCode
{
    Ok = 0,
    InvalidArgument,
    OutOfRange,
    InvalidEnum,
    EncodeFailed,
    DecodeFailed,
    ChecksumError,
    LengthError,
    PortNotOpen,
    PortError,
    BindingConflict,
    ConfigInvalid,
    NotFound,
    InternalError
};

#endif // ERRORCODE_H