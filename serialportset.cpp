#include "serialportset.h"
#include <QDebug>
SerialPortSet::SerialPortSet(QObject *parent)
    : QObject{parent}
{
    // const auto serialport = QSerialPortInfo::availablePorts();
    // for (const QSerialPortInfo &info:serialport)
    // {
    //     info.portName();
    //     // qDebug() <<"info.portName()"<<info.portName();
    // }

    // serialp.setPortName(QStringLiteral("COM2"));
    // serialp.setBaudRate(QSerialPort::Baud115200);
    // serialp.setDataBits(QSerialPort::Data8);
    // serialp.setFlowControl(QSerialPort::NoFlowControl);
    // serialp.setParity(QSerialPort::NoParity);
    // serialp.setStopBits(QSerialPort::OneStop);
    // const bool opened = serialp.open(QIODevice::ReadWrite);
    // qDebug()<<"opened" <<opened;
    // if(opened==true)
    // {
    //     ;
    // }
}


// void SerialPortSet::start(const QString &portName, qint32 baudRate)
// {
//     if (m_port && m_port->isOpen())          // 幂等：已经打开就只回报状态
//     {
//         emit portStateChanged(true, m_port->portName());
//         return;
//     }

//     if (!m_port)                              // 只创建一次，复用到下次打开
//     {
//         m_port = new QSerialPort(this);
//         // connect(m_port, &QSerialPort::readyRead, this, &SerialPortSet::onReadyRead);
//     }

//     m_port->setPortName(portName);
//     m_port->setBaudRate(baudRate);
//     m_port->setDataBits(QSerialPort::Data8);
//     m_port->setStopBits(QSerialPort::OneStop);
//     m_port->setParity(QSerialPort::NoParity);
//     m_port->setFlowControl(QSerialPort::NoFlowControl);

//     if (!m_port->open(QIODevice::ReadOnly))
//     {
//         emit errorOccurred(m_port->errorString());
//         emit portStateChanged(false, QString());
//         return;
//     }

//     // 新会话从干净状态开始
//     m_ring.clear();
//     m_okFrames = 0;
//     m_rejectedFrames = 0;
//     m_droppedBytes = 0;
//     m_lastReason = NoReject;
//     m_statsTimer.start();
//     // publishStats(true);

//     emit portStateChanged(true, portName);
// }

// void SerialPortSet::stop()
// {
//     if (m_port && m_port->isOpen())
//         m_port->close();
//     m_ring.clear();
//     emit portStateChanged(false, QString());
// }
