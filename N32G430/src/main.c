#include "main.h"
//#include "log.h"
#include <stdio.h>
#include "coremark.h"

#define SYSTICK_1MS             ((uint32_t)1000)
#define SYSTICK_COUNTER_DIASBLE ((uint32_t)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE  ((uint32_t)0x00000001)
#define SYSTICK_COUNTER_CLEAR   ((uint32_t)0x00000000)

extern int CoreMark(void);

void USART_Init(void);
    
/**
*\*\name   main
*\*\fun    Main program.
*\*\return none
**/
int main(void)
{   
    //log_init();
    /* USART Init */
    USART_Init();
    
    ee_printf("CoreMark Test Start\r\n");
    /* Disables the Prefetch Buffer */
    FLASH_Prefetch_Buffer_Disable();    
    /* Enable iCache */
    FLASH_ICache_Enable();
    
    /* SysTick Init */
    SysTick_Init(SYSTICK_1MS);
    
    CoreMark();

    ee_printf("CoreMark Test Stop\r\n");
    while (1)
    {
    }   
}




#define LOG_USARTx      USART1
#define LOG_PERIPH      RCC_APB2_PERIPH_USART1
#define LOG_GPIO        GPIOA
#define LOG_PERIPH_GPIO RCC_AHB_PERIPH_GPIOA
#define LOG_TX_PIN      GPIO_PIN_4
#define LOG_RX_PIN      GPIO_PIN_10

void USART_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    USART_InitType USART_InitStructure;

    RCC_AHB_Peripheral_Clock_Enable(LOG_PERIPH_GPIO);
    RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO | LOG_PERIPH);

    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = LOG_TX_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2_USART1;
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_Peripheral_Initialize(LOG_GPIO, &GPIO_InitStructure);

//    GPIO_InitStructure.Pin             = LOG_RX_PIN;
//    GPIO_InitStructure.GPIO_Alternate  = GPIO_AF5_USART1;
//    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_TX;

    /* init uart */
    USART_Initializes(LOG_USARTx, &USART_InitStructure);

    /* enable uart */
    USART_Enable(LOG_USARTx);
}

/**
*\*\name   SysTick_Init
*\*\fun    SysTick tick initialize.
*\*\return none
**/
void SysTick_Init(uint32_t NUM)
{
    /* SystemCoreClock / NUM */
    if (SysTick_Config(SystemClockFrequency / NUM))
    {
        while (1)
            ;
    }
}

/**
*\*\name   SysTick_Stop_time
*\*\fun    SysTick tick stop.
*\*\return none
**/
void SysTick_Stop_time(void)
{
    SysTick->CTRL &= SYSTICK_COUNTER_DIASBLE;
    /* Clear the SysTick Counter */
    SysTick->VAL = SYSTICK_COUNTER_CLEAR;
}
