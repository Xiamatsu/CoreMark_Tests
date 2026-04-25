/******************************************************************************
* Include files                                                              *
******************************************************************************/
#include "cw32l031.h"


/******************************************************************************
 ** System Clock Frequency (Core Clock) Variable according CMSIS
 ******************************************************************************/

uint32_t SystemCoreClock = 67840000;

void SystemCoreClockUpdate(void) // Update SystemCoreClock variable
{
    SystemCoreClock = 67840000;
}

/**
 ******************************************************************************
 ** \brief  Setup the microcontroller system. Initialize the System and update
 ** the SystemCoreClock variable.
 **
 ** \param  none
 ** \return none
 ******************************************************************************/
void SystemInit(void)
{
    // Load TrimCode for HSI - 48 MHz
    CW_SYSCTRL->HSI_f.TRIM = *((volatile uint16_t *)0x00100A00U);

    CW_SYSCTRL->AHBEN_f.FLASH = ENABLE;
    //CW_FLASH->CR2 = 0x5A5A0000; // 0 ws  
    //CW_FLASH->CR2 = 0x5A5A0001; // 1 ws  
    CW_FLASH->CR2 = 0x5A5A0002; // 2 ws  
    //CW_FLASH->CR2 = 0x5A5A0003; // 3 ws  

    //  min HSI = ?? MHz
    //CW_SYSCTRL->HSI_f.TRIM = 0x000;
    //  max HSI = 67,84 MHz
    CW_SYSCTRL->HSI_f.TRIM = 0x7FF;

    CW_SYSCTRL->HSI_f.DIV = 6; // :1 => 48MHz
    //CW_SYSCTRL->HSI_f.DIV = 8; // :2 => 24MHz
    //CW_SYSCTRL->HSI_f.DIV = 9; // :4 => 12MHz
    //CW_SYSCTRL->HSI_f.DIV = 5; // :6 =>  8MHz
    

    //Load TrimCode for LSI
    //...
    //CW_SYSCTRL->LSI_f.TRIM = *((volatile uint16_t *)0x00100A02U);

    SystemCoreClockUpdate();

}

void FirmwareDelay(uint32_t DlyCnt )
{
    volatile uint32_t thisCnt = DlyCnt;
    while( thisCnt-- )
    {
        ;
    }
}

/**
 * \brief   clear memory
 *
 * \param   [in]  start addr
 * \param   [in]  memory size(byte)
 * \retval  void
 */
void MemClr(void *pu8Address, uint32_t u32Count)
{
    uint8_t *pu8Addr = (uint8_t *)pu8Address;

    if(NULL == pu8Addr)
    {
        return;
    }

    while (u32Count--)
    {
        *pu8Addr++ = 0;
    }
}

