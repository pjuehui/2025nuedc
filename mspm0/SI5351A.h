#ifndef _SI5351A_H_
#define _SI5351A_H_

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdbool.h>

// 寄存器定义
#define SI_CLK0_CONTROL     16
#define SI_CLK1_CONTROL     17
#define SI_CLK2_CONTROL     18
#define SI_SYNTH_PLL_A      26
#define SI_SYNTH_PLL_B      34
#define SI_SYNTH_MS_0       42
#define SI_SYNTH_MS_1       50
#define SI_SYNTH_MS_2       58
#define SI_PLL_RESET        177

// R分频系数
#define SI_R_DIV_1          0x00
#define SI_R_DIV_2          16
#define SI_R_DIV_4          32
#define SI_R_DIV_8          48
#define SI_R_DIV_16         64
#define SI_R_DIV_32         80
#define SI_R_DIV_64         96
#define SI_R_DIV_128        112

// 时钟源选择
#define SI_CLK_SRC_PLL_A    0x00
#define SI_CLK_SRC_PLL_B    0x20

// 晶振频率(25MHz)
#define XTAL_FREQ           25000000

// I2C地址
#define SI5351_I2C_ADDR     0xC0



// 引脚操作宏
#define SI5351_SCL_H()      DL_GPIO_setPins(SI5351_PORT, SI5351_SI5351_SCL_PIN )
#define SI5351_SCL_L()      DL_GPIO_clearPins(SI5351_PORT, SI5351_SI5351_SCL_PIN)
#define SI5351_SDA_H()      DL_GPIO_setPins(SI5351_PORT, SI5351_SI5351_SDA_PIN)
#define SI5351_SDA_L()      DL_GPIO_clearPins(SI5351_PORT, SI5351_SI5351_SDA_PIN)

// 函数声明
void Si5351_Init(void);
void Si5351_SetFrequency(uint32_t frequency, uint8_t channel);
void Si5351_EnableOutputs(uint8_t channel_0, uint8_t channel_1, uint8_t channel_2);
void Si5351_SetPLL(uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom);
void Si5351_SetMultisynth(uint8_t synth, uint32_t divider, uint8_t rDiv);

#endif // _SI5351A_H_