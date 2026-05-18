#include "MyCAN.h"
#include "FreeRTOS.h"
#include "queue.h"

// 引用外部定义好的 CAN 消息队列
extern QueueHandle_t CAN_Queue; 

void MyCAN_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 初始化 GPIO PA11: RX, PA12: TX
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; // CAN_TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // CAN_RX
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 初始化 CAN1 核心寄存器 (波特率 500kbps)
    // 计算公式: 36MHz / Prescaler(4) / (1 + BS1(9) + BS2(8)) = 500k
    CAN_InitTypeDef CAN_InitStructure;
    CAN_StructInit(&CAN_InitStructure);
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
    CAN_InitStructure.CAN_Prescaler = 4;
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;  // 自动离线恢复(极大地提高稳定性)
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE; // 允许自动重传
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_Init(CAN1, &CAN_InitStructure);
    
    // 配置 CAN 硬件过滤器 只接收ID为 0x101 的帧
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = (CAN_RX_ID << 5); 
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFE0; // 精确匹配标准ID
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    // 配置 CAN 接收中断
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6; // 处于 RTOS 安全调用范围内
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// CAN 发送函数
void MyCAN_Transmit(uint32_t StdId, uint8_t *Data, uint8_t Length)
{
    CanTxMsg TxMessage;
    TxMessage.StdId = StdId;
    TxMessage.ExtId = 0x00;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = Length;
    for (uint8_t i = 0; i < Length; i++) {
        TxMessage.Data[i] = Data[i];
    }
    CAN_Transmit(CAN1, &TxMessage);
}

// CAN 接收中断服务函数 硬件收到合法 ID 报文后触发
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
    {
        CanRxMsg RxMessage;
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
        
        // 确保是标准帧且 ID 正确
        if (RxMessage.IDE == CAN_Id_Standard && RxMessage.StdId == CAN_RX_ID)
        {
            CAN_Msg_t msg;
            msg.StdId = RxMessage.StdId;
            msg.DLC = RxMessage.DLC;
            for(uint8_t i=0; i<8; i++) msg.Data[i] = RxMessage.Data[i];
            
            // 将接收到的结构体发送到 FreeRTOS 队列
            if (CAN_Queue != NULL) {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xQueueSendFromISR(CAN_Queue, &msg, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // 任务切换
            }
        }
    }
}
