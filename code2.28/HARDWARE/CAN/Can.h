#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//CAN���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.1 
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved	
//********************************************************************************
//V1.1�޸�˵�� 20150528
//������CAN��ʼ�����������ע�ͣ������˲����ʼ��㹫ʽ
////////////////////////////////////////////////////////////////////////////////// 	


#define 	PDU_ZhuanSu					0xf004		//ת��
#define 	PDU_YouShuiWen				0xfeee		//�����¶�ˮ��
#define 	PDU_RanYouYaLi				0xfeef		//ȼ��ѹ����ѹ��

#define 	PDU_PaiQI1					0xfea3		//����1
#define 	PDU_PaiQI2					0xfea2		//����2
#define 	PDU_PaiQI3					0xfea1		//����3

#define 	PDU_GuZhang					0xffa7		//����������
#define 	PDU_BaoJing					0xff66		//����������
#define 	PDU_PaiwenZhouwen			0xff67		//�������´���������
#define 	PDU_FengLang				0xff6c		//Խ��ģʽ������ģʽ��ң��ģʽ
#define 	PDU_ZhouWen1				0xff81		//����1
#define 	PDU_ZhouWen2				0xff82		//����2
#define 	PDU_WenDu					0xff85		//����ת�١����ֻ����¶ȡ���ѹ�¶ȡ���չ�¶�
#define 	PDU_YaLi					0xff86		//ˮѹ��ȼ��ѹ�����𶯿���ѹ������ѹ����ѹ������������ѹ��
#define 	PDU_DianYa					0xff87		//����Դ��ѹ�����õ�Դ��ѹ
#define 	PDU_ZhuangTai				0xff88		//Խ��ģʽ������ģʽ����ͣ״̬�����Ƽ����
#define 	PDU_KaiGuanLiang			0xff8a		//
#define 	PDU_YunXing					0xff8b		//�ۼ�����ʱ��




 
 
 
//CAN����RX0�ж�ʹ��
#define CAN_RX0_INT_ENABLE	0		//0,��ʹ��;1,ʹ��.								    									 							 				    
extern u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��
extern u8 Can_Send_Msg(u8* msg,u8 len);						//��������
extern u8 Can_Receive_Msg(u8 *buf);							//��������


extern void GuZhang_Scan(u16 type,u8* arr);
extern void BaoJing_Scan(u8* arr);
extern void Can_Sever(void);
//--------------------------------------------------------


#endif

















