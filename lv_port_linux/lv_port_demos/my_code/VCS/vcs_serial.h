#ifndef SERIAL_H
#define SERIAL_H

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
#define DEVICE "/dev/ttymxc5"

extern char send_led[32];
extern char send_servo[32];
extern char get_temp[32];

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 * GLOBAL PROTOTYPES
 **********************/

 
void serial_init(void);
void vcs_serial_send(void);
void vcs_serial_get(void);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SERIAL_H */





