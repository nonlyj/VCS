#ifndef __HCSR04_H
#define __HCSR04_H
#include "stm32f10x.h"

// 定义超声波模块引脚 (Trig接PA4, Echo接PA5)
#define HCSR04_PORT     GPIOA
#define HCSR04_CLK      RCC_APB2Periph_GPIOA
#define HCSR04_TRIG     GPIO_Pin_4
#define HCSR04_ECHO     GPIO_Pin_5

void HCSR04_Init(void);
void HCSR04_Start(void);

#endif
