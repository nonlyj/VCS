#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Buzzer.h"


//蜂鸣器初始化
void Buzzer_Init(void){
	RCC_APB2PeriphClockCmd(BUZZER_CLK,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=BUZZER_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(BUZZER_PORT,&GPIO_InitStructure);	
	GPIO_SetBits(BUZZER_PORT,BUZZER_PIN);
}
//蜂鸣器响
void Buzzer_On(void){
	GPIO_ResetBits(BUZZER_PORT,BUZZER_PIN);
}
//蜂鸣器不响
void Buzzer_Off(void){
	GPIO_SetBits(BUZZER_PORT,BUZZER_PIN);
}

void Buzzer_Sentry(uint16_t Buzzer_Time){
	Buzzer_On();
	Delay_ms(Buzzer_Time);
	Buzzer_Off();
	Delay_ms(Buzzer_Time);
	Buzzer_On();
	Delay_ms(Buzzer_Time);
	Buzzer_Off();
}

