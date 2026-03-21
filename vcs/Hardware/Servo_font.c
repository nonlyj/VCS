#include "stm32f10x.h"                  // Device header
#include "Servo_font.h"

void Servo_Font_Init(void){
	RCC_APB1PeriphClockCmd(SERVO_FONT_TIM_CLK,ENABLE);
	RCC_APB2PeriphClockCmd(SERVO_FONT_GPIO_CLK,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=SERVO_FONT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(SERVO_FONT_GPIO_PORT,&GPIO_InitStruct);	
	
	TIM_InternalClockConfig(SERVO_FONT_TIM_PORT);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=20000 - 1;	//ARR
	TIM_TimeBaseInitStruct.TIM_Prescaler=72 - 1;	//PSC
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(SERVO_FONT_TIM_PORT,&TIM_TimeBaseInitStruct);
	
	TIM_ClearFlag(SERVO_FONT_TIM_PORT,TIM_FLAG_Update);
	TIM_ITConfig(SERVO_FONT_TIM_PORT,TIM_IT_Update,ENABLE);
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=0;	//CCR
	TIM_OC2Init(SERVO_FONT_TIM_PORT,&TIM_OCInitStruct);
	
	TIM_Cmd(SERVO_FONT_TIM_PORT,ENABLE);
}

void Servo_Font_SetCompare2(uint16_t Compare){
	TIM_SetCompare2(SERVO_FONT_TIM_PORT,Compare);
}

/*
		CCR					rad
		500 	 -> 	0бу
		1500	 -> 	90бу
		2500	 -> 	180бу	
*/
void Servo_Font_SetAngle(float Angle){
	Servo_Font_SetCompare2(Angle/180*2000+500);
}
