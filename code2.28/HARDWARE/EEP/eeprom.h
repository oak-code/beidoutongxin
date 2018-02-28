#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "sys.h"
//如果移植程序时只要改一下三个地方就行了  
#define I2C_SCL 		GPIO_Pin_14  
#define I2C_SDA 		GPIO_Pin_15 
#define GPIO_I2C 	GPIOC 
#define EEP_WP 		GPIO_Pin_13
#define GPIO_WP 	GPIOC 

#define EEP_WP_H GPIO_SetBits(GPIO_WP,EEP_WP) 
#define EEP_WP_L GPIO_ResetBits(GPIO_WP,EEP_WP) 

#define I2C_SCL_H GPIO_SetBits(GPIO_I2C,I2C_SCL)  
#define I2C_SCL_L GPIO_ResetBits(GPIO_I2C,I2C_SCL)  
  
#define I2C_SDA_H GPIO_SetBits(GPIO_I2C,I2C_SDA)  
#define I2C_SDA_L GPIO_ResetBits(GPIO_I2C,I2C_SDA)  
  
extern void I2C_Configuration(void);  
void I2C_SDA_OUT(void);  
void I2C_SDA_IN(void);  
void I2C_Start(void);  
void I2C_Stop(void);  
void I2C_Ack(void);  
void I2C_NAck(void);  
u8   I2C_Wait_Ack(void);  
void I2C_Send_Byte(u8 txd);  
u8   I2C_Read_Byte(u8 ack); 

//////////////24Cxx相关//////////////////////
#define AT24C01  	127  
#define AT24C02  	255  
#define AT24C04  	511  
#define AT24C08  	1023  
#define AT24C16  	2047  
#define AT24C32  	4095  
#define AT24C64  	8191  
#define AT24C128 	16383  
#define AT24C256 	32767  
#define EE_TYPE  	AT24C32
extern void AT24Cxx_Init(void);
extern u8 AT24Cxx_ReadOneByte(u16 addr);  
extern u16 AT24Cxx_ReadTwoByte(u16 addr);  
extern void AT24Cxx_WriteOneByte(u16 addr,u8 dt);  
extern void AT24Cxx_WriteTwoByte(u16 addr,u16 dt);  
extern u32 AT24Cxx_ReadU32(u16 addr);
extern void AT24Cxx_WriteU32(u16 addr,u32 dt);
extern void AT24Cxx_ReadString(u16 addr,u8* buff,u8 len_Max);
extern void AT24Cxx_WriteString(u16 addr,u8* buff,u8 len_Max);
extern void Read_data_from_EEP(u16 Fram_add,u8 *p);



/*****************************测试用管脚配置*************************/

#define EEP_EMPTY_FLAG 							0xa1		//空标识
#define EEP_EMPTY_FLAG_ADDR					10			//空标识地址
#define EEP_Base_ADDR							20			//数据起始地址
#define EEP_PRARM_Base_ADDR					(EEP_Base_ADDR + 0)	//数据(参数)起始地址

#define EEP_PRARM_Tongxin_ID			(EEP_PRARM_Base_ADDR + (0*4))			// 北斗通信ID地址
#define EEP_PRARM_ID					(EEP_PRARM_Tongxin_ID+20)				//本机ID地址





#endif

