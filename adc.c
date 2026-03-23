#include "adc.h"
#include "hardware/uart.h" 

uint16_t adcBuff0[FFT_LENGTH];
uint16_t adcBuff1[FFT_LENGTH];
float fft_inputbuf0[FFT_LENGTH * 2];
float fft_outputbuf0[FFT_LENGTH];
float fft_inputbuf1[FFT_LENGTH * 2];
float fft_outputbuf1[FFT_LENGTH];
float adc_input_0[FFT_LENGTH];			//前一个数表示实部，后一个表示虚部
float adc_input_1[FFT_LENGTH];			        //前一个数表示实部，后一个表示虚部
volatile float fund_wave_amp,fund_wave_amp_2,fund_wave_amp_3,fund_wave_amp_4,fund_wave_amp_5;						
volatile uint32_t fund_wave_coord;				//基波索引
volatile uint32_t frequency1;				//把采到的信号频率赋给这个进行采样率的调整
volatile float THD;
volatile bool gCheckADC;
volatile float frequency_resolution = SAMPLE_FRE / FFT_LENGTH; // 频率分辨



void ADC_Init(void)
{

        // 初始化DMA通道0 (ADC0)
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, 
                     (uint32_t)DL_ADC12_getMemResultAddress(ADC12_0_INST, ADC12_0_ADCMEM_0));
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)adcBuff0);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);

    // // 初始化DMA通道1 (ADC1)
    // DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID,
    //                  (uint32_t)DL_ADC12_getMemResultAddress(ADC12_1_INST, ADC12_1_ADCMEM_0));
    // DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)adcBuff1);
    // DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
    // NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);

    // // 启动ADC采样
    // DL_ADC12_startConversion(ADC12_0_INST);
    // // DL_ADC12_startConversion(ADC12_1_INST);
    // DL_TimerA_startCounter(TIMER_0_INST);
}


float square(float x) 
{
    float result;
    arm_mult_f32(&x, &x, &result, 1);
    return result;
}

/**
 * @brief 汉宁窗函数（用于减少频谱泄漏）
 * @param t 当前点的索引（0 ≤ i < FFT_LENGTH）
 * @return 窗函数系数
 */


float Hanningwindow(int i) {
    return 0.5f * (1.0f - cosf(2.0f * PI * i / (FFT_LENGTH - 1)));
}

// /**
//  * @brief 双峰谱线插值法校正频率偏移
//  * @param peak_index FFT频谱中目标峰值的离散索引（如100表示100Hz）
//  * @return 校正后的精确频率（Hz）


// 双峰谱线插值频率校正，返回频率（Hz）
float get_precise_frequency(uint16_t peak_index) {
    if (peak_index <= 0 || peak_index >= FFT_LENGTH/2 - 1) {
        return peak_index * frequency_resolution;
    }
    float A_prev = fft_outputbuf0[peak_index - 1];
    float A_curr = fft_outputbuf0[peak_index];
    float A_next = fft_outputbuf0[peak_index + 1];
    float delta = 0.0f;

    if (A_next >= A_prev) {
        delta = (2.0f * A_next - A_curr) / (A_next + A_curr) - 1.0f;
        delta /= 2.0f;
    } else {
        delta = 1.0f - (A_curr - 2.0f * A_prev) / (A_curr + A_prev);
        delta /= 2.0f;
        delta = -delta;
    }

    return (peak_index + delta) * frequency_resolution;
}


// void ADC_ProcessFFT(void)
// {
//     static uint16_t i ;
//     // 处理ADC0数据
//     for(int i = 0; i < FFT_LENGTH; i++) {
//         adc_input_0[i]=adcBuff0[i];
//         fft_inputbuf0[i*2] = (float32_t)adc_input_0[i] * 3.3f / 4095.0f;
//         fft_inputbuf0[i*2+1] = 0;
//         //   u0_printf("第%d个点的电压值%lf\r\n",i,fft_inputbuf0[i*2]*1000);
//         u0_printf("第%d个点的电压值%lf\r\n",i,adc_input_0[i]*1000);
//     }


//     arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf0, 0, 1);
//     arm_cmplx_mag_f32(fft_inputbuf0, fft_outputbuf0, FFT_LENGTH);

//     fft_outputbuf0[0] /= FFT_LENGTH;
//     for(int i = 1; i < FFT_LENGTH; i++) {
//         fft_outputbuf0[i] /= (FFT_LENGTH/2);
//     }

//     // 处理ADC1数据
//     // for(int i = 0; i < FFT_LENGTH; i++) {
//     //     fft_inputbuf1[i*2] =  adcBuff1[i] * 3.3f / 4095.0f;
//     //     fft_inputbuf1[i*2+1] = 0;

//     //     u0_printf("第%d个点的电压值%lf\r\n",i,adcBuff1[i] * 3.3f / 4095.0f*1000);
//     // }
//     // arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf1, 0, 1);
//     // arm_cmplx_mag_f32(fft_inputbuf1, fft_outputbuf1, FFT_LENGTH);
//     // fft_outputbuf1[0] /= FFT_LENGTH;
//     // for(int i = 1; i < FFT_LENGTH; i++) {
//     //     fft_outputbuf1[i] /= (FFT_LENGTH/2);
//     //     u0_printf("第%d个点的电压值%lf\r\n",i, fft_outputbuf1[i]*1000);
//     // }
// }

//失真度

