#include "RDSS.h"
#include "delay.h"
#include "usart.h"
#include "pcf8563.h"
#include <string.h>
#include "led.h"

rdss RDSS;
rdss_rx RDSS_RX;
rdss_tx RDSS_TX;

rdss_data RDSS_DATA;
rdss_card RDSS_CADE;
	

u8 longitude[10];	//经度
u8 latitude[10];	//纬度



DMA_InitTypeDef DMA_InitStructure;
void DMAs_SetCurrDataCounter(DMA_Channel_TypeDef*DMA_CHx, uint16_t DataNumber){
	DMA_Cmd(DMA_CHx, DISABLE);  //关闭USART1 TX DMA1 所指示的通道 
 	DMA_SetCurrDataCounter(DMA_CHx, DataNumber);//1设置计数器值时必须关闭
	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}
void DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr){
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //4DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //4DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //4数据传输方向，从外设读取发送到内存
	DMA_InitStructure.DMA_BufferSize = cndtr;  //4DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在循环
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器	  
	DMA_Cmd(DMA_CHx, ENABLE);  // DMA通道 	
} 



void RDSS_uart_init(u32 bound){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART4时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOC时钟
	//USART4_TX   PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART4_RX	  PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	//UART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口

	DMA_Config(DMA1_Channel6,(u32)&USART2->DR,(u32)RDSS_RX.RDSS_RX_BUFF,RDSS_RX_RXBUFF_LEN);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);////使能接收DMA请求
	USART_Cmd(USART2, ENABLE);                    //使能串口 

}


void Send_Char(u8 ch){
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
	USART2->DR = (u8) ch;      
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
}

void Send_Buff(u8* buff,u16 len){
	u16 i;
	for(i = 0;i < len;i++){
		Send_Char(buff[i]);
	}
}

void Send_String(u8 *str){
	while(*str){
	    Send_Char(*str++);   
	}
}

/*=========================================================================
函数名称: FucCalcXor()
功能描述: 对数据进行异或运算             
输入参数: 
输出参数: 
=========================================================================*/
u8 FucCalcXor(u8 *P,u16 Len)
{
	u8 tmp=0;
	unsigned int i;
	for(i=0;i<Len;i++){
		tmp =tmp ^ *(P+i);
	}
	return tmp;
}

/*=========================================================================
函数名称: NUM_HEX_TO_DEC()
功能描述: 数字16进制0x0-9转换为10进制 0-9          
输入参数: 
输出参数: 
=========================================================================*/
u8 NUM_HEX_TO_DEC(u8 data)
{
	switch(data)
	{
		case 0x00: return 0;
		case 0x01: return 1;
		case 0x02: return 2;
		case 0x03: return 3;
		case 0x04: return 4;
		case 0x05: return 5;
		case 0x06: return 6;
		case 0x07: return 7;
		case 0x08: return 8;
		case 0x09: return 9;
		
	}
	return 0;
}
	
/*=========================================================================
函数名称: ascii_to_hex()
功能描述: 将数据从ascii转换为hex
输出参数: 
=========================================================================*/
u8 ascii_to_hex(u8 buff)
{
	if(buff>0x39)
	{
		switch(buff-0x41){
		case 0:	return 0x0a;
		case 1:	return 0x0b;	
		case 2:	return 0x0c;
		case 3:	return 0x0d;
		case 4:	return 0x0e;	
		case 5:	return 0x0f;		
		default:
						break;
		
		}
	}
	return buff-0x30;
}


