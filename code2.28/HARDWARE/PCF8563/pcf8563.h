#ifndef _PCF_8563_
#define _PCF_8563_
#include "sys.h"
/**/
typedef struct{
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 second;
}DATE;

typedef struct{
	u8 update;//用于向LCD更新数据
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 second;
	u16 adjusting;//校准时间
}RTC_;	//hex数据

//日期
typedef struct{
	u16 year;
	u16 month;
	u16 day;
}date;
extern RTC_ rtc;
//////////////////////////////////////////////////////////////////////////////////							  
////////////////////////////////////////////////////////////////////////////////// 
#define RTC_SDA_IN()    {GPIOC->CRL &= 0XF0FFFFFF;GPIOC->CRL |= 0X08000000;}
#define RTC_SDA_OUT() {GPIOC->CRL &= 0XF0FFFFFF;GPIOC->CRL |= 0X01000000;}

#define RTC_SCL			PBout(15)
#define RTC_SDA 			PCout(6)
#define RTC_Read_SDA 	PCin(6)


extern void RTC_i2c_start(void);
extern void RTC_i2c_stop(void);
extern void RTC_i2c_send_byte (u8 ICdate);
extern u8 RTC_Read_data (void);
extern u8 RTC_i2c_rec_byte(u8 address);
extern void RTC_i2c_write_byte(u8 data_addr,u8 data_byte);
extern void RTC_IO_init(void);
extern void RTC_init(void);
extern void RTC_Reset(void);
extern void RTC_Read_Time(RTC_* Date);
extern void RTC_Write_Time(RTC_* Date);
extern date Now_to_Later(date now, u16 gap);

#endif
