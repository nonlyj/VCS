#ifndef _VCS_SERVO_H
#define _VCS_SERVO_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "../lv_port_demos.h"


/*********************
 *      DEFINES
 *********************/

 
/**********************
 *      TYPEDEFS
 **********************/
/* SERVO界面变量 */
struct ui_vcs_servo_t
{
    lv_obj_t *label_fl;
    lv_obj_t *label_bl;
    lv_obj_t *label_fr;
    lv_obj_t *label_br;
    lv_obj_t *btn_fl;
    lv_obj_t *btn_bl;
    lv_obj_t *btn_fr;
    lv_obj_t *btn_br;
};
typedef struct ui_vcs_servo_t ui_vcs_servo_t;
extern ui_vcs_servo_t ui_vcs_servo;



/**********************
 * GLOBAL PROTOTYPES
 **********************/
void vcs_servo_func(void);



/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _VCS_SERVO_H */





