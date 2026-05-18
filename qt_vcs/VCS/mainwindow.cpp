#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , fl_state(false), fr_state(false), bl_state(false), br_state(false), led_state(false)
{
    ui->setupUi(this);

    // 实例化串口管理器
//    serialManager = new SerialPortManager(this);
    // 实例化 CAN 管理器
    canManager = new CanBusManager(this);
    // 实例化 MQTT 管理器并连接服务器
    mqttManager = new MqttManager(this);
    mqttManager->init("tcp://192.168.5.11:1883", "qt_car_888", "", ""); // 默认连 192.168.5.11

//    video_process = new QProcess(this);

//    videoTimer = new QTimer(this);
//    videoTimer->setSingleShot(true); // 必须设为 true，表示只触发一次，不循环
//    connect(videoTimer, &QTimer::timeout, this, &MainWindow::stopCamera);

    // CAN 收到温度数据的信号，到更新 UI Label 的槽函数
    connect(canManager, &CanBusManager::temperatureReceived,
            this, &MainWindow::handleTemperatureUpdate);

    // CAN 收到哨兵数据的信号，到更新 UI Label 的槽函数
    connect(canManager, &CanBusManager::sentrywarningReceived,
            this, &MainWindow::handleSentryWarningUpdate);

    // MQTT 收到云端的合法命令信号，到CAN的发送槽函数！
    connect(mqttManager, &MqttManager::cloudCommandReceived,
            canManager, &CanBusManager::sendCommand);

    // CAN收到 STM32 的温度信号
    connect(canManager, &CanBusManager::temperatureReceived,
            mqttManager, &MqttManager::publishTemperature);

    // 初始化串口 (连接开发板上的 ttymxc5)
//    if (!serialManager->init("/dev/ttymxc5"))
//    {
//        ui->temp->setText("Port Error");
//    }
//    else
//    {
//        ui->temp->setText("Port Ready");
//    }

    // 初始化 CAN 总线 (连接开发板上的 can0)
    if (!canManager->init("can0"))
    {
        ui->temp->setText("CAN Error");
    }
    else
    {
        ui->temp->setText("CAN Ready");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 温度更新槽函数
void MainWindow::handleTemperatureUpdate(const QString &tempData)
{
    ui->temp->setText(tempData);
}

// 哨兵更新槽函数
void MainWindow::handleSentryWarningUpdate(const QString &warnData)
{
    // MQTT 云端推送
    mqttManager->publishSentryWarning(warnData);

    // 开启摄像头
//    if (video_process->state() == QProcess::NotRunning) {
//        QStringList args;
//        args.append("/dev/video1");
//        video_process->start("./video2lcd", args);
//        qDebug() << "Sentry Alert: video started";
//    }

    // 喂狗 重置定时器为 5000 毫秒（5秒）
//    videoTimer->start(5000);
}

//void MainWindow::stopCamera()
//{
//    qDebug() << "No alert for 5s";

//     杀死摄像头应用进程
//    if (video_process->state() != QProcess::NotRunning) {
//        video_process->kill();
//        video_process->waitForFinished(1000); // 等待进程彻底被回收，避免僵尸进程
//    }

//    this->repaint();
//}

void MainWindow::on_bt_fl_clicked()
{
    fl_state = !fl_state; // 翻转状态
    if (fl_state) {
        canManager->sendCommand("@Servo_fl!#"); // 开门指令
    }
    else {
        canManager->sendCommand("@Servo_fc!#"); // 关门指令
    }
}

void MainWindow::on_bt_fr_clicked()
{
    fr_state = !fr_state;
    if (fr_state) {
        canManager->sendCommand("@Servo_fr!#");
    }
    else {
        canManager->sendCommand("@Servo_fc!#"); // 前门关闭指令
    }
}

void MainWindow::on_bt_bl_clicked()
{
    bl_state = !bl_state;
    if (bl_state) {
        canManager->sendCommand("@Servo_bl!#");
    }
    else {
        canManager->sendCommand("@Servo_bc!#"); // 后门关闭指令
    }
}

void MainWindow::on_bt_br_clicked()
{
    br_state = !br_state;
    if (br_state) {
        canManager->sendCommand("@Servo_br!#");
    }
    else {
        canManager->sendCommand("@Servo_bc!#");
    }
}

void MainWindow::on_bt_led_clicked()
{
     canManager->sendCommand("@LED!#");
}
