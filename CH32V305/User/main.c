/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/01/01
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *Multiprocessor communication mode routine:
 *Master:USART1_Tx(PD5)\USART1_Rx(PD6).
 *This routine demonstrates that USART1 receives the data sent by CH341 and inverts
 *it and sends it (baud rate 115200).
 *
 *Hardware connection:PD5 -- Rx
 *                     PD6 -- Tx
 *
 */

#include "debug.h"
#include "coremark.h"

#define SYSTICK_1MS             ((uint32_t)1000)
#define SYSTICK_COUNTER_DIASBLE ((uint32_t)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE  ((uint32_t)0x00000001)
#define SYSTICK_COUNTER_CLEAR   ((uint32_t)0x00000000)

extern int CoreMark(void);

/* Global define */


/* Global Variable */
vu8 val;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    /* SysTick Init */
    systick_Init();

    Delay_Init();
    USART_Printf_Init(115200);

    printf("CoreMark Test Start\r\n");
    
    CoreMark();

    printf("CoreMark Test Stop\r\n");

    while (1);
}
