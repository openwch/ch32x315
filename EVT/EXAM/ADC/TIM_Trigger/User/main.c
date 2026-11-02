/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/10/19
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
*@Note
*External lines trigger ADC conversion routine:
*ADC channel 2 (PA4) - injection group channel, external trigger pin (PA15) high level triggers EXTI line 15 event,
*In this mode, an ADC conversion is triggered by an event on EXTI line 15, and a JEOC interrupt is generated after
*the conversion is completed.
*
*/


#include "debug.h"

/* Global define */
/* ADC Conversion Mode Definition*/
#define ADC_trig_Mode     0
#define ADC_inje_Mode     1

/* ADC Conversion Mode Selection*/
// #define ADC_MODE_Conversion   ADC_trig_Mode
#define ADC_MODE_Conversion   ADC_inje_Mode

/* Global Variable */
vu16 Adc_Val[2];
vu32 temp;

void ADC1_2_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));

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
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1, ENABLE );
	RCC_ADCCLKSourceConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCCLKConfig(RCC_ADCPRE_Div8,RCC_PPRE2_Div1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
#else
      ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;
#endif
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);


#if  (ADC_MODE_Conversion == ADC_inje_Mode)
	ADC_SampleTimeModeConfig(ADC1, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_2,ADC_SampleTime_CyclesMode7);
    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T2_CC1);  
    ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
#else
	ADC_SampleTimeModeConfig(ADC1, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_2,ADC_SampleTime_CyclesMode7);
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
#endif

    NVIC_EnableIRQ(ADC1_IRQn);
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
#else
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
#endif
	ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	
}
/*********************************************************************
 * @fn      TIM1_PWM_Init
 *
 * @brief   Initializes PWM.
 *
 * @return  none
 */

void TIM1_PWM_Init(u16 arr, u16 psc, u16 ccp)
{
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
#else
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
#endif
 
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
#if  (ADC_MODE_Conversion == ADC_inje_Mode)
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
#else
  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
#endif
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    TIM_Cmd(TIM2, ENABLE);
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
	USART_Printf_Init(115200);
	SystemCoreClockUpdate();
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n",DBGMCU_GetCHIPID() );
	TIM1_PWM_Init(7200,1000-1,3600);
	ADC_Function_Init();


	while(1);
}
void ADC1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      ADC1_2_IRQHandler
 *
 * @brief   ADC1_2 Interrupt Service Function.
 *
 * @return  none
 */
void ADC1_IRQHandler()
{
	u16 ADC_val;

#if  (ADC_MODE_Conversion == ADC_inje_Mode)
    if(ADC_GetITStatus( ADC1, ADC_IT_JEOC)){
               ADC_val = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
     #if 0
	    printf("ADC Extline trigger inje conversion...\r\n");
        printf("JADC %04d\r\n", ADC_val);
		#endif
    }

    ADC_ClearITPendingBit( ADC1, ADC_IT_JEOC);
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
#else
    if(ADC_GetITStatus( ADC1, ADC_IT_EOC)){
       
        ADC_val = ADC_GetConversionValue(ADC1);
		#if 0
		 printf("ADC Extline trigger conversion...\r\n");
        printf("ADC %04d\r\n", ADC_val);
		#endif
    }

    ADC_ClearITPendingBit( ADC1, ADC_IT_EOC);
#endif
}
