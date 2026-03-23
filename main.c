//////////////////////////////////////////////////////////////////////////////////	 
//  文 件 名   : juehui.c
//  版 本 号   : v2.0++
//  作    者   : Juehui
//  生成日期   : 2025-07-04
//  最近修改   :
//  功能描述   : 2025国赛各种外设组合
//******************************************************************************/
#include "ti_msp_dl_config.h"
#include "oled.h"
#include "bmp.h"
#include "arm_const_structs.h"
#include "arm_math.h"
#include <stdio.h>
#include "string.h"
#include "hardware/ad9959.h"
#include "hardware/adc.h" 
#include "hardware/uart.h" 
#include "hardware/PE4302.h"
#include "hardware/ADS8688.h"
#include "hardware/dac8552.h"


int main(void){
    SYSCFG_DL_init();
    UART_Init();
    OLED_Init();
    ADC_Init();
    // PE4302_Set(0);
    //    AD9959_Init();
    //    AD9959_Write_frequence(0,1000000);
    //    AD9959_Write_Phase(0,0);
    //    AD9959_Write_Amplitude(0,1023/1);
    //    AD9959_IO_Updata();

    // ADS.ADS8688_Init(CH0_EN | CH1_EN); 
    // ADS.ADS8688_Use(ADS8688_CH_0, ADS8688_IR_N2_5V);

    //  DAC8552_GPIO_Init();
    //  DAC8552_Write_CHA(65535);  // 设置通道A   0-65535分别对应-4.94到+4.94v 自行矫正
    //  DAC8552_Write_CHB(65535);  // 设置通道B

    while(1) 
    {		


        // float ch0_voltage = ADS.ADS8688_Use(ADS8688_CH_0, ADS8688_IR_N2_5V);
        // float ch1_voltage = ADS.ADS8688_Use(ADS8688_CH_1, ADS8688_IR_N1_25V);
        // u0_printf("CH0: %.3f V\n, CH1: %.3f V\n", ch0_voltage, ch1_voltage);




   
        gCheckADC = false;
        DL_TimerA_startCounter(TIMER_0_INST);  // 启动定时器（触发ADC采样）
        // 等待ADC完成
        while (gCheckADC == false) 
        {

        }  // 忙等待，实际应用建议用中断/DMA
        adc_fft_THD();

        // // 重启采样
         gCheckADC = false;
         DL_TimerA_startCounter(TIMER_0_INST);



    }
}

