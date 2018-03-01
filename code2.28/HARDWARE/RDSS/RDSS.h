#ifndef __RDSS_H
#define __RDSS_H	 
#include "sys.h"

#define RDSS_RX_RXBUFF_LEN	1000
#define RDSS_TX_TXBUFF_LEN	77

#define RDSS_CMD_ICA	"$CCICA,0,00*7B\r\n"//��ȡSIM����Ϣָ��
#define RDSS_CMD_BSI	"$CCRMO,BSI,2,0*26\r\n"//��ȡ����
#define RDSS_CMD_DWA	"$CCDWA,0000000,V,1,L,,0,,,0*65\r\n"//��λ����
#define RDSS_CMD_ZDA	"$CCRMO,ZDA,2,0*21\r\n"//��ȡʱ��

#define RDSS_CCICA	"CCICA"//��ȡ����
#define RDSS_CCMOR	"CCMOR"//��ȡ�ź�ǿ��
#define RDSS_CCROM	"CCROM"//��ȡ��ǰʱ��
#define RDSS_CCDWA  "CCDWA"//��ȡ��ǰλ��
#define RDSS_CCTXA	"CCTXA"//���Ͷ̱���

#define RDSS_BDICI	"BDICI"//������Ϣ
#define RDSS_BDFKI	"BDFKI"//��ִ��Ϣ
#define RDSS_BDZDA	"BDZDA"//ʱ���ִ
#define RDSS_BDBSI	"BDBSI"//�ź�ǿ��
#define RDSS_BDDWR	"BDDWR"//��ȡ����
#define RDSS_BDTXR	"BDTXR"//��ȡ����
#define RDSS_CCICA	"CCICA"//��ȡ����
#define RDSS_CCICA	"CCICA"//��ȡ����
#define RDSS_CCICA	"CCICA"//��ȡ����

extern u16 Read_Signal_Counter;
typedef struct{
	u16 rdss_rx_timeout;
	u16 RDSS_RXBUFF_LEN;
	u8 RDSS_RX_BUFF[RDSS_RX_RXBUFF_LEN];
	
	

}rdss_rx;//����

typedef struct{
	u32 RDSS_TX_Time;//��������ʱ����
	u8 RDSS_TX_DATA_STA;//���ͱ��ı�־ 0����1����2���� 
	u32 RDSS_TX_DATA_Time;//��������ʱ����
	u8 RDSS_TXBUFF1_LEN;
	u8 RDSS_TXBUFF2_LEN;
	u8 RDSS_TX_BUFF1[RDSS_TX_TXBUFF_LEN];
	u8 RDSS_TX_BUFF2[RDSS_TX_TXBUFF_LEN];		
	u8 RDSS_TX_ADDR[7];
}rdss_tx;//����

typedef struct{
	u8 CMD[5];
	u16 LEN;
	u8 BUFF[RDSS_RX_RXBUFF_LEN];
}rdss_data;	//����

typedef struct{
	u8 addr[7];
	u8 num[8];
	u8 T_addr[7];
	u8 characteristic;
	u8 Frequentness[2];
	u8 Class;
	u8 Encryption;
	u8 BUFF[RDSS_RX_RXBUFF_LEN];
}rdss_card;	//sim��

typedef struct{
	u8 signal_sta;//�ź�״̬
	u32 signal_Timeout;
	u32 Read_Signal_Counter;//�ź�ʱ��Ƶ��
	u32 Read_Location_Counter;//��λʱ��Ƶ��
	u32 Server_Counter;//ģ�����Ƶ��
}rdss;	//ģ��


void Send_String(u8 *str);
void RDSS_Init(void);
void RDSS_Send_TXBuff(u8* buff,u16 len);
void RDSS_uart_init(u32 bound);
void RDSS_DMA_data_processing(void);//RDSS�������ݴ�����
void RDSS_Time_incident(void);
void RDSS_Write_TXBuff(u8* buff,u16 len);
extern rdss_rx RDSS_RX;	
extern rdss_tx RDSS_TX;	
extern rdss RDSS;
extern u8 longitude[];	//����
extern u8 latitude[];	//γ��
#endif
