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

#define UART_LED								"LED"
#define UART_SERVO_FONT_LEFT		"Servo_fl"
#define UART_SERVO_FONT_RIGHT		"Servo_fr"
#define UART_SERVO_FONT_CLOSE		"Servo_fc"
#define UART_SERVO_BACK_LEFT		"Servo_bl"
#define UART_SERVO_BACK_RIGHT		"Servo_br"
#define UART_SERVO_BACK_CLOSE		"Servo_bc"


TaskHandle_t Task_led_handle;	// LED任务句柄
TaskHandle_t Task_dht11_handle;	// DHT11任务句柄
TaskHandle_t Task_Servo_font_handle;	// Servo_font任务句柄
TaskHandle_t Task_Servo_back_handle;	// Servo_back任务句柄

SemaphoreHandle_t Serial_Mutex;	// 互斥量，保护串口资源

u8 temp;	// 温湿度参数
u8 humi;

/* LED */
static void Task_led(void *arg)
{
	while(1)
	{
		if(1 == Serial_RxFlag)
		{
			if(strcmp(Serial_RxPacket, UART_LED) == 0)	/* @LEDpq */
			{
				LED_Turn();
				//// 仅在需要使用串口时获取互斥锁
				xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
				//Serial_Printf("LED Turn\r\n");
				xSemaphoreGive(Serial_Mutex);
				
				Serial_RxFlag = 0;
			}
			
		}
		// 必须在死循环内部添加小延时，让出 CPU 给其他任务
		vTaskDelay(10);	// 10ms
	}
}
/* Dht11 */
static void Task_dht11(void *arg)
{
	vTaskDelay(1000);
	while(DHT11_Init())
	{
		xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
		//Serial_Printf("DHT11 Error \r\n");
		xSemaphoreGive(Serial_Mutex);
		
		vTaskDelay(1000);
	}		
	while (1)
	{
		DHT11_Read_Data(&temp,&humi);	// 获取温湿度的值
		
		xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
		Serial_Printf("temp %d\nhumi %d%%RH\n",temp,humi);
		xSemaphoreGive(Serial_Mutex);
		
		vTaskDelay(1000);
	}
}

/* Servo_font */
static void Task_Servo_font(void *arg)
{
	while(1)
	{
		if(1 == Serial_RxFlag)
		{
			if(strcmp(Serial_RxPacket, UART_SERVO_FONT_LEFT) == 0)	/* @Servo_flpq */
			{
				Servo_Font_SetAngle(180);
				
				xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
				//Serial_Printf("font left door\r\n");
				xSemaphoreGive(Serial_Mutex);
				
				Serial_RxFlag = 0;
			}
			else if(strcmp(Serial_RxPacket, UART_SERVO_FONT_RIGHT) == 0)	/* @Servo_frpq */
			{
				Servo_Font_SetAngle(0);
				
				xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
				//Serial_Printf("font right door\r\n");
				xSemaphoreGive(Serial_Mutex);
				
				Serial_RxFlag = 0;
			}
			else if(strcmp(Serial_RxPacket, UART_SERVO_FONT_CLOSE) == 0)	/* @Servo_fcpq */
			{
				Servo_Font_SetAngle(90);
				
				xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
				//Serial_Printf("font close\r\n");
				xSemaphoreGive(Serial_Mutex);
				
				Serial_RxFlag = 0;
			}
			
		}
		// 必须在死循环内部添加小延时，让出 CPU 给其他任务
		vTaskDelay(10);	// 10ms
	}
}

/* Servo_back */
static void Task_Servo_back(void *arg)
{
	while(1)
	{
		if(1 == Serial_RxFlag)
		{
			if(strcmp(Serial_RxPacket, UART_SERVO_BACK_LEFT) == 0)	/* @Servo_blpq */
			{
				Servo_Back_SetAngle(180);
				
				xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
				//Serial_Printf("back left door\r\n");
				xSemaphoreGive(Serial_Mutex);
				
				Serial_RxFlag = 0;
			}
			else if(strcmp(Serial_RxPacket, UART_SERVO_BACK_RIGHT) == 0)	/* @Servo_brpq */
			{
				Servo_Back_SetAngle(0);
				
				xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
				//Serial_Printf("back right door\r\n");
				xSemaphoreGive(Serial_Mutex);
				
				Serial_RxFlag = 0;
			}
			else if(strcmp(Serial_RxPacket, UART_SERVO_BACK_CLOSE) == 0)	/* @Servo_bcpq */
			{
				Servo_Back_SetAngle(90);
				
				xSemaphoreTake(Serial_Mutex, portMAX_DELAY);
				//Serial_Printf("back close\r\n");
				xSemaphoreGive(Serial_Mutex);
				
				Serial_RxFlag = 0;
			}
			
		}
		// 必须在死循环内部添加小延时，让出 CPU 给其他任务
		vTaskDelay(10);	// 10ms
	}
}

int main(void){
	Delay_Init();
	LED_Init();
	Serial_Init();
	Servo_Font_Init();
	Servo_Back_Init();
	
	Servo_Font_SetAngle(90);
	Servo_Back_SetAngle(90);
	
	Serial_Printf("\r\n");
	Serial_Mutex = xSemaphoreCreateMutex();
	
	xTaskCreate(Task_led,"led",512,NULL,2,&Task_led_handle);
	xTaskCreate(Task_dht11,"dht11",512,NULL,2,&Task_dht11_handle);
	xTaskCreate(Task_Servo_font,"Servo_font",512,NULL,2,&Task_Servo_font_handle);
	xTaskCreate(Task_Servo_back,"Servo_back",512,NULL,2,&Task_Servo_back_handle);
	
	vTaskStartScheduler();
	
	return 0;
}