void adc_fft_THD()
{
    uint32_t i=0;
		//生成信号序列
		for(i=0;i<FFT_LENGTH;i++)
			{		//采样数据转化为电压值
                adc_input_0[i]=adcBuff0[i]*3.3f / 4095.0;
                fft_inputbuf0[i*2] = (float32_t)adc_input_0[i]*Hanningwindow(i);
                fft_inputbuf0[i*2+1] = 0;
                // u0_printf("%d\n",adc_input_0[i]);
                //u0_printf("第%d个点的电压值%lf\r\n",i,adc_input_0[i]*1000);
                // u0_printf("%lf,",adc_input_0[i]*1000);


			}
            //u0_printf("end\r\n");
		//FFT计算	
        arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf0, 0, 1);		
        arm_cmplx_mag_f32(fft_inputbuf0, fft_outputbuf0, FFT_LENGTH);							
		//FFT幅度的简单计算，i=0处FFT幅度为直流分量，在i！=0处FFT幅度为信号幅值的一半				
		for(i=0;i<FFT_LENGTH/2;i++)
			{
			if(i==0)
			{
				fft_outputbuf0[i]=fft_outputbuf0[i]/FFT_LENGTH;
			}
			else
			{
				fft_outputbuf0[i]=fft_outputbuf0[i]*2/FFT_LENGTH;
			}
			//u0_printf("采样第%dHz的幅值:%.2f\r\n",i*(SAMPLE_FRE/FFT_LENGTH),fft_outputbuf0[i]);								
			}
        for(i=0;i<FFT_LENGTH;i++)
        {
            adc_input_0[i]-=fft_outputbuf0[0];
        }
			//计算幅度
					fund_wave_amp=fft_outputbuf0[1];//从1开始，避免取到直流量
					fund_wave_coord=1;
			     for(i=1;i<FFT_LENGTH/2;i++)
			      {
			        if(fft_outputbuf0[i]>fund_wave_amp)
			        	{
                  fund_wave_amp=fft_outputbuf0[i];
                  fund_wave_coord=i;
                }
			      }//取最大幅值谐波	
							fund_wave_amp=fft_outputbuf0[fund_wave_coord]*2.0f;
							fund_wave_amp_2=fft_outputbuf0[fund_wave_coord*2]*2.0f;
							fund_wave_amp_3=fft_outputbuf0[fund_wave_coord*3]*2.0f;
							fund_wave_amp_4=fft_outputbuf0[fund_wave_coord*4]*2.0f;
							fund_wave_amp_5=fft_outputbuf0[fund_wave_coord*5]*2.0f;
							THD=sqrt(+square(fund_wave_amp_2)+square(fund_wave_amp_3)+square(fund_wave_amp_4)+square(fund_wave_amp_5))/fund_wave_amp;
                            u0_printf("直流分量为%lf\r\n",fft_outputbuf0[0]);							
                            u0_printf("一次谐波为%lf，频率为%d\r\n",fund_wave_amp,fund_wave_coord*(SAMPLE_FRE/FFT_LENGTH));
							u0_printf("二次谐波为%lf，频率为%d\r\n",fund_wave_amp_2,2*fund_wave_coord*(SAMPLE_FRE/FFT_LENGTH));
							u0_printf("三次谐波为%lf，频率为%d\r\n",fund_wave_amp_3,3*fund_wave_coord*(SAMPLE_FRE/FFT_LENGTH));
							u0_printf("四次谐波为%lf，频率为%d\r\n",fund_wave_amp_4,4*fund_wave_coord*(SAMPLE_FRE/FFT_LENGTH));
							u0_printf("五次谐波为%lf，频率为%d\r\n",fund_wave_amp_5,5*fund_wave_coord*(SAMPLE_FRE/FFT_LENGTH));
							u0_printf("THD为%lf\r\n",THD);

}





// // ADC中断处理函数
// void ADC12_0_INST_IRQHandler(void)
// {
//     if(DL_ADC12_getPendingInterrupt(ADC12_0_INST) == DL_ADC12_IIDX_DMA_DONE) {
//         DL_TimerA_stopCounter(TIMER_0_INST);
//     }
// }

void ADC12_0_INST_IRQHandler(void) {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:  // DMA传输完成中断
            DL_TimerA_stopCounter(TIMER_0_INST);  
            gCheckADC = true;                     // 标记ADC完成

            break;
        default:
            break;
    }
}

// void ADC12_1_INST_IRQHandler(void)
// {
//     if(DL_ADC12_getPendingInterrupt(ADC12_1_INST) == DL_ADC12_IIDX_DMA_DONE) {
//         DL_TimerA_stopCounter(TIMER_0_INST);
//     }
// }



void configureSampling(void) 
{
    // 高采样率模式（1MHz）：适用于频率范围 2.5kHz ~ 500.1kHz
    if (frequency1 > 2500 && frequency1 < 500100) {
        DL_TimerA_setLoadValue(TIMER_0_INST, 31);  // 32MHz / (31 + 1) = 1MHz
        //  SAMPLE_FRE = 1000000;                         // 1MHz
    }
    // 低采样率模式（102.4kHz）：适用于频率范围 0Hz ~ 2.5kHz
    else if (frequency1 >= 0 && frequency1 <= 2500) {
        DL_TimerA_setLoadValue(TIMER_0_INST, 312); // 32MHz / (312 + 1) ≈ 102.24kHz
        //  SAMPLE_FRE = 102400;                          // 目标 102.4kHz（误差 -0.15%）
    }
    // 默认处理（频率超出范围时）
    else {
        DL_TimerA_setLoadValue(TIMER_0_INST, 312); // 默认使用低采样率
        // SAMPLE_FRE = 102400;
    }
}