/*=========================================================================
函数名称: hex_to_ascii()
功能描述: 将数据从hex转换为ascii
输出参数: 
=========================================================================*/
u8 hex_to_ascii(u8 buff)
{
	if(buff>0x09)
	{
		switch(buff){
		case 0x0a:	return 0x41;
		case 0x0b:	return 0x42;	
		case 0x0c:	return 0x43;
		case 0x0d:	return 0x44;
		case 0x0e:	return 0x45;	
		case 0x0f:	return 0x46;		
		default:
						break;
		
		}
	}
	return buff+0x30;
}
/*=========================================================================
函数名称: RDSS_BDICI_Analysis()
功能描述: SIM卡信息报文解析函数
输出参数: 
=========================================================================*/
void RDSS_BDICI_Analysis(void)
{
	u8 i=0;
	
#if	_Debug
		printf("本机地址：");
#endif	
	for(i=0;i<7;i++)
	{
		RDSS_CADE.addr[i]=RDSS_DATA.BUFF[i+6];
#if	_Debug
		printf("%c",RDSS_CADE.addr[i]);
#endif	
	}
#if	_Debug
		printf("\r\n序列号：");
#endif	
	for(i=0;i<8;i++)
	{
		RDSS_CADE.num[i]=RDSS_DATA.BUFF[i+14];
#if	_Debug
		printf("%c",RDSS_CADE.num[i]);
#endif	
	}
#if	_Debug
		printf("\r\n通拨地址：");
#endif	
	for(i=0;i<7;i++)
	{
		RDSS_CADE.T_addr[i]=RDSS_DATA.BUFF[i+23];
#if	_Debug
		printf("%c",RDSS_CADE.T_addr[i]);
#endif	
	}
	
		RDSS_CADE.characteristic=RDSS_DATA.BUFF[31];
#if	_Debug
		printf("\r\n用户特征：%c",RDSS_CADE.characteristic);
#endif	
	
	#if	_Debug
		printf("\r\n服务频度：");
#endif	

		RDSS_CADE.Frequentness[0]=RDSS_DATA.BUFF[33];
		RDSS_CADE.Frequentness[1]=RDSS_DATA.BUFF[34];
#if	_Debug
		printf("%c%cS\r\n",RDSS_CADE.Frequentness[0],RDSS_CADE.Frequentness[1]);
	printf("-------------------------------------\r\n");
#endif	

}
/*=========================================================================
函数名称: RDSS_BDICI_Analysis()
功能描述: 信号信息报文解析函数
输出参数: 
=========================================================================*/
void RDSS_BDBSI_Analysis(void)
{
#if	_Debug	
	u8 i=0,n=0;
	
	printf("响应波束：%c%c\r\n",RDSS_DATA.BUFF[6],RDSS_DATA.BUFF[7]);
	printf("时差波束：%c%c\r\n",RDSS_DATA.BUFF[9],RDSS_DATA.BUFF[10]);
	printf("1：");
	
	n=RDSS_DATA.BUFF[12]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n2：");
	
	n=RDSS_DATA.BUFF[14]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n3：");
	
	n=RDSS_DATA.BUFF[16]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n4：");
	
	n=RDSS_DATA.BUFF[18]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");

	}

		printf("\r\n5：");

	n=RDSS_DATA.BUFF[20]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n6：");

	n=RDSS_DATA.BUFF[22]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n7：");

	n=RDSS_DATA.BUFF[24]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n8：");

	n=RDSS_DATA.BUFF[26]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n9：");

	n=RDSS_DATA.BUFF[28]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n10：");
	
	n=RDSS_DATA.BUFF[30]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n-------------------------------------\r\n");

