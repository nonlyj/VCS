#include "stm32f10x.h"                  // Device header
#include <LED.h>

void LED_Init(void){
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruture;
	GPIO_InitStruture.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruture.GPIO_Pin=LED_PIN_PIN;
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruture);
	GPIO_SetBits(LED_GPIO_Port,LED_PIN_PIN);
}

void LED_ON(void){
	GPIO_ResetBits(LED_GPIO_Port,LED_PIN_PIN);
}

void LED_Turn(void){
	if(GPIO_ReadOutputDataBit(LED_GPIO_Port,LED_PIN_PIN)==0)
		GPIO_SetBits(LED_GPIO_Port,LED_PIN_PIN);
	else
		GPIO_ResetBits(LED_GPIO_Port,LED_PIN_PIN);
}

void LED_OFF(void){
	GPIO_SetBits(LED_GPIO_Port,LED_PIN_PIN);
}

