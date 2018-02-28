#include "pcf8563.h" 


u16 month[12]={31,28,31,30,31,30,31,31,30,31,30,31};
RTC_ rtc;
/*
*/

//***********************************************************************************
//*���ƣ�u8 BCD_TO_HEX(u8 bcd_data)         	 							*
//*���ܣ�BCD����ת����hex����                    						*
//*������bcd����                             										*
//*���أ�hex���� 														*
//***********************************************************************************
u8 BCD_TO_HEX(u8 bcd_data){
	u8 temp;
	temp=((bcd_data>>4)*10)+(bcd_data&0x0F);
	return temp;
}
//***********************************************************************************
//*���ƣ�u8 HEX_TO_BCD(u8 hex_data)         	 								*
//*���ܣ�HEX����ת����BCD����                    						*
//*������hex����                             										*
//*���أ�bcd����                  												*
//***********************************************************************************
u8 HEX_TO_BCD(u8 hex_data){	//HEXתBCD
    u8 bcd_data = 0;
    u8 TMP     = 0;
    TMP = hex_data%100;
    bcd_data =(TMP/10)<<4;
    bcd_data |=TMP%10;
    return (bcd_data);
}



//
//***********************************************************************************
//*���ƣ�void RTC_i2c_start()          	 										*
//*���ܣ�����I2C����                    										*
//*��������                             											*
//*���أ���                             											*
//***********************************************************************************
void RTC_i2c_start(){	
	RTC_SDA_OUT();   //sda�����
	delay_us(10);		
	RTC_SCL=0;
	RTC_SDA=1; 
	delay_us(1); 
	RTC_SCL=1;
	delay_us(10);
	RTC_SDA=0;
	delay_us(10);
	RTC_SCL=0;
}
//***********************************************************************************
//*���ƣ�void RTC_i2c_stop()												*
//*���ܣ�ֹͣI2C����                    										*
//*��������                             											*
//*���أ���                             											*
//***********************************************************************************
void RTC_i2c_stop(){	
	RTC_SDA_OUT();     //sda�����
	delay_us(10);
	RTC_SCL=0;
	RTC_SDA=0;
	delay_us(1); 
	RTC_SCL=1;
	delay_us(10);
	RTC_SCL=1; 
	delay_us(10);
	RTC_SCL=0;
	delay_us(1); 
	RTC_SCL=1;
	RTC_SDA=1;
}

//***********************************************************************************
//*���ƣ�void RTC_i2c_send_byte(u8 data_byte)              						*								
//*���ܣ���I2C���߷�������              								*
//*������data_byte  �ַ���  ���͵�����  							*
//*���أ���                             											*
//***********************************************************************************


void RTC_i2c_send_byte (u8 ICdate){ //reentrant
    	u8 j,dat;
	dat = ICdate;
	RTC_SDA_OUT();     //sda�����
	for(j=0;j<8;j++)
	{
		RTC_SCL = 0;
	  	delay_us(4);
		if(dat & 0x80)
		RTC_SDA=1;
		else
		RTC_SDA=0;		
		//RTC_SDA=dat & 0x80;
		delay_us(4);
		RTC_SCL=1;
		delay_us(4);
		RTC_SCL=0;
		dat=dat<<1;
	}/**/
	delay_us(10);	
	RTC_SDA=1;						//�ͷ�������----Ӧ���ź�
	delay_us(8);
	RTC_SCL=1;
	//cy=SDA;
	delay_us(8);
	RTC_SCL=0;						//��λʱ����
	delay_us(2);
}
	/***************************/
u8 RTC_Read_data (void){ 		//��һ�ֽ�����
	u8 i;
	u8 tempdate =0;	
	RTC_SDA_IN();
	for(i=0;i<8;i++)
	{
		RTC_SCL=0;
		delay_us(4);
		RTC_SCL=1;
		tempdate=tempdate<<1;
		if(RTC_Read_SDA)
		tempdate |= 0x01;
		delay_us(4);
		RTC_SCL=0;
	}
	RTC_SDA=1;
	delay_us(8);
	RTC_SCL=1;
	delay_us(8);
	RTC_SDA=0;
	delay_us(2);
	RTC_SCL=0;
	return(tempdate);
}

