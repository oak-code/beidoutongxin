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
 
	SYSCLK_Configuration();	 //时钟初始化为72MHz 
	
	
	RTC_init();								//PCF8563系统时钟初始化
 	IO_Init();			     //LED端口初始化
	uart4_init(115200);	 //串口初始化为115200
	RDSS_uart_init(115200);//uart2
	TIM3_Int_Init(199,7199);				//20ms定时
	AT24Cxx_Init();//eeprom初始化
	Value_Init();
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,16,CAN_Mode_Normal);//CAN初始化环普通模式,波特率250Kbps   
	 
	RDSS_Init();
	IWDG_Init(4,625);//1s
 	while(1)
	{
		Can_Sever();					//CAN数据读取函数
		RDSS_DMA_data_processing();		//RDSS接收数据处理函数
		RDSS_Time_incident();			//RDSS时间事件处理函数
		LED_STA ();						//LED状态更改函数
		UART_Allocation();				//串口命令解析函数		
		IWDG_ReloadCounter();			//喂狗	
	}	 
 }
 
 
 
u16 RDSS_RXBUFF_LEN_old;
u16 main_time;
void TIM3_IRQHandler(void)//TIM3中断20ms  TIM3_IRQChannel   
{   				
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查TIM3更新中断发生与否	
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志 
		
		main_time++;
		if(main_time>6000)
			main_time=0;
//------------------------------1S计时-------------------------------		
//		if(main_time%50==0)
//		{
//			//LED_WARR_Flash;
//		}
		
//------------------------------搜索信号计时-----------------------------	
		if(RDSS.signal_sta==0)
		{
			RDSS.signal_Timeout--;
		}
		if(RDSS.signal_Timeout==0)//信号超时点亮报警灯
		{
			LED_WARR_ON;
			RDSS.signal_Timeout=3000*2;
		}
		
//------------------------------指示灯闪烁-----------------------------
		
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
		
//------------------------------RDSS定时部分----------------------------
		//模块服务频度
		if(RDSS.Server_Counter!=0)
		{
			RDSS.Server_Counter--;
		}
		//开机时间校准时间
		if(2000>=rtc.adjusting>0)
		{
			rtc.adjusting--;
		}
		//数据间隔
		if(RDSS_TX.RDSS_TX_DATA_Time != 0){
			RDSS_TX.RDSS_TX_DATA_Time --;
		}
		
		//检查信号频度
		if(RDSS.Read_Signal_Counter != 0){
			RDSS.Read_Signal_Counter --;
		}
		//获取定位频度
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

