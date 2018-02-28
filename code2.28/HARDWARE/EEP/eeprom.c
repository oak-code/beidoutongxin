#include "eeprom.h"



void I2C_Configuration(void)  {  
//	u8 i;
	GPIO_InitTypeDef GPIO_InitStructure;      
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);	 
	GPIO_InitStructure.GPIO_Pin=I2C_SCL|I2C_SDA;  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  
	GPIO_Init(GPIO_I2C,&GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin=EEP_WP; 
	GPIO_Init(GPIO_WP,&GPIO_InitStructure);  
	GPIO_ResetBits(GPIO_WP,EEP_WP) ;
	EEP_WP_L;
	I2C_SCL_H;  
	I2C_SDA_H;  
	delay_ms(1);
/*	************测试*************************
	for(i=100;i<255;i++){  
		AT24Cxx_WriteOneByte(i,i);  
	}             

	for(i=100;i<255;i++){  
		sys.u8_buff = AT24Cxx_ReadOneByte(i);  
		delay_ms(1000);
	}
	while(1);
	*/
}  
void I2C_SDA_OUT(void)  {  
	GPIO_InitTypeDef GPIO_InitStructure;      
	  
	GPIO_InitStructure.GPIO_Pin=I2C_SDA;  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  
	GPIO_Init(GPIO_I2C,&GPIO_InitStructure);  
}  
  
void I2C_SDA_IN(void)  {  
	GPIO_InitTypeDef GPIO_InitStructure;      
	GPIO_InitStructure.GPIO_Pin=I2C_SDA;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;  
	GPIO_Init(GPIO_I2C,&GPIO_InitStructure);  
}  
  
//产生起始信号  
void I2C_Start(void) {  
	I2C_SDA_OUT();  
	I2C_SDA_H; 
	delay_us(5);  //后续添加
	I2C_SCL_H;  
	delay_us(5);  
	I2C_SDA_L;  
	delay_us(6);  
	I2C_SCL_L;  
	delay_us(5);  //后续添加 
}  
  
//产生停止信号  
void I2C_Stop(void)  {  
	I2C_SDA_OUT();  
	I2C_SCL_L;  
	delay_us(5);  //后续添加
	I2C_SDA_L;  
	delay_us(5);  //
	I2C_SCL_H;  
	delay_us(6);  
	I2C_SDA_H;  
	delay_us(6);  
}  
  
//主机产生应答信号ACK  
void I2C_Ack(void)  {  
	I2C_SCL_L;  
	I2C_SDA_OUT();  
	I2C_SDA_L;  
	delay_us(2);  
	I2C_SCL_H;  
	delay_us(5);  
	I2C_SCL_L;  
	delay_us(5);  //后续添加
}  
  
//主机不产生应答信号NACK  
void I2C_NAck(void)  {  
	I2C_SCL_L;  
	I2C_SDA_OUT();  
	I2C_SDA_H;  
	delay_us(2);  
	I2C_SCL_H;  
	delay_us(5);  
	I2C_SCL_L;  
	delay_us(5);  //后续添加
}  
//等待从机应答信号  
//返回值：1 接收应答失败  
//        0 接收应答成功  
u8 I2C_Wait_Ack(void)  {  
	u8 tempTime=0;  
	I2C_SDA_IN();  
	I2C_SDA_H;  
	delay_us(5);
	I2C_SCL_H;  
	delay_us(5); 

	while(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA))  
	{  
	    tempTime++;  
	    if(tempTime>250)  
	    {  
	        I2C_Stop();  
	        return 1;  
	    }      
	}  
	I2C_SCL_L;  
    return 0;  
}  
//I2C 发送一个字节  
void I2C_Send_Byte(u8 txd)  {  
    u8 i=0;   
    I2C_SDA_OUT();  
    I2C_SCL_L;//拉低时钟开始数据传输  
    for(i=0;i<8;i++)  
    {  
        if((txd&0x80)>0) //0x80  1000 0000  
            I2C_SDA_H;  
        else  
            I2C_SDA_L;  
	 	delay_us(5); //后续添加
		txd<<=1;  
        I2C_SCL_H;  
        delay_us(5); //发送数据  
        I2C_SCL_L;  
        delay_us(5);  
    }  
}  
  
//I2C 读取一个字节  
  
u8 I2C_Read_Byte(u8 ack){  
   u8 i=0,receive=0;  
   I2C_SDA_IN();  
   for(i=0;i<8;i++){  
        I2C_SCL_L;  
        delay_us(3);
        I2C_SCL_H;  
        receive<<=1;  
        if(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA))  
           receive++;  
        delay_us(3);
   }  
    if(ack==0)  
        I2C_NAck();  
    else  
        I2C_Ack();    
    return receive;  
}  
/////////////////////////////////////////////////////////////
void AT24Cxx_Init(void){
	I2C_Configuration();
}
u8 AT24Cxx_ReadOneByte(u16 addr)  {  
    u8 temp=0;  
  	__disable_irq();// Disable Interrupts
	I2C_Start();  

	if(EE_TYPE>AT24C16)  {  
		I2C_Send_Byte(0xA0);  
		I2C_Wait_Ack();  
		I2C_Send_Byte(addr>>8);   //发送数据地址高位  
	}else{  
		I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址  
	}  

	I2C_Wait_Ack();  
	I2C_Send_Byte(addr%256);//双字节是数据地址低位          
	                    //单字节是数据地址低位  
	I2C_Wait_Ack();  

	I2C_Start();  
	I2C_Send_Byte(0xA1);  
	I2C_Wait_Ack();  

	temp=I2C_Read_Byte(0); //  0   代表 NACK  
	I2C_Stop();  
	__enable_irq();// ensable Interrupts
	return temp;      
}  
  
