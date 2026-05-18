#ifndef __MYCAN_H
#define __MYCAN_H

#include "stm32f10x.h"

// 定义 CAN ID
#define CAN_RX_ID          0x101  // STM32 监听的下发指令 ID
#define CAN_TX_ID          0x102  // STM32 上报数据的 ID

// 定义指令码 (替代以前的字符串)
#define CAN_CMD_LED         0x01
#define CAN_CMD_SERVO_FL    0x11
#define CAN_CMD_SERVO_FC    0x10
#define CAN_CMD_SERVO_FR    0x21
#define CAN_CMD_SERVO_BL    0x31
#define CAN_CMD_SERVO_BC    0x30
#define CAN_CMD_SERVO_BR    0x41

// CAN 接收数据包结构体 用于队列极速传输
typedef struct {
    uint32_t StdId;
    uint8_t Data[8];
    uint8_t DLC;
} CAN_Msg_t;

void MyCAN_Init(void);
void MyCAN_Transmit(uint32_t StdId, uint8_t *Data, uint8_t Length);

#endif
