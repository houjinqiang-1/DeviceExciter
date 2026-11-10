
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "work\devicework.h"
#include "icd.h"
#include <QMainWindow>
#include <QSerialPort>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>
#include <QTimer>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDateTime>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    //发送信号
    void Msignals_inforPSend();  //发送周期发送信号
    void Msignals_inforTSend();  //发送触发发送信号
    void Msignals_updateArray(const QVector<double> &values); //发送更新后数值结构体信号
    void Msignals_updateValue(const int periUp); //发送更新周期频率信号

    //接收信号
    void Msignals_openRequested(const QString &sendPortName, qint32 baudRate); //打开接收端口信号
    void Msignals_closeRequested(); //关闭接收端口信号

private slots:
    void Mslots_on_PStart_clicked();
    void Mslots_on_TriggerStart_clicked();
    void Mslots_setPeriodLamp(bool onLamp);
    void Mslots_on_PUpdate_clicked();
    void Mslots_on_pushButton_clicked();



    void Mslots_refreshPorts();//刷新端口列表，程序启动时、每次点"打开"前

    void Mslots_onOpenClicked(); //点"打开/关闭串口"按钮

    void Mslots_onFrameReceived(const QDateTime &recvTime, const QByteArray &payload);//收到一帧（Worker解析完一帧之后）

    void Mslots_onSerialError(const QString &message);  //打开端口失败

    void Mslots_onPortStateChanged(bool open, const QString &portName); //端口状态灯变化

    void Mslots_onStatisticsChanged(quint64 okFrames, quint64 rejectedFrames,//有效、拒绝、丢弃的计数统计变化（刷新状态栏）
                             quint64 droppedBytes, quint8 lastReason);  //lastreason 最近一次拒绝的原因

    void Mslots_updateRawData(const QStringList &values);
private:
    Ui::MainWindow *ui;
private:
    void tableSet(const QStringList &resolutions,
                  const QStringList &offsets,
                  const QStringList &ranges);
    void openSerialPort();
    void portReceive();
    void timeoutd();
    void buildUi();
    QByteArray getByte();
    void setIndicator(bool open);

    //提示条提示
    void showNotice(const QString &text, bool isError = true);


public:
    DeviceWork *m_towork;
    QThread m_thread;
    QSerialPort serialp;
    QByteArray bytearray;
    bool sendlabel =false;

    QComboBox *m_portCombo = nullptr;   //端口下拉框
    QComboBox *m_baudCombo = nullptr;   //波特率下拉框
    QPushButton *m_openBtn = nullptr;   //打开、关闭串口按钮
    QPushButton *clearBtn = nullptr;    //清空窗口
    QLabel *m_indicator = nullptr;  //指示灯
    // QPlainTextEdit *m_receiveView = nullptr;    //接收显示区（每一帧结果）
    QThread *m_workerThread = nullptr;  //工作线程
    // SerialWorker *m_worker = nullptr;   //工作线程里的工作对象
    bool m_portOpen = false;    //端口是否打开
    QLabel *m_notice = nullptr; //提示条
    QTimer *m_noticeTimer = nullptr;    //提示定时器
    // QCheckBox *m_saveChk = nullptr; //是否接收数据到文件的勾选框
    // fuel_sendParamters fuelSendP;

};
#endif // MAINWINDOW_H
