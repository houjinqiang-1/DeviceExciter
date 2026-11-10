#include "work/devicework.h"
#include "icd.h"
#include "icd.cpp"
#include <QDebug>
#include <QDateTime>
#include <cmath>


DeviceWork::DeviceWork(QObject *parent)
    : QObject{parent}
{
}

void DeviceWork::Wslots_init()
{
    tw_timer = new QTimer(this);
    tw_timer->setInterval(1000);
    connect(tw_timer, &QTimer::timeout, this, &DeviceWork::Wslots_OncePeriod,
            Qt::DirectConnection);

    // 串口对象不在这里创建，也不在这里打开
    // 端口由用户在界面上选择，点"打开串口"时再打开
    publishFuelFieldInfo();
}

void DeviceWork::Wslots_startPeriod()
{
    tw_timer->start();
    emit Wsignals_periodLamp(true);
}

void DeviceWork::Wslots_OncePeriod()
{
    if (!port || !port->isOpen())
        return;                 // 端口没打开，静默忽略
    port->write(sendArray);
}

void DeviceWork::Wslots_UpdatePeriod(const int periUp)
{
    tw_timer->setInterval(periUp); //在运行中本身就可以暂停再运行
}

inline quint16 sumMod(QVector<qint32> rawValues)
{
    quint16 sum = 0;

    for(int i = 2; i<rawValues.size()-2; i++)
    {
        sum += static_cast<quint8>(rawValues[i]);
    }

    return sum % 256;
}

void DeviceWork::Wslots_updateArray(const QVector<double> &values)
{

    fuelStruct fuel{};
    QVector<qint32> rawValues;

    const QString error = fillPacket(values, kFuelFields, fuel, rawValues);

    if (!error.isEmpty()) {
        qDebug()<<"fillPacket解析错误";
        return;
    }
    // 当前燃油载荷按小端协议布局直接复制。
    static_assert(Q_BYTE_ORDER == Q_LITTLE_ENDIAN);
    static_assert(sizeof(fuelStruct) == 60);

    const QByteArray payload(
        reinterpret_cast<const char *>(&fuel),
        static_cast<int>(sizeof(fuel)));

    sendArray = QByteArray::fromHex("EB90424F")
                + payload
                + QByteArray::fromHex(
                    QStringLiteral("%1").arg(static_cast<quint16>(sumMod(rawValues)),
                             4, 16, QLatin1Char('0'))
                        .toLatin1());

    QStringList hexValues;
    int byteOffset = 0;

    for (int row = 0; row < rawValues.size(); ++row)
    {
        // 第19行之前跳过 reserve1。
        if (row == 19)
            ++byteOffset;

        // 根据当前 fuel_sendParamters 的字段类型确定长度。
        const bool singleByte =
            row == 7 ||
            row == 8 ||
            (row >= 11 && row <= 14) ||
            row >= 30;

        const int byteSize = singleByte ? 1 : 2;

        const QByteArray byArray = payload.mid(byteOffset,byteSize);
        // qDebug() <<"byArray"<<byArray;
        const QString hexValue = QString::fromUtf8(byArray.toHex(' ').toUpper());
       //  qDebug() <<"hexValue"<<hexValue;
        // 按小端内存布局读取原始值。
        // quint16 raw = static_cast<quint8>(payload[byteOffset]);

        // if (byteSize == 2)
        // {
        //     raw |= static_cast<quint16>(
        //                static_cast<quint8>(payload[byteOffset + 1]))
        //            << 8;
        // }

        // QString hex = QStringLiteral("%1")
        //                   .arg(raw, byteSize * 2, 16, QLatin1Char('0'))
        //                   .toUpper();

        // if (byteSize == 2)
        //     hex.insert(2, QLatin1Char(' '));
        hexValues.append(hexValue);

        byteOffset += byteSize;
    }
    // qDebug() <<"hexValues"<<hexValues;
    emit WSignals_rawData(hexValues);
}

void DeviceWork::publishStats(bool force)
{
    if (!force && m_statsTimer.elapsed() < 200)   // 状态类：最多 5 次/秒
        return;
    m_statsTimer.restart();
    emit Wsignals_statisticsChanged(m_okFrames, m_rejectedFrames,
                           m_droppedBytes, static_cast<quint8>(m_lastReason));// 重新计时，避免上一条的定时器把新消息提前藏掉
}

void DeviceWork::reject(RejectReason reason)
{
    ++m_rejectedFrames;
    ++m_droppedBytes;          // 至少滑掉 1 字节
    m_lastReason = reason;
    ringb.discard(1);
    publishStats(true);        // 拒绝是事件：立即上报
}

