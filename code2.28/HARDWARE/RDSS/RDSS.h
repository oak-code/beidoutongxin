#ifndef __RDSS_H
#define __RDSS_H	 
#include "sys.h"

#define RDSS_RX_RXBUFF_LEN	1000
#define RDSS_TX_TXBUFF_LEN	77

#define RDSS_CMD_ICA	"$CCICA,0,00*7B\r\n"//读取SIM卡信息指令
#define RDSS_CMD_BSI	"$CCRMO,BSI,2,0*26\r\n"//读取卡号
#define RDSS_CMD_DWA	"$CCDWA,0000000,V,1,L,,0,,,0*65\r\n"//定位命令
#define RDSS_CMD_ZDA	"$CCRMO,ZDA,2,0*21\r\n"//读取时间

#define RDSS_CCICA	"CCICA"//读取卡号
#define RDSS_CCMOR	"CCMOR"//获取信号强度
#define RDSS_CCROM	"CCROM"//读取当前时间
#define RDSS_CCDWA  "CCDWA"//读取当前位置
#define RDSS_CCTXA	"CCTXA"//发送短报文

#define RDSS_BDICI	"BDICI"//卡号信息
#define RDSS_BDFKI	"BDFKI"//回执信息
#define RDSS_BDZDA	"BDZDA"//时间回执
#define RDSS_BDBSI	"BDBSI"//信号强度
#define RDSS_BDDWR	"BDDWR"//读取卡号
#define RDSS_BDTXR	"BDTXR"//读取卡号
#define RDSS_CCICA	"CCICA"//读取卡号
#define RDSS_CCICA	"CCICA"//读取卡号
#define RDSS_CCICA	"CCICA"//读取卡号

extern u16 Read_Signal_Counter;
typedef struct{
	u16 rdss_rx_timeout;
	u16 RDSS_RXBUFF_LEN;
	u8 RDSS_RX_BUFF[RDSS_RX_RXBUFF_LEN];
	
	

}rdss_rx;//接收

typedef struct{
	u32 RDSS_TX_Time;//发送数据时间间隔
	u8 RDSS_TX_DATA_STA;//发送报文标志 0报文1报文2报文 
	u32 RDSS_TX_DATA_Time;//发送数据时间间隔
	u8 RDSS_TXBUFF1_LEN;
	u8 RDSS_TXBUFF2_LEN;
	u8 RDSS_TX_BUFF1[RDSS_TX_TXBUFF_LEN];
	u8 RDSS_TX_BUFF2[RDSS_TX_TXBUFF_LEN];		
	u8 RDSS_TX_ADDR[7];
}rdss_tx;//发送

typedef struct{
	u8 CMD[5];
	u16 LEN;
	u8 BUFF[RDSS_RX_RXBUFF_LEN];
}rdss_data;	//数据

typedef struct{
	u8 addr[7];
	u8 num[8];
	u8 T_addr[7];
	u8 characteristic;
	u8 Frequentness[2];
	u8 Class;
	u8 Encryption;
	u8 BUFF[RDSS_RX_RXBUFF_LEN];
}rdss_card;	//sim卡

typedef struct{
	u8 signal_sta;//信号状态
	u32 signal_Timeout;
	u32 Read_Signal_Counter;//信号时间频度
	u32 Read_Location_Counter;//定位时间频度
	u32 Server_Counter;//模块服务频度
}rdss;	//模块


void Send_String(u8 *str);
void RDSS_Init(void);
void RDSS_Send_TXBuff(u8* buff,u16 len);
void RDSS_uart_init(u32 bound);
void RDSS_DMA_data_processing(void);//RDSS接收数据处理函数
void RDSS_Time_incident(void);
void RDSS_Write_TXBuff(u8* buff,u16 len);
extern rdss_rx RDSS_RX;	
extern rdss_tx RDSS_TX;	
extern rdss RDSS;
extern u8 longitude[];	//经度
extern u8 latitude[];	//纬度
#endif
