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
	

u8 longitude[10];	//����
u8 latitude[10];	//γ��



DMA_InitTypeDef DMA_InitStructure;
void DMAs_SetCurrDataCounter(DMA_Channel_TypeDef*DMA_CHx, uint16_t DataNumber){
	DMA_Cmd(DMA_CHx, DISABLE);  //�ر�USART1 TX DMA1 ��ָʾ��ͨ�� 
 	DMA_SetCurrDataCounter(DMA_CHx, DataNumber);//1���ü�����ֵʱ����ر�
	DMA_Cmd(DMA_CHx, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}
void DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr){
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //4DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //4DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //4���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_InitStructure.DMA_BufferSize = cndtr;  //4DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���	  
	DMA_Cmd(DMA_CHx, ENABLE);  // DMAͨ�� 	
} 



void RDSS_uart_init(u32 bound){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART4ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOCʱ��
	//USART4_TX   PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART4_RX	  PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	//UART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������

	DMA_Config(DMA1_Channel6,(u32)&USART2->DR,(u32)RDSS_RX.RDSS_RX_BUFF,RDSS_RX_RXBUFF_LEN);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);////ʹ�ܽ���DMA����
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

}


void Send_Char(u8 ch){
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
	USART2->DR = (u8) ch;      
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
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
��������: FucCalcXor()
��������: �����ݽ����������             
�������: 
�������: 
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
��������: NUM_HEX_TO_DEC()
��������: ����16����0x0-9ת��Ϊ10���� 0-9          
�������: 
�������: 
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
��������: ascii_to_hex()
��������: �����ݴ�asciiת��Ϊhex
�������: 
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
��������: hex_to_ascii()
��������: �����ݴ�hexת��Ϊascii
�������: 
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
��������: RDSS_BDICI_Analysis()
��������: SIM����Ϣ���Ľ�������
�������: 
=========================================================================*/
void RDSS_BDICI_Analysis(void)
{
	u8 i=0;
	
#if	_Debug
		printf("������ַ��");
#endif	
	for(i=0;i<7;i++)
	{
		RDSS_CADE.addr[i]=RDSS_DATA.BUFF[i+6];
#if	_Debug
		printf("%c",RDSS_CADE.addr[i]);
#endif	
	}
#if	_Debug
		printf("\r\n���кţ�");
#endif	
	for(i=0;i<8;i++)
	{
		RDSS_CADE.num[i]=RDSS_DATA.BUFF[i+14];
#if	_Debug
		printf("%c",RDSS_CADE.num[i]);
#endif	
	}
#if	_Debug
		printf("\r\nͨ����ַ��");
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
		printf("\r\n�û�������%c",RDSS_CADE.characteristic);
#endif	
	
	#if	_Debug
		printf("\r\n����Ƶ�ȣ�");
#endif	

		RDSS_CADE.Frequentness[0]=RDSS_DATA.BUFF[33];
		RDSS_CADE.Frequentness[1]=RDSS_DATA.BUFF[34];
#if	_Debug
		printf("%c%cS\r\n",RDSS_CADE.Frequentness[0],RDSS_CADE.Frequentness[1]);
	printf("-------------------------------------\r\n");
#endif	

}
/*=========================================================================
��������: RDSS_BDICI_Analysis()
��������: �ź���Ϣ���Ľ�������
�������: 
=========================================================================*/
void RDSS_BDBSI_Analysis(void)
{
#if	_Debug	
	u8 i=0,n=0;
	
	printf("��Ӧ������%c%c\r\n",RDSS_DATA.BUFF[6],RDSS_DATA.BUFF[7]);
	printf("ʱ�����%c%c\r\n",RDSS_DATA.BUFF[9],RDSS_DATA.BUFF[10]);
	printf("1��");
	
	n=RDSS_DATA.BUFF[12]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n2��");
	
	n=RDSS_DATA.BUFF[14]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n3��");
	
	n=RDSS_DATA.BUFF[16]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n4��");
	
	n=RDSS_DATA.BUFF[18]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");

	}

		printf("\r\n5��");

	n=RDSS_DATA.BUFF[20]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n6��");

	n=RDSS_DATA.BUFF[22]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n7��");

	n=RDSS_DATA.BUFF[24]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n8��");

	n=RDSS_DATA.BUFF[26]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n9��");

	n=RDSS_DATA.BUFF[28]-0x30;
	for(i=0;i<n;i++)
	{
		

		printf("* ");
	
	}

		printf("\r\n10��");
	
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
		RDSS.Read_Signal_Counter=1000;//�����
	}
	else 
	{
		RDSS.signal_sta=0;
		RDSS.Read_Signal_Counter=100;//1s
	}
	
	
	
}
/*=========================================================================
��������:RDSS_BDZDA_Analysis()
��������: ʱ�䱨�Ľ�������
�������: 
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
	hour=hour+8;//����ʱ�䶫8��
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
		RTC_Write_Time(&rtc);//�޸�ʱ��оƬʱ��
	}

	
}



/*=========================================================================
��������:RDSS_BDFKI_Analysis()
��������: ��ִ��Ϣ��������
�������: 
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
		printf("��λ����");
#endif
		if(RDSS_DATA.BUFF[10]=='Y')
		{
	#if	_Debug
			printf("�ɹ�");
	#endif
		}
		else 
		{
	#if	_Debug
			printf("ʧ��");
	#endif
		}
#if	_Debug
		printf(" ����Ƶ��");
#endif
		if(RDSS_DATA.BUFF[12]=='Y')
		{
	#if	_Debug
			printf("�ѵ�");
	#endif
		}
		else 
		{
	#if	_Debug
			printf("δ��");
	#endif
		}
		
	}
	if(strcmp((char *)&buff,"TXA")==0)
	{
#if	_Debug
		printf("ͨ������");
#endif
		if(RDSS_DATA.BUFF[10]=='Y')
		{
	#if	_Debug
			printf("�ɹ�");
	#endif
		}
		else 
		{
	#if	_Debug
			printf("ʧ��");
	#endif
		}
		
#if	_Debug
		printf(" ����Ƶ��");
#endif
		if(RDSS_DATA.BUFF[12]=='Y')
		{
	#if	_Debug
			printf("�ѵ�");
	#endif
		}
		else 
		{
	#if	_Debug
			printf("δ��");
	#endif
		}

	}
#if	_Debug
		printf(" �������Ƶ��ʣ��%c%c��\r\n",RDSS_DATA.BUFF[18],RDSS_DATA.BUFF[19]);
		printf("-------------------------------------\r\n");
#endif
	if((RDSS_DATA.BUFF[12]=='Y')&&(RDSS_DATA.BUFF[10]=='Y'))//���ͱ��ĳɹ���������һ������
	{
		RDSS_TX.RDSS_TX_DATA_STA++;
		if(RDSS_TX.RDSS_TX_DATA_STA>=3) RDSS_TX.RDSS_TX_DATA_STA=0;
			
	}
	RDSS.Server_Counter=(NUM_HEX_TO_DEC(RDSS_DATA.BUFF[18]-0x30)*10)+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[19]-0x30);
	RDSS.Server_Counter=(RDSS.Server_Counter+6)*50;//+5���ȥʱ�����
}



/*=========================================================================
��������: RDSS_BDDWR_Analysis()
��������: RDSS��λ���ݽ�������
�������: 
=========================================================================*/
void RDSS_BDDWR_Analysis(void)
{


//-------------------------------γ��-----------------------------	
	
	latitude[0]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[26]-0x30)*10;
	latitude[0]=latitude[0]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[27]-0x30);//��
	
	latitude[1]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[28]-0x30)*10;
	latitude[1]=latitude[1]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[29]-0x30);//��
		
	latitude[2]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[31]-0x30)*10;
	latitude[2]=latitude[2]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[32]-0x30);
	latitude[3]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[33]-0x30)*10;
	latitude[3]=latitude[3]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[34]-0x30);//��
	
	latitude[4]=RDSS_DATA.BUFF[36];//����
