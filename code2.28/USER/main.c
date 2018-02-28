#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "RDSS.h"
#include "pcf8563.h" 
#include "eeprom.h"
#include "Can.h"
 int main(void)
 {		
 
	SYSCLK_Configuration();	 //ʱ�ӳ�ʼ��Ϊ72MHz 
	
	
	RTC_init();								//PCF8563ϵͳʱ�ӳ�ʼ��
 	IO_Init();			     //LED�˿ڳ�ʼ��
	uart4_init(115200);	 //���ڳ�ʼ��Ϊ115200
	RDSS_uart_init(115200);//uart2
	TIM3_Int_Init(199,7199);				//20ms��ʱ
	AT24Cxx_Init();//eeprom��ʼ��
	Value_Init();
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,16,CAN_Mode_Normal);//CAN��ʼ������ͨģʽ,������250Kbps   
	 
	RDSS_Init();
	IWDG_Init(4,625);//1s
 	while(1)
	{
		Can_Sever();					//CAN���ݶ�ȡ����
		RDSS_DMA_data_processing();		//RDSS�������ݴ�����
		RDSS_Time_incident();			//RDSSʱ���¼�������
		LED_STA ();						//LED״̬���ĺ���
		UART_Allocation();				//���������������		
		IWDG_ReloadCounter();			//ι��	
	}	 
 }
 
 
 
u16 RDSS_RXBUFF_LEN_old;
u16 main_time;
void TIM3_IRQHandler(void)//TIM3�ж�20ms  TIM3_IRQChannel   
{   				
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���TIM3�����жϷ������	
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־ 
		
		main_time++;
		if(main_time>6000)
			main_time=0;
//------------------------------1S��ʱ-------------------------------		
//		if(main_time%50==0)
//		{
//			//LED_WARR_Flash;
//		}
		
//------------------------------�����źż�ʱ-----------------------------	
		if(RDSS.signal_sta==0)
		{
			RDSS.signal_Timeout--;
		}
		if(RDSS.signal_Timeout==0)//�źų�ʱ����������
		{
			LED_WARR_ON;
			RDSS.signal_Timeout=3000*2;
		}
		
//------------------------------ָʾ����˸-----------------------------
		
		if(main_time%2==0)
		{
			if(RDSS.signal_sta==0)LED_GPS_Flash;
			
		
		}
		
		if(RDSS_TX.RDSS_TX_DATA_Time>=450)
		{
			if(RDSS_TX.RDSS_TX_DATA_Time%2==0)
			{
				LED_DAT_Flash;
			}	
		}
		else LED_DAT_OFF;	
		
//------------------------------RDSS��ʱ����----------------------------
		//ģ�����Ƶ��
		if(RDSS.Server_Counter!=0)
		{
			RDSS.Server_Counter--;
		}
		//����ʱ��У׼ʱ��
		if(2000>=rtc.adjusting>0)
		{
			rtc.adjusting--;
		}
		//���ݼ��
		if(RDSS_TX.RDSS_TX_DATA_Time != 0){
			RDSS_TX.RDSS_TX_DATA_Time --;
		}
		
		//����ź�Ƶ��
		if(RDSS.Read_Signal_Counter != 0){
			RDSS.Read_Signal_Counter --;
		}
		//��ȡ��λƵ��
		if(RDSS.Read_Location_Counter!= 0){
			RDSS.Read_Location_Counter --;
		}
//--------------------------------RDSS DMA time----------------------------
		RDSS_RXBUFF_LEN_old=RDSS_RX.RDSS_RXBUFF_LEN;
		RDSS_RX.RDSS_RXBUFF_LEN=RDSS_RX_RXBUFF_LEN-DMA_GetCurrDataCounter(DMA1_Channel6);
		if((RDSS_RX.RDSS_RXBUFF_LEN==RDSS_RXBUFF_LEN_old)&&(RDSS_RX.RDSS_RXBUFF_LEN!=0))
		{
			RDSS_RX.rdss_rx_timeout++;
			if(RDSS_RX.rdss_rx_timeout>100)
			{
				RDSS_RX.rdss_rx_timeout=100;
			}
		}
		else{
			RDSS_RX.rdss_rx_timeout=0;
		}
	
	}
}

//-------------------------------END-------------------------------------------	