#endif
	if((RDSS_DATA.BUFF[12]-0x30>1)||(RDSS_DATA.BUFF[14]-0x30>1)||(RDSS_DATA.BUFF[16]-0x30>1)||(RDSS_DATA.BUFF[18]-0x30>1)\
		||(RDSS_DATA.BUFF[20]-0x30>1)||(RDSS_DATA.BUFF[22]-0x30>1)||(RDSS_DATA.BUFF[24]-0x30>1)||(RDSS_DATA.BUFF[26]-0x30>1)\
		||(RDSS_DATA.BUFF[28]-0x30>1)||(RDSS_DATA.BUFF[30]-0x30>1))
	{
		RDSS.signal_sta=1;
		RDSS.Read_Signal_Counter=1000;//半分钟
	}
	else 
	{
		RDSS.signal_sta=0;
		RDSS.Read_Signal_Counter=100;//1s
	}
	
	
	
}
/*=========================================================================
函数名称:RDSS_BDZDA_Analysis()
功能描述: 时间报文解析函数
输出参数: 
=========================================================================*/
void RDSS_BDZDA_Analysis(void)
{
	u16 year,month,day,hour,min,second;
	int difference=0;
	 
	year=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[26]-0x30);
	year=year*10;
	year+=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[27]-0x30);
	if((rtc.year!=year)&&(year>=18))
	{
		rtc.year=year;
		difference=10;
	}
	
	
	month=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[21]-0x30);
	month=month*10;
	month+=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[22]-0x30);
	
	if((rtc.month!=month)&&(month>0))
	{
		rtc.month=month;
		difference=10;
	}
	
	day=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[18]-0x30);
	day=day*10;
	day+=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[19]-0x30);
	
	if((rtc.day!=day)&&(day>0))
	{
		rtc.day=day;
		difference=10;
	}
	
	hour=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[8]-0x30);
	hour=hour*10;
	hour+=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[9]-0x30);
	hour=hour+8;//北京时间东8区
	if(rtc.hour!=hour)
	{
		rtc.hour=hour;
		difference=10;
	}
	
	
	min=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[10]-0x30);
	min=min*10;
	min+=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[11]-0x30);
	if(rtc.min!=min)
	{
		rtc.min=min;
		difference=10;
	}
	second=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[12]-0x30);	
	second=second*10;
	second+=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[13]-0x30);
	
	if(rtc.second!=second)
	{
		
		difference=rtc.second-second;
		rtc.second=second;	
	}
	
	
	#if	_Debug
	printf("RTC: 20%d-%d-%d  %d:%d:%d\n",rtc.year,rtc.month,rtc.day,rtc.hour,rtc.min,rtc.second);
	printf("\r\n-------------------------------------\r\n");
	#endif
	if((difference>=5)||(difference<=-5))
	{
		RTC_Write_Time(&rtc);//修改时钟芯片时间
	}

	
}



/*=========================================================================
函数名称:RDSS_BDFKI_Analysis()
功能描述: 回执信息解析函数
输出参数: 
=========================================================================*/
void RDSS_BDFKI_Analysis(void)
{
	u8 buff[4];
	buff[0]=RDSS_DATA.BUFF[6];
	buff[1]=RDSS_DATA.BUFF[7];
	buff[2]=RDSS_DATA.BUFF[8];	
	buff[3]=0;
	if(strcmp((char *)&buff,"DWA")==0)
	{
#if	_Debug
		printf("定位申请");
#endif
		if(RDSS_DATA.BUFF[10]=='Y')
		{
	#if	_Debug
			printf("成功");
	#endif
		}
		else 
		{
	#if	_Debug
			printf("失败");
	#endif
		}
#if	_Debug
		printf(" 服务频度");
#endif
		if(RDSS_DATA.BUFF[12]=='Y')
		{
	#if	_Debug
			printf("已到");
	#endif
		}
		else 
		{
	#if	_Debug
			printf("未到");
	#endif
		}
		
	}
	if(strcmp((char *)&buff,"TXA")==0)
	{
#if	_Debug
		printf("通信申请");
#endif
		if(RDSS_DATA.BUFF[10]=='Y')
		{
	#if	_Debug
			printf("成功");
	#endif
		}
		else 
		{
	#if	_Debug
			printf("失败");
	#endif
		}
		
#if	_Debug
		printf(" 发射频度");
#endif
		if(RDSS_DATA.BUFF[12]=='Y')
		{
	#if	_Debug
			printf("已到");
	#endif
		}
		else 
		{
	#if	_Debug
			printf("未到");
	#endif
		}

	}
#if	_Debug
		printf(" 到达服务频度剩余%c%c秒\r\n",RDSS_DATA.BUFF[18],RDSS_DATA.BUFF[19]);
		printf("-------------------------------------\r\n");
#endif
	if((RDSS_DATA.BUFF[12]=='Y')&&(RDSS_DATA.BUFF[10]=='Y'))//发送报文成功，发送下一条报文
	{
		RDSS_TX.RDSS_TX_DATA_STA++;
		if(RDSS_TX.RDSS_TX_DATA_STA>=3) RDSS_TX.RDSS_TX_DATA_STA=0;
			
	}
	RDSS.Server_Counter=(NUM_HEX_TO_DEC(RDSS_DATA.BUFF[18]-0x30)*10)+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[19]-0x30);
	RDSS.Server_Counter=(RDSS.Server_Counter+6)*50;//+5秒除去时钟误差
}



