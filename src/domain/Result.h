#ifndef RESULT_H
#define RESULT_H

#include "domain/ErrorCode.h"

#include <QString>
#include <utility>

struct ErrorInfo
{
    ErrorCode code{ErrorCode::Ok};
    QString fieldId;
    QString message;

    bool hasError() const
    {
        return code != ErrorCode::Ok;
    }
};

template<typename T>
class Result
{
public:
    static Result success(T value)
    {
        Result result;
        result.ok_ = true;
        result.value_ = std::move(value);
        return result;
    }

    static Result failure(ErrorCode code,
                          const QString& message,
                          const QString& fieldId = QString())
    {
        Result result;
        result.ok_ = false;
        result.error_.code = code;
        result.error_.message = message;
        result.error_.fieldId = fieldId;
        return result;
    }

    static Result failure(const ErrorInfo& error)
    {
        Result result;
        result.ok_ = false;
        result.error_ = error;
        return result;
    }

    bool isSuccess() const
    {
        return ok_;
    }

    bool isFailure() const
    {
        return !ok_;
    }

    const T& value() const
    {
        return value_;
    }

    T& value()
    {
        return value_;
    }

    const ErrorInfo& error() const
    {
        return error_;
    }

private:
    bool ok_{false};
    T value_{};
    ErrorInfo error_{};
};

#endif // RESULT_H