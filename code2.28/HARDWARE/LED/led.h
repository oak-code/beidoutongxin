#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
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
#define LED0 PBout(5)// PB5
#define LED1 PEout(5)// PE5	
#define LED_PWR			PBout(5)		//LED1
#define LED_DAT			PBout(3)		//LED2
#define LED_WARR		PDout(2)		//LED3
#define LED_GPS			PBout(4)		//LED4

#define Bee				PCout(12)	
#define RDSS_PWR		PAout(4)	//����ģ��ʹ�����ţ��ߵ�ƽʹ��
#define RDSS_TXEN		PAout(6)	//����ģ�鷢��ʹ�����ţ��͵�ƽʹ��
#define RDSS_RXEN		PAout(7)	//����ģ�����ʹ�����ţ��͵�ƽʹ��
#define GSM_KEY		PCout(9)	

#define RDSS_PWR_ON		RDSS_PWR = 1
#define RDSS_PWR_OFF	RDSS_PWR = 0
#define RDSS_TXEN_ON	RDSS_TXEN = 1
#define RDSS_TXEN_OFF	RDSS_TXEN = 0
#define RDSS_RXEN_ON	RDSS_RXEN = 1
#define RDSS_RXEN_OFF	RDSS_RXEN = 0

////////////////////////////////////////////////////////////////////////////////// 

extern u8 LED_Flash_sta;
extern u8 LED_DAT_sta;

#define LED_GPS_ON		LED_GPS = 1
#define LED_GPS_OFF		LED_GPS = 0
#define LED_GPS_Flash	LED_GPS = ~(LED_GPS)

#define LED_DAT_ON		LED_DAT = 1
#define LED_DAT_OFF		LED_DAT = 0
#define LED_DAT_Flash	LED_DAT = ~(LED_DAT)

#define LED_WARR_ON	LED_WARR = 1
#define LED_WARR_OFF	LED_WARR = 0
#define LED_WARR_Flash	LED_WARR = ~(LED_WARR)

#define LED_PWR_ON		LED_PWR = 1
#define LED_PWR_OFF	LED_PWR = 0
#define LED_PWR_Flash	LED_PWR = ~(LED_PWR)

#define Bee_ON		Bee = 1
#define Bee_OFF		Bee = 0
#define Bee_Flash	Bee = ~(Bee)


#define Key1_is_Down	(Bit_RESET == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9))

extern void IO_Init(void);
void LED_Init(void);//��ʼ��
void LED_STA (void);
		 				    
#endif
