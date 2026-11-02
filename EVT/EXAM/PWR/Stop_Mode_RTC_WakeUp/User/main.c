/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/10/14
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 * RTC timed wake up stop mode routine:
 * This example demonstrates periodic wake-up from Stop mode using the RTC timer. 
 * After setting the alarm time, the system enters Stop mode. Approximately 5 seconds later, 
 * the RTC automatically wakes up the system, which resumes execution, and the cycle continues. 
 * The internal LSI oscillator is used as the RTC clock source.
 * For the small package model of the chip, there are some pins that have not been 
 * led out compared to the largest package, or some pins that have been packaged but 
 * not used. These pins need to be set as pull-down inputs to reduce current consumption. 
 */

#include "debug.h"

/* Global define */

/* definition of execute with WFI or WFE */
#define  Entry_WFI   0
#define  Entry_WFE   1
// #define  Entry_Method  Entry_WFI
#define  Entry_Method  Entry_WFE

/* Global Variable */

/*********************************************************************
 * @fn      RTC_Alarm_WakeUp
 *
 * @brief  Initializes RTC alarm wake-up function.
 *
 * @return  none
 */
void RTC_Alarm_WakeUp(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    RTC_ClearITPendingBit(RTC_IT_ALR);
    RTC_ClearITPendingBit(RTC_IT_SEC);
    /* Enable LSI oscillator */
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
    RTC_RTCCLKConfig(RTC_RTCCLKSource_LSI);

    /* Enable the RTC clock */
    RTC_RTCCLKCmd(ENABLE);
    RTC_WaitForLastTask();
    RTC_WaitForSynchro();
    /* Enable alarm interrupt */
    RTC_ITConfig( RTC_IT_ALR, ENABLE );
    RTC_WaitForLastTask();
    RTC_EnterConfigMode();
    /* Set the RTC prescaler value */
    RTC_SetPrescaler(39999);
    RTC_WaitForLastTask();
    RTC_SetCounter(0);
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();
#if(Entry_Method == Entry_WFI)
    /* Enable the RTC alarm interrupt */
    NVIC_SetPriority(RTCAlarm_IRQn, 1);
    NVIC_EnableIRQ(RTCAlarm_IRQn);
#endif
    /* Configure EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line20;
#if(Entry_Method == Entry_WFI)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

/*********************************************************************
 * @fn      All_GPIO_IPD_Init
 *
 * @brief   To reduce power consumption, unused I/O is configured in pull-down input mode.
 *
 * @return  none
 */
void All_GPIO_IPD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC |
                          RCC_HB2Periph_GPIOD | RCC_HB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    GPIO_Init( GPIOB, &GPIO_InitStructure );
    GPIO_Init( GPIOC, &GPIO_InitStructure );
    GPIO_Init( GPIOD, &GPIO_InitStructure );
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    uint32_t tmp = 0;
    SystemCoreClockUpdate();
    Delay_Init();
    Delay_Ms(1000);
    All_GPIO_IPD_Init();
    USART_Printf_Init(115200);	
    printf("SystemClk:%u\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    /* Enable the power regulator preset function. Adjust the configuration values of 
    LDO_VDD12A/LDO_VDDK to reduce the voltage, keeping the LDO in a low-voltage state during Stop mode, 
    thereby lowering the chip's power consumption current. When the power preset function is enabled, 
    the LDO_VDD12A/LDO_VDDK configuration values take effect only during Stop mode.
       Note: If the preset function is disabled after wake-up, ensure that LDO_VDD12A/LDO_VDDK have been switched 
    back to their reset values before doing so.*/
    PWR_PLANCmd(ENABLE);
    tmp = PWR->CTLR;
    tmp &= ~(0x3f << 8);
    tmp |= (0x1b << 8);
    PWR->CTLR = tmp;

    RTC_Alarm_WakeUp();

    while(1)
    {
        printf(">>> enter stop mode\r\n");
        Delay_Ms(10);
        /* Set the RTC alarm value */
        RTC_SetAlarm(RTC_GetCounter()+5);
        RTC_WaitForLastTask();
        #if(Entry_Method == Entry_WFI)
           PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        #else
            PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
            SystemInit();
        #endif
        printf("wake up\r\n\r\n");
        Delay_Ms(2000);
    }
}

#if(Entry_Method == Entry_WFI)
/*********************************************************************
 * @fn      RTCAlarm_IRQHandler
 *
 * @brief   This function handles RTCAlarm Handler.
 *
 * @return  none
 */
void RTCAlarm_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void RTCAlarm_IRQHandler(void)
{
    SystemInit();
    if(RTC_GetFlagStatus(RTC_FLAG_ALR) != RESET)
    {
        /* Clears EXTI line and alarm clock flag */
        EXTI_ClearITPendingBit(EXTI_Line20);
        RTC_ClearITPendingBit(RTC_IT_ALR); 
        RTC_WaitForLastTask();
    }
}
#endif