//***********************************************************************************
//*���ƣ�u8 i2c_receive_byte()           								*
//*���ܣ���I2C���߽�������              						*
//*��������                             									*
//*���أ����յ���һ���ֽڵ�����            				*
//***********************************************************************************
u8 RTC_i2c_rec_byte(u8 address){ //reentrant
	u8 rdata;
	RTC_i2c_start();  
	RTC_i2c_send_byte (0xA2);            						 /*д����*/ 
	RTC_i2c_send_byte (address);         					/*д��ַ*/ 

	RTC_i2c_start(); 										//�������--����ָ��
	RTC_i2c_send_byte (0xA3);             					/*������*/		
	rdata = RTC_Read_data ();   
	RTC_i2c_stop();  
	delay_us(10);

	RTC_SDA=1;
	RTC_SCL=1;

	return(rdata);  
}
//*******************************************************************************************************************
//*���ƣ�bit RTC_i2c_write_byte(u8 sla,u8 data_addr,u8 data_byte)														*
//*���ܣ���I2C����������ָ����ַд�ֽڵ�����               											*
//*������ic_add    		������ַ        																		*   
//*      data_addr  	���ݵ�ַ      																					*
//*      data_byte 		Ҫд���ֽ�����	    																		* 
//*���أ�0	д����ʧ��	1	д���ݳɹ�                             												*
//*******************************************************************************************************************
void RTC_i2c_write_byte(u8 data_addr,u8 data_byte){
		
	RTC_i2c_start();//	����I2C START�ź�
	//RTC_i2c_send_byte(sla);//	���������ӵ�ַ
	RTC_i2c_send_byte (0xa2);          /*д����*/   
	RTC_i2c_send_byte (data_addr);       /*д��ַ*/   
	RTC_i2c_send_byte (data_byte);         /*д����*/   
	RTC_i2c_stop();
	delay_us(10);
	RTC_SDA=1;
	RTC_SCL=1;
} 

/***********************************************************************
	PC8563 IO�ڳ�ʼ��
	����PC7 ,PC8,Ϊ������� 
	RTC_SCL			PB15
	RTC_SDA		PC6
	RTC_Read_SDA	PC6
************************************************************************/
void RTC_IO_init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
	//IIC SDA,SCL �˿ڳ�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PF�˿�ʱ��	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;				 //IO���ٶ�Ϊ10MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 		//�����趨������ʼ��GPIOB.0
	GPIO_SetBits(GPIOB,GPIO_Pin_15);					//PB.0 �����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 		//�����趨������ʼ��GPIOB.0
	GPIO_SetBits(GPIOC,GPIO_Pin_6);					//PB.0 �����



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;						//�ж����ţ���ʱδ�ã���Ϊ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_7);
}
 void RTC_init(void){
	RTC_IO_init(); 
	RTC_Reset();
	RTC_Read_Time(&rtc);
	if(rtc.year <= 17){//
		rtc.year = 	18;
		rtc.month = 	1;
		rtc.day = 	1;
		rtc.hour = 	1;
		rtc.min = 	1;
		rtc.second = 	1;
		RTC_Write_Time(&rtc);
	}
/*++++++++++++++++++����ʹ��+++++++++++++++++++++++
	while(1){
		delay_ms(1000);
		RTC_Read_Time(&rtc);
		printf("20%d-%d-%d  %d:%d:%d\r",rtc.year,rtc.month,rtc.day,rtc.hour,rtc.min,rtc.second);
	}
*/
}
void RTC_Reset(void){
	RTC_i2c_write_byte(0,0);

	RTC_i2c_write_byte(0x0a,0xff);
	RTC_i2c_write_byte(0x0b,0xff);
	RTC_i2c_write_byte(0x0c,0xff);
}
/*
������ʱ��ΪBCD��
*/
void RTC_Read_Time(RTC_* Date){//��ȡ��ǰʱ��
 	DATE DATE_Buff;
 	u8  ErrorFlag = 3;
 START_READ_RTC_START:
//--------����Զ�����ʱ��ֱ𵥶��������ʱ����ʾ�����----------
	DATE_Buff.year 		= 	RTC_i2c_rec_byte(0x08);	//��
	DATE_Buff.month 		= 	RTC_i2c_rec_byte(0x07);	//��
	DATE_Buff.day 		=	RTC_i2c_rec_byte(0x05);	//��
	DATE_Buff.hour	 	= 	RTC_i2c_rec_byte(0x04);	//ʱ
	DATE_Buff.min 		= 	RTC_i2c_rec_byte(0x03);	//��
	DATE_Buff.second 		=	RTC_i2c_rec_byte(0x02);	//��

	DATE_Buff.year 		&= 0x7f;
	DATE_Buff.month 		&= 0x1f;
	DATE_Buff.day  		&= 0x3f;
	DATE_Buff.hour 		&= 0x3f;
	DATE_Buff.min		&= 0x7f;
	DATE_Buff.second 		&= 0x7f;
 	
 	DATE_Buff.year 		=	BCD_TO_HEX(DATE_Buff.year);
	DATE_Buff.month 		=	BCD_TO_HEX(DATE_Buff.month);
 	DATE_Buff.day  		=	BCD_TO_HEX(DATE_Buff.day);
 	DATE_Buff.hour 		=	BCD_TO_HEX(DATE_Buff.hour);
 	DATE_Buff.min		=	BCD_TO_HEX(DATE_Buff.min);
 	DATE_Buff.second 		=	BCD_TO_HEX(DATE_Buff.second);

	if((DATE_Buff.year < 100)
	&&(DATE_Buff.month <= 12)
	&&(DATE_Buff.day	<= 31)
	&&(DATE_Buff.hour< 24)
	&&(DATE_Buff.min	< 60)
	&&(DATE_Buff.second< 60)){
		Date->year 		= DATE_Buff.year;		//��
		Date->month 	= DATE_Buff.month;	//��
		Date->day 		= DATE_Buff.day;		//��
		Date->hour	 	= DATE_Buff.hour;		//ʱ
		Date->min 		= DATE_Buff.min;		//��
		Date->second 	= DATE_Buff.second;	//��
		ErrorFlag = 0;
	}else{
		ErrorFlag--;
	}

	if(ErrorFlag != 0){
		goto START_READ_RTC_START;
	}
}

