#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <QObject>
#include <QString>
#include "MQTTClient.h" // 引入 Paho MQTT C 库头文件

class MqttManager : public QObject
{
    Q_OBJECT
public:
    explicit MqttManager(QObject *parent = nullptr);
    ~MqttManager();

    // 初始化并连接 MQTT 服务器
    bool init(const QString &brokerAddress = "192.168.5.11",
              const QString &clientId = "test3",
              const QString &username = "mqtt",
              const QString &password = "123456");

signals:
    // 当收到云端发来的控制指令时，广播这个信号
    void cloudCommandReceived(const QString &cmd);

public slots:
    // 把本地的温湿度数据发布(Publish)到云端
    void publishTemperature(const QString &temp);
    // 把本地的哨兵数据发布(Publish)到云端
    void publishSentryWarning(const QString &warn);

private:
    MQTTClient client;

    // 必须加 static 关键字，并通过 context 把 MqttManager 的对象指针传进来。
    static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
    static void connlost(void *context, char *cause);

    // 真正的消息处理函数
    void handleMessage(const QString &topic, const QString &payload);
};

#endif // MQTTMANAGER_H
