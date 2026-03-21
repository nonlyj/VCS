#include "serialportmanager.h"
#include <QDebug>


SerialPortManager::SerialPortManager(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
}

SerialPortManager::~SerialPortManager()
{
    if (serial->isOpen())
    {
        serial->close();
    }
}

bool SerialPortManager::init(const QString &portName)
{
    serial->setPortName(portName);

    // 对应你之前 C 代码里的 set_opt(115200, 8, 'N', 1)
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);

    // 关键：关闭硬件流控 (对应 C 代码里的 newtio.c_cflag &= ~CRTSCTS)
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadWrite))
    {
        QString err = QString("Failed to open port %1, error: %2").arg(portName).arg(serial->errorString());
        qDebug() << err;
        emit errorOccurred(err);
        return false;
    }

    // 绑定数据接收信号
    connect(serial, &QSerialPort::readyRead, this, &SerialPortManager::onReadyRead);

    qDebug() << "Serial port initialized successfully on" << portName;
    return true;
}

void SerialPortManager::sendCommand(const QString &cmd)
{
    if (serial->isOpen() && !cmd.isEmpty())
    {
        serial->write(cmd.toUtf8());

        serial->flush();
        serial->waitForBytesWritten(10);
        qDebug() << "UART Send:" << cmd;
    }
    else
    {
         qDebug() << "Cannot send, port not open.";
    }
}

void SerialPortManager::onReadyRead()
{
    // 追加新收到的数据
    readBuffer.append(serial->readAll());

    // 防止数据包被截断
    while (readBuffer.contains('\n'))
    {
        int newlineIndex = readBuffer.indexOf('\n');
        // 提取一行完整的数据
        QByteArray completeData = readBuffer.left(newlineIndex);
        // 从缓冲区移除已处理的数据（包含换行符）
        readBuffer.remove(0, newlineIndex + 1);

        QString Str = QString::fromUtf8(completeData).trimmed();

        if (!Str.isEmpty())
        {
            if(Str.contains("temp"))    // 温湿度
            {
                qDebug() << "UART Get Temp:" << Str;
                emit temperatureReceived(Str);
            }
            else    // 哨兵
            {
                qDebug() << "UART Get Alert:" << Str;
                emit sentrywarningReceived(Str);
            }
        }
    }
}
