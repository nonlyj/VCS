#include "HCSR04.h"

void HCSR04_Init(void)
{
    /* 初始化 GPIO */
    RCC_APB2PeriphClockCmd(HCSR04_CLK | RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    // Trig 引脚：推挽输出，用于发送触发信号
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = HCSR04_TRIG;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    
    // Echo 引脚：下拉输入，用于接收高电平回响
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = HCSR04_ECHO;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG); // Trig默认拉低

    /* 初始化 TIM1 (高级定时器，用于 1us 级别的微秒计时) */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // 72MHz 时钟，预分频 72，即 1MHz 的频率，数一次就是 1us
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; 
    TIM_TimeBaseInitStructure.TIM_Period = 65535; // 最大可测 65535us (约11米)
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
    
    /* 初始化 Echo 引脚的外部中断 (EXTI5) */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
    EXTI_Init(&EXTI_InitStructure);

    /* 配置 NVIC 中断优先级 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// 触发一次超声波模块 (发送 >10us 的高电平脉冲)
void HCSR04_Start(void)
{
    GPIO_SetBits(HCSR04_PORT, HCSR04_TRIG);
    // 粗略延时，大约十几个微秒
    uint16_t i = 300; 
    while(i--); 
    GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);
}
