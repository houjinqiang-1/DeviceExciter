#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSerialPort>
#include <QMetaType>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,m_towork(new DeviceWork)
{
    ui->setupUi(this);
    //创建表
    tableSet(5,9);
    buildUi();
    //
    //控件显示
    connect(ui->Trigger,&QRadioButton::toggled,this,[this](bool triggerMode){
        ui->PUpdate->setEnabled(!triggerMode);
        ui->PStart->setEnabled(!triggerMode);
        ui->PStop->setEnabled(!triggerMode);
        ui->spinBox->setEnabled(!triggerMode);

        ui->TriggerStart->setEnabled(triggerMode);
        if(triggerMode)
        {
            ui->PStop->clicked(true);
        }
    },Qt::DirectConnection);

    ui->Trigger->setChecked(true);
    ui->Trigger->clicked(true);

    //移动到子线程
    m_towork->moveToThread(&m_thread);
    //初始化子线程定时任务
    connect(&m_thread, &QThread::started, m_towork, &DeviceWork::Wslots_init,Qt::QueuedConnection);

    //将表格中的数据发送给子线程进行处理
    //发送周期处理信号
    connect(this,&MainWindow::Msignals_inforPSend,m_towork,&DeviceWork::Wslots_startPeriod,Qt::QueuedConnection);
    //发送触发处理信号
    connect(this,&MainWindow::Msignals_inforTSend,m_towork,&DeviceWork::Wslots_OncePeriod,Qt::QueuedConnection);

    //发送停止信号
    connect(ui->PStop,&QPushButton::clicked,m_towork,&DeviceWork::Wslots_workPeriodStop,Qt::QueuedConnection);
    //灯 信号
    connect(m_towork, &DeviceWork::Wsignals_periodLamp, this, &MainWindow::Mslots_setPeriodLamp);

    //发送更新信号
    qRegisterMetaType<sendParamters>("sendParamters");
    connect(this,&MainWindow::Msignals_updateArray,m_towork,&DeviceWork::Wslots_updateArray);

    //更新定时任务周期值
    ui->spinBox->setValue(1000);
    connect(this,&MainWindow::Msignals_updateValue,m_towork,&DeviceWork::Wslots_UpdatePeriod);

    //任务结束销毁线程
    connect(&m_thread, &QThread::finished, m_towork, &QObject::deleteLater,Qt::DirectConnection);

    //打开串口
    connect(this, &MainWindow::Msignals_openRequested,  m_towork, &DeviceWork::Wslots_start);
    //关闭串口
    connect(this, &MainWindow::Msignals_closeRequested, m_towork, &DeviceWork::Wslots_stop);

    connect(m_towork, &DeviceWork::Wsignals_frameReceived,     this, &MainWindow::Mslots_onFrameReceived);
    connect(m_towork, &DeviceWork::Wsignals_errorOccurred,     this, &MainWindow::Mslots_onSerialError);
    connect(m_towork, &DeviceWork::Wsignals_portStateChanged,  this, &MainWindow::Mslots_onPortStateChanged);
    connect(m_towork, &DeviceWork::Wsignals_statisticsChanged, this, &MainWindow::Mslots_onStatisticsChanged);

    m_thread.start();
    Mslots_on_pushButton_clicked();

    // openSerialPort();
}

MainWindow::~MainWindow()
{
    m_thread.quit();
    m_thread.wait();
    m_towork = nullptr;

    delete ui;
}

void MainWindow::tableSet(int col, int row)
{
    ui->TX->setColumnCount(col);
    ui->TX->setRowCount(row);
    QStringList clHeads;
    clHeads <<"工程值"<<"下发原始值"<<"偏移量"<<"工程值范围";
    ui->TX->setHorizontalHeaderLabels(clHeads);

    QStringList rowHeads;
    rowHeads<<"左1油箱油量"<<"左二邮箱油量"<<"右1邮箱油量"<<"右2油箱油量"<<"左邮箱油量"<<"右邮箱油量"
             <<"总油量"<<"燃油温度"<<"燃油高温告警";
    ui->TX->setVerticalHeaderLabels(rowHeads);

    for(int i=0;i<row;i++)
    {
        ui->TX->setItem(i,0,new QTableWidgetItem(QString("%1%2").arg(i).arg(1)));
    }


}

