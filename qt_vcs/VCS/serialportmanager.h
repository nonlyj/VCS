#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QString>

class SerialPortManager : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortManager(QObject *parent = nullptr);
    ~SerialPortManager();

    // 初始化串口，参数默认指向 imx6ull 串口6
    bool init(const QString &portName = "/dev/ttymxc5");

public slots:
    // UI 层调用这个接口发送字符串
    void sendCommand(const QString &cmd);

signals:
    // 当收到温度数据时，把数据作为信号“广播”出去
    void temperatureReceived(const QString &temp);
    // 当收到哨兵时，把数据作为信号“广播”出去
    void sentrywarningReceived(const QString &temp);
    // 发送错误信号，方便 UI 提示
    void errorOccurred(const QString &errorStr);

private slots:
    // 当串口有数据到达时，触发这个槽函数
    void onReadyRead();

private:
    QSerialPort *serial;
    QByteArray readBuffer; // 用于拼接可能被截断的串口数据
};

#endif // SERIALPORTMANAGER_H