#if	_Debug	

	printf("\r\nγ�ȣ�");
	

	printf("%d��",latitude[0]);	
	
	printf("%d��",latitude[1]);
	
	printf("%d.",latitude[2]);
	printf("%d��",latitude[3]);
	
	printf(" %c",latitude[4]);
#endif	
	
//-------------------------------����-----------------------------	

	
	longitude[0]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[38]-0x30)*100;
	longitude[0]=longitude[0]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[39]-0x30)*10;	
	longitude[0]=longitude[0]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[40]-0x30);	//��

	
	longitude[1]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[41]-0x30)*10;	
	longitude[1]=longitude[1]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[42]-0x30);	//��
	
	longitude[2]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[44]-0x30)*10;	
	longitude[2]=longitude[2]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[45]-0x30);	
	longitude[3]=NUM_HEX_TO_DEC(RDSS_DATA.BUFF[46]-0x30)*10;	
	longitude[3]=longitude[3]+NUM_HEX_TO_DEC(RDSS_DATA.BUFF[47]-0x30);	//��

	
	longitude[4]=RDSS_DATA.BUFF[49];
#if	_Debug	
	printf("\r\n���ȣ�");
	
	printf("%d��",longitude[0]);
	
	printf("%d��",longitude[1]);
	
	printf("%d.",longitude[2]);
	printf("%d��",longitude[3]);
	
	printf(" %c",longitude[4]);
	printf("\r\n-------------------------------------\r\n");
