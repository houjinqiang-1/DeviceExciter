#include "icd.h"
#include <cmath>
// #include <limits>
#include <QVector>
#include <QDebug>
#include <utility>
//引入C++模板
//PacketField (&fields)[N] 数组引用可以直接推导得到数组的长度
template<class Packet, std::size_t N>
QString fillPacket(const QVector<double> &values,
                   const PacketField (&fields)[N],
                   Packet &packet,
                   QVector<qint32> &rawValues)
{
    // 对结构体的形式是否符合规范进行检查，同常是对有偏移量操作的结构体进行检查
    static_assert(std::is_standard_layout_v<Packet>);
    // 对结构体是否可以简单复制进行判断，这针对需要操作内存的结构体
    static_assert(std::is_trivially_copyable_v<Packet>);
    // qDebug()<<"static_cast<int>(N)"<<N;
    const int count = static_cast<int>(N);

    if (values.size() != count) {
        return QStringLiteral("输入值数量与字段表不一致");
    }
    //创建临时数据包
    Packet next{};
    //将整个数据包看成一段连续的字节内存，只需要知道多少字节不需要知道哪个字节对应哪些值
    auto *bytes = reinterpret_cast<unsigned char *>(&next);
    //它用于保存每个工程量转换后的原始整数值
    QVector<qint32> converted(count);

    for (int row = 0; row < count; ++row) {
        const PacketField &field = fields[row];
        const double raw = std::round(
            (values.at(row) - field.engineeringOffset) / field.resolution);

        const bool oneByte = field.byteCount == 1;
        const int unsignedMax = oneByte ? 255 : 65535;
        const int maxRaw = field.isSigned ? unsignedMax / 2 : unsignedMax;
        const int minRaw = field.isSigned ? -maxRaw - 1 : 0;
        // 范围检查针对取整后的结果
        if (!std::isfinite(raw) || raw < minRaw || raw > maxRaw) {
            return QStringLiteral("「%1」超出可编码范围")
                .arg(QString::fromUtf8(field.name));
        }

        const auto value = static_cast<qint32>(raw);  //值
        auto *dest = bytes + field.memberOffset;  //值在结构体中的位置

        if (oneByte) {
            *dest = static_cast<quint8>(value); //1个字节直接复制给dest
        } else {
            const auto word = static_cast<quint16>(value); //两个字节需要先转换
            std::memcpy(dest, &word, sizeof(word));
        }

        converted[row] = value;  //保存原始数据
    }

    packet = next;
    rawValues = converted;
    return {};
}


constexpr std::pair<qint32, qint32> packetRawRange(const PacketField &field)
{
    const qint32 unsignedMax = field.byteCount == 1 ? 255 : 65535;
    const qint32 maxRaw = field.isSigned ? unsignedMax / 2 : unsignedMax;
    const qint32 minRaw = field.isSigned ? -maxRaw - 1 : 0;

    return {minRaw, maxRaw};
}