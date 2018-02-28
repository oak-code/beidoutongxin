#include "delay.h"


	
void SYSCLK_Configuration(void)
{
	RCC_DeInit() ;
	RCC_HSEConfig(RCC_HSE_ON);//�����ⲿ���پ���
	while(!RCC_WaitForHSEStartUp()); //�ȴ�HSE����
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);	//ѡ��PLL��ʱ��Դ�ͱ�Ƶ��8*9 = 72MHz
	RCC_PLLCmd(ENABLE);//ʹ��PLL
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //����ϵͳʱ�ӣ�SYSCLK?
}
			   
/*************************************************************
	��Ƶ��32MHz�������ms����ʱ��
*************************************************************/
void delay_ms(u16 nms){
	u16 i;
	while(nms --){
		i = 10000;
		while(i--);
	}	
}

/*************************************************************
	��Ƶ��32MHz�������us����ʱ��
*************************************************************/
void delay_us(u32 nus){
	u16 i;
	while(nus --){
		i = 10;
		while(i--);
	}
}






































