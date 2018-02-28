#include "led.h"
#include "RDSS.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.5 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOE,GPIO_Pin_5); 						 //PE.5 ����� 
}
 
void IO_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);	 //ʹ��PB,PD�˿�ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;							
	GPIO_Init(GPIOB, &GPIO_InitStructure);									
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	//����ʹ�����ţ��ߵ�ƽʹ��							
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	//����ʹ�����ţ��ߵ�ƽʹ��							
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//����ʹ�����ţ��ߵ�ƽʹ��							
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;									
	GPIO_Init(GPIOD, &GPIO_InitStructure);									

	
	LED_GPS_ON;
	LED_WARR_ON;
	LED_DAT_ON;
	LED_PWR_ON;
	Bee_ON;	
	RDSS_PWR_ON;		//ʹ�ܱ���ģ��
	RDSS_TXEN_ON;//����ģ�鷢��ʹ�����ţ��͵�ƽʹ��
	RDSS_RXEN_ON;//����ģ�����ʹ�����ţ��͵�ƽʹ��
	delay_ms(500);
	LED_GPS_OFF;
	LED_WARR_OFF;
	LED_DAT_OFF;
	Bee_OFF;


 }

 u8 LED_Flash_sta;
 u8 LED_DAT_sta=0;
void LED_STA (void)
{
	if(RDSS.signal_sta)
	{
		LED_GPS_ON;
		
	}
	else LED_Flash_sta=4;
	
	if(RDSS_TX.RDSS_TX_DATA_Time==0)LED_DAT_OFF;
//	LED_DAT_Flash;
//	LED_WARR_Flash;	
}
