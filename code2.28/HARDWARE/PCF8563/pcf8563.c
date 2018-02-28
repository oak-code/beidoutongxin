#include "pcf8563.h" 


u16 month[12]={31,28,31,30,31,30,31,31,30,31,30,31};
RTC_ rtc;
/*
*/

//***********************************************************************************
//*名称：u8 BCD_TO_HEX(u8 bcd_data)         	 							*
//*功能：BCD数据转换成hex数据                    						*
//*参数：bcd数据                             										*
//*返回：hex数据 														*
//***********************************************************************************
u8 BCD_TO_HEX(u8 bcd_data){
	u8 temp;
	temp=((bcd_data>>4)*10)+(bcd_data&0x0F);
	return temp;
}
//***********************************************************************************
//*名称：u8 HEX_TO_BCD(u8 hex_data)         	 								*
//*功能：HEX数据转换成BCD数据                    						*
//*参数：hex数据                             										*
//*返回：bcd数据                  												*
//***********************************************************************************
u8 HEX_TO_BCD(u8 hex_data){	//HEX转BCD
    u8 bcd_data = 0;
    u8 TMP     = 0;
    TMP = hex_data%100;
    bcd_data =(TMP/10)<<4;
    bcd_data |=TMP%10;
    return (bcd_data);
}



//
//***********************************************************************************
//*名称：void RTC_i2c_start()          	 										*
//*功能：启动I2C总线                    										*
//*参数：无                             											*
//*返回：无                             											*
//***********************************************************************************
void RTC_i2c_start(){	
	RTC_SDA_OUT();   //sda线输出
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
//*名称：void RTC_i2c_stop()												*
//*功能：停止I2C总线                    										*
//*参数：无                             											*
//*返回：无                             											*
//***********************************************************************************
void RTC_i2c_stop(){	
	RTC_SDA_OUT();     //sda线输出
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
//*名称：void RTC_i2c_send_byte(u8 data_byte)              						*								
//*功能：往I2C总线发送数据              								*
//*参数：data_byte  字符型  发送的数据  							*
//*返回：无                             											*
//***********************************************************************************


void RTC_i2c_send_byte (u8 ICdate){ //reentrant
    	u8 j,dat;
	dat = ICdate;
	RTC_SDA_OUT();     //sda线输出
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
	RTC_SDA=1;						//释放数据线----应答信号
	delay_us(8);
	RTC_SCL=1;
	//cy=SDA;
	delay_us(8);
	RTC_SCL=0;						//箝位时钟线
	delay_us(2);
}
	/***************************/
u8 RTC_Read_data (void){ 		//读一字节数据
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
//*名称：u8 i2c_receive_byte()           								*
//*功能：从I2C总线接受数据              						*
//*参数：无                             									*
//*返回：接收到的一个字节的数据            				*
//***********************************************************************************
u8 RTC_i2c_rec_byte(u8 address){ //reentrant
	u8 rdata;
	RTC_i2c_start();  
	RTC_i2c_send_byte (0xA2);            						 /*写命令*/ 
	RTC_i2c_send_byte (address);         					/*写地址*/ 

	RTC_i2c_start(); 										//必须带有--启动指令
	RTC_i2c_send_byte (0xA3);             					/*读命令*/		
	rdata = RTC_Read_data ();   
	RTC_i2c_stop();  
	delay_us(10);

	RTC_SDA=1;
	RTC_SCL=1;

	return(rdata);  
}
//*******************************************************************************************************************
//*名称：bit RTC_i2c_write_byte(u8 sla,u8 data_addr,u8 data_byte)														*
//*功能：往I2C总线上器件指定地址写字节的数据               											*
//*参数：ic_add    		器件地址        																		*   
//*      data_addr  	数据地址      																					*
//*      data_byte 		要写的字节数据	    																		* 
//*返回：0	写数据失败	1	写数据成功                             												*
//*******************************************************************************************************************
void RTC_i2c_write_byte(u8 data_addr,u8 data_byte){
		
	RTC_i2c_start();//	发送I2C START信号
	//RTC_i2c_send_byte(sla);//	发送器件从地址
	RTC_i2c_send_byte (0xa2);          /*写命令*/   
	RTC_i2c_send_byte (data_addr);       /*写地址*/   
	RTC_i2c_send_byte (data_byte);         /*写数据*/   
	RTC_i2c_stop();
	delay_us(10);
	RTC_SDA=1;
	RTC_SCL=1;
} 

/***********************************************************************
	PC8563 IO口初始化
	设置PC7 ,PC8,为推挽输出 
	RTC_SCL			PB15
	RTC_SDA		PC6
	RTC_Read_SDA	PC6
************************************************************************/
void RTC_IO_init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
	//IIC SDA,SCL 端口初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PF端口时钟	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;				 //IO口速度为10MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 		//根据设定参数初始化GPIOB.0
	GPIO_SetBits(GPIOB,GPIO_Pin_15);					//PB.0 输出高
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 		//根据设定参数初始化GPIOB.0
	GPIO_SetBits(GPIOC,GPIO_Pin_6);					//PB.0 输出高



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;						//中断引脚，暂时未用，设为输入
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
/*++++++++++++++++++测试使用+++++++++++++++++++++++
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
读出的时间为BCD码
*/
void RTC_Read_Time(RTC_* Date){//读取当前时间
 	DATE DATE_Buff;
 	u8  ErrorFlag = 3;
 START_READ_RTC_START:
//--------必须对读出的时间分别单独处理否则时间显示会混乱----------
	DATE_Buff.year 		= 	RTC_i2c_rec_byte(0x08);	//年
	DATE_Buff.month 		= 	RTC_i2c_rec_byte(0x07);	//月
	DATE_Buff.day 		=	RTC_i2c_rec_byte(0x05);	//日
	DATE_Buff.hour	 	= 	RTC_i2c_rec_byte(0x04);	//时
	DATE_Buff.min 		= 	RTC_i2c_rec_byte(0x03);	//分
	DATE_Buff.second 		=	RTC_i2c_rec_byte(0x02);	//秒

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
		Date->year 		= DATE_Buff.year;		//年
		Date->month 	= DATE_Buff.month;	//月
		Date->day 		= DATE_Buff.day;		//日
		Date->hour	 	= DATE_Buff.hour;		//时
		Date->min 		= DATE_Buff.min;		//分
		Date->second 	= DATE_Buff.second;	//秒
		ErrorFlag = 0;
	}else{
		ErrorFlag--;
	}

	if(ErrorFlag != 0){
		goto START_READ_RTC_START;
	}
}

/*
写入的时间为BCD码
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
功能:是否为闰年
参数:
返回:
1----成功
0--失败
*******************************************************************************/
u16 Leap_Year(u16 year){
	return ((year % 400 == 0) || ((year % 4 == 0)&& (year %100 != 0))) ? 1 : 0;
}
/*******************************************************************************
功能:判断输入日期是否合法
参数:
返回:
1----成功
0--失败
*******************************************************************************/
u16 Judge(date d1){
	return ((d1.year > 0 && d1.year <= 9999) 	//判断年是否正确
		&&(d1.month > 0 && d1.month <= 12) 	//判断月是否正确
		//判断日是否正确，并且区分2月，闰年等情况
		&&(d1.day > 0 && (((d1.month == 2) && (d1.day < month[d1.month - 1] + Leap_Year(d1.year)))||((d1.month != 2) && (d1.day < month[d1.month - 1]))))) ? 1 : 0;
}
/*******************************************************************************
功能:从现在日期往后计算多少天之后的日期
参数:
返回:
---多少天之后的日期
修改:
备注:
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

