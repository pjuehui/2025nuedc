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
#include "hardware/adcfft.h" 
#include "hardware/uart.h" 
#include "hardware/PE4302.h"
#include "hardware/ADS8688.h"
#include "hardware/dac8552.h"
#include "hardware/SI5351A.h"


int main(void){
    SYSCFG_DL_init();
    UART_Init();
    OLED_Init();
    adc_init_all();

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


    // Si5351_Init();
    // Si5351_EnableOutputs(1,1, 1);
	// Si5351_SetFrequency(4000000,0);			//通道0输出5khz
	// Si5351_SetFrequency(1000000,1);	//通道1输出1Mhz
	// Si5351_SetFrequency(10000000,2);	//通道2输出10Mhz




    while(1) 
    {		

        // 处理ADC0数据
        // FFT0_process();
        
        // 处理ADC1数据
        FFT1_process();



					// u0_printf("page0.n0.val=%d\xff\xff\xff",223);
        // float ch0_voltage = ADS.ADS8688_Use(ADS8688_CH_0, ADS8688_IR_N2_5V);
        // float ch1_voltage = ADS.ADS8688_Use(ADS8688_CH_1, ADS8688_IR_N1_25V);
        // u0_printf("CH0: %.3f V\n, CH1: %.3f V\n", ch0_voltage, ch1_voltage);






    }
}

