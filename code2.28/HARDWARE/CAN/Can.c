#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "RDSS.h"
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
 
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;  tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//������Ϊ:36M/((8+9+1)*4)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��IO

	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;			//��ʱ�䴥��ͨ��ģʽ  
	CAN_InitStructure.CAN_ABOM=DISABLE;			//����Զ����߹���	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=ENABLE;			//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=DISABLE;			//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode= mode;	        //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; 
	//���ò�����
	CAN_InitStructure.CAN_SJW=tsjw;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	//��ʼ��CAN1 

	CAN_FilterInitStructure.CAN_FilterNumber=0;	//������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//����λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32λ�� 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	//32λID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//���������0

	CAN_FilterInit(&CAN_FilterInitStructure);			//�˲�����ʼ��
	
#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0��Ϣ�Һ��ж�����.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN_RX0_INT_ENABLE	//ʹ��RX0�ж�
//�жϷ�����			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	int i=0;
    CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<8;i++)
	printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif


/*=========================================================================
��������: Can_Sever()
��������: can�������ݴ�����
�������: 
=========================================================================*/
void Can_Sever()
{
	u16 PUD;
	u8 i;
	CanRxMsg RxMessage;
	
	
	
	if( CAN_MessagePending(CAN1,CAN_FIFO0)!=0) 		//û�н��յ�����,ֱ���˳� 
	{

		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
		PUD = (u16)(RxMessage.ExtId >> 8);
		
//		#if	_Debug
//		printf("\r\n=-=-=-=-=-=-=-=-=-=-=-=-=-CAN����-=-=-=-=-=-=-=-=-=-=-=-=-\r\n");
//		printf("֡ID��%x\r\n",PUD);
//		
//		printf("���ݣ�%x\r\n",PUD);
//		for(i=0;i<8;i++)
//		printf("%x",RxMessage.Data[i]);
//		
//		printf("\r\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n");
//		#endif
		switch(PUD)
		{
			
		

			case PDU_PaiQI1:	//����1
				
				RDSS_TX.RDSS_TX_BUFF1[1]=RxMessage.Data[0];//����1
				RDSS_TX.RDSS_TX_BUFF1[2]=RxMessage.Data[1];//����1
				RDSS_TX.RDSS_TX_BUFF1[3]=RxMessage.Data[2];//����2
				RDSS_TX.RDSS_TX_BUFF1[4]=RxMessage.Data[3];//����2
				RDSS_TX.RDSS_TX_BUFF1[5]=RxMessage.Data[4];//����3
				RDSS_TX.RDSS_TX_BUFF1[6]=RxMessage.Data[5];//����3
				RDSS_TX.RDSS_TX_BUFF1[7]=RxMessage.Data[6];//����4
				RDSS_TX.RDSS_TX_BUFF1[8]=RxMessage.Data[7];//����4
						
				break;
			case PDU_PaiQI2:	//����2
				
				RDSS_TX.RDSS_TX_BUFF1[9]=RxMessage.Data[0];//����5
				RDSS_TX.RDSS_TX_BUFF1[10]=RxMessage.Data[1];//����5
				RDSS_TX.RDSS_TX_BUFF1[11]=RxMessage.Data[2];//����6
				RDSS_TX.RDSS_TX_BUFF1[12]=RxMessage.Data[3];//����6
				RDSS_TX.RDSS_TX_BUFF1[13]=RxMessage.Data[4];//����7
				RDSS_TX.RDSS_TX_BUFF1[14]=RxMessage.Data[5];//����7
				RDSS_TX.RDSS_TX_BUFF1[15]=RxMessage.Data[6];//����8
				RDSS_TX.RDSS_TX_BUFF1[16]=RxMessage.Data[7];//����8
						
				break;
			case PDU_PaiQI3:	//����3
				
				RDSS_TX.RDSS_TX_BUFF1[17]=RxMessage.Data[0];//����9
				RDSS_TX.RDSS_TX_BUFF1[18]=RxMessage.Data[1];//����9
				RDSS_TX.RDSS_TX_BUFF1[19]=RxMessage.Data[2];//����10
				RDSS_TX.RDSS_TX_BUFF1[20]=RxMessage.Data[3];//����10
				RDSS_TX.RDSS_TX_BUFF1[21]=RxMessage.Data[4];//����11
				RDSS_TX.RDSS_TX_BUFF1[22]=RxMessage.Data[5];//����11
				RDSS_TX.RDSS_TX_BUFF1[23]=RxMessage.Data[6];//����12
				RDSS_TX.RDSS_TX_BUFF1[24]=RxMessage.Data[7];//����12
						
				break;
			case PDU_ZhouWen1:	//����1-8
				
				RDSS_TX.RDSS_TX_BUFF1[25]=RxMessage.Data[0];//����1
				RDSS_TX.RDSS_TX_BUFF1[26]=RxMessage.Data[1];//����2
				RDSS_TX.RDSS_TX_BUFF1[27]=RxMessage.Data[2];//����3
				RDSS_TX.RDSS_TX_BUFF1[28]=RxMessage.Data[3];//����4
				RDSS_TX.RDSS_TX_BUFF1[29]=RxMessage.Data[4];//����5
				RDSS_TX.RDSS_TX_BUFF1[30]=RxMessage.Data[5];//����6
				RDSS_TX.RDSS_TX_BUFF1[31]=RxMessage.Data[6];//����7
				RDSS_TX.RDSS_TX_BUFF1[32]=RxMessage.Data[7];//����8
						
				break;
			case PDU_ZhouWen2:	//����9
				
				RDSS_TX.RDSS_TX_BUFF1[33]=RxMessage.Data[0];//����9
		
				break;
			case PDU_ZhuanSu:		//ת��
		
				RDSS_TX.RDSS_TX_BUFF1[34]=RxMessage.Data[3];
				RDSS_TX.RDSS_TX_BUFF1[35]=RxMessage.Data[4];
			
				break;
			
			case PDU_RanYouYaLi:	//ȼ��ѹ����ѹ��
				
				RDSS_TX.RDSS_TX_BUFF1[38]=RxMessage.Data[0];//ȼ��ѹ��
				RDSS_TX.RDSS_TX_BUFF1[39]=RxMessage.Data[1];//ȼ��ѹ��
				RDSS_TX.RDSS_TX_BUFF1[40]=RxMessage.Data[3];//����ѹ��
				RDSS_TX.RDSS_TX_BUFF1[41]=RxMessage.Data[4];//����ѹ��	
				RDSS_TX.RDSS_TX_BUFF1[44]=RxMessage.Data[3];//��������ѹ��
				RDSS_TX.RDSS_TX_BUFF1[45]=RxMessage.Data[4];//��������ѹ��			
				break;
			case PDU_YouShuiWen:	//��ˮ��
				
				RDSS_TX.RDSS_TX_BUFF1[43]=RxMessage.Data[0];//��ȴˮ��
				RDSS_TX.RDSS_TX_BUFF1[42]=RxMessage.Data[3];//�����¶�
						
				break;
			case PDU_WenDu:	//����ת�١����ֻ����¶ȡ���ѹ�¶ȡ���չ�¶�
				
				RDSS_TX.RDSS_TX_BUFF1[36]=RxMessage.Data[0];//����ת��
				RDSS_TX.RDSS_TX_BUFF1[37]=RxMessage.Data[1];//����ת��
				RDSS_TX.RDSS_TX_BUFF1[54]=RxMessage.Data[2];//���ֻ����¶�
				RDSS_TX.RDSS_TX_BUFF1[55]=RxMessage.Data[3];//��ѹ�¶�
				RDSS_TX.RDSS_TX_BUFF1[56]=RxMessage.Data[4];//��չ�¶�
			
						
				break;
			case PDU_YaLi:	//ˮѹ��ȼ��ѹ�����𶯿���ѹ������ѹ����ѹ��
				
				RDSS_TX.RDSS_TX_BUFF1[52]=RxMessage.Data[0];//��ȴˮѹ
				RDSS_TX.RDSS_TX_BUFF1[53]=RxMessage.Data[1];//��ȴˮѹ
				RDSS_TX.RDSS_TX_BUFF1[46]=RxMessage.Data[2];//���ֻ���ѹ
				RDSS_TX.RDSS_TX_BUFF1[47]=RxMessage.Data[3];//���ֻ���ѹ
				RDSS_TX.RDSS_TX_BUFF1[48]=RxMessage.Data[4];//��������ѹ
				RDSS_TX.RDSS_TX_BUFF1[49]=RxMessage.Data[5];//��������ѹ
				RDSS_TX.RDSS_TX_BUFF1[50]=RxMessage.Data[6];//��ѹ����ѹ
				RDSS_TX.RDSS_TX_BUFF1[51]=RxMessage.Data[7];//��ѹ����ѹ
			
						
				break;
		
			case PDU_DianYa:	//��ѹ
				
				RDSS_TX.RDSS_TX_BUFF1[57]=RxMessage.Data[0];//����Դ��ѹ
				RDSS_TX.RDSS_TX_BUFF1[58]=RxMessage.Data[1];//����Դ��ѹ
				RDSS_TX.RDSS_TX_BUFF1[59]=RxMessage.Data[2];//���õ�Դ��ѹ
				RDSS_TX.RDSS_TX_BUFF1[60]=RxMessage.Data[3];//���õ�Դ��ѹ

						
				break;
			case PDU_YunXing:	//�ۼ�����ʱ��
				
				RDSS_TX.RDSS_TX_BUFF1[61]=RxMessage.Data[0];
				RDSS_TX.RDSS_TX_BUFF1[62]=RxMessage.Data[1];
				RDSS_TX.RDSS_TX_BUFF1[63]=RxMessage.Data[2];
		
				break;
				
			case PDU_GuZhang:	//����
				
				RDSS_TX.RDSS_TX_BUFF2[1]=RxMessage.Data[0];//��ͨ����������
				RDSS_TX.RDSS_TX_BUFF2[2]=RxMessage.Data[1];//��ͨ����������
				RDSS_TX.RDSS_TX_BUFF2[3]=RxMessage.Data[2];//��ͨ����������
				RDSS_TX.RDSS_TX_BUFF2[4]=RxMessage.Data[3];//��ͨ����������
				RDSS_TX.RDSS_TX_BUFF2[5]=RxMessage.Data[4];//���´���������
				RDSS_TX.RDSS_TX_BUFF2[6]=RxMessage.Data[5];//���´���������
				RDSS_TX.RDSS_TX_BUFF2[7]=RxMessage.Data[6];//���´���������
				RDSS_TX.RDSS_TX_BUFF2[8]=RxMessage.Data[7];//���´���������
						
				break;
			case PDU_BaoJing:	//����
				
				RDSS_TX.RDSS_TX_BUFF2[9]=RxMessage.Data[0];//��ͨ����������
				RDSS_TX.RDSS_TX_BUFF2[10]=RxMessage.Data[1];//��ͨ����������
				RDSS_TX.RDSS_TX_BUFF2[11]=RxMessage.Data[2];//��ͨ����������
				RDSS_TX.RDSS_TX_BUFF2[12]=RxMessage.Data[3];//��ͨ����������

						
				break;
			
			case PDU_PaiwenZhouwen:	//���±���
				
				RDSS_TX.RDSS_TX_BUFF2[13]=RxMessage.Data[0];//���´���������
				RDSS_TX.RDSS_TX_BUFF2[14]=RxMessage.Data[1];//���´���������
				RDSS_TX.RDSS_TX_BUFF2[15]=RxMessage.Data[2];//���´���������
				RDSS_TX.RDSS_TX_BUFF2[16]=RxMessage.Data[3];//���´���������
				RDSS_TX.RDSS_TX_BUFF2[17]=RxMessage.Data[4];//���´���������
				RDSS_TX.RDSS_TX_BUFF2[18]=RxMessage.Data[5];//���´���������
				break;
			
			case PDU_KaiGuanLiang:	//������
				
				RDSS_TX.RDSS_TX_BUFF2[19]=RxMessage.Data[0];//
				RDSS_TX.RDSS_TX_BUFF2[20]=RxMessage.Data[1];//
		
				break;
			
			
			case PDU_FengLang:	//Խ��ģʽ������ģʽ��ң��ģʽ
				
				RDSS_TX.RDSS_TX_BUFF2[22]=RxMessage.Data[0];//Խ��ģʽ
				RDSS_TX.RDSS_TX_BUFF2[21]=RxMessage.Data[1];//����ģʽ
				RDSS_TX.RDSS_TX_BUFF2[23]=RxMessage.Data[2];//ң��ģʽ
						
				break;

			default:
				break;
		}
	}
	
	RDSS_TX.RDSS_TX_BUFF1[0]='%';
	RDSS_TX.RDSS_TX_BUFF2[0]='#';


	RDSS_TX.RDSS_TX_BUFF1[64]=latitude[0];
	RDSS_TX.RDSS_TX_BUFF1[65]=latitude[1];
	RDSS_TX.RDSS_TX_BUFF1[66]=latitude[2];
	RDSS_TX.RDSS_TX_BUFF1[67]=latitude[3];
	RDSS_TX.RDSS_TX_BUFF1[68]=latitude[4];//γ��
	RDSS_TX.RDSS_TX_BUFF1[69]=longitude[0];
	RDSS_TX.RDSS_TX_BUFF1[70]=longitude[1];
	RDSS_TX.RDSS_TX_BUFF1[71]=longitude[2];
	RDSS_TX.RDSS_TX_BUFF1[72]=longitude[3];
	RDSS_TX.RDSS_TX_BUFF1[73]=longitude[4];//����
	
	RDSS_TX.RDSS_TXBUFF1_LEN=74;
	RDSS_TX.RDSS_TXBUFF2_LEN=24;
}













