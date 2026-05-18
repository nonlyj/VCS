#ifndef CANBUSMANAGER_H
#define CANBUSMANAGER_H

#include <QObject>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>
#include <QString>

class CanBusManager : public QObject
{
    Q_OBJECT

public:
    explicit CanBusManager(QObject *parent = nullptr);
    ~CanBusManager();

    // 初始化 CAN 设备，默认挂载在 Linux 的 can0 节点
    bool init(const QString &interfaceName = "can0");

public slots:
    // 保持与旧版 UART 相同的接口，内部自动转换为 CAN 报文
    void sendCommand(const QString &cmd);

signals:
    // 保持与旧版完全相同的信号，UI 和 MQTT 依然能无缝接收
    void temperatureReceived(const QString &temp);
    void sentrywarningReceived(const QString &warn);
    void errorOccurred(const QString &errorStr);

private slots:
    // CAN 数据到达槽函数
    void onFramesReceived();

private:
    QCanBusDevice *canDevice;
};

#endif // CANBUSMANAGER_H
