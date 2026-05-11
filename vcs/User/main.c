#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "FreeRTOS.H"
#include "task.h"
#include "Serial.h" 
#include "string.h" 
#include "Delay.h"
#include "dht11.h"
#include "semphr.h"
#include "Servo_font.h"
#include "Servo_back.h"
#include "queue.h"
#include "event_groups.h"
#include "HCSR04.h"
#include "Buzzer.h"
#include "pid.h"
#include "MyCAN.h"

#define UART_LED								"LED"
#define UART_SERVO_FONT_LEFT		"Servo_fl"
#define UART_SERVO_FONT_RIGHT		"Servo_fr"
#define UART_SERVO_FONT_CLOSE		"Servo_fc"
#define UART_SERVO_BACK_LEFT		"Servo_bl"
#define UART_SERVO_BACK_RIGHT		"Servo_br"
#define UART_SERVO_BACK_CLOSE		"Servo_bc"

//#define EVENT_LED								1 << 0
#define EVENT_SERVO_FONT_LEFT		1 << 1
#define EVENT_SERVO_FONT_RIGHT	1 << 2
#define EVENT_SERVO_FONT_CLOSE	1 << 3
#define EVENT_SERVO_BACK_LEFT		1 << 4
#define EVENT_SERVO_BACK_RIGHT	1 << 5
#define EVENT_SERVO_BACK_CLOSE	1 << 6


TaskHandle_t Task_led_flash_handle;			// LED任务句柄
TaskHandle_t Task_dht11_handle;					// DHT11任务句柄
//TaskHandle_t Task_uart_handle;					// uart任务句柄
TaskHandle_t Task_can_handle;					// can任务句柄
TaskHandle_t Task_Sentry_handle;      	// 哨兵任务句柄
TaskHandle_t Task_Servo_Smooth_handle;  // PID任务句柄

QueueHandle_t CAN_Queue;	// 队列，CAN中断中发送数据
EventGroupHandle_t	LED_Event;	// 事件组，四车门均关闭时led闪烁
SemaphoreHandle_t Serial_Mutex;	// 互斥量，保护串口资源
SemaphoreHandle_t Servo_Font_Binary; // 同步量，保护串口资源

static u8 temp;	// 温湿度参数
static u8 humi;
volatile float Sentry_Distance = 0.0; // 记录最新距离 (cm)

// 实例化两个车门的 PID 控制器
PID_Controller Font_Door_PID = {0.15f, 0.08f, 90.0f, 90.0f, 0.0f, 3.0f};
PID_Controller Back_Door_PID = {0.15f, 0.08f, 90.0f, 90.0f, 0.0f, 3.0f};


/* 处理uart收到的信息 */
//static void Task_uart(void *arg)
//{
//	char buffer[100];
//	while(1)
//	{
//		if(pdPASS == xQueueReceive(CAN_Queue, buffer, portMAX_DELAY))
//		{
//			if(!strcmp(buffer, UART_LED))	// LED
//			{
//				LED_Turn();
//			}
//			else if(!strcmp(buffer, UART_SERVO_FONT_LEFT))	// SERVO_FONT_LEFT
//			{
//				xEventGroupClearBits(LED_Event, EVENT_SERVO_FONT_CLOSE);
//				Font_Door_PID.target = 180.0f;
//			}
//			else if(!strcmp(buffer, UART_SERVO_FONT_RIGHT))	// SERVO_FONT_RIGHT
//			{
//				xEventGroupClearBits(LED_Event, EVENT_SERVO_FONT_CLOSE);
//				Font_Door_PID.target = 0.0f;
//			}
//			else if(!strcmp(buffer, UART_SERVO_FONT_CLOSE))	// SERVO_FONT_CLOSE
//			{
//				xEventGroupSetBits(LED_Event, EVENT_SERVO_FONT_CLOSE);
//				Font_Door_PID.target = 90.0f;
//			}
//			else if(!strcmp(buffer, UART_SERVO_BACK_LEFT))	// SERVO_BACK_LEFT
//			{
//				xEventGroupClearBits(LED_Event, EVENT_SERVO_BACK_CLOSE);
//				Back_Door_PID.target = 180.0f;
//			}
//			else if(!strcmp(buffer, UART_SERVO_BACK_RIGHT))	// SERVO_BACK_RIGHT
//			{
//				xEventGroupClearBits(LED_Event, EVENT_SERVO_BACK_CLOSE);
//				Back_Door_PID.target = 0.0f;
//			}
//			else if(!strcmp(buffer, UART_SERVO_BACK_CLOSE))	// SERVO_BACK_CLOSE
//			{
//				xEventGroupSetBits(LED_Event, EVENT_SERVO_BACK_CLOSE);
//				Back_Door_PID.target = 90.0f;
//			}
//			else
//			{
//				xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
//				Serial_Printf("Unknown %s\r\n",buffer);
//				xSemaphoreGive(Serial_Mutex);
//			}
//		}
//		else
//		{
//				xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
//				Serial_Printf("xQueueReceive failed\r\n");
//				xSemaphoreGive(Serial_Mutex);
//		}
//		vTaskDelay(10);	// 10ms
//	}
//}

/* CAN */
static void Task_CAN_Rx(void *arg)
{
    CAN_Msg_t rx_msg;
    while(1)
    {
        // 阻塞等待 CAN 队列的数据
        if(pdPASS == xQueueReceive(CAN_Queue, &rx_msg, portMAX_DELAY))
        {
            // Data[0] 存放指令码
            switch(rx_msg.Data[0]) 
            {
                case CAN_CMD_LED:
                    LED_Turn();
                    break;
                case CAN_CMD_SERVO_FL:
                    xEventGroupClearBits(LED_Event, EVENT_SERVO_FONT_CLOSE);
                    Font_Door_PID.target = 180.0f;
                    break;
                case CAN_CMD_SERVO_FC:
                    xEventGroupSetBits(LED_Event, EVENT_SERVO_FONT_CLOSE);
                    Font_Door_PID.target = 90.0f;
                    break;
                case CAN_CMD_SERVO_FR:
                    xEventGroupClearBits(LED_Event, EVENT_SERVO_FONT_CLOSE);
                    Font_Door_PID.target = 0.0f;
                    break;								
                default:
                    break;
            }
        }
    }
}

