#ifndef __UART_H__
#define __UART_H__

#include "ti_msp_dl_config.h"

extern volatile uint16_t receiveData;
extern volatile uint16_t MODE;
extern uint8_t UART0_TX_BUF[200];
extern uint8_t UART1_TX_BUF[200];
extern uint8_t UART2_TX_BUF[200];


void UART_Init(void);

void u0_printf(char *fmt, ...);
void u1_printf(char *fmt, ...);
void u2_printf(char *fmt, ...);


void UART_0_INST_IRQHandler();
void UART_1_INST_IRQHandler();
void UART_2_INST_IRQHandler();


#endif