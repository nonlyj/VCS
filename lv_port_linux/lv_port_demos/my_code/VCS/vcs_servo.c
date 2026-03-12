/**
 ******************************************************************************
 * @file    servo.c
 * @author  fore
 * @version V1.0
 * @date    2026-3-7
 * @brief	servo
 ******************************************************************************
 * Change Logs:
 * Date           Author          Notes
 * 2026-3-7     fore     First version
 ******************************************************************************
 */


/*********************
 *      INCLUDES
 *********************/
#include "../../lv_port_demos.h"

#if VCS_SERVO

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
#define BTN_SIZE_W lv_pct(10)
#define BTN_SIZE_H lv_pct(10)

/**********************
 *  STATIC VARIABLES
 **********************/
ui_vcs_servo_t ui_vcs_servo;
//static int servo_press_flag;    // 1 is L1, 2 is L2, 3 is R1, 4 is R2
lv_timer_t * timer_servo = NULL;

static void btn_cb(lv_event_t * e);

static void ui_servo_func(void)
{
/* 按钮 */
    ui_vcs_servo.btn_fl = lv_btn_create(ui_vcs.obj);  // 创建按钮
    lv_obj_add_flag(ui_vcs_servo.btn_fl, LV_OBJ_FLAG_CHECKABLE);    // 设置按钮为可切换状态
    lv_obj_align_to(ui_vcs_servo.btn_fl, ui_vcs.img, LV_ALIGN_OUT_TOP_MID, -10, -30);   // 设置按钮位置
    lv_obj_set_size(ui_vcs_servo.btn_fl, BTN_SIZE_W, BTN_SIZE_H);   // 设置按钮大小
    lv_obj_set_style_bg_color(ui_vcs_servo.btn_fl, lv_palette_main(LV_PALETTE_NONE), LV_STATE_DEFAULT); // 设置背景颜色（本地样式）
    lv_obj_add_event_cb(ui_vcs_servo.btn_fl, btn_cb, LV_EVENT_CLICKED, NULL);   // 设置按钮事件

    ui_vcs_servo.btn_bl = lv_btn_create(ui_vcs.obj);  // 创建按钮
    lv_obj_add_flag(ui_vcs_servo.btn_bl, LV_OBJ_FLAG_CHECKABLE);    // 设置按钮为可切换状态
    lv_obj_align_to(ui_vcs_servo.btn_bl, ui_vcs.img, LV_ALIGN_OUT_TOP_MID, -130, -30);   // 设置按钮位置
    lv_obj_set_size(ui_vcs_servo.btn_bl, BTN_SIZE_W, BTN_SIZE_H);   // 设置按钮大小
    lv_obj_set_style_bg_color(ui_vcs_servo.btn_bl, lv_palette_main(LV_PALETTE_NONE), LV_STATE_DEFAULT); // 设置背景颜色（本地样式）
    lv_obj_add_event_cb(ui_vcs_servo.btn_bl, btn_cb, LV_EVENT_CLICKED, NULL);   // 设置按钮事件

    ui_vcs_servo.btn_fr = lv_btn_create(ui_vcs.obj);  // 创建按钮
    lv_obj_add_flag(ui_vcs_servo.btn_fr, LV_OBJ_FLAG_CHECKABLE);    // 设置按钮为可切换状态
    lv_obj_align_to(ui_vcs_servo.btn_fr, ui_vcs.img, LV_ALIGN_OUT_BOTTOM_MID, 0, -20);   // 设置按钮位置
    lv_obj_set_size(ui_vcs_servo.btn_fr, BTN_SIZE_W, BTN_SIZE_H);   // 设置按钮大小
    lv_obj_set_style_bg_color(ui_vcs_servo.btn_fr, lv_palette_main(LV_PALETTE_NONE), LV_STATE_DEFAULT); // 设置背景颜色（本地样式）
    lv_obj_add_event_cb(ui_vcs_servo.btn_fr, btn_cb, LV_EVENT_CLICKED, NULL);   // 设置按钮事件

    ui_vcs_servo.btn_br = lv_btn_create(ui_vcs.obj);  // 创建按钮
    lv_obj_add_flag(ui_vcs_servo.btn_br, LV_OBJ_FLAG_CHECKABLE);    // 设置按钮为可切换状态
    lv_obj_align_to(ui_vcs_servo.btn_br, ui_vcs.img, LV_ALIGN_OUT_BOTTOM_MID, -130, -20);   // 设置按钮位置
    lv_obj_set_size(ui_vcs_servo.btn_br, BTN_SIZE_W, BTN_SIZE_H);   // 设置按钮大小
    lv_obj_set_style_bg_color(ui_vcs_servo.btn_br, lv_palette_main(LV_PALETTE_NONE), LV_STATE_DEFAULT); // 设置背景颜色（本地样式）
    lv_obj_add_event_cb(ui_vcs_servo.btn_br, btn_cb, LV_EVENT_CLICKED, NULL);   // 设置按钮事件

/* 按钮文本 */
    ui_vcs_servo.label_fl = lv_label_create(ui_vcs_servo.btn_fl);   // 创建文本
    lv_label_set_text_fmt(ui_vcs_servo.label_fl, "FL");    // 设置文本内容
    lv_obj_align(ui_vcs_servo.label_fl, LV_ALIGN_CENTER, 0, 0);   // 设置文本位置

    ui_vcs_servo.label_bl = lv_label_create(ui_vcs_servo.btn_bl);   // 创建文本
    lv_label_set_text_fmt(ui_vcs_servo.label_bl, "BL");    // 设置文本内容
    lv_obj_align(ui_vcs_servo.label_bl, LV_ALIGN_CENTER, 0, 0);   // 设置文本位置

    ui_vcs_servo.label_fr = lv_label_create(ui_vcs_servo.btn_fr);   // 创建文本
    lv_label_set_text_fmt(ui_vcs_servo.label_fr, "FR");    // 设置文本内容
    lv_obj_align(ui_vcs_servo.label_fr, LV_ALIGN_CENTER, 0, 0);   // 设置文本位置

    ui_vcs_servo.label_br = lv_label_create(ui_vcs_servo.btn_br);   // 创建文本
    lv_label_set_text_fmt(ui_vcs_servo.label_br, "BR");    // 设置文本内容
    lv_obj_align(ui_vcs_servo.label_br, LV_ALIGN_CENTER, 0, 0);   // 设置文本位置

}

