#include "delay.h"


	
void SYSCLK_Configuration(void)
{
	RCC_DeInit() ;
	RCC_HSEConfig(RCC_HSE_ON);//配置外部高速晶振
	while(!RCC_WaitForHSEStartUp()); //等待HSE起振
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);	//选择PLL的时钟源和倍频数8*9 = 72MHz
	RCC_PLLCmd(ENABLE);//使能PLL
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //设置系统时钟（SYSCLK?
}
			   
/*************************************************************
	主频在32MHz，情况下ms级延时，
*************************************************************/
void delay_ms(u16 nms){
	u16 i;
	while(nms --){
		i = 10000;
		while(i--);
	}	
}

/*************************************************************
	主频在32MHz，情况下us级延时，
*************************************************************/
void delay_us(u32 nus){
	u16 i;
	while(nus --){
		i = 10;
		while(i--);
	}
}






