/* Dht11 */
static void Task_dht11(void *arg)
{
	vTaskDelay(1000);
	while(DHT11_Init())
	{
		xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
		Serial_Printf("DHT11 Error \r\n");
		xSemaphoreGive(Serial_Mutex);
		
		vTaskDelay(1000);
	}		
	while (1)
	{
		DHT11_Read_Data(&temp,&humi);	// 获取温湿度的值
		
		xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
		Serial_Printf("temp %d   humi %d%%RH\n",temp,humi);
		xSemaphoreGive(Serial_Mutex);
		
		vTaskDelay(1000);
	}
}

/* 四车门均关闭时led闪烁 */
static void Task_led_flash(void *arg)
{
	while(1)
	{
		xEventGroupWaitBits(LED_Event, EVENT_SERVO_FONT_CLOSE | EVENT_SERVO_BACK_CLOSE, pdTRUE, pdTRUE, portMAX_DELAY);
		LED_Flash();
	}
}

/* 哨兵任务 */
static void Task_Sentry(void *arg)
{
	while(1)
	{
		// 发送触发脉冲，探测一次前方距离
		HCSR04_Start();

		// 任务进入阻塞态等待警报。设置 100ms 超时（即没警报时1秒测10次）
		// 如果没有收到危险通知，任务休眠不占 CPU；收到通知立刻执行！
		if(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100)) > 0)
		{
			// 有人进入了 3cm 以内的防撞警戒线
			xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
			Serial_Printf("WARNING! INTRUDER DETECTED! Dist: %.1f cm\r\n", Sentry_Distance);
			xSemaphoreGive(Serial_Mutex);

			/* 蜂鸣器鸣叫，强制关门 */
			Buzzer_Sentry(100);
			Font_Door_PID.target = 90.0f; 
			Back_Door_PID.target = 90.0f;

			// 报警后强制延时 500ms，防止串口一直被疯狂刷屏
			vTaskDelay(500); 	// pdMS_TO_TICKS(500) 500ms
		}
	}
}

/* 前车门 PID 驱动任务 */
static void Task_Servo_Smooth(void *arg)
{
	while(1)
	{
		
		// 1. 分别计算前后门当前的平滑过渡角度
		float font_next_angle = PID_Compute_Step(&Font_Door_PID);
		float back_next_angle = PID_Compute_Step(&Back_Door_PID);

		// 2. 将平滑变化的角度真正输出给底层 PWM
		// （前提是你的 Servo_Font_SetAngle 里面是通过修改 TIM_SetCompare 来更新占空比的）
		Servo_Font_SetAngle(font_next_angle);
		Servo_Back_SetAngle(back_next_angle);

		// 3. 阻塞延时 20ms (即 50Hz 控制周期，正好与舵机 PWM 周期完美契合)
		vTaskDelay(pdMS_TO_TICKS(20)); 
	}
}


int main(void){
	Serial_Mutex = xSemaphoreCreateMutex();
	CAN_Queue = xQueueCreate(4, 100);
	LED_Event = xEventGroupCreate();	
	
	Delay_Init();
	LED_Init();
	Serial_Init();
	Servo_Font_Init();
	Servo_Back_Init();
	HCSR04_Init();
	Buzzer_Init();
	
	Serial_Printf("\r\n");
	
	xTaskCreate(Task_dht11,"dht11",128,NULL,2,&Task_dht11_handle);
//	xTaskCreate(Task_uart,"uart",128,NULL,3,&Task_uart_handle);
	xTaskCreate(Task_CAN_Rx,"can",128,NULL,3,&Task_can_handle);	
	xTaskCreate(Task_led_flash,"led_flash",128,NULL,2,&Task_led_flash_handle);
	xTaskCreate(Task_Sentry,"Sentry",128,NULL,4,&Task_Sentry_handle);
	xTaskCreate(Task_Servo_Smooth,"Servo_Smooth",128,NULL,2,&Task_Servo_Smooth_handle);
	
	vTaskStartScheduler();
	
	return 0;
}

/* EXTI 中断服务函数，配合 TIM1 测量回响脉宽 */
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) == SET)
    {
        // 判断当前是上升沿还是下降沿
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 1) 
        {
            // 上升沿：Echo 刚变高，开始微秒计时
            TIM_SetCounter(TIM1, 0); // TIM1 清零
            TIM_Cmd(TIM1, ENABLE);   // TIM1 开始计时
        }
        else 
        {
            // 下降沿：Echo 变低，计时结束
            TIM_Cmd(TIM1, DISABLE);  // TIM1 停止计时
            uint16_t time_us = TIM_GetCounter(TIM1); // 获取持续时间(us)
            
            // 计算距离：距离 = 时间(us) * 0.017
            Sentry_Distance = time_us * 0.017f;
            
            // 如果距离小于 3cm，视为危险入侵！立刻唤醒最高优先级的任务！
            if(Sentry_Distance > 0 && Sentry_Distance < 3.0f) 
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                // 通过任务通知唤醒 Task_Sentry
                vTaskNotifyGiveFromISR(Task_Sentry_handle, &xHigherPriorityTaskWoken);
                // 如果 Task_Sentry 优先级较高，立刻触发任务调度切换
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        }
        // 清除中断标志位
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}
