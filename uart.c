#include "uart.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

volatile uint16_t receiveData = 0;
volatile uint16_t MODE = 0;
uint8_t UART0_TX_BUF[200];  // 发送缓冲区
uint8_t UART1_TX_BUF[200];  // 发送缓冲区
uint8_t UART2_TX_BUF[200];  // 发送缓冲区


void UART_Init(void)
{
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(UART_2_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_2_INST_INT_IRQN);
}


void u0_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t i, len;
    va_start(ap, fmt);
    vsprintf((char *)UART0_TX_BUF, fmt, ap);
    va_end(ap);
    len = strlen((const char *)UART0_TX_BUF);
    
    for (i = 0; i < len; i++) {

        while (DL_UART_Main_isBusy(UART_0_INST)) 
        {

        }  
        

        DL_UART_Main_transmitData(UART_0_INST, UART0_TX_BUF[i]);
    }
    
    while (DL_UART_Main_isBusy(UART_0_INST)) 
    {

    } 
}

void u1_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t i, len;
    va_start(ap, fmt);
    vsprintf((char *)UART1_TX_BUF, fmt, ap);
    va_end(ap);
    len = strlen((const char *)UART1_TX_BUF);
    
    for (i = 0; i < len; i++) {

        while (DL_UART_Main_isBusy(UART_1_INST)) 
        {

        }  

        DL_UART_Main_transmitData(UART_1_INST, UART1_TX_BUF[i]);
    }
    
    while (DL_UART_Main_isBusy(UART_1_INST)) 
    {

    } 
}

void u2_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t i, len;
    va_start(ap, fmt);
    vsprintf((char *)UART2_TX_BUF, fmt, ap);
    va_end(ap);
    len = strlen((const char *)UART2_TX_BUF);
    

    for (i = 0; i < len; i++) {

        while (DL_UART_Main_isBusy(UART_2_INST)) 
        {

        }  
        

        DL_UART_Main_transmitData(UART_2_INST, UART2_TX_BUF[i]);
    }
    
    while (DL_UART_Main_isBusy(UART_2_INST)) 
    {

    } 
}


void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            receiveData = DL_UART_Main_receiveData(UART_0_INST);
            // DL_UART_Main_transmitData(UART_0_INST,receiveData);
            break;
        default:
            break;
    }
}

void UART_1_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_1_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            MODE= DL_UART_Main_receiveData(UART_1_INST);
            DL_UART_Main_transmitData(UART_1_INST,MODE);
            break;
        default:
            break;
    }
}

void UART_2_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_2_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            // receiveData = DL_UART_Main_receiveData(UART_1_INST);
            // DL_UART_Main_transmitData(UART_0_INST,receiveData);
            break;
        default:
            break;
    }
}

