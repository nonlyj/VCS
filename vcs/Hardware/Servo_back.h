#ifndef __SERVO_BACK_H
#define __SERVO_BACK_H

/* 改了 SERVO_BACK_GPIO_PIN 需修改 PWM_SetCompareX */
#define SERVO_BACK_TIM_CLK    RCC_APB1Periph_TIM3
#define SERVO_BACK_TIM_PORT   TIM3
#define SERVO_BACK_GPIO_CLK   RCC_APB2Periph_GPIOA
#define SERVO_BACK_GPIO_PORT  GPIOA
#define SERVO_BACK_GPIO_PIN   GPIO_Pin_7


void Servo_Back_Init(void);
void Servo_Back_SetAngle(float Angle);

#endif