void MainWindow::Mslots_on_pushButton_clicked()
{
    qDebug()<<"开始";

    sendParamters sdp{};
    sdp.left1fuel = ui->TX->item(0,0)->text().toUInt();
    sdp.left2fuel = ui->TX->item(1,0)->text().toUInt();
    sdp.right1fuel = ui->TX->item(2,0)->text().toUInt();
    sdp.right2fuel = ui->TX->item(3,0)->text().toUInt();
    sdp.leftfuel = ui->TX->item(4,0)->text().toUInt();
    sdp.rightfuel = ui->TX->item(5,0)->text().toUInt();
    sdp.allfuel = ui->TX->item(6,0)->text().toUInt();
    sdp.fuelheat = ui->TX->item(7,0)->text().toUInt();
    sdp.fuelheatcas = ui->TX->item(8,0)->text().toUInt();
    // sdp.leftsendpress = ui->TX->item(9,0)->text().toUInt();
    // sdp.rigthsendpress = ui->TX->item(2,0)->text().toUInt();

    emit Msignals_updateArray(sdp);

}

void MainWindow::Mslots_on_PStart_clicked()
{
    emit Msignals_inforPSend();
}


void MainWindow::Mslots_on_TriggerStart_clicked()
{
    emit Msignals_inforTSend();
}

void MainWindow::Mslots_setPeriodLamp(bool onLamp)
{
    ui->label_3->setProperty("lit", onLamp);
    ui->label_3->style()->unpolish(ui->label_3);
    ui->label_3->style()->polish(ui->label_3);
}


void MainWindow::Mslots_on_PUpdate_clicked()
{
    emit Msignals_updateValue(ui->spinBox->value());
}



void MainWindow::Mslots_onFrameReceived(const QDateTime &recvTime, const QByteArray &payload)
{
    MSG_Struct msg;
    memcpy(&msg, payload.constData(), sizeof(MSG_Struct));

    ui->plainTextEdit->appendPlainText(
        recvTime.toString(QStringLiteral("HH:mm:ss.zzz"))
    + QStringLiteral("  总油量=%1 燃油温度=%2 | %3")
          .arg(msg.P7)
          .arg(static_cast<int>(msg.P8))
          .arg(QString::fromLatin1(payload.toHex(' '))));

}


void MainWindow::buildUi()
{
    m_portCombo = new QComboBox(this);
    m_portCombo->setMinimumWidth(180);

    m_baudCombo = new QComboBox(this);
    m_baudCombo->addItem(QStringLiteral("9600"),   9600);
    m_baudCombo->addItem(QStringLiteral("19200"),  19200);
    m_baudCombo->addItem(QStringLiteral("38400"),  38400);
    m_baudCombo->addItem(QStringLiteral("57600"),  57600);
    m_baudCombo->addItem(QStringLiteral("115200"), 115200);
    m_baudCombo->addItem(QStringLiteral("230400"), 230400);
    m_baudCombo->setCurrentIndex(4);          // 默认 115200

    m_openBtn = new QPushButton(QStringLiteral("打开串口"), this);
    clearBtn = new QPushButton(QStringLiteral("清空窗口"), this);

    m_indicator = new QLabel(this);
    m_indicator->setFixedSize(18, 18);

    m_notice = new QLabel(this);
    m_notice->hide();                       // 初始不占位

    m_noticeTimer = new QTimer(this);
    m_noticeTimer->setSingleShot(true);
    m_noticeTimer->setInterval(3000);
    connect(m_noticeTimer, &QTimer::timeout, m_notice, &QLabel::hide);

    auto *topBar = new QHBoxLayout;
    topBar->addWidget(new QLabel(QStringLiteral("接收口: COM5"), this));
    topBar->addSpacing(12);
    topBar->addWidget(new QLabel(QStringLiteral("发送端口:"), this));
    topBar->addWidget(m_portCombo);
    topBar->addWidget(clearBtn);
    topBar->addSpacing(12);
    topBar->addWidget(new QLabel(QStringLiteral("波特率:"), this));
    topBar->addWidget(m_baudCombo);
    topBar->addSpacing(12);
    topBar->addWidget(m_openBtn);
    topBar->addSpacing(8);
    topBar->addWidget(m_indicator);
    topBar->addStretch();

    ui->gridLayout->addLayout(topBar, 3, 0);
    ui->gridLayout->addWidget(m_notice, 4, 0);
    // ui->gridLayout->addWidget(m_receiveView, 5, 0);

    setIndicator(false);      // 初始红
    Mslots_refreshPorts();

    // topBar->addSpacing(16);
    // topBar->addWidget(m_saveChk);
    connect(m_openBtn,   &QPushButton::clicked, this, &MainWindow::Mslots_onOpenClicked);
    connect(clearBtn, &QPushButton::clicked, ui->plainTextEdit, &QPlainTextEdit::clear);


}

