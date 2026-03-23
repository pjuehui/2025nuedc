#ifndef __ADC_H__
#define __ADC_H__

#include "ti_msp_dl_config.h"
#include "arm_const_structs.h"
#include "arm_math.h"

#define FFT_LENGTH 1024
#define SAMPLE_FRE 1024*2051

// 声明外部缓冲区

extern uint16_t adcBuff0[FFT_LENGTH];
extern uint16_t adcBuff1[FFT_LENGTH];
extern float fft_inputbuf0[FFT_LENGTH * 2];
extern float fft_outputbuf0[FFT_LENGTH];
extern float fft_inputbuf1[FFT_LENGTH * 2]; 
extern float fft_outputbuf1[FFT_LENGTH];
extern float adc_input_1[FFT_LENGTH];
extern float adc_input_2[FFT_LENGTH];

// 声明外部变量
extern volatile float fund_wave_amp;
extern volatile float fund_wave_amp_2;
extern volatile float fund_wave_amp_3;
extern volatile float fund_wave_amp_4;
extern volatile float fund_wave_amp_5;
extern volatile uint32_t fund_wave_coord;
extern volatile uint32_t frequency1;
extern volatile float THD;
extern volatile bool gCheckADC;
extern volatile float frequency_resolution;

// 函数声明
void ADC_Init(void);
void adc_fft_THD(void);
void ADC_ProcessFFT(void);
void configureSampling(void);

#endif // __ADC_H__