/*
д���ʱ��ΪBCD��
*/
void RTC_Write_Time(RTC_* Date){ 
	RTC_i2c_write_byte(0x08,HEX_TO_BCD(Date->year));
	RTC_i2c_write_byte (0x07,HEX_TO_BCD(Date->month));
	RTC_i2c_write_byte (0x05,HEX_TO_BCD(Date->day));
	RTC_i2c_write_byte (0x04,HEX_TO_BCD(Date->hour));
	RTC_i2c_write_byte (0x03,HEX_TO_BCD(Date->min));
	RTC_i2c_write_byte(0x02,HEX_TO_BCD(Date->second));
	RTC_i2c_write_byte(0,0);
}


/*******************************************************************************
����:�Ƿ�Ϊ����
����:
����:
1----�ɹ�
0--ʧ��
*******************************************************************************/
u16 Leap_Year(u16 year){
	return ((year % 400 == 0) || ((year % 4 == 0)&& (year %100 != 0))) ? 1 : 0;
}
/*******************************************************************************
����:�ж����������Ƿ�Ϸ�
����:
����:
1----�ɹ�
0--ʧ��
*******************************************************************************/
u16 Judge(date d1){
	return ((d1.year > 0 && d1.year <= 9999) 	//�ж����Ƿ���ȷ
		&&(d1.month > 0 && d1.month <= 12) 	//�ж����Ƿ���ȷ
		//�ж����Ƿ���ȷ����������2�£���������
		&&(d1.day > 0 && (((d1.month == 2) && (d1.day < month[d1.month - 1] + Leap_Year(d1.year)))||((d1.month != 2) && (d1.day < month[d1.month - 1]))))) ? 1 : 0;
}
/*******************************************************************************
����:����������������������֮�������
����:
����:
---������֮�������
�޸�:
��ע:
fast_sky@sina.com
********************************************************************************/
date Now_to_Later(date now, u16 gap){
	date d1;
	int sum, sum_bak;
	int temp;
	d1 = now;
	sum = gap;
	if(Judge(d1)&&(sum>0) && (sum < 3649270)){
		while(sum>365){
			if(d1.month>=3){
				d1.year++;
				sum=sum-365-Leap_Year(d1.year);
			}else{
				sum=sum-365-Leap_Year(d1.year);
				d1.year++;
			}
		}
		while(sum > 0){
			if (d1.month != 2){
				temp = month[d1.month - 1] -d1.day + 1;
			}else{
				temp = month[d1.month - 1] +Leap_Year(d1.year)- d1.day + 1;
			}
			sum_bak = sum;
			sum = sum - temp;
			if (sum >= 0){
				d1.month++;
				d1.day = 1;
				if (d1.month > 12){
					d1.month = 1;
					d1.year++;
				}
				sum_bak = sum;
			}
		}
		d1.day += sum_bak;
	}
	return d1;
}