void MainWindow::Mslots_refreshPorts()
{
    const QString keep = m_portCombo->currentData().toString();
    m_portCombo->clear();

    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports)
        m_portCombo->addItem(info.portName() + QStringLiteral("  ") + info.description(),
                             info.portName());   // 界面显示名字，data 里只存端口名

    const int idx = m_portCombo->findData(keep);
    if (idx >= 0)
        m_portCombo->setCurrentIndex(idx);
}
void MainWindow::setIndicator(bool open)
{
    m_indicator->setStyleSheet(open
                                   ? QStringLiteral("QLabel{background-color:#2ecc71;border-radius:9px;}")
                                   : QStringLiteral("QLabel{background-color:#e74c3c;border-radius:9px;}"));
}


void MainWindow::Mslots_onOpenClicked()
{

    if (m_portOpen)
    {
        emit Msignals_closeRequested();
        return;
    }

    Mslots_refreshPorts();
    const QString port = m_portCombo->currentData().toString();
    if (port.isEmpty()) {
        statusBar()->showMessage(QStringLiteral("没有可用串口"));
        return;
    }
    emit Msignals_openRequested(port, m_baudCombo->currentData().toInt());
}



void MainWindow::Mslots_onPortStateChanged(bool open, const QString &portName)
{
    m_portOpen = open;
    m_openBtn->setText(open ? QStringLiteral("关闭串口") : QStringLiteral("打开串口"));
    m_portCombo->setEnabled(!open);
    m_baudCombo->setEnabled(!open);
    setIndicator(open);

    if (open)
        showNotice(QStringLiteral("已打开 %1").arg(portName), false);   // false = 绿色信息
}



void MainWindow::showNotice(const QString &text, bool isError)
{
    m_notice->setStyleSheet(isError
                                ? QStringLiteral("QLabel{color:#c0392b;font-weight:bold;padding:4px;}")
                                : QStringLiteral("QLabel{color:#27ae60;font-weight:bold;padding:4px;}"));
    m_notice->setText(text);
    m_notice->show();
    m_noticeTimer->start();
}



void MainWindow::Mslots_onStatisticsChanged(quint64 okFrames, quint64 rejectedFrames, quint64 droppedBytes, quint8 lastReason)
{
    QString reason;
    switch (static_cast<DeviceWork::RejectReason>(lastReason)) {
    case DeviceWork::NoReject:        reason = QStringLiteral("无");        break;
    case DeviceWork::IdMismatch:      reason = QStringLiteral("识别符不符"); break;
    case DeviceWork::LengthMismatch:  reason = QStringLiteral("帧长度不符"); break;
    case DeviceWork::ChecksumError:   reason = QStringLiteral("校验错");     break;
    case DeviceWork::RingOverflow:    reason = QStringLiteral("缓冲溢出");   break;
    }

    statusBar()->showMessage(
        QStringLiteral("有效 %1 帧 | 拒绝 %2 帧 | 丢弃 %3 字节 | 最近: %4")
            .arg(okFrames).arg(rejectedFrames).arg(droppedBytes).arg(reason));
}



void MainWindow::Mslots_onSerialError(const QString &message)
{
    showNotice(QStringLiteral("串口错误: ") + message);
}



