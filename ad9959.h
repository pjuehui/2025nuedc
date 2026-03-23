//**************************************************
//适用于MSPM0G3507
//**************************************************
#ifndef _AD9959_H_
#define _AD9959_H_
#include "ti_msp_dl_config.h"
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>
#include "ti/driverlib/dl_gpio.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#define CSR_ADD   0x00   //CSR 通道选择寄存器
#define FR1_ADD   0x01   //FR1 功能寄存器1
#define FR2_ADD   0x02   //FR2 功能寄存器2
#define CFR_ADD   0x03 

#define CFTW0_ADD 0x04 
#define CPOW0_ADD 0x05 
#define ACR_ADD   0x06

#define LSRR_ADD  0x07 
#define RDW_ADD   0x08 
#define FDW_ADD   0x09 

#define PROFILE_ADDR_BASE   0x0A

//CSR[7:4]通道选择启用位
#define CH0 0x10
#define CH1 0x20
#define CH2 0x40
#define CH3 0x80

//FR1[9:8] 调制电平选择位
#define LEVEL_MOD_2     0x00//2电平调制 2阶调制
#define LEVEL_MOD_4     0x01//4电平调制 4阶调制
#define LEVEL_MOD_8     0x02//8电平调制 8阶调制
#define LEVEL_MOD_16    0x03//16电平调制    16阶调制

//CFR[23:22]  幅频相位（AFP）选择位
#define DISABLE_Mod     0x00    //00    调制已禁用
#define ASK                     0x40    //01    振幅调制，幅移键控
#define FSK                     0x80    //10    频率调制，频移键控
#define PSK                     0xc0    //11    相位调制，相移键控

//（CFR[14]）线性扫描启用 sweep enable
#define SWEEP_ENABLE    0x40    //1 启用
#define SWEEP_DISABLE   0x00    //0 不启用


#define  FREQ_CON_20K          171803
#define  FREQ_CON_25K          214754
#define  FREQ_CON_30K          257705
#define  FREQ_CON_35K          300656
#define  FREQ_CON_40K          343607
#define  FREQ_CON_45K          386558
#define  FREQ_CON_50K          429508
#define  FREQ_CON_55K          472459
#define  FREQ_CON_60K          515410
#define  FREQ_CON_65K          558361
#define  FREQ_CON_70K          601312
#define  FREQ_CON_75K          644263
#define  FREQ_CON_80K          687214
#define  FREQ_CON_85K          730165
#define  FREQ_CON_90K          773116
#define  FREQ_CON_95K          816066
#define  FREQ_CON_100K         859017


#define CS_H    DL_GPIO_setPins(AD9959_PIN_PORT, AD9959_PIN_PIN_CS_PIN)
#define CS_L    DL_GPIO_clearPins(AD9959_PIN_PORT, AD9959_PIN_PIN_CS_PIN)

#define SCLK_H  DL_GPIO_setPins(AD9959_PIN_PORT, AD9959_PIN_PIN_SCLK_PIN)
#define SCLK_L  DL_GPIO_clearPins(AD9959_PIN_PORT, AD9959_PIN_PIN_SCLK_PIN)

#define UPDATE_H  DL_GPIO_setPins(AD9959_PIN_PORT, AD9959_PIN_PIN_UPDATA_PIN)
#define UPDATE_L  DL_GPIO_clearPins(AD9959_PIN_PORT, AD9959_PIN_PIN_UPDATA_PIN)

#define AD9959_PWR_H   DL_GPIO_setPins(AD9959_PIN_PORT, AD9959_PIN_PIN_PWR_PIN)
#define AD9959_PWR_L   DL_GPIO_clearPins(AD9959_PIN_PORT, AD9959_PIN_PIN_PWR_PIN)

#define Reset_H     DL_GPIO_setPins(AD9959_PIN_PORT, AD9959_PIN_PIN_RST_PIN)
#define Reset_L     DL_GPIO_clearPins(AD9959_PIN_PORT, AD9959_PIN_PIN_RST_PIN)

#define SDIO0_H   DL_GPIO_setPins(AD9959_PIN_PORT, AD9959_PIN_PIN_SDIO_PIN)
#define SDIO0_L   DL_GPIO_clearPins(AD9959_PIN_PORT, AD9959_PIN_PIN_SDIO_PIN)

//剩下都接地

//IN\OUT-PIN 最小时钟需要大于系统时钟
void AD9959_Init(void);
void Init_IO(void);    //Intserve  IO初始化
void IntReset_IO(void); //IntReset  复位
void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData,uint8_t   temp);//传递数据
void AD9959_IO_Updata(void);  //更新数据
void WriteDATA_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData);

void AD9959_Write_frequence(uint8_t channel,uint32_t frequence);
void AD9959_Selfwrite_frequence(uint8_t channel,uint32_t frequence);
void AD9959_Write_Phase(uint8_t channel,uint16_t phase);
void AD9959_Write_Amplitude(uint8_t channel,uint16_t Amplitude);
void ad9959delay(uint16_t length);

/*****************************点频操作函数***********************************/
void AD9959_setFreq(uint8_t channel, float freq); //写频率
void AD9959_setAmp(uint8_t channel, uint16_t amp); //写幅度
void AD9959_setPhase(uint8_t channel, uint16_t phase); //写相位
/****************************************************************************/

/*****************************调制操作函数  ***********************************/
void AD9959_modulationInit(uint8_t channel, uint8_t modulation, uint8_t sweepEn, uint8_t nLevel);//设置各个通道的调制模式。
void AD9959_setFSK(uint8_t channel, uint32_t* data, uint16_t phase);//设置FSK调制的参数
void AD9959_setASK(uint8_t channel, uint16_t* data, uint32_t freq, uint16_t phase);//设置ASK调制的参数
void AD9959_setPSK(uint8_t channel, uint16_t* data, uint32_t freq);//设置PSK调制的参数

void AD9959_setFreqSweep(uint8_t channel, uint32_t s_data, uint32_t e_data, uint32_t r_delta, uint32_t f_delta, uint8_t rsrr, uint8_t fsrr, uint16_t amp, uint16_t phase);//设置线性扫频的参数
void AD9959_setAmpSweep(uint8_t channel, uint32_t s_Ampli, uint16_t e_Ampli, uint32_t r_delta, uint32_t f_delta, uint8_t rsrr, uint8_t fsrr, uint32_t freq, uint16_t phase);//设置线性扫幅的参数
void AD9959_setPhaseSweep(uint8_t channel, uint16_t s_data, uint16_t e_data, uint16_t r_delta, uint16_t f_delta, uint8_t rsrr, uint8_t fsrr, uint32_t freq, uint16_t amp);//设置线性扫相的参数
/********************************************************************************************/


#endif

//    AD9959_Init();
//        AD9959_Write_frequence(3,40000000);
//        AD9959_Write_Phase(3,0);
//        AD9959_Write_Amplitude(3,1023/1);
//        AD9959_IO_Updata();
