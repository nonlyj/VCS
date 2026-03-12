#ifndef _VCS_TEMP_H
#define _VCS_TEMP_H

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
struct ui_vcs_temp_t
{
    lv_obj_t *label;
};
typedef struct ui_vcs_temp_t ui_vcs_temp_t;
extern ui_vcs_temp_t ui_vcs_temp;



/**********************
 * GLOBAL PROTOTYPES
 **********************/
void vcs_temp_func(void);



/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _VCS_TEMP_H */