u16 AT24Cxx_ReadTwoByte(u16 addr)  {  
    u16 temp=0;  
  	__disable_irq();// Disable Interrupts
	I2C_Start();  

	if(EE_TYPE>AT24C16)  {  
		I2C_Send_Byte(0xA0);  
		I2C_Wait_Ack();  
		I2C_Send_Byte(addr>>8);   //发送数据地址高位  
	}else{  
		I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址  
	}  

	I2C_Wait_Ack();  
	I2C_Send_Byte(addr%256);//双字节是数据地址低位          
	            //单字节是数据地址低位  
	I2C_Wait_Ack();  

	I2C_Start();  
	I2C_Send_Byte(0xA1);  
	I2C_Wait_Ack();  

	temp=I2C_Read_Byte(1); //  1   代表 ACK  
	temp<<=8;  
	temp|=I2C_Read_Byte(0); //  0  代表 NACK  

	I2C_Stop();  
      __enable_irq();// ensable Interrupts
    return temp;      
}  
  
  
void AT24Cxx_WriteOneByte(u16 addr,u8 dt)  {  
	__disable_irq();// Disable Interrupts

	I2C_Start();  
	if(EE_TYPE>AT24C16){  
		I2C_Send_Byte(0xA0);  
		I2C_Wait_Ack();  
		I2C_Send_Byte(addr>>8);   //发送数据地址高位  
	}else{  
		I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址  
	}  

	I2C_Wait_Ack();  
	I2C_Send_Byte(addr%256);//双字节是数据地址低位          
	                        	//单字节是数据地址低位  
	I2C_Wait_Ack();  

	I2C_Send_Byte(dt);  
	I2C_Wait_Ack();  
	I2C_Stop();  
	__enable_irq();// ensable Interrupts
	delay_ms(10);  
}  
  
void AT24Cxx_WriteTwoByte(u16 addr,u16 dt) { 
	__disable_irq();// Disable Interrupts
	I2C_Start();  

	if(EE_TYPE>AT24C16)  {  
		I2C_Send_Byte(0xA0);  
		I2C_Wait_Ack();  
		I2C_Send_Byte(addr>>8);   //发送数据地址高位  
	}else{  
		I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址  
	}  

	I2C_Wait_Ack();  
	I2C_Send_Byte(addr%256);//双字节是数据地址低位          
	        //单字节是数据地址低位  
	I2C_Wait_Ack();  

	I2C_Send_Byte(dt>>8);  
	I2C_Wait_Ack();  

	I2C_Send_Byte(dt&0xFF);  
	I2C_Wait_Ack();  

	I2C_Stop();  
	__enable_irq();// ensable Interrupts
	delay_ms(10);  
	
}  

u32 AT24Cxx_ReadU32(u16 addr){
	u32 Buff;
	Buff = AT24Cxx_ReadOneByte(addr + 0);
	Buff <<= 8;
	Buff += AT24Cxx_ReadOneByte(addr + 1);
	Buff <<= 8;
	Buff += AT24Cxx_ReadOneByte(addr + 2);
	Buff <<= 8;
	Buff += AT24Cxx_ReadOneByte(addr + 3);
	return Buff;
}
void AT24Cxx_WriteU32(u16 addr,u32 dt){
	AT24Cxx_WriteOneByte(addr + 0,(u8)(dt >> 24));
	AT24Cxx_WriteOneByte(addr + 1,(u8)(dt >> 16));
	AT24Cxx_WriteOneByte(addr + 2,(u8)(dt >> 8));
	AT24Cxx_WriteOneByte(addr + 3,(u8)(dt));
}

void AT24Cxx_ReadString(u16 addr,u8* buff,u8 len_Max){
	u8 i;
	for(i = 0;i < len_Max;i++){
		buff[i] = AT24Cxx_ReadOneByte(addr + i);
		if(buff[i] == '\0'){//如果是字符串的结束符就跳出
			return;
		}
	}
}

//len_Max  最长限制
void AT24Cxx_WriteString(u16 addr,u8* buff,u8 len_Max){
	u8 i;
	for(i = 0;i < len_Max;i++){
		AT24Cxx_WriteOneByte(addr + i,buff[i]);
		if(buff[i] == '\0'){//如果是字符串的结束符就跳出
			return;
		}
	}
}


void Read_data_from_EEP(u16 Fram_add,u8 *p){
	u8 i;
	for(i=0;i<8;i++){
	 *p = AT24Cxx_ReadOneByte(Fram_add); p++;
	 Fram_add++;
	}
}
