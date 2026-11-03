#ifndef SERIALPORTSET_H
#define SERIALPORTSET_H

#include <QObject>
#include <QSerialPort>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QByteArray>
#include "ringbuffer.h"
#include <QElapsedTimer>
class SerialPortSet : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortSet(QObject *parent = nullptr);

public:
    QSerialPort serialp;

public:
    //枚举拒绝原因
    enum RejectReason : quint8 {
        NoReject = 0,
        IdMismatch, //识别符不符
        LengthMismatch, //帧长度不符
        ChecksumError,  //校验失败
        RingOverflow    //环形缓冲区溢出
    };

// public:
//     void sendDeviceToPort(const QByteArray &bytea);

// public slots:
//     void slots_start(const QString &portName, qint32 baudRate);
//     // MainWindow 发 openRequested 时打开串口
//     void slots_stop();
//     // MainWindow 发 closeRequested 时关闭串口


    // void onReadyRead();

signals:
    // void frameReceived(const QDateTime &recvTime, const QByteArray &payload);
    // //成功解析出一帧之后收到一帧
    // void errorOccurred(const QString &message);
    // //打开端口失败
    // void portStateChanged(bool open, const QString &portName);
    // void statisticsChanged(quint64 okFrames, quint64 rejectedFrames,
    //                        quint64 droppedBytes, quint8 lastReason);


private:
    void parseFrames();
    //从环形区里切出完整帧
    void reject(RejectReason reason);
    //拒绝一帧，记账 + 滑掉 1 字节 + 上报
    void publishStats(bool force = false);
    //上报统计数据，bool force = true时不管节流立刻上报

    QSerialPort *m_port = nullptr;

    RingBuffer m_ring;

    quint64 m_okFrames = 0;
    quint64 m_rejectedFrames = 0;
    quint64 m_droppedBytes = 0;
    RejectReason m_lastReason = NoReject;
    QElapsedTimer m_statsTimer;
    //距离上次上报够不够xx毫秒

};

#endif // SERIALPORTSET_H