#endif
}
/*=========================================================================
��������: RDSS_BDTXR_Analysis()
��������: RDSS�������ݽ�������
�������: 
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
��������:data_analysis()
��������: RDSS���ݽ�������
�������: 
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
		printf("��SIM����Ϣ\r\n");
#endif	
		RDSS_BDICI_Analysis();
	}else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDFKI)==0){
#if	_Debug
		printf("����ִ��Ϣ\r\n");
#endif	
		RDSS_BDFKI_Analysis();
			}
	else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDZDA)==0){
#if	_Debug
		printf("��ʱ����Ϣ\r\n");
#endif	
			RDSS_BDZDA_Analysis();
			}
	else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDBSI)==0){
#if	_Debug
		printf("���ź�ǿ��\r\n");
#endif	
		RDSS_BDBSI_Analysis();
			}
else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDDWR)==0){
#if	_Debug
		printf("����λ��Ϣ\r\n");
#endif	
		RDSS_BDDWR_Analysis();
			}
else if(strcmp((char *)RDSS_DATA.CMD,RDSS_BDTXR)==0){
#if	_Debug
		printf("�������ı���Ϣ\r\n");
#endif	
		RDSS_BDTXR_Analysis();
			}				
		        
}





/*=========================================================================
��������: RDSS_DMA_data_processing()
��������: �������ݴ�����
�������: 
=========================================================================*/
void RDSS_DMA_data_processing(void)
{ 
	u16 i=0;
	char * Buff_Str;
	u8 check;
	
	if(RDSS_RX.rdss_rx_timeout>2)//�ж���û�н��յ�����
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
		
		
	Buff_Str=strstr((char *)RDSS_RX.RDSS_RX_BUFF,"$");//���ұ�־λ
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
		printf("��־λ����\r\n");
#endif			
		}
		i++;
	}
	RDSS_DATA.LEN=i-1;
	check=FucCalcXor(RDSS_DATA.BUFF,RDSS_DATA.LEN);
#if	_Debug
	printf("\r\n����У��ֵ��%x\r\n����У��ֵ��%c%c\r\n",check,Buff_Str[i+1],Buff_Str[i+2]);
