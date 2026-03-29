#ifndef __BUZZER_H
#define __BUZZER_H

#define BUZZER_PORT     GPIOB
#define BUZZER_CLK      RCC_APB2Periph_GPIOB
#define BUZZER_PIN      GPIO_Pin_12


void Buzzer_Init(void);
void Buzzer_Sentry(uint16_t Buzzer_Time);

#endif
