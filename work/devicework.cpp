#include "work/devicework.h"
#include "icd.h"
#include <QDebug>
#include <QDateTime>

namespace {
const QString kReceivePortName = QStringLiteral("COM5");
}

DeviceWork::DeviceWork(QObject *parent)
    : QObject{parent}
{
}

void DeviceWork::Wslots_init()
{
    //初始化串口 TODO 可注释
    sendPort = new QSerialPort(this);      // 关键：在 worker 线程里创建
    sendPort->setPortName(QStringLiteral("COM2"));
    sendPort->setBaudRate(QSerialPort::Baud115200);
    sendPort->setDataBits(QSerialPort::Data8);
    sendPort->setStopBits(QSerialPort::OneStop);
    sendPort->setParity(QSerialPort::NoParity);

    if (!sendPort->open(QIODevice::ReadWrite))
        emit Wsignals_errorOccurred(sendPort->errorString());

}

void DeviceWork::Wslots_startPeriod()
{
    tw_timer->start();
    emit Wsignals_periodLamp(true);
}

void DeviceWork::Wslots_OncePeriod()
{
    if (!sendPort || !sendPort->isOpen())
        return;                 // 发送口没开，直接忽略
    sendPort->write(sendArray);
}

void DeviceWork::Wslots_UpdatePeriod(const int periUp)
{
    tw_timer->setInterval(periUp); //在运行中本身就可以暂停再运行
}

void DeviceWork::Wslots_updateArray(const sendParamters &udp)
{
    const QByteArray loadCopy(reinterpret_cast<const char *>(&udp),
                             static_cast<int>(sizeof(sendParamters)));
    sendArray = loadCopy;
    for(int i=0;i< 60 - loadCopy.size();i++)
    {
        sendArray.append((char)(i));
    }
    QByteArray frameHeader;
    QByteArray frameTail;
    frameHeader.append(static_cast<char>(0xEB));
    frameHeader.append(static_cast<char>(0x90));
    frameHeader.append(static_cast<char>(0x42));
    frameHeader.append(static_cast<char>(0x4F));
    frameTail.append(static_cast<char>(0xFE));
    frameTail.append(static_cast<char>(0xFE));

    sendArray = frameHeader + sendArray +frameTail;
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
    const QByteArray data = receivePort->readAll();
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

void DeviceWork::Wslots_start(const QString &sendPortName, qint32 baudRate)
{
    // 两个口都已打开：只回报状态
    if (receivePort && receivePort->isOpen() && sendPort && sendPort->isOpen())
    {
        emit Wsignals_portStateChanged(true, QStringLiteral("%1 收 / %2 发")
                                        .arg(kReceivePortName, sendPort->portName()));
        return;
    }

    // ---------- ① 接收口：固定 COM5 ----------
    if (!receivePort)
    {
        receivePort = new QSerialPort(this);
        connect(receivePort, &QSerialPort::readyRead, this, &DeviceWork::Wslots_onReadyRead);
    }
    if (!receivePort->isOpen())
    {
        receivePort->setPortName(kReceivePortName);
        receivePort->setBaudRate(baudRate);
        receivePort->setDataBits(QSerialPort::Data8);
        receivePort->setStopBits(QSerialPort::OneStop);
        receivePort->setParity(QSerialPort::NoParity);
        receivePort->setFlowControl(QSerialPort::NoFlowControl);

        if (!receivePort->open(QIODevice::ReadOnly))
        {
            emit Wsignals_errorOccurred(QStringLiteral("接收口 %1 打开失败：%2")
                                   .arg(kReceivePortName, receivePort->errorString()));
            emit Wsignals_portStateChanged(false, QString());
            return;
        }
    }

    // ---------- ② 发送口：由界面下拉框决定 ----------
    if (!sendPort)
        sendPort = new QSerialPort(this);
    if (!sendPort->isOpen())
    {
        sendPort->setPortName(sendPortName);
        sendPort->setBaudRate(baudRate);
        sendPort->setDataBits(QSerialPort::Data8);
        sendPort->setStopBits(QSerialPort::OneStop);
        sendPort->setParity(QSerialPort::NoParity);
        sendPort->setFlowControl(QSerialPort::NoFlowControl);

        if (!sendPort->open(QIODevice::ReadWrite))
        {
            emit Wsignals_errorOccurred(QStringLiteral("发送口 %1 打开失败：%2")
                                   .arg(sendPortName, sendPort->errorString()));
            receivePort->close();          // 回滚，不留"半个会话"
            emit Wsignals_portStateChanged(false, QString());
            return;
        }
    }

    // ---------- ③ 新会话：计数清零 ----------
    ringb.clear();
    m_okFrames = 0;
    m_rejectedFrames = 0;
    m_droppedBytes = 0;
    m_lastReason = NoReject;
    m_statsTimer.start();
    publishStats(true);

    emit Wsignals_portStateChanged(true, QStringLiteral("%1 收 / %2 发")
                                    .arg(kReceivePortName, sendPortName));
}

void DeviceWork::Wslots_stop()
{
    if (receivePort && receivePort->isOpen())
        receivePort->close();
    if (sendPort && sendPort->isOpen())
        sendPort->close();
    ringb.clear();
    emit Wsignals_portStateChanged(false, QString());
}



void DeviceWork::parseFrames()
{
    constexpr quint8 kId        = 0x4F;
    constexpr int    kFrameSize = 5 + sizeof(MSG_Struct);   // 66 = 0x42

    while (ringb.size() >= 4)        // 4 = 下面所有 peek 的最大(偏移+长度)
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

        quint8 lenByte = 0;
        quint8 idByte  = 0;
        ringb.peek(&lenByte, 1, 2);
        ringb.peek(&idByte,  1, 3);

        if (idByte != kId)          { reject(IdMismatch);     continue; }
        if (lenByte != kFrameSize)  { reject(LengthMismatch); continue; }

        if (ringb.size() < kFrameSize)
            break;                    // 半包，等下一批

        const QByteArray frame = ringb.peekBytes(kFrameSize);

        // 校验码 1 字节：累加范围/是否取反待 ICD 确认，暂不校验
        constexpr bool kVerifyChecksum = false;
        if (kVerifyChecksum)
        {
            // TODO: 待校验算法确定后实现
        }

        const QByteArray payload = frame.mid(4, sizeof(MSG_Struct));
        ringb.discard(kFrameSize);

        ++m_okFrames;
        publishStats();
        emit Wsignals_frameReceived(QDateTime::currentDateTime(), payload);
    }
}

void DeviceWork::Wslots_workPeriodStop()
{
    tw_timer->stop();
    emit Wsignals_periodLamp(false);
}