#endif
	
	if(check==((ascii_to_hex(Buff_Str[i+1])<<4)+(ascii_to_hex(Buff_Str[i+2]))))
	{
#if	_Debug
		printf("У��ɹ�\r\n");
#endif
		data_analysis();//���ݽ���
		
	}else{
#if	_Debug
		printf("У��ʧ��\r\n");
#endif	
	}
	
		DMAs_SetCurrDataCounter(DMA1_Channel6,RDSS_RX_RXBUFF_LEN);//����DMA
	
		for(i=0;i<RDSS_RX.RDSS_RXBUFF_LEN;i++)//�������
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
��������: RDSS_Init()
��������: RDSS��ʼ������
�������: 
=========================================================================*/
void RDSS_Init(void)
{
	

#if	_Debug
	printf("=============����ͨ��ģ��============\r\n");
	
	printf("ͨ��Ŀ���ַ��%c%c%c%c%c%c%c\r\n",RDSS_TX.RDSS_TX_ADDR[0],RDSS_TX.RDSS_TX_ADDR[1],\
						RDSS_TX.RDSS_TX_ADDR[2],RDSS_TX.RDSS_TX_ADDR[3],RDSS_TX.RDSS_TX_ADDR[4],\
						RDSS_TX.RDSS_TX_ADDR[5],RDSS_TX.RDSS_TX_ADDR[6]);
	
	printf("RTC: 20%d-%d-%d  %d:%d:%d\r\n",rtc.year,rtc.month,rtc.day,rtc.hour,rtc.min,rtc.second);
	
	
	printf("=====================================\r\n\r\n\r\n");
	
	printf(">>>>>>$CCICA,0,00*7B\r\n");
	printf("-------------------------------------\r\n");
#endif
	Send_String((u8 *)RDSS_CMD_ICA);//��ȡSIM����Ϣ
	RDSS_TX.RDSS_TX_DATA_STA = 0;	//���ķ���״̬
	RDSS.signal_Timeout=3000*2;		//�����źų�ʱ��ʱ2����
	RDSS.Read_Signal_Counter = 100;	//2s
	RDSS_TX.RDSS_TX_DATA_Time = 500;// ����ʱ����10s 
	rtc.adjusting=2000;				//����40���ʱ��У׼
	
}


/*=========================================================================
��������: RDSS_PackBuff()
��������: RDSS ���ݴ������
�������: 
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
��������: RDSS_Time_incident()
��������: RDSSʱ���¼�������
�������: 
=========================================================================*/
void RDSS_Time_incident(void)
{

//-----------------------------------	��ȡ�����ź���Ϣ	--------------------------------
	if(RDSS.Read_Signal_Counter==0)
	{
		if(RDSS.signal_sta)					
		{
			RDSS.Read_Signal_Counter=1000;//�����
		}	
		else RDSS.Read_Signal_Counter=100;//1s
		Send_String((u8 *)RDSS_CMD_BSI);
#if	_Debug	
	printf(">>>>>>$CCRMO,BSI,2,0*26\r\n");
	printf("-------------------------------------\r\n");
#endif
	}
	
	
	
	
	
//----------------------------------	����Ƶ���ѵ�	------------------------------------
	if((RDSS.Server_Counter==0)&&(RDSS_TX.RDSS_TX_DATA_Time==0)&&(RDSS.signal_sta==1))
	{	
		RDSS_TX.RDSS_TX_DATA_Time = 500;
		switch(RDSS_TX.RDSS_TX_DATA_STA)
		{				
			
			case 0://��λ����
				{
					
					Send_String((u8 *)RDSS_CMD_DWA);
					#if	_Debug
							
						printf(">>>>>>$CCDWA,0000000,V,1,L,,0,,,0*65\r\n");
						printf("-------------------------------------\r\n");
					#endif
				}			
			break;
			case 1://���ݱ���1
				//RDSS_PackBuff((u8 *)"00001",5);
				RDSS_PackBuff(RDSS_TX.RDSS_TX_BUFF1,RDSS_TX.RDSS_TXBUFF1_LEN);
			break;
			case 2://���ݱ���2
				//RDSS_PackBuff((u8 *)"00002",5);
				RDSS_PackBuff(RDSS_TX.RDSS_TX_BUFF2,RDSS_TX.RDSS_TXBUFF2_LEN);
			break;
				
			default:
				
			break;
		}
	
	}
	
	
//------------------------------	����40S ��ȡʱ����Ϣ		---------------------------	
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

