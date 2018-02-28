#include "led.h"
#include "RDSS.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.5 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOE,GPIO_Pin_5); 						 //PE.5 输出高 
}
 
void IO_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);	 //使能PB,PD端口时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;							
	GPIO_Init(GPIOB, &GPIO_InitStructure);									
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	//北斗使能引脚，高电平使能							
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	//北斗使能引脚，高电平使能							
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//北斗使能引脚，高电平使能							
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;									
	GPIO_Init(GPIOD, &GPIO_InitStructure);									

	
	LED_GPS_ON;
	LED_WARR_ON;
	LED_DAT_ON;
	LED_PWR_ON;
	Bee_ON;	
	RDSS_PWR_ON;		//使能北斗模块
	RDSS_TXEN_ON;//北斗模块发送使能引脚，低电平使能
	RDSS_RXEN_ON;//北斗模块接收使能引脚，低电平使能
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
