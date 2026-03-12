/**
 ******************************************************************************
 * @file    temp.c
 * @author  fore
 * @version V1.0
 * @date    2026-3-7
 * @brief	temp
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

#if VCS_TEMP

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

ui_vcs_temp_t ui_vcs_temp;
static bool temp_press_flag;
lv_timer_t * timer_temp = NULL;

static void timer_temp_cb(lv_timer_t * tmr);

static void ui_temp_func(void)
{
/* 文本 */
    ui_vcs_temp.label = lv_label_create(ui_vcs.obj);   // 创建文本
    lv_label_set_text(ui_vcs_temp.label, "temp 0	humi 0%%RH\n");    // 设置文本内容
    lv_obj_align(ui_vcs_temp.label, LV_ALIGN_TOP_RIGHT, -50, 50);   // 设置文本位置
    lv_obj_set_style_text_color(ui_vcs_temp.label,  lv_palette_main(LV_PALETTE_NONE), LV_STATE_DEFAULT); // 设置文本颜色（本地样式）
    lv_obj_set_style_text_font(ui_vcs_temp.label, &lv_font_montserrat_28, LV_PART_MAIN); // 设置文本大小（本地样式）

    timer_temp = lv_timer_create(timer_temp_cb, 10, 0);      // 运行周期为lvgl的10个滴答时钟    10ms
}

void vcs_temp_func(void)
{
    ui_temp_func();
}


/* 定时器回调函数 */
static void timer_temp_cb(lv_timer_t * tmr)
{
    lv_label_set_text(ui_vcs_temp.label, get_temp);    // 设置文本内容
}


#endif /* VCS_TEMP */
