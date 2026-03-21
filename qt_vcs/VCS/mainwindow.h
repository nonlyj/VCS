#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialportmanager.h" // 引入串口管理类
#include "mqttmanager.h"
#include <QProcess>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_bt_fl_clicked();
    void on_bt_fr_clicked();
    void on_bt_bl_clicked();
    void on_bt_br_clicked();
    void on_bt_led_clicked();
    void handleTemperatureUpdate(const QString &tempData);
    void handleSentryWarningUpdate(const QString &warnData);
    void stopCamera();

private:
    Ui::MainWindow *ui;
    SerialPortManager *serialManager; // 串口管理对象指针
    MqttManager *mqttManager;
    QProcess *video_process;
    QTimer *videoTimer;

    // 记录四个车门和 LED 的状态 (false = 关/灭, true = 开/亮)
    bool fl_state;
    bool fr_state;
    bool bl_state;
    bool br_state;
    bool led_state;
};
#endif // MAINWINDOW_H
