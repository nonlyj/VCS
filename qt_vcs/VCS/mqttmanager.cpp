#include "mqttmanager.h"
#include <QDebug>

#define WILL_TOPIC "mqtt/will"
#define LED_TOPIC "mqtt/led"
#define TEMP_TOPIC "mqtt/temperature"
#define SERVO_TOPIC "mqtt/servo"
#define SENTRY_TOPIC "mqtt/alert"

MqttManager::MqttManager(QObject *parent) : QObject(parent), client(nullptr)
{
}

MqttManager::~MqttManager()
{
    if (client)
    {
        MQTTClient_disconnect(client, 10000);
        MQTTClient_destroy(&client);
    }
}

bool MqttManager::init(const QString &brokerAddress, const QString &clientId,
                       const QString &username, const QString &password)
{
    int rc;
    // 创建客户端
    rc = MQTTClient_create(&client, brokerAddress.toUtf8().constData(),
                           clientId.toUtf8().constData(),
                           MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        qDebug() << "Failed to create MQTT client, return code" << rc;
        return false;
    }

    // 设置回调 第二个参数传的是 this (当前对象的指针)
    rc = MQTTClient_setCallbacks(client, this, connlost, msgarrvd, NULL);

    // 配置连接选项
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 30;
    conn_opts.cleansession = 0;
    if(!username.isEmpty()) conn_opts.username = username.toUtf8().constData();
    if(!password.isEmpty()) conn_opts.password = password.toUtf8().constData();

    // 遗嘱消息
    MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
    will_opts.topicName = WILL_TOPIC;
    will_opts.message = "Unexpected disconnection";
    will_opts.retained = 1;
    will_opts.qos = 0;
    conn_opts.will = &will_opts;

    // 连接服务器
    rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        qDebug() << "Failed to connect to MQTT broker, return code" << rc;
        return false;
    }
    qDebug() << "MQTT Connected to" << brokerAddress << "successfully!";

    // 订阅主题
    MQTTClient_subscribe(client, LED_TOPIC, 0);
    MQTTClient_subscribe(client, SERVO_TOPIC, 0);

    // 发布上线消息
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = (void*)"Online";
    pubmsg.payloadlen = 6;
    pubmsg.qos = 0;
    pubmsg.retained = 1;
    MQTTClient_publishMessage(client, WILL_TOPIC, &pubmsg, NULL);

    return true;
}

// 槽函数：被别人调用时，把温度发给云端
void MqttManager::publishTemperature(const QString &temp)
{
    if (!client || !MQTTClient_isConnected(client)) return;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    QByteArray payload = temp.toUtf8();
    pubmsg.payload = payload.data();
    pubmsg.payloadlen = payload.length();
    pubmsg.qos = 0;
    pubmsg.retained = 1;

    MQTTClient_publishMessage(client, TEMP_TOPIC, &pubmsg, NULL);
}

// 槽函数：被别人调用时，把哨兵发给云端
void MqttManager::publishSentryWarning(const QString &warn)
{
    if (!client || !MQTTClient_isConnected(client)) return;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    QByteArray payload = warn.toUtf8();
    pubmsg.payload = payload.data();
    pubmsg.payloadlen = payload.length();
    pubmsg.qos = 1;
    pubmsg.retained = 1;

    MQTTClient_publishMessage(client, SENTRY_TOPIC, &pubmsg, NULL);
}

// 接收消息回调
int MqttManager::msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    // 把 context 强转回 MqttManager 对象指针
    MqttManager *mgr = static_cast<MqttManager*>(context);
    if (mgr)
    {
        QString topic = QString::fromUtf8(topicName);
        QString payload = QString::fromUtf8((char*)message->payload, message->payloadlen);

        // 调用对象的成员函数去处理
        mgr->handleMessage(topic, payload);
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1; // 返回 1 表示消息处理成功
}

// 断开连接回调
void MqttManager::connlost(void *context, char *cause)
{
    qDebug() << "MQTT Connection lost:" << (cause ? cause : "unknown");
}

void MqttManager::handleMessage(const QString &topic, const QString &payload)
{
    qDebug() << "MQTT Rx [" << topic << "]:" << payload;

    QString safeCmd = payload;
    // 只要是 @ 开头 !# 结尾的合法指令，一律作为信号发射出去
    if (safeCmd.startsWith("@") && safeCmd.endsWith("!#")) {
        emit cloudCommandReceived(safeCmd);
    }
}
