/**
 ******************************************************************************
 * @file    led.c
 * @author  fore
 * @version V1.0
 * @date    2026-3-6
 * @brief	led
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

#if VCS_LED

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>


/*********************
 *      DEFINES
 *********************/
#define BTN_SIZE_W lv_pct(5)
#define BTN_SIZE_H lv_pct(5)

/**********************
 *  STATIC VARIABLES
 **********************/
ui_vcs_led_t ui_vcs_led;
static bool led_press_flag;
lv_timer_t * timer_led = NULL;

static int led_init(void);
static int led_on(void);
static int led_off(void);
static void led_control(void);
static void btn_cb(lv_event_t * e);
static void timer_led_cb(lv_timer_t * tmr);

static void ui_led_func(void)
{
/* 按钮 */
    ui_vcs_led.btn = lv_btn_create(ui_vcs.obj);  // 创建按钮
    lv_obj_add_flag(ui_vcs_led.btn, LV_OBJ_FLAG_CHECKABLE);    // 设置按钮为可切换状态
    lv_obj_align_to(ui_vcs_led.btn, ui_vcs.img, LV_ALIGN_OUT_RIGHT_MID, 20, -25);   // 设置按钮位置
    lv_obj_set_size(ui_vcs_led.btn, BTN_SIZE_W, BTN_SIZE_H);   // 设置按钮大小
    lv_obj_set_style_bg_color(ui_vcs_led.btn, lv_palette_main(LV_PALETTE_NONE), LV_STATE_DEFAULT); // 设置背景颜色（本地样式）
    lv_obj_add_event_cb(ui_vcs_led.btn, btn_cb, LV_EVENT_CLICKED, NULL);   // 设置按钮事件

/* 按钮文本 */
    ui_vcs_led.label = lv_label_create(ui_vcs_led.btn);   // 创建文本
    lv_label_set_text_fmt(ui_vcs_led.label, "LED");    // 设置文本内容
    lv_obj_align(ui_vcs_led.label, LV_ALIGN_CENTER, 0, 0);   // 设置文本位置

    if(-1 == led_init())
    {
        printf("led init err\n");
        return;
    }

    timer_led = lv_timer_create(timer_led_cb, 10, 0);      // 运行周期为lvgl的10个滴答时钟(10ms)
}

void vcs_led_func(void)
{
    ui_led_func();
}

/* 初始化led */
static int led_init(void)
{
    int fd;
    if(access("/sys/class/gpio/gpio131", F_OK))    //gpio131不存在
    {
        fd = open("/sys/class/gpio/export", O_WRONLY);
        if(-1 == fd)
        {
            printf("open export err\n");
            return -1;
        }
        if(strlen("131") != write(fd, "131", strlen("131")))
        {
            printf("write export 131 err\n");
            return -1;
        }
        close(fd);
    }

    fd = open("/sys/class/gpio/gpio131/direction", O_WRONLY);   // 输出
    if(-1 == fd)
    {
        printf("open direction err\n");
        return -1;
    }
    if(strlen("out") != write(fd, "out", strlen("out")))
    {
        printf("write direction out err\n");
        return -1;
    }
    close(fd);

    fd = open("/sys/class/gpio/gpio131/active_low", O_WRONLY);   // 极性
    if(-1 == fd)
    {
        printf("open active_low err\n");
        return -1;
    }
    if(strlen("0") != write(fd, "0", strlen("0")))
    {
        printf("write active_low 0 err\n");
        return -1;
    }
    close(fd);
    return 0;
}

/* led亮 */
static int led_on(void)
{
    int fd = open("/sys/class/gpio/gpio131/value", O_WRONLY);
    if(-1 == fd)
    {
        printf("open(WR) value err\n");
        return -1;
    }
    if(strlen("0") != write(fd, "0", strlen("0")))
    {
        printf("write value 0 err\n");
        return -1;
    }
    close(fd);
    return 0;
}

/* led灭 */
static int led_off(void)
{
    int fd = open("/sys/class/gpio/gpio131/value", O_WRONLY);
    if(-1 == fd)
    {
        printf("open(WR) value err\n");
        return -1;
    }
    if(strlen("1") != write(fd, "1", strlen("1")))
    {
        printf("write value 1 err\n");
        return -1;
    }
    close(fd);
    return 0;
}

//extern pthread_mutex_t lvgl_mutex;
static void led_control(void)
{
    //pthread_mutex_lock(&lvgl_mutex);
    int fd = open("/sys/class/gpio/gpio131/value", O_RDONLY);
    if(-1 == fd)
    {
        printf("open(RD) value err\n");
        return;
    }
    char argv[2] = {0};
    if(0  >= read(fd, (char *)argv, 1))
    {
        printf("read value err\n");
        return;
    }
    if (!strcmp("0", argv)) { // LED 常亮       du qu wei liang 
        if(-1 == led_off())
        {
            printf("led off err\n");
            return;
        }
        printf("led off\n");
    }
    else if (!strcmp("1", argv)) {// LED 熄灭       du qu wei mie
        if(-1 == led_on())
        {
            printf("led on err\n");
            return;
        }
        printf("led on\n");
    }
    close(fd);
    //pthread_mutex_unlock(&lvgl_mutex);

}

/* 按钮回调函数 */
static void btn_cb(lv_event_t * e)
{
    /* led按下标志位置1 */
    led_press_flag = 1;
    sprintf(send_led, "@LEDpq");
}

/* 定时器回调函数 */
static void timer_led_cb(lv_timer_t * tmr)
{
    if(led_press_flag == 1)
    {
        led_control();
        led_press_flag = 0;
    }
}


#endif /* VCS_LED */
