#include "icd/IcdRegistry.h"
#include "icd/IcdDefinitions.h"
namespace{
    bool rangeOverlap(int a0,int a1,int b0, int b1)
    {
        return a0<b1 & b0<a1;    //重叠返回true
    }
}//namespace


IcdRegistry::IcdRegistry()
{
    registerDevice(FuleIcdDefinition::create());
    registerDevice(TransponderIcdDefinition::create());
}

const DeviceDefinition *IcdRegistry::findDevice(const QString &deviceId) const
{
    const auto it = decives_.constFind(deviceId);
    if (it == decives_.cend()){
        return nullptr;
    }
    return &it.value();
}

QList<const DeviceDefinition *> IcdRegistry::device() const
{
    QList<const DeviceDefinition *> list;
    list.reserve(decives_.size());
    for(auto it=decives_.begin();it!=decives_.cend();++it)
    {
        list.append(&it.value());
    }
    return list;
}

Result<bool> IcdRegistry::registerDevice(DeviceDefinition definition)
{
    const Result<bool> validated = validateDevice(definition);
    if(validated.isFailure()){
        return validated;
    }

}

Result<bool> IcdRegistry::validateDevice(const DeviceDefinition &definition) const
{

}



