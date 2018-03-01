#include "sys.h"
#include "eeprom.h"
#include "usart.h"
#include "RDSS.h"
#include <string.h>


 u16 rdss_rx_tim;
sys SYS;	

void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单?
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器  
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}


void IWDG_Init(u8 prer,u16 rlr) 
{	
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能对寄存器IWDG_PR和IWDG_RLR的写操作
	
	IWDG_SetPrescaler(prer);  //设置IWDG预分频值:设置IWDG预分频值为64
	
	IWDG_SetReload(rlr);  //设置IWDG重装载值
	
	IWDG_ReloadCounter();  //按照IWDG重装载寄存器的值重装载IWDG计数器
	
	IWDG_Enable();  //使能IWDG
}


//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}


void Value_Init(void)
{
	u8 EEP_state;

	EEP_state = AT24Cxx_ReadOneByte(EEP_EMPTY_FLAG_ADDR);
	if(EEP_state != EEP_EMPTY_FLAG)//这个EEP 是新的空的EEP,则写入默认值
	{
		AT24Cxx_WriteString(EEP_PRARM_Tongxin_ID,"0311886",8);		//通信ID号码
		AT24Cxx_WriteString(EEP_PRARM_ID,"00010001",9);				//本机ID号码
		AT24Cxx_WriteOneByte(EEP_EMPTY_FLAG_ADDR,EEP_EMPTY_FLAG);//写标志位		
	}
	
	{
	
	AT24Cxx_ReadString(EEP_PRARM_Tongxin_ID,RDSS_TX.RDSS_TX_ADDR,7);
	AT24Cxx_ReadString(EEP_PRARM_ID,SYS.EQID,8);
//#if	_Debug
//	for(i=0;i<7;i++)
//		printf("%c\r\n",RDSS_TX.RDSS_TX_ADDR[i]);
//#endif
	}
}

void UART_Allocation(void)
{
	u8 UART_CMD[5]={0},UART_DATA[128];
	u8 i;
	if((USART_RX_STA&0x8000))//接收完成
	{
		if(USART_RX_BUF[0]=='$')
		{
			RDSS_Send_TXBuff(USART_RX_BUF,USART_RX_STA&0X3FFF);
			while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
				USART2->DR = 0X0D;
			while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
				USART2->DR =0X0A;
		}
		if(USART_RX_BUF[0]=='#')
		{
			for(i=0;i<4;i++)
			UART_CMD[i]=USART_RX_BUF[i+1];

			if(strcmp((char *)UART_CMD,"TXID")==0)
			{
				#if	_Debug
					printf("更改北斗通信ID为：");
				#endif
				for(i=0;i<(USART_RX_STA&0X3FFF)-6;i++)
				{
					UART_DATA[i]=USART_RX_BUF[i+6];
				#if	_Debug
						printf("%c",UART_DATA[i]);
				#endif
				}
				#if	_Debug
					printf("\r\n");
				#endif
				AT24Cxx_WriteString(EEP_PRARM_Tongxin_ID,UART_DATA,8);		//通信ID号码		
			}
			if(strcmp((char *)UART_CMD,"EQID")==0)
			{
				#if	_Debug
					printf("更改设备ID为：");
				#endif
				for(i=0;i<(USART_RX_STA&0X3FFF)-6;i++)
				{
					UART_DATA[i]=USART_RX_BUF[i+6];
				#if	_Debug
						printf("%c",UART_DATA[i]);
				#endif
				}
				#if	_Debug
					printf("\r\n");
				#endif
				AT24Cxx_WriteString(EEP_PRARM_ID,UART_DATA,8);		//通信ID号码		
			}
			
			
		}
		USART_RX_STA=0;			
	}	

}
	

