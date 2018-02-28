#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "RDSS.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//CAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.1 
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved	
//********************************************************************************
//V1.1修改说明 20150528
//修正了CAN初始化函数的相关注释，更正了波特率计算公式
////////////////////////////////////////////////////////////////////////////////// 	 
 
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;  tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO

	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;			//非时间触发通信模式  
	CAN_InitStructure.CAN_ABOM=DISABLE;			//软件自动离线管理	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=ENABLE;			//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP=DISABLE;			//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode= mode;	        //模式设置： mode:0,普通模式;1,回环模式; 
	//设置波特率
	CAN_InitStructure.CAN_SJW=tsjw;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        //分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	//初始化CAN1 

	CAN_FilterInitStructure.CAN_FilterNumber=0;	//过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32位宽 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	//32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//激活过滤器0

	CAN_FilterInit(&CAN_FilterInitStructure);			//滤波器初始化
	
#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0消息挂号中断允许.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	int i=0;
    CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<8;i++)
	printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif


/*=========================================================================
函数名称: Can_Sever()
功能描述: can监听数据处理函数
输出参数: 
=========================================================================*/
void Can_Sever()
{
	u16 PUD;
	u8 i;
	CanRxMsg RxMessage;
	
	
	
	if( CAN_MessagePending(CAN1,CAN_FIFO0)!=0) 		//没有接收到数据,直接退出 
	{

		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
		PUD = (u16)(RxMessage.ExtId >> 8);
		
//		#if	_Debug
//		printf("\r\n=-=-=-=-=-=-=-=-=-=-=-=-=-CAN数据-=-=-=-=-=-=-=-=-=-=-=-=-\r\n");
//		printf("帧ID：%x\r\n",PUD);
//		
//		printf("数据：%x\r\n",PUD);
//		for(i=0;i<8;i++)
//		printf("%x",RxMessage.Data[i]);
//		
//		printf("\r\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n");
//		#endif
		switch(PUD)
		{
			
		

			case PDU_PaiQI1:	//排温1
				
				RDSS_TX.RDSS_TX_BUFF1[1]=RxMessage.Data[0];//排温1
				RDSS_TX.RDSS_TX_BUFF1[2]=RxMessage.Data[1];//排温1
				RDSS_TX.RDSS_TX_BUFF1[3]=RxMessage.Data[2];//排温2
				RDSS_TX.RDSS_TX_BUFF1[4]=RxMessage.Data[3];//排温2
				RDSS_TX.RDSS_TX_BUFF1[5]=RxMessage.Data[4];//排温3
				RDSS_TX.RDSS_TX_BUFF1[6]=RxMessage.Data[5];//排温3
				RDSS_TX.RDSS_TX_BUFF1[7]=RxMessage.Data[6];//排温4
				RDSS_TX.RDSS_TX_BUFF1[8]=RxMessage.Data[7];//排温4
						
				break;
			case PDU_PaiQI2:	//排温2
				
				RDSS_TX.RDSS_TX_BUFF1[9]=RxMessage.Data[0];//排温5
				RDSS_TX.RDSS_TX_BUFF1[10]=RxMessage.Data[1];//排温5
				RDSS_TX.RDSS_TX_BUFF1[11]=RxMessage.Data[2];//排温6
				RDSS_TX.RDSS_TX_BUFF1[12]=RxMessage.Data[3];//排温6
				RDSS_TX.RDSS_TX_BUFF1[13]=RxMessage.Data[4];//排温7
				RDSS_TX.RDSS_TX_BUFF1[14]=RxMessage.Data[5];//排温7
				RDSS_TX.RDSS_TX_BUFF1[15]=RxMessage.Data[6];//排温8
				RDSS_TX.RDSS_TX_BUFF1[16]=RxMessage.Data[7];//排温8
						
				break;
			case PDU_PaiQI3:	//排温3
				
				RDSS_TX.RDSS_TX_BUFF1[17]=RxMessage.Data[0];//排温9
				RDSS_TX.RDSS_TX_BUFF1[18]=RxMessage.Data[1];//排温9
				RDSS_TX.RDSS_TX_BUFF1[19]=RxMessage.Data[2];//排温10
				RDSS_TX.RDSS_TX_BUFF1[20]=RxMessage.Data[3];//排温10
				RDSS_TX.RDSS_TX_BUFF1[21]=RxMessage.Data[4];//排温11
				RDSS_TX.RDSS_TX_BUFF1[22]=RxMessage.Data[5];//排温11
				RDSS_TX.RDSS_TX_BUFF1[23]=RxMessage.Data[6];//排温12
				RDSS_TX.RDSS_TX_BUFF1[24]=RxMessage.Data[7];//排温12
						
				break;
			case PDU_ZhouWen1:	//轴温1-8
				
				RDSS_TX.RDSS_TX_BUFF1[25]=RxMessage.Data[0];//轴温1
				RDSS_TX.RDSS_TX_BUFF1[26]=RxMessage.Data[1];//轴温2
				RDSS_TX.RDSS_TX_BUFF1[27]=RxMessage.Data[2];//轴温3
				RDSS_TX.RDSS_TX_BUFF1[28]=RxMessage.Data[3];//轴温4
				RDSS_TX.RDSS_TX_BUFF1[29]=RxMessage.Data[4];//轴温5
				RDSS_TX.RDSS_TX_BUFF1[30]=RxMessage.Data[5];//轴温6
				RDSS_TX.RDSS_TX_BUFF1[31]=RxMessage.Data[6];//轴温7
				RDSS_TX.RDSS_TX_BUFF1[32]=RxMessage.Data[7];//轴温8
						
				break;
			case PDU_ZhouWen2:	//轴温9
				
				RDSS_TX.RDSS_TX_BUFF1[33]=RxMessage.Data[0];//轴温9
		
				break;
			case PDU_ZhuanSu:		//转速
		
				RDSS_TX.RDSS_TX_BUFF1[34]=RxMessage.Data[3];
				RDSS_TX.RDSS_TX_BUFF1[35]=RxMessage.Data[4];
			
				break;
			
			case PDU_RanYouYaLi:	//燃油压机油压力
				
				RDSS_TX.RDSS_TX_BUFF1[38]=RxMessage.Data[0];//燃油压力
				RDSS_TX.RDSS_TX_BUFF1[39]=RxMessage.Data[1];//燃油压力
				RDSS_TX.RDSS_TX_BUFF1[40]=RxMessage.Data[3];//机油压力
				RDSS_TX.RDSS_TX_BUFF1[41]=RxMessage.Data[4];//机油压力	
				RDSS_TX.RDSS_TX_BUFF1[44]=RxMessage.Data[3];//安保机油压力
				RDSS_TX.RDSS_TX_BUFF1[45]=RxMessage.Data[4];//安保机油压力			
				break;
			case PDU_YouShuiWen:	//油水温
				
				RDSS_TX.RDSS_TX_BUFF1[43]=RxMessage.Data[0];//冷却水温
				RDSS_TX.RDSS_TX_BUFF1[42]=RxMessage.Data[3];//机油温度
						
				break;
			case PDU_WenDu:	//安保转速、涡轮机油温度、增压温度、扩展温度
				
				RDSS_TX.RDSS_TX_BUFF1[36]=RxMessage.Data[0];//安保转速
				RDSS_TX.RDSS_TX_BUFF1[37]=RxMessage.Data[1];//安保转速
				RDSS_TX.RDSS_TX_BUFF1[54]=RxMessage.Data[2];//涡轮机油温度
				RDSS_TX.RDSS_TX_BUFF1[55]=RxMessage.Data[3];//增压温度
				RDSS_TX.RDSS_TX_BUFF1[56]=RxMessage.Data[4];//扩展温度
			
						
				break;
			case PDU_YaLi:	//水压、燃油压力、起动空气压力、增压空气压力
				
				RDSS_TX.RDSS_TX_BUFF1[52]=RxMessage.Data[0];//冷却水压
				RDSS_TX.RDSS_TX_BUFF1[53]=RxMessage.Data[1];//冷却水压
				RDSS_TX.RDSS_TX_BUFF1[46]=RxMessage.Data[2];//涡轮机油压
				RDSS_TX.RDSS_TX_BUFF1[47]=RxMessage.Data[3];//涡轮机油压
				RDSS_TX.RDSS_TX_BUFF1[48]=RxMessage.Data[4];//启动空气压
				RDSS_TX.RDSS_TX_BUFF1[49]=RxMessage.Data[5];//启动空气压
				RDSS_TX.RDSS_TX_BUFF1[50]=RxMessage.Data[6];//增压空气压
				RDSS_TX.RDSS_TX_BUFF1[51]=RxMessage.Data[7];//增压空气压
			
						
				break;
		
			case PDU_DianYa:	//电压
				
				RDSS_TX.RDSS_TX_BUFF1[57]=RxMessage.Data[0];//主电源电压
				RDSS_TX.RDSS_TX_BUFF1[58]=RxMessage.Data[1];//主电源电压
				RDSS_TX.RDSS_TX_BUFF1[59]=RxMessage.Data[2];//备用电源电压
				RDSS_TX.RDSS_TX_BUFF1[60]=RxMessage.Data[3];//备用电源电压

						
				break;
			case PDU_YunXing:	//累计运行时间
				
				RDSS_TX.RDSS_TX_BUFF1[61]=RxMessage.Data[0];
				RDSS_TX.RDSS_TX_BUFF1[62]=RxMessage.Data[1];
				RDSS_TX.RDSS_TX_BUFF1[63]=RxMessage.Data[2];
		
				break;
				
			case PDU_GuZhang:	//故障
				
				RDSS_TX.RDSS_TX_BUFF2[1]=RxMessage.Data[0];//普通传感器故障
				RDSS_TX.RDSS_TX_BUFF2[2]=RxMessage.Data[1];//普通传感器故障
				RDSS_TX.RDSS_TX_BUFF2[3]=RxMessage.Data[2];//普通传感器故障
				RDSS_TX.RDSS_TX_BUFF2[4]=RxMessage.Data[3];//普通传感器故障
				RDSS_TX.RDSS_TX_BUFF2[5]=RxMessage.Data[4];//排温传感器故障
				RDSS_TX.RDSS_TX_BUFF2[6]=RxMessage.Data[5];//排温传感器故障
				RDSS_TX.RDSS_TX_BUFF2[7]=RxMessage.Data[6];//轴温传感器故障
				RDSS_TX.RDSS_TX_BUFF2[8]=RxMessage.Data[7];//轴温传感器故障
						
				break;
			case PDU_BaoJing:	//报警
				
				RDSS_TX.RDSS_TX_BUFF2[9]=RxMessage.Data[0];//普通传感器报警
				RDSS_TX.RDSS_TX_BUFF2[10]=RxMessage.Data[1];//普通传感器报警
				RDSS_TX.RDSS_TX_BUFF2[11]=RxMessage.Data[2];//普通传感器报警
				RDSS_TX.RDSS_TX_BUFF2[12]=RxMessage.Data[3];//普通传感器报警

						
				break;
			
			case PDU_PaiwenZhouwen:	//排温报警
				
				RDSS_TX.RDSS_TX_BUFF2[13]=RxMessage.Data[0];//排温传感器报警
				RDSS_TX.RDSS_TX_BUFF2[14]=RxMessage.Data[1];//排温传感器报警
				RDSS_TX.RDSS_TX_BUFF2[15]=RxMessage.Data[2];//排温传感器报警
				RDSS_TX.RDSS_TX_BUFF2[16]=RxMessage.Data[3];//排温传感器报警
				RDSS_TX.RDSS_TX_BUFF2[17]=RxMessage.Data[4];//轴温传感器报警
				RDSS_TX.RDSS_TX_BUFF2[18]=RxMessage.Data[5];//轴温传感器报警
				break;
			
			case PDU_KaiGuanLiang:	//开关量
				
				RDSS_TX.RDSS_TX_BUFF2[19]=RxMessage.Data[0];//
				RDSS_TX.RDSS_TX_BUFF2[20]=RxMessage.Data[1];//
		
				break;
			
			
			case PDU_FengLang:	//越控模式、风浪模式、遥控模式
				
				RDSS_TX.RDSS_TX_BUFF2[22]=RxMessage.Data[0];//越控模式
				RDSS_TX.RDSS_TX_BUFF2[21]=RxMessage.Data[1];//风浪模式
				RDSS_TX.RDSS_TX_BUFF2[23]=RxMessage.Data[2];//遥控模式
						
				break;

			default:
				break;
		}
	}
	
	RDSS_TX.RDSS_TX_BUFF1[0]='%';
	RDSS_TX.RDSS_TX_BUFF2[0]='#';


	RDSS_TX.RDSS_TX_BUFF1[64]=latitude[0];
	RDSS_TX.RDSS_TX_BUFF1[65]=latitude[1];
	RDSS_TX.RDSS_TX_BUFF1[66]=latitude[2];
	RDSS_TX.RDSS_TX_BUFF1[67]=latitude[3];
	RDSS_TX.RDSS_TX_BUFF1[68]=latitude[4];//纬度
	RDSS_TX.RDSS_TX_BUFF1[69]=longitude[0];
	RDSS_TX.RDSS_TX_BUFF1[70]=longitude[1];
	RDSS_TX.RDSS_TX_BUFF1[71]=longitude[2];
	RDSS_TX.RDSS_TX_BUFF1[72]=longitude[3];
	RDSS_TX.RDSS_TX_BUFF1[73]=longitude[4];//经度
	
	RDSS_TX.RDSS_TXBUFF1_LEN=74;
	RDSS_TX.RDSS_TXBUFF2_LEN=24;
}













