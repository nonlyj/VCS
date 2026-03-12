/**
 * @file lv_port_demos.h
 *
 */

#ifndef lv_port_demos_H
#define lv_port_demos_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../lvgl/lvgl.h"
#endif

#if defined(LV_100ASK_DEMO_CONF_PATH)
#define __LV_TO_STR_AUX(x) #x
#define __LV_TO_STR(x) __LV_TO_STR_AUX(x)
#include __LV_TO_STR(LV_100ASK_DEMO_CONF_PATH)
#undef __LV_TO_STR_AUX
#undef __LV_TO_STR
#elif defined(LV_100ASK_DEMO_CONF_INCLUDE_SIMPLE)
#include "lv_port_demos_conf.h"
#else
#include "../lv_port_demos_conf.h"
#endif


//#include "my_code/1.test/my_test.h"

/* my mfd h */
// #include "my_code/MFD/mfd.h"
// #include "my_code/MFD/password.h"
// #include "my_code/MFD/menue.h"
// #include "my_code/MFD/calculate.h"
// #include "my_code/MFD/time_show.h"
// #include "my_code/MFD/led.h"

/* my vcs h */
#include "my_code/VCS/vcs.h"
#include "my_code/VCS/vcs_led.h"
#include "my_code/VCS/vcs_mqtt.h"
#include "my_code/VCS/vcs_serial.h"
#include "my_code/VCS/vcs_servo.h"
#include "my_code/VCS/vcs_temp.h"

/*********************
 *      DEFINES
 *********************/
/*Test  lvgl version*/
#if LV_VERSION_CHECK(9, 1, 0) == 0
#warning "lv_port_demos: Wrong lvgl version"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*lv_port_demos_H*/
