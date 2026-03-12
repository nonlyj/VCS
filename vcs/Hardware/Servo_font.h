#ifndef __SERVO_FONT_H
#define __SERVO_FONT_H

/* 改了 SERVO_FONT_GPIO_PIN 需修改 PWM_SetCompareX */
#define SERVO_FONT_TIM_CLK    RCC_APB1Periph_TIM2
#define SERVO_FONT_TIM_PORT   TIM2
#define SERVO_FONT_GPIO_CLK   RCC_APB2Periph_GPIOA
#define SERVO_FONT_GPIO_PORT  GPIOA
#define SERVO_FONT_GPIO_PIN   GPIO_Pin_1


void Servo_Font_Init(void);
void Servo_Font_SetAngle(float Angle);

#endif