/*=========================================================================
函数名称: RDSS_BDDWR_Analysis()
功能描述: RDSS定位数据解析函数
输出参数: 
=========================================================================*/
void RDSS_BDDWR_Analysis(void)
{


//-------------------------------纬度-----------------------------	
	
	latitude[0]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[26]-0x30)*10;
	latitude[0]=latitude[0]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[27]-0x30);//度
	
	latitude[1]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[28]-0x30)*10;
	latitude[1]=latitude[1]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[29]-0x30);//分
		
	latitude[2]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[31]-0x30)*10;
	latitude[2]=latitude[2]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[32]-0x30);
	latitude[3]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[33]-0x30)*10;
	latitude[3]=latitude[3]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[34]-0x30);//秒
	
	latitude[4]=RDSS_DATA.BUFF[36];//方向
#if	_Debug	

	printf("\r\n纬度：");
	

	printf("%d°",latitude[0]);	
	
	printf("%d′",latitude[1]);
	
	printf("%d.",latitude[2]);
	printf("%d″",latitude[3]);
	
	printf(" %c",latitude[4]);
#endif	
	
//-------------------------------经度-----------------------------	

	
	longitude[0]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[38]-0x30)*100;
	longitude[0]=longitude[0]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[39]-0x30)*10;	
	longitude[0]=longitude[0]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[40]-0x30);	//度

	
	longitude[1]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[41]-0x30)*10;	
	longitude[1]=longitude[1]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[42]-0x30);	//分
	
	longitude[2]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[44]-0x30)*10;	
	longitude[2]=longitude[2]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[45]-0x30);	
	longitude[3]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[46]-0x30)*10;	
	longitude[3]=longitude[3]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[47]-0x30);	//秒

	
	longitude[4]=RDSS_DATA.BUFF[49];
#if	_Debug	
	printf("\r\n经度：");
	
	printf("%d°",longitude[0]);
	
	printf("%d′",longitude[1]);
	
	printf("%d.",longitude[2]);
	printf("%d″",longitude[3]);
	
	printf(" %c",longitude[4]);
	printf("\r\n-------------------------------------\r\n");
#endif
}
/*=========================================================================
函数名称: RDSS_BDTXR_Analysis()
功能描述: RDSS接收数据解析函数
输出参数: 
=========================================================================*/
void RDSS_BDTXR_Analysis(void)
{
	u16 i=0;
#if	_Debug
	printf("\r\n");	
#endif
	for(i=19;i<RDSS_DATA.LEN;i++)
	{
#if	_Debug
		printf("%c",RDSS_DATA.BUFF[i]);
#endif
		
	}
	
	
	printf("\r\n-------------------------------------\r\n");

}
/*=========================================================================
函数名称:data_analysis()
功能描述: RDSS数据解析函数
输出参数: 
=========================================================================*/
void data_analysis(void)
{
	
	u16 i=0;
	for(i=0;i<5;i++)
	{
		RDSS_DATA.CMD[i]=RDSS_DATA.BUFF[i];
#if	_Debug
		printf("%c",RDSS_DATA.CMD[i]);
#endif
	}

	if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDICI)==0)
	{
#if	_Debug
		printf("：SIM卡信息\r\n");
#endif	
		RDSS_BDICI_Analysis();
	}else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDFKI)==0){
#if	_Debug
		printf("：回执信息\r\n");
#endif	
		RDSS_BDFKI_Analysis();
			}
	else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDZDA)==0){
#if	_Debug
		printf("：时间信息\r\n");
#endif	
			RDSS_BDZDA_Analysis();
			}
	else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDBSI)==0){
#if	_Debug
		printf("：信号强度\r\n");
#endif	
		RDSS_BDBSI_Analysis();
			}
else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDDWR)==0){
#if	_Debug
		printf("：定位信息\r\n");
#endif	
		RDSS_BDDWR_Analysis();
			}
else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDTXR)==0){
#if	_Debug
		printf("：接收文本信息\r\n");
#endif	
		RDSS_BDTXR_Analysis();
			}				
		        
}