void vcs_servo_func(void)
{
    ui_servo_func();
}

/* 按钮回调函数 */
static void btn_cb(lv_event_t * e)
{
    lv_obj_t *target = lv_event_get_target(e);  // 获取触发事件的部件(对象)
    if(ui_vcs_servo.btn_fl == target)   // L1   font_left
    {
        static uint8_t fl_flag;
        fl_flag = !fl_flag;
        if(fl_flag) // 开门
        {
            sprintf(send_servo, "@Servo_flpq");
        }
        else    // 关门
        {
            sprintf(send_servo, "@Servo_fcpq");
        }
    }
    else if(ui_vcs_servo.btn_bl == target)   // L2  back_left
    {
        static uint8_t bl_flag;
        bl_flag = !bl_flag;
        if(bl_flag) // 开门
        {
            sprintf(send_servo, "@Servo_blpq");
        }
        else    // 关门
        {
            sprintf(send_servo, "@Servo_bcpq");
        }
    }
    else if(ui_vcs_servo.btn_fr == target)   // R1  font_right
    {
        static uint8_t fr_flag;
        fr_flag = !fr_flag;
        if(fr_flag) // 开门
        {
            sprintf(send_servo, "@Servo_frpq");
        }
        else    // 关门
        {
            sprintf(send_servo, "@Servo_fcpq");
        }
    }
    else if(ui_vcs_servo.btn_br == target)   // R2  back_right
    {
        static uint8_t br_flag;
        br_flag = !br_flag;
        if(br_flag) // 开门
        {
            sprintf(send_servo, "@Servo_brpq");
        }
        else    // 关门
        {
            sprintf(send_servo, "@Servo_bcpq");
        }
    }
}


#endif /* VCS_LED */
