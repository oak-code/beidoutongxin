#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
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


#define 	PDU_ZhuanSu					0xf004		//转速
#define 	PDU_YouShuiWen				0xfeee		//机油温度水温
#define 	PDU_RanYouYaLi				0xfeef		//燃油压机油压力

#define 	PDU_PaiQI1					0xfea3		//排温1
#define 	PDU_PaiQI2					0xfea2		//排温2
#define 	PDU_PaiQI3					0xfea1		//排温3

#define 	PDU_GuZhang					0xffa7		//传感器故障
#define 	PDU_BaoJing					0xff66		//传感器报警
#define 	PDU_PaiwenZhouwen			0xff67		//排温轴温传感器报警
#define 	PDU_FengLang				0xff6c		//越控模式、风浪模式、遥控模式
#define 	PDU_ZhouWen1				0xff81		//轴温1
#define 	PDU_ZhouWen2				0xff82		//轴温2
#define 	PDU_WenDu					0xff85		//安保转速、涡轮机油温度、增压温度、扩展温度
#define 	PDU_YaLi					0xff86		//水压、燃油压力、起动空气压力、增压空气压力、安保机油压力
#define 	PDU_DianYa					0xff87		//主电源电压、备用电源电压
#define 	PDU_ZhuangTai				0xff88		//越控模式、风浪模式、急停状态、控制监控仪
#define 	PDU_KaiGuanLiang			0xff8a		//
#define 	PDU_YunXing					0xff8b		//累计运行时间




 
 
 
//CAN接收RX0中断使能
#define CAN_RX0_INT_ENABLE	0		//0,不使能;1,使能.								    									 							 				    
extern u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
extern u8 Can_Send_Msg(u8* msg,u8 len);						//发送数据
extern u8 Can_Receive_Msg(u8 *buf);							//接收数据


extern void GuZhang_Scan(u16 type,u8* arr);
extern void BaoJing_Scan(u8* arr);
extern void Can_Sever(void);
//--------------------------------------------------------


#endif

















