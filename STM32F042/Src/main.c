/****************************************Copyright (c)****************************************************
**                                      
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GPIO LED application function
**
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "coremark.h"


#define SYSTICK_1MS ((uint32_t)1000)
#define SYSTICK_COUNTER_DIASBLE ((uint32_t)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE ((uint32_t)0x00000001)
#define SYSTICK_COUNTER_CLEAR ((uint32_t)0x00000000)
#define COREMARK_USART_BAUDRATE ((uint32_t)115200)

void SysTick_Init(uint32_t);
void SysTick_Stop_time(void);

/* Private functions ---------------------------------------------------------*/
void APP_Clock_Init(void);
void APP_GPIO_USART_Init(void);
void APP_USART_Init(void);
void UART_SendByte(USART_TypeDef* USARTx, uint8_t data);
    
void Delay (uint32_t nCount);

extern int CoreMark(void);

/*******************************************************************************
* Function Name  : main
*******************************************************************************/
int main(void)
{
    APP_Clock_Init();
	APP_USART_Init();
    ee_printf("** CoreMark Test Start\r\n");

    CoreMark();

    ee_printf("** CoreMark Test Stop\r\n");

    while (1);
}

void APP_Clock_Init(void) {
    // Latency = 1 for 48 MHz
    //FLASH->ACR |= FLASH_ACR_LATENCY;
    
    //FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    
    // div2 for 24MHz
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE_Msk) | RCC_CFGR_HPRE_DIV2;

    // HSI48 ON
    RCC->CR2 |= RCC_CR2_HSI48ON;
    while ( (RCC->CR2 & RCC_CR2_HSI48RDY) == 0 );
        
    RCC->CFGR |= RCC_CFGR_SW_HSI48;
    while ( (RCC->CFGR & RCC_CFGR_SWS_HSI48) != RCC_CFGR_SWS_HSI48);
        
    // HSI8 disable
    RCC->CR &= ~RCC_CR_HSION;

    
    SystemCoreClockUpdate();

    SysTick_Init(SYSTICK_1MS);
}


/*******************************************************************************
* Function Name  : GPIO_Configuration
*******************************************************************************/
void APP_GPIO_Init(void)
{
    // enable ckock GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    
    // enable MCO as SYSCLK    
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCO_Msk) | RCC_CFGR_MCO_SYSCLK;  
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOPRE_Msk) | RCC_CFGR_MCOPRE_DIV1;  

    //  PA8 for MCO - output  pushpull - pull-up  
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODER8_Msk) | (2 << GPIO_MODER_MODER8_Pos); 
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~GPIO_OSPEEDR_OSPEEDR8_Msk) | (3 << GPIO_OSPEEDR_OSPEEDR8_Pos);
    //GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPDR8_Msk) | (1 << GPIO_PUPDR_PUPDR8_Pos);
    //GPIOA->AFR[1] = (GPIOA->AFR[1] & ~GPIO_AFRH_AFSEL8_Msk) | (0 << GPIO_AFRH_AFSEL8_Pos);

    // add io  PA9  - output  pushpull - no pull-up no pull-down 
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODER9_Msk) | (1 << GPIO_MODER_MODER9_Pos); 
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~GPIO_OSPEEDR_OSPEEDR9_Msk) | (3 << GPIO_OSPEEDR_OSPEEDR9_Pos);
    //GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPDR9_Msk) | (1 << GPIO_PUPDR_PUPDR9_Pos);
}

void APP_USART_Init(void) {
    // GPIOA Clock enable
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Включаем тактирование USART2

    // 2. Настройка GPIO выводов
    // Сбрасываем настройки режима для PA2 и PA3
    GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);
    // Устанавливаем альтернативную функцию для PA2 и PA3
    GPIOA->MODER |= (0x02 << GPIO_MODER_MODER2_Pos) | (0x02 << GPIO_MODER_MODER3_Pos);
    // Настраиваем тип альтернативной функции (AF7 для USART2 на STM32F4)
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk);
    GPIOA->AFR[0] |= (0x01 << GPIO_AFRL_AFSEL2_Pos) | (0x01 << GPIO_AFRL_AFSEL3_Pos);

    // 3. Настройка параметров UART
    // Сбрасываем регистры USART2 в исходное состояние
    USART2->CR1 = 0;
    USART2->CR2 = 0;
    USART2->CR3 = 0;

    // Устанавливаем скорость передачи (бод)
    // Формула: BRR = PCLK1 / baudrate
    USART2->BRR = SystemCoreClock/115200; 

    // Включаем приёмник и передатчик, 8 бит данных, 1 стоп-бит, без контроля четности
    USART2->CR1 |= USART_CR1_RE | USART_CR1_TE;

    // 4. Включение UART
    USART2->CR1 |= USART_CR1_UE;
}

// Отправка одного байта (блокирующий режим)
void UART_SendByte(USART_TypeDef* USARTx, uint8_t data) {
    // Ждём, пока регистр передачи не освободится
    while (!(USARTx->ISR & USART_ISR_TXE));
    // Записываем данные в регистр данных
    USARTx->TDR = data;

    // Ждём завершения передачи
    while (!(USARTx->ISR & USART_ISR_TC));
}


/**
 * @brief  SysTick Init.
 * @param  NUM   Interrupt Time(us)
 */
void SysTick_Init(uint32_t NUM)
{
    /* SystemCoreClock / NUM */
    if (SysTick_Config(SystemCoreClock / NUM))
    {
        while (1)
            ;
    }
}

/**
 * @brief  SysTick_Stop_time.
 */
void SysTick_Stop_time(void)
{
    SysTick->CTRL &= SYSTICK_COUNTER_DIASBLE;
    /* Clear the SysTick Counter */
    SysTick->VAL = SYSTICK_COUNTER_CLEAR;
}


/*******************************************************************************
* Function Name  : Delay
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
