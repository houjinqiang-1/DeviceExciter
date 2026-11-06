#ifndef DEVICEWORK_H
#define DEVICEWORK_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QSerialPort>
#include <QElapsedTimer>

#include "ringbuffer.h"
#include "icd.h"

class DeviceWork : public QObject
{
    Q_OBJECT
public:
    explicit DeviceWork(QObject *parent = nullptr);
public slots:
    //初始化Qtimer、发送串口
    void Wslots_init();
    //去掉参数
    void Wslots_startPeriod();
    //去掉参数
    void Wslots_OncePeriod();
    // void RceiveByte(const QByteArray &bytea);
    void Wslots_workPeriodStop();

    void Wslots_UpdatePeriod(const int periUp);
    //更新参数
    void Wslots_updateArray(const sendParamters &sdp);

    //MainWindow 发 openRequested 时打开串口
    void Wslots_start(const QString &portName, qint32 baudRate);
    //MainWindow 发 closeRequested 时关闭串口
    void Wslots_stop();
    void Wslots_onReadyRead();

signals:
    //指示灯信号
    void Wsignals_periodLamp(bool onLamp);

    //成功解析出一帧之后收到一帧
    void Wsignals_frameReceived(const QDateTime &recvTime, const QByteArray &payload);
    //打开端口失败
    void Wsignals_errorOccurred(const QString &message);
    void Wsignals_portStateChanged(bool open, const QString &portName);
    void Wsignals_statisticsChanged(quint64 okFrames, quint64 rejectedFrames,
                           quint64 droppedBytes, quint8 lastReason);

public:
    void sendUpdate();
    void buildFrame(const QByteArray &bytea);
    void parseFrames();

public:
    //枚举拒绝原因
    enum RejectReason : quint8 {
        NoReject = 0,
        IdMismatch, //识别符不符
        LengthMismatch, //帧长度不符
        ChecksumError,  //校验失败
        RingOverflow    //环形缓冲区溢出
    };

private:
    QByteArray sendArray;
    // QByteArray receiveArray;
    bool time_label = false;
    RingBuffer ringb;
    // SerialPortSet *portSet = nullptr;
    QTimer *tw_timer = nullptr;
    QSerialPort *port = nullptr;
    //QSerialPort *sendPort = nullptr;
    //QSerialPort *receivePort = nullptr;

    quint64 m_okFrames = 0;
    quint64 m_rejectedFrames = 0;
    quint64 m_droppedBytes = 0;
    RejectReason m_lastReason = NoReject;
    QElapsedTimer m_statsTimer;

private:
    // void publishStats(bool force);
    void publishStats(bool force = false);

    void reject(RejectReason reason);

// TODO 新增参数update后的槽函数
// 主线程的结构体，更新本地的sendArray (调用buildframe)
// 删除start，stop 只干开关的事
// QTimer 关联到写串口的槽，当前也就是OncePeriod
// 命名规范,以及槽加slot，信号加signal前缀,命名与任务相关
// 加注释，打印内容清晰
// 控件换RadioButton
};

#endif // DEVICEWORK_H
