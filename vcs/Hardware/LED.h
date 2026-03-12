#ifndef __LED_H
#define __LED_H


#define LED_GPIO_CLK    RCC_APB2Periph_GPIOC
#define LED_GPIO_Port		GPIOC
#define LED_PIN_PIN			GPIO_Pin_13


void LED_Init(void);
void LED_ON(void);
void LED_Turn(void);
void LED_OFF(void);

#endif
