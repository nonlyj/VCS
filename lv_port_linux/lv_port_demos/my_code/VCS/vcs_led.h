#ifndef _VCS_LED_H
#define _VCS_LED_H

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
/* LED界面变量 */
struct ui_vcs_led_t
{
    lv_obj_t *label;
    lv_obj_t *btn;
};
typedef struct ui_vcs_led_t ui_vcs_led_t;
extern ui_vcs_led_t ui_vcs_led;



/**********************
 * GLOBAL PROTOTYPES
 **********************/
void vcs_led_func(void);



/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _VCS_LED_H */





