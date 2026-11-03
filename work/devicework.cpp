#include "work/devicework.h"
#include "icd.h"
#include <QDebug>
#include <QDateTime>
DeviceWork::DeviceWork(QObject *parent)
    : QObject{parent}
{


}

void DeviceWork::init()
{
    //周期发送
    tw_timer = new QTimer(this);
    tw_timer->setInterval(1000);
    connect(tw_timer,&QTimer::timeout,this,&DeviceWork::OncePeriod,Qt::DirectConnection);

    //初始化串口 TODO 可注释
    sendPort = new QSerialPort(this);      // 关键：在 worker 线程里创建
    sendPort->setPortName(QStringLiteral("COM2"));
    sendPort->setBaudRate(QSerialPort::Baud115200);
    sendPort->setDataBits(QSerialPort::Data8);
    sendPort->setStopBits(QSerialPort::OneStop);
    sendPort->setParity(QSerialPort::NoParity);

    if (!sendPort->open(QIODevice::ReadWrite))
        emit errorOccurred(sendPort->errorString());

}

void DeviceWork::startPeriod()
{
    tw_timer->start();
    emit periodLamp(true);
}

void DeviceWork::OncePeriod()
{
    //可直接替换为写串口
    sendPort->write(sendArray);
}

void DeviceWork::UpdatePeriod(const int periUp)
{
    tw_timer->setInterval(periUp); //在运行中本身就可以暂停再运行
}

void DeviceWork::updateArray(const sendParamters &udp)
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
    emit statisticsChanged(m_okFrames, m_rejectedFrames,
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

void DeviceWork::onReadyRead()
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

void DeviceWork::start(const QString &portName, qint32 baudRate)
{
    if (receivePort && receivePort->isOpen())          // 串口已打开就立即发送信号
    {
        emit portStateChanged(true, receivePort->portName());
        return;
    }

    if (!receivePort)                  // 创建接收对象，并且有参数就执行槽函数
    {
        receivePort = new QSerialPort(this);
        connect(receivePort, &QSerialPort::readyRead, this, &DeviceWork::onReadyRead);
    }

    receivePort->setPortName(portName);
    receivePort->setBaudRate(baudRate);
    receivePort->setDataBits(QSerialPort::Data8);
    receivePort->setStopBits(QSerialPort::OneStop);
    receivePort->setParity(QSerialPort::NoParity);
    receivePort->setFlowControl(QSerialPort::NoFlowControl);

    if (!receivePort->open(QIODevice::ReadWrite))
    {
        emit errorOccurred(receivePort->errorString());
        emit portStateChanged(false, QString());
        return;
    }

    // 新会话从干净状态开始
    ringb.clear();
    m_okFrames = 0;
    m_rejectedFrames = 0;
    m_droppedBytes = 0;
    m_lastReason = NoReject;
    m_statsTimer.start();
    publishStats(true);

    emit portStateChanged(true, portName);
}

void DeviceWork::stop()
{
    if (receivePort && receivePort->isOpen())
        receivePort->close();
    ringb.clear();
    emit portStateChanged(false, QString());
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
        emit frameReceived(QDateTime::currentDateTime(), payload);
    }
}

void DeviceWork::workPeriodStop()
{
    tw_timer->stop();
    emit periodLamp(false);
}






