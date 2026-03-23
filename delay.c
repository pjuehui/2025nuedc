#include "delay.h"
#include "ti_msp_dl_config.h"

// 微秒延时（直接计算周期数）
void delay_us(unsigned long us)
{
    unsigned long cycles = us * (CPUCLK_FREQ / 1000000UL);
    while(cycles--) {
        __NOP();
    }
}

// 毫秒延时（优化实现）
void delay_ms(unsigned long ms)
{
    unsigned long cycles_per_ms = CPUCLK_FREQ / 1000UL;
    while(ms--) {
        unsigned long cycles = cycles_per_ms;
        while(cycles--) {
            __NOP();
        }
    }
}