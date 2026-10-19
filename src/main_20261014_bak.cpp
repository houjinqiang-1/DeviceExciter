#include "ui/mainwindow.h"

#include "domain/Definitions.h"
#include "domain/RuntimeModels.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DeviceDefinition device;
    device.id = QStringLiteral("demo");
    device.name = QStringLiteral("Demo Device");

    FrameDefinition txFrame;
    txFrame.id = QStringLiteral("tx1");
    txFrame.direction = FrameDirection::Tx;
    txFrame.frameLength = 8;

    FieldDefinition field;
    field.id = QStringLiteral("f1");
    field.type = FieldType::UInt8;
    field.byteLength = 1;
    txFrame.fields.append(field);
    device.txFrames.append(txFrame);

    FieldValue value;
    value.fieldId = field.id;
    value.inputValue = 1;

    SerialPortConfig portConfig;
    portConfig.portName = QStringLiteral("COM1");

    PortRuntimeState portState;
    SendTask task;
    task.deviceId = device.id;
    task.frameId = txFrame.id;

    ParseResult parseResult;
    MessageRecord message;
    message.rawBytes = QByteArray::fromHex("0102");

    Q_UNUSED(device)
    Q_UNUSED(value)
    Q_UNUSED(portConfig)
    Q_UNUSED(portState)
    Q_UNUSED(task)
    Q_UNUSED(parseResult)
    Q_UNUSED(message)

    MainWindow window;
    window.show();
    return app.exec();
}