/*=========================================================================
函数名称: RDSS_DMA_data_processing()
功能描述: 接收数据处理函数
输出参数: 
=========================================================================*/
void RDSS_DMA_data_processing(void)
{ 
	u16 i=0;
	char * Buff_Str;
	u8 check;
	
	if(RDSS_RX.rdss_rx_timeout>2)//判断有没有接收到数据
	{
		RDSS_RX.rdss_rx_timeout=0;
#if	_Debug
		printf("<<<<<<");
#endif
		for(i=0;i<RDSS_RX.RDSS_RXBUFF_LEN;i++)
		{
#if	_Debug		
			printf("%c",RDSS_RX.RDSS_RX_BUFF[i]);
#endif
		}
		
		
	Buff_Str=strstr((char *)RDSS_RX.RDSS_RX_BUFF,"$");//查找标志位
	i=1;
	while(1)
	{	
		if((Buff_Str[i]==0x2a)&&(i>=RDSS_RX.RDSS_RXBUFF_LEN-5)) break;
		RDSS_DATA.BUFF[i-1]=Buff_Str[i];
//#if	_Debug
//		printf("%c",RDSS_DATA.BUFF[i-1]);
//#endif	
		if(i>1024 )
		{
#if	_Debug
		printf("标志位错误\r\n");
#endif			
		}
		i++;
	}
	RDSS_DATA.LEN=i-1;
	check=FucCalcXor(RDSS_DATA.BUFF,RDSS_DATA.LEN);
#if	_Debug
	printf("\r\n计算校验值：%x\r\n接收校验值：%c%c\r\n",check,Buff_Str[i+1],Buff_Str[i+2]);
#endif
	
	if(check==((ascii_to_hex(Buff_Str[i+1])<<4)+(ascii_to_hex(Buff_Str[i+2]))))
	{
#if	_Debug
		printf("校验成功\r\n");
#endif
		data_analysis();//数据解析
		
	}else{
#if	_Debug
		printf("校验失败\r\n");
#endif	
	}
	
		DMAs_SetCurrDataCounter(DMA1_Channel6,RDSS_RX_RXBUFF_LEN);//重置DMA
	
		for(i=0;i<RDSS_RX.RDSS_RXBUFF_LEN;i++)//清除缓存
		{
			RDSS_RX.RDSS_RX_BUFF[i]=0;
		}
		RDSS_RX.RDSS_RXBUFF_LEN=0;
	}
}
 


void RDSS_Send_TXBuff(u8* buff,u16 len)
{
	u16 i;
	for(i = 0;i<len;i++){
		Send_Char(buff[i]);
	}
}


/*=========================================================================
函数名称: RDSS_Init()
功能描述: RDSS初始化函数
输出参数: 
=========================================================================*/
void RDSS_Init(void)
{
	

#if	_Debug
	printf("=============北斗通信模块============\r\n");
	
	printf("通信目标地址：%c%c%c%c%c%c%c\r\n",RDSS_TX.RDSS_TX_ADDR[0],RDSS_TX.RDSS_TX_ADDR[1],\
						RDSS_TX.RDSS_TX_ADDR[2],RDSS_TX.RDSS_TX_ADDR[3],RDSS_TX.RDSS_TX_ADDR[4],\
						RDSS_TX.RDSS_TX_ADDR[5],RDSS_TX.RDSS_TX_ADDR[6]);
	
	printf("RTC: 20%d-%d-%d  %d:%d:%d\r\n",rtc.year,rtc.month,rtc.day,rtc.hour,rtc.min,rtc.second);
	
	
	printf("=====================================\r\n\r\n\r\n");
	
	printf(">>>>>>$CCICA,0,00*7B\r\n");
	printf("-------------------------------------\r\n");
#endif
	Send_String((u8 *)RDSS_CMD_ICA);//读取SIM卡信息
	RDSS_TX.RDSS_TX_DATA_STA = 0;	//报文发送状态
	RDSS.signal_Timeout=3000*2;		//搜索信号超时计时2分钟
	RDSS.Read_Signal_Counter = 100;	//2s
	RDSS_TX.RDSS_TX_DATA_Time = 500;// 发送时间间隔10s 
	rtc.adjusting=2000;				//开机40秒后时间校准
	
}


