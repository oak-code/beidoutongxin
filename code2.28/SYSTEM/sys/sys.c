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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������?
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���  
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx					 
}


void IWDG_Init(u8 prer,u16 rlr) 
{	
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
	
	IWDG_SetPrescaler(prer);  //����IWDGԤ��Ƶֵ:����IWDGԤ��ƵֵΪ64
	
	IWDG_SetReload(rlr);  //����IWDG��װ��ֵ
	
	IWDG_ReloadCounter();  //����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
	
	IWDG_Enable();  //ʹ��IWDG
}


//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}


void Value_Init(void)
{
	u8 EEP_state;

	EEP_state = AT24Cxx_ReadOneByte(EEP_EMPTY_FLAG_ADDR);
	if(EEP_state != EEP_EMPTY_FLAG)//���EEP ���µĿյ�EEP,��д��Ĭ��ֵ
	{
		AT24Cxx_WriteString(EEP_PRARM_Tongxin_ID,"0311886",8);		//ͨ��ID����
		AT24Cxx_WriteString(EEP_PRARM_ID,"00010001",9);				//����ID����
		AT24Cxx_WriteOneByte(EEP_EMPTY_FLAG_ADDR,EEP_EMPTY_FLAG);//д��־λ		
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
	if((USART_RX_STA&0x8000))//�������
	{
		if(USART_RX_BUF[0]=='$')
		{
			RDSS_Send_TXBuff(USART_RX_BUF,USART_RX_STA&0X3FFF);
			while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
				USART2->DR = 0X0D;
			while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
				USART2->DR =0X0A;
		}
		if(USART_RX_BUF[0]=='#')
		{
			for(i=0;i<4;i++)
			UART_CMD[i]=USART_RX_BUF[i+1];

			if(strcmp((char *)UART_CMD,"TXID")==0)
			{
				#if	_Debug
					printf("���ı���ͨ��IDΪ��");
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
				AT24Cxx_WriteString(EEP_PRARM_Tongxin_ID,UART_DATA,8);		//ͨ��ID����		
			}
			if(strcmp((char *)UART_CMD,"EQID")==0)
			{
				#if	_Debug
					printf("�����豸IDΪ��");
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
				AT24Cxx_WriteString(EEP_PRARM_ID,UART_DATA,8);		//ͨ��ID����		
			}
			
			
		}
		USART_RX_STA=0;			
	}	

}
	

