#ifndef _DAC8552_H_
#define _DAC8552_H_

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdbool.h>

// // 寄存器地址定义
#define DAC8552_WRITE_CHA      0x10  // 写入通道A并更新
#define DAC8552_WRITE_CHB      0x24  // 写入通道B并更新
#define DAC8552_WRITE_CHA_BUF  0x00  // 写入通道A缓冲
#define DAC8552_WRITE_CHB_BUF  0x34  // 写入通道B缓冲



// 引脚操作宏
#define DAC8552_SYNC_H    DL_GPIO_setPins(DAC8552_PORT, DAC8552_SYNC_PIN)
#define DAC8552_SYNC_L    DL_GPIO_clearPins(DAC8552_PORT, DAC8552_SYNC_PIN)
#define DAC8552_SCLK_H    DL_GPIO_setPins(DAC8552_PORT, DAC8552_SCLK_PIN)
#define DAC8552_SCLK_L    DL_GPIO_clearPins(DAC8552_PORT, DAC8552_SCLK_PIN)
#define DAC8552_DATA_H     DL_GPIO_setPins(DAC8552_PORT, DAC8552_DATA_PIN)
#define DAC8552_DATA_L     DL_GPIO_clearPins(DAC8552_PORT, DAC8552_DATA_PIN)

// 函数声明
void DAC8552_GPIO_Init(void);
void DAC8552_Write_Reg(uint32_t data);
void DAC8552_Write_CHA(uint16_t data);
void DAC8552_Write_CHB(uint16_t data);
void DAC8552_delay(uint16_t length);

#endif