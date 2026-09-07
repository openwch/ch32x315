/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/08/20
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *ADC DMA sampling routines:
 *ADC channel 1 (PA3), the rule group channel obtains ADC conversion data
 *for 1024 consecutive times through DMA.
 *ADC3 channel 10 (PD4) channel 11 (PD5), the rule group channel obtains ADC conversion data
 *for 1024 consecutive times through DMA.
 */

#include "debug.h"

/* Global define */
/* ADC  Mode Definition*/
#define ADC1_MODE   0
#define ADC3_MODE     1

/* ADC Mode Selection*/
// #define ADC_MODE   ADC1_MODE
#define ADC_MODE   ADC3_MODE
/* Global Variable */
u16 TxBuf[1024];

/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void ADC_Function_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure={0};
	GPIO_InitTypeDef GPIO_InitStructure={0};

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE );
	#if  (ADC_MODE == ADC1_MODE)
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1, ENABLE );
	#else
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC3, ENABLE );	

	if((DBGMCU_GetCHIPID()&0x000000F0)==0)
	{
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1, ENABLE );
	}
	#endif
	RCC_ADCCLKSourceConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCCLKConfig(RCC_ADCPRE_Div2,RCC_PPRE2_Div8);
#if  (ADC_MODE == ADC1_MODE)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
#else

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	ADC_DeInit(ADC3);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init(ADC3, &ADC_InitStructure);

	ADC_DMACmd(ADC3, ENABLE);
	ADC_Cmd(ADC3, ENABLE);
	if((DBGMCU_GetCHIPID()&0x000000F0)==0)
	{
	  ADC_Cmd(ADC1, ENABLE);
	}
	ADC_BufferCmd(ADC3, DISABLE);   //disable buffer
	ADC_ResetCalibration(ADC3);
	while(ADC_GetResetCalibrationStatus(ADC3));
	ADC_StartCalibration(ADC3);
	while(ADC_GetCalibrationStatus(ADC3));
#endif
	
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
	DMA_InitTypeDef DMA_InitStructure={0};

	RCC_HBPeriphClockCmd( RCC_HBPeriph_DMA1, ENABLE );

	DMA_DeInit(DMA_CHx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
	DMA_InitStructure.DMA_Memory0BaseAddr = memadr;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = bufsize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA_CHx, &DMA_InitStructure );
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
    u16 i = 0;

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);	
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    ADC_Function_Init();
	#if  (ADC_MODE == ADC1_MODE)
	DMA_Tx_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)TxBuf, 1024 );
	DMA_Cmd( DMA1_Channel1, ENABLE );
	ADC_SampleTimeModeConfig(ADC1, ADC_Channel_1, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_1,ADC_SampleTime_CyclesMode7);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET) /* Wait until ADC1 DMA1 Transfer Complete */
    {

    }
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
#else
	DMA_Tx_Init( DMA1_Channel9, (u32)&ADC3->RDATAR, (u32)TxBuf, 1024 );
	DMA_Cmd( DMA1_Channel9, ENABLE );
	ADC_SampleTimeModeConfig(ADC3, ADC_Channel_10, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1);
	ADC_SetMode0SampleTimeConfig(ADC3, ADC_Channel_10,ADC_SampleTime_CyclesMode7);
	ADC_SampleTimeModeConfig(ADC3, ADC_Channel_11, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_11,2);
	ADC_SetMode0SampleTimeConfig(ADC3, ADC_Channel_11,ADC_SampleTime_CyclesMode7);
	ADC_SoftwareStartConvCmd(ADC3, ENABLE);
    while(DMA_GetFlagStatus(DMA1_FLAG_TC9) == RESET) /* Wait until ADC1 DMA1 Transfer Complete */
    {

    }
	 ADC_SoftwareStartConvCmd(ADC3, DISABLE);
#endif

	for(i=0; i<1024; i++)
	{
		printf( "%04d\r\n", TxBuf[i]);
		Delay_Ms(10);
	}


    while(1);
}
