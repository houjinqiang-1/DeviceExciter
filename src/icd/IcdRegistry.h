#ifndef ICDREGISTRY_H
#define ICDREGISTRY_H

#include "domain/Definitions.h"
#include "domain/Result.h"

#include <QHash>
#include <QList>
#include <QString>

class IcdRegistry final
{
public:
    IcdRegistry();
    const DeviceDefinition* findDevice(const QString& deviceId) const;
    QList<const DeviceDefinition*> device() const;
private:
    Result<bool>  registerDevice(DeviceDefinition definition);
    Result<bool>  validateDevice(const DeviceDefinition& definition) const;
    QHash<QString,DeviceDefinition> decives_;
};


#endif // ICDREGISTRY_H