/*=========================================================================
函数名称: RDSS_PackBuff()
功能描述: RDSS 数据打包函数
输出参数: 
=========================================================================*/
void RDSS_PackBuff(u8* buff,u16 len)
{
	u8 tmp=0,TX_BUFF[256]={0};
	u16 i=0,j=0,TX_Len;
	
	TX_BUFF[0]='$';
	TX_BUFF[1]='C';
	TX_BUFF[2]='C';
	TX_BUFF[3]='T';
	TX_BUFF[4]='X';
	TX_BUFF[5]='A';
	TX_BUFF[6]=',';
	
	TX_BUFF[7]=RDSS_TX.RDSS_TX_ADDR[0];
	TX_BUFF[8]=RDSS_TX.RDSS_TX_ADDR[1];
	TX_BUFF[9]=RDSS_TX.RDSS_TX_ADDR[2];
	TX_BUFF[10]=RDSS_TX.RDSS_TX_ADDR[3];
	TX_BUFF[11]=RDSS_TX.RDSS_TX_ADDR[4];
	TX_BUFF[12]=RDSS_TX.RDSS_TX_ADDR[5];	
	TX_BUFF[13]=RDSS_TX.RDSS_TX_ADDR[6];
	TX_BUFF[14]=',';
	
	TX_BUFF[15]='1';
	TX_BUFF[16]=',';
	
	TX_BUFF[17]='2';
	TX_BUFF[18]=',';
	
	for(i=0;i<len;i++)
	{
		TX_BUFF[19+i]=buff[i];
	//	printf("%d",TX_BUFF[19+i]);
	}
	
	
	for(j=1;j<19+len;j++)
	{
		tmp =tmp ^ TX_BUFF[j];	
	}

	
	TX_BUFF[19+i]='*';
	TX_BUFF[20+i]=hex_to_ascii(tmp>>4);
	TX_BUFF[21+i]=hex_to_ascii(tmp&0x0f);

	TX_BUFF[22+i]=0x0d;
	TX_BUFF[23+i]=0x0a;
	TX_Len=24+i;
	RDSS_Send_TXBuff(TX_BUFF,TX_Len);
	
	
#if	_Debug
	printf(">>>>>>");
	for(i=0;i<TX_Len;i++)
	{
		printf("%c",TX_BUFF[i]);
	}
	printf("\r\n--------------------------------\r\n");
#endif
}

/*=========================================================================
函数名称: RDSS_Time_incident()
功能描述: RDSS时间事件处理函数
输出参数: 
=========================================================================*/
void RDSS_Time_incident(void)
{

//-----------------------------------	读取北斗信号信息	--------------------------------
	if(RDSS.Read_Signal_Counter==0)
	{
		if(RDSS.signal_sta)					
		{
			RDSS.Read_Signal_Counter=1000;//半分钟
		}	
		else RDSS.Read_Signal_Counter=100;//1s
		Send_String((u8 *)RDSS_CMD_BSI);
#if	_Debug	
	printf(">>>>>>$CCRMO,BSI,2,0*26\r\n");
	printf("-------------------------------------\r\n");
#endif
	}
	
	
	
	
	
//----------------------------------	服务频度已到	------------------------------------
	if((RDSS.Server_Counter==0)&&(RDSS_TX.RDSS_TX_DATA_Time==0)&&(RDSS.signal_sta==1))
	{	
		RDSS_TX.RDSS_TX_DATA_Time = 500;
		switch(RDSS_TX.RDSS_TX_DATA_STA)
		{				
			
			case 0://定位报文
				{
					
					Send_String((u8 *)RDSS_CMD_DWA);
					#if	_Debug
							
						printf(">>>>>>$CCDWA,0000000,V,1,L,,0,,,0*65\r\n");
						printf("-------------------------------------\r\n");
					#endif
				}			
			break;
			case 1://数据报文1
				//RDSS_PackBuff((u8 *)"00001",5);
				RDSS_PackBuff(RDSS_TX.RDSS_TX_BUFF1,RDSS_TX.RDSS_TXBUFF1_LEN);
			break;
			case 2://数据报文2
				//RDSS_PackBuff((u8 *)"00002",5);
				RDSS_PackBuff(RDSS_TX.RDSS_TX_BUFF2,RDSS_TX.RDSS_TXBUFF2_LEN);
			break;
				
			default:
				
			break;
		}
	
	}
	
	
//------------------------------	开机40S 获取时间信息		---------------------------	
	if(rtc.adjusting==0)
	{
		rtc.adjusting=2500;
		Send_String((u8 *)RDSS_CMD_ZDA);
		
		
#if	_Debug	
	printf(">>>>>>$CCRMO,ZDA,2,0*21\r\n");
	printf("-------------------------------------\r\n");
#endif
	}
	
}

