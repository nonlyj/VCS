#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "FreeRTOS.h"
#include "task.h"

/**
  * @brief  延时函数初始化 (使用 TIM4)
  * @param  无
  * @retval 无
  */
void Delay_Init(void)
{
    // 1. 开启 TIM4 时钟 (TIM4 挂载在 APB1 总线上，通常此时钟为 72MHz)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // 2. 初始化 TIM4 参数
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 自动重装载值设为最大 16-bit (65535)，意味着单次最大延时为 65.5ms
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 
    // 预分频器设为 72-1。72MHz / 72 = 1MHz。使得计数器 1us 增加 1
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // 3. 使能 TIM4
    TIM_Cmd(TIM4, ENABLE);
}

/**
  * @brief  微秒级硬件延时
  * @param  us: 要延时的微秒数 (最大 65535us)
  * @retval 无
  */
void Delay_us(uint16_t us)
{
    // 将 TIM4 的计数器清零
    TIM_SetCounter(TIM4, 0);
    // 等待计数器达到指定的微秒数
    while (TIM_GetCounter(TIM4) < us);
}

/**
  * @brief  毫秒级延时 (智能切换裸机与RTOS模式)
  * @param  ms: 要延时的毫秒数
  * @retval 无
  */
void Delay_ms(uint32_t ms)
{
    // 检查 FreeRTOS 的调度器是否已经启动
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        // 如果系统已经跑起来了，优先使用操作系统的阻塞延时，把 CPU 让给别的任务
        vTaskDelay(pdMS_TO_TICKS(ms));
    }
    else
    {
        // 如果系统还没启动（比如在 main 里的初始化阶段），使用硬件死等
        while (ms--)
        {
            Delay_us(1000);
        }
    }
}
