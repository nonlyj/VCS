/**
 ******************************************************************************
 * @file    vsc.c
 * @author  fore
 * @version V1.0
 * @date    2026-3-6
 * @brief	vsc 
 ******************************************************************************
 * Change Logs:
 * Date           Author          Notes
 * 2026-3-6     fore     First version
 ******************************************************************************
 */


/*********************
 *      INCLUDES
 *********************/
#include "../../lv_port_demos.h"

#if VCS_VCS

#include "vcs.h"
#include <pthread.h>
#include <stdio.h>


/*********************
 *      DEFINES
 *********************/
//extern pthread_mutex_t lvgl_mutex;

/**********************
 *  STATIC VARIABLES
 **********************/
static void *mqtt_thread_func(void *argv);
static void *uartsend_thread_func(void *argv);
static void *uartget_thread_func(void *argv);

ui_vcs_t ui_vcs;
void ui_vcs_func(void)
{
    ui_vcs.obj = lv_obj_create(NULL);   // 创建对象
    lv_obj_align(ui_vcs.obj, LV_ALIGN_CENTER, 0, 0);   // 设置对象位置

    LV_IMAGE_DECLARE(car);  // 声明图片
    ui_vcs.img = lv_image_create(ui_vcs.obj);  // 创建图片
    lv_image_set_src(ui_vcs.img, &car);  // 设置图片源
    lv_obj_center(ui_vcs.img);   // 设置图片位置

    lv_scr_load(ui_vcs.obj);
}


void vcs(void)
{
    ui_vcs_func();

    serial_init();
    vcs_led_func();
    vcs_servo_func();
    vcs_temp_func();

    /* 创建线程 mqtt处理 */
    pthread_t tid_mqtt;
    pthread_create(&tid_mqtt, NULL, mqtt_thread_func, NULL);
    // 将线程分离，让它自己在后台跑，不需要我们去 join 回收它
    pthread_detach(tid_mqtt);

    /* 创建线程UART Send处理 */
    pthread_t tid_uart_send_code;
    pthread_create(&tid_uart_send_code, NULL, uartsend_thread_func, NULL);
    pthread_detach(tid_uart_send_code);

    pthread_t tid_uart_get_code;
    pthread_create(&tid_uart_get_code, NULL, uartget_thread_func, NULL);
    pthread_detach(tid_uart_get_code);

}

/* mqtt处理 */
 static void *mqtt_thread_func(void *argv)
 {
    printf("MQTT 后台线程启动...\n");
    my_mqtt(); // 运行那个带有死循环的 MQTT 客户端代码
    return NULL;
 }

/* UART与STM32通信 */
static void *uartsend_thread_func(void *argv)
{
    printf("UART Send 后台线程启动...\n");
    vcs_serial_send();
    return NULL;
}
static void *uartget_thread_func(void *argv)
{
    printf("UART Get 后台线程启动...\n");
    vcs_serial_get();
    return NULL;
}


#endif /* VSC_VSC */
