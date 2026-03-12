#ifndef VCS_H
#define VCS_H

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
/* VCS变量 */
struct ui_vcs_t
{
    lv_obj_t *obj;
    lv_obj_t *img;
};
typedef struct ui_vcs_t ui_vcs_t;
extern ui_vcs_t ui_vcs;



/**********************
 * GLOBAL PROTOTYPES
 **********************/
void vcs(void);



/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VCS_H */