void DeviceWork::Wslots_onReadyRead()
{
    if (!port)
        return;

    const QByteArray data = port->readAll();
    if (data.isEmpty())
        return;

    const qsizetype written = ringb.write(data);
    if (written != data.size())
    {
        m_droppedBytes += static_cast<quint64>(data.size() - written);
        m_lastReason = RingOverflow;
        publishStats(true);
    }
    parseFrames();
}

void DeviceWork::Wslots_start(const QString &portName, qint32 baudRate)
{
    // 已经打开了：只回报状态
    if (port && port->isOpen())
    {
        emit Wsignals_portStateChanged(true, port->portName());
        return;
    }

    if (!port)
    {
        port = new QSerialPort(this);          // 在 worker 线程里创建
        connect(port, &QSerialPort::readyRead, this, &DeviceWork::Wslots_onReadyRead);
    }

    port->setPortName(portName);
    port->setBaudRate(baudRate);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setParity(QSerialPort::NoParity);
    port->setFlowControl(QSerialPort::NoFlowControl);

    // 收发同一个口，必须 ReadWrite
    if (!port->open(QIODevice::ReadWrite))
    {
        emit Wsignals_errorOccurred(QStringLiteral("端口 %1 打开失败：%2")
                                        .arg(portName, port->errorString()));
        emit Wsignals_portStateChanged(false, QString());
        return;
    }

    // 新会话：计数清零
    ringb.clear();
    m_okFrames = 0;
    m_rejectedFrames = 0;
    m_droppedBytes = 0;
    m_lastReason = NoReject;
    m_statsTimer.start();
    publishStats(true);

    emit Wsignals_portStateChanged(true, portName);
}

void DeviceWork::Wslots_stop()
{
    if (port && port->isOpen())
        port->close();
    ringb.clear();
    emit Wsignals_portStateChanged(false, QString());
}



void DeviceWork::parseFrames()
{
    constexpr int    kFrameSize = 3 + sizeof(receiveParamters) + 1;   // 2+1+20+1 = 24
    constexpr int    kDataOffset = 3;
    constexpr quint8 kFrameLenByte = kFrameSize;                      // 0x18

    while (ringb.size() >= 4)          // 4 = 所有 peek 的(偏移+长度)最大值
    {
        quint8 header[2] = {0, 0};
        ringb.peek(header, 2, 0);
        if (header[0] != 0xEB || header[1] != 0x90)
        {
            ++m_droppedBytes;
            ringb.discard(1);
            publishStats();
            continue;
        }

        // 接收帧没有识别符：偏移 2 直接是帧长度
        quint8 lenByte = 0;
        ringb.peek(&lenByte, 1, 2);
        if (lenByte != kFrameLenByte)
        {
            reject(LengthMismatch);
            continue;
        }

        if (ringb.size() < kFrameSize)
            break;                     // 半包，等下一批

        const QByteArray frame = ringb.peekBytes(kFrameSize);

        // 校验：整帧所有字节相加 % 256 == 0
        constexpr bool kVerifyChecksum = true;
        if (kVerifyChecksum)
        {
            quint16 sum = 0;
            for (int i = 0; i < kFrameSize; ++i)
                sum = static_cast<quint16>(sum + static_cast<quint8>(frame.at(i)));

            if ((sum % 256) != 0)
            {
                reject(ChecksumError);
                continue;
            }
        }

        const QByteArray payload = frame.mid(kDataOffset, sizeof(receiveParamters));
        ringb.discard(kFrameSize);

        ++m_okFrames;
        publishStats();
        emit Wsignals_frameReceived(QDateTime::currentDateTime(), payload);
    }
}

void DeviceWork::publishFuelFieldInfo()
{
    QStringList resolutions;
    QStringList offsets;
    QStringList ranges;

    for (const PacketField &field : kFuelFields) {
        const auto [minRaw, maxRaw] = packetRawRange(field);
        const double first =
            minRaw * field.resolution + field.engineeringOffset;
        const double last =
            maxRaw * field.resolution + field.engineeringOffset;

        resolutions.append(QString::number(field.resolution, 'g', 15));
        offsets.append(QString::number(field.engineeringOffset, 'g', 15));
        ranges.append(QStringLiteral("[%1, %2]")
                          .arg(QString::number(std::min(first, last), 'g', 15))
                          .arg(QString::number(std::max(first, last), 'g', 15)));
    }

    emit Wsignals_fuelFieldsReady(resolutions, offsets, ranges);
}

void DeviceWork::Wslots_workPeriodStop()
{
    tw_timer->stop();
    emit Wsignals_periodLamp(false);
}






