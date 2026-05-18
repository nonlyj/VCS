#include "canbusmanager.h"
#include <QDebug>

CanBusManager::CanBusManager(QObject *parent) : QObject(parent), canDevice(nullptr)
{
}

CanBusManager::~CanBusManager()
{
    if (canDevice && canDevice->state() == QCanBusDevice::ConnectedState) {
        canDevice->disconnectDevice();
    }
    delete canDevice;
}

bool CanBusManager::init(const QString &interfaceName)
{
    QString errorString;
    // 实例化底层的 SocketCAN 设备
    canDevice = QCanBus::instance()->createDevice(QStringLiteral("socketcan"), interfaceName, &errorString);

    if (!canDevice) {
        qDebug() << "Error creating CAN device:" << errorString;
        emit errorOccurred(errorString);
        return false;
    }

    // 绑定数据接收信号
    connect(canDevice, &QCanBusDevice::framesReceived, this, &CanBusManager::onFramesReceived);

    if (!canDevice->connectDevice()) {
        qDebug() << "CAN Connection error:" << canDevice->errorString();
        emit errorOccurred(canDevice->errorString());
        return false;
    }

    qDebug() << "CAN bus initialized successfully on" << interfaceName;
    return true;
}

void CanBusManager::sendCommand(const QString &cmd)
{
    if (!canDevice || canDevice->state() != QCanBusDevice::ConnectedState) return;

    quint8 hexCmd = 0x00;

    // 将旧版的长字符串指令，翻译为 1 个字节的高效 CAN 指令码
    if(cmd == "@LED!#") hexCmd = 0x01;
    else if(cmd == "@Servo_fl!#") hexCmd = 0x11;
    else if(cmd == "@Servo_fc!#") hexCmd = 0x10;
    else if(cmd == "@Servo_fr!#") hexCmd = 0x21;
    else if(cmd == "@Servo_bl!#") hexCmd = 0x31;
    else if(cmd == "@Servo_bc!#") hexCmd = 0x30;
    else if(cmd == "@Servo_br!#") hexCmd = 0x41;

    if(hexCmd == 0x00) return; // 未知指令不发送

    QByteArray payload;
    payload.append(hexCmd);

    // 构建标准数据帧，发送至 ID 0x101
    QCanBusFrame frame;
    frame.setFrameId(0x101);
    frame.setPayload(payload);
    frame.setFrameType(QCanBusFrame::DataFrame);

    canDevice->writeFrame(frame);
    qDebug() << "CAN Send CMD:" << hex << hexCmd;
}

void CanBusManager::onFramesReceived()
{
    if (!canDevice) return;

    while (canDevice->framesAvailable()) {
        QCanBusFrame frame = canDevice->readFrame();

        // 只处理 STM32 上报的数据 (ID: 0x102)
        if (frame.frameType() == QCanBusFrame::DataFrame && frame.frameId() == 0x102) {
            QByteArray data = frame.payload();
            if (data.isEmpty()) continue;

            quint8 type = data.at(0); // 提取标志位

            // 解析哨兵警报 (0xFF)
            if (type == 0xFF && data.size() >= 3) {
                quint16 dist_int = (static_cast<quint8>(data.at(1)) << 8) | static_cast<quint8>(data.at(2));
                float dist = dist_int / 10.0f;
                // 还原回旧版的字符串格式
                QString warnStr = QString("WARNING! INTRUDER DETECTED! Dist: %1 cm").arg(dist, 0, 'f', 1);
                qDebug() << "CAN Get Alert:" << warnStr;
                emit sentrywarningReceived(warnStr);
            }
            // 解析温湿度 (0xEE)
            else if (type == 0xEE && data.size() >= 3) {
                quint8 temp = data.at(1);
                quint8 humi = data.at(2);
                QString tempStr = QString("temp %1   humi %2%RH").arg(temp).arg(humi);
                qDebug() << "CAN Get Temp:" << tempStr;
                emit temperatureReceived(tempStr);
            }
        }
    }
}
