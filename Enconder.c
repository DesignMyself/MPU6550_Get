/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2018-04-21
  * ��    ��: �������ӿ�����
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F1Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "enconder.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "HDMI.H"
#include "app_uSart.h"
#include "pin_def.h"
int32_t OverflowCount = 0;//��ʱ���������
/* Timer handler declaration */
TIM_HandleTypeDef    htimx_Encoder;
uint32_t Encoder1=32767;//���ڼ�¼������������
	uint32_t Dis_Rec1=0;
	uint32_t kk=0;//��ȡоƬ�Ĵ����е�ֵ
	uint8_t sign=0;//�����жϱ�־
	uint32_t sum_dis=0;//����������ͳ��
	uint32_t gd=0;//���ڴ洢�仯��
	uint8_t a=0;//��־������
	uint8_t c=0;//������־
	float wheel=50;//���������Ӵ�С
	rt_uint16_t resolution=50;//�ֱ���
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ͨ�ö�ʱ����ʼ��������ͨ��PWM���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void ENCODER_TIMx_Init(void)
{
  /* Timer Encoder Configuration Structure declaration */
  TIM_Encoder_InitTypeDef sEncoderConfig;

  ENCODER_TIM_RCC_CLK_ENABLE();//����ʱ��
  htimx_Encoder.Instance = ENCODER_TIMx;
  htimx_Encoder.Init.Prescaler = ENCODER_TIM_PRESCALER;
  htimx_Encoder.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx_Encoder.Init.Period = ENCODER_TIM_PERIOD;
  htimx_Encoder.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;

  sEncoderConfig.EncoderMode        = TIM_ENCODERMODE_TIx;    
  sEncoderConfig.IC1Polarity        = TIM_ICPOLARITY_RISING;   
  sEncoderConfig.IC1Selection       = TIM_ICSELECTION_DIRECTTI;  
  sEncoderConfig.IC1Prescaler       = TIM_ICPSC_DIV1; 
  sEncoderConfig.IC1Filter          = 0;
  
  sEncoderConfig.IC2Polarity        = TIM_ICPOLARITY_RISING;   
  sEncoderConfig.IC2Selection       = TIM_ICSELECTION_DIRECTTI;  
  sEncoderConfig.IC2Prescaler       = TIM_ICPSC_DIV1; 
  sEncoderConfig.IC2Filter          = 0;  
  HAL_TIM_Encoder_Init(&htimx_Encoder, &sEncoderConfig);

//  __HAL_TIM_CLEAR_IT(&htimx_Encoder, TIM_IT_UPDATE);  // ��������жϱ�־λ
//  __HAL_TIM_URS_ENABLE(&htimx_Encoder);               // ���������������Ų��������ж�
//  __HAL_TIM_ENABLE_IT(&htimx_Encoder,TIM_IT_UPDATE);  // ʹ�ܸ����ж�
//  
  HAL_NVIC_SetPriority(ENCODER_TIM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ENCODER_TIM_IRQn);
}

/**
  * ��������: ������ʱ��Ӳ����ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_base)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==ENCODER_TIMx)
  {
    /* ������ʱ������ʱ��ʹ�� */
    ENCODER_TIM_GPIO_CLK_ENABLE();

    /* ��ʱ��ͨ��1��������IO��ʼ�� */
    GPIO_InitStruct.Pin = ENCODER_TIM_CH1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Alternate = ENCODER_TIM_GPIO_AF;
    HAL_GPIO_Init(ENCODER_TIM_CH1_GPIO, &GPIO_InitStruct);
    
    /* ��ʱ��ͨ��2��������IO��ʼ�� */
    GPIO_InitStruct.Pin = ENCODER_TIM_CH2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Alternate = ENCODER_TIM_GPIO_AF;
    HAL_GPIO_Init(ENCODER_TIM_CH2_GPIO, &GPIO_InitStruct);
  }
}

/**
  * ��������: ������ʱ��Ӳ������ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==ENCODER_TIMx)
  {
    /* ������ʱ������ʱ�ӽ��� */
    ENCODER_TIM_RCC_CLK_DISABLE();
    
    HAL_GPIO_DeInit(ENCODER_TIM_CH1_GPIO, ENCODER_TIM_CH1_PIN);
    HAL_GPIO_DeInit(ENCODER_TIM_CH2_GPIO, ENCODER_TIM_CH2_PIN);
  }
} 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htimx_Encoder))
    OverflowCount--;       //���¼������
  else
    OverflowCount++;       //���ϼ������
}
/*******************���������ݲɼ�����ʾ��������********************/
int Encoder1_Catch()
{
	kk=__HAL_TIM_GET_COUNTER(&htimx_Encoder);
		
	if(kk==Encoder1)
		{
			Dis_Rec1=Dis_Rec1;
			
			return 0;
		}
	/*******���������ж�*******/
	if(kk>Encoder1)//����������ʱ
		{
			a=0;
			gd=kk-Encoder1;
			if(sign==0)//��ʱDisΪ������������
			{
				Dis_Rec1=Dis_Rec1+gd;
				return 0;
			}
			if(sign==1)//��ʱDISΪ������������
			{
				if(gd>Dis_Rec1)//�ɸ����������Ϊ��
				{
					Dis_Rec1=gd-Dis_Rec1;
					sign=0;
					c=0;
					return 0;
				}
				else{
					Dis_Rec1=Dis_Rec1-gd;//��ȻΪ��
					c=1;
					return 0;
				}
			}
			
	}
		/*******���������ж�*******/
	if(kk<Encoder1)//����������ʱ
{
			a=1;
			gd=Encoder1-kk;
			if(sign==1)
			{
				Dis_Rec1=Dis_Rec1+gd;
				return 0;
			}
			if(sign==0)//��ʱDisΪ������������
			{
				if(gd>Dis_Rec1)//���������ߴ���Disʱ,��ʱΪ���Ľ��
				{
					Dis_Rec1=gd-Dis_Rec1;
					c=0;
					sign=1;
					return 0;
				}else{
					Dis_Rec1=Dis_Rec1-gd;
					c=1;
					return 0;
				}
			}
		
		
}
		return 0;
}
void HMI1_Display()
{
	sum_dis=((float)((Dis_Rec1*wheel*3.14159f/(4*resolution))))*100;
	if((a==0&&sign==0)|(a==1&&sign==0&&c==1))
	{
		HDMI0_2("page0.t0.txt=",Dis_Rec1*100);
		HDMI0_2("page0.t4.txt=",sum_dis);
		
	}
	else if((a==0&&sign==1&&c==1)|(a==1&&sign==1))
	{
		HDMI_0_2("page0.t0.txt=",Dis_Rec1*100);
		HDMI_0_2("page0.t4.txt=",sum_dis);
		
	}
	
}
/*************������������һ��������ȡ���ݣ�һ��������ʾ����******************/
/* �̵߳�TCB���ƿ� */
static struct rt_thread *Encoder1_thread;
static struct rt_thread *HMI1_thread;

static void Encoder1_entry(void *parameter)
{
	while(1)
	{
		rt_enter_critical();
		Encoder1_Catch();
		__HAL_TIM_SET_COUNTER(&htimx_Encoder,Encoder1);	
		rt_exit_critical();
		rt_thread_delay(1);
	}
	
}

static void HMI1_entry(void *parameter)
{
	while(1){
			HMI1_Display();
		
			rt_thread_delay(5);
	}
}
/****************������������������***************/
rt_uint8_t ch[14];
rt_uint32_t SumMotor1;
rt_uint32_t SumMotor2;
static struct rt_thread  * Encoder_cls_thread;
static void Encoder_cls_entry(void *parameter)
{	
		
	while(1)
	{
		
		if(ch[3]==0x06&&ch[1]==0x03)
		{
			Dis_Rec1=0;
			SumMotor1=0;
			SumMotor2=0;
			ch[3]=0;

		}
		rt_thread_delay(2);
	}
}
/****************������ǰ�����˾����趨����***************/
rt_uint8_t h=0;
static struct rt_thread  * Encoder_autorun_thread;
static void Encoder_autorun_entry(void *parameter)
{	
		rt_uint32_t Enc_auto_ahead=0;
		rt_uint32_t Enc_auto_back=0;
	while(1)
	{
		
		if(ch[3]==0x05&&ch[9]==0xE0)
		{
			Enc_auto_ahead=ch[5]*256+ch[6];
			Enc_auto_back=ch[7]*256+ch[8];
			ch[3]=0;
			rt_pin_write(PB3,1);//�������
			h=1;
		}
		if(h==1)
		{
		if((a==0&&sign==0)|(a==1&&sign==0&&c==1))
		{
			if((Enc_auto_ahead*100)<=sum_dis)
			{
				rt_pin_write(PA7,0);//ǰ����ʾ��
				rt_pin_write(PB3,0);//�������
				h=0;
			}
			
		}
		if((a==0&&sign==1&&c==1)|(a==1&&sign==1))
		{
			if((Enc_auto_back*100)<=sum_dis)
			{
				
				rt_pin_write(PA6,0);//������ʾ��
				rt_pin_write(PB3,0);//�������
				h=0;
			}
		}
	}
		rt_thread_delay(2);
	}
}

/****************����������ֱ���ͷֱ����趨***************/

static struct rt_thread  * Encoder_Para_thread;
static void Encoder_Para_entry(void *parameter)
{	
		
	while(1)
	{
		
		if(ch[1]==0x04)
		{
			wheel=(float)((ch[2]*256+ch[3])/(ch[4]*256+ch[5]));
			resolution=ch[6]*256+ch[7];
			ch[1]=0;//�Ͽ���־λ
		}
		
		rt_thread_delay(2);
	}
}

/******************�����ʼ��*****************/
void  Enconder1_Hmi_Init()
{
	ENCODER_TIMx_Init();
	HAL_TIM_Encoder_Start(&htimx_Encoder, TIM_CHANNEL_ALL);
	__HAL_TIM_SET_COUNTER(&htimx_Encoder,Encoder1);	
	
	/********������������ȡ������ƿ鶨��**************/
	Encoder1_thread= rt_thread_create("Encoder1",//���������
														Encoder1_entry,//��������������
														RT_NULL,//����Ĵ������
														1024,//��ջ�Ĵ�С
														4,//���ȼ���С
														5);//ʱ��Ƭ����ͬһ���ȼ�ʱʹ��
		if(Encoder1_thread!= RT_NULL)
		{
			
			rt_thread_startup(Encoder1_thread);
			
		}
		HMI1_thread= rt_thread_create("HMI1",//���������
														HMI1_entry,//��������������
														RT_NULL,//����Ĵ������
														1024,//��ջ�Ĵ�С
														10,//���ȼ���С
														10);//ʱ��Ƭ����ͬһ���ȼ�ʱʹ��
		if(HMI1_thread!= RT_NULL)
		{
			
			rt_thread_startup(HMI1_thread);
			
		}
		
		Encoder_cls_thread= rt_thread_create("Encoder_cls",//���������
														Encoder_cls_entry,//��������������
														RT_NULL,//����Ĵ������
														1024,//��ջ�Ĵ�С
														11,//���ȼ���С
														10);//ʱ��Ƭ����ͬһ���ȼ�ʱʹ��
		if(Encoder_cls_thread!=RT_NULL)
		{
			
			rt_thread_startup(Encoder_cls_thread);
			
		}
		
		/********************************************/
		Encoder_autorun_thread= rt_thread_create("Encoder_autorun",//���������
														Encoder_autorun_entry,//��������������
														RT_NULL,//����Ĵ������
														1024,//��ջ�Ĵ�С
														2,//���ȼ���С
														5);//ʱ��Ƭ����ͬһ���ȼ�ʱʹ��
		if(Encoder_autorun_thread!=RT_NULL)
		{
			
			rt_thread_startup(Encoder_autorun_thread);
			
		}
	/********************************************/
		Encoder_Para_thread= rt_thread_create("Encoder_Para",//���������
														Encoder_Para_entry,//��������������
														RT_NULL,//����Ĵ������
														1024,//��ջ�Ĵ�С
														12,//���ȼ���С
														10);//ʱ��Ƭ����ͬһ���ȼ�ʱʹ��
		if(Encoder_Para_thread!=RT_NULL)
		{
			
			rt_thread_startup(Encoder_Para_thread);
			
		}
	
}
/* ���������RT_SAMPLES_AUTORUN������뵽��ʼ���߳����Զ����� */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(Enconder1_Hmi_Init);
#endif
/* ������ msh �����б��� */
MSH_CMD_EXPORT(Enconder1_Hmi_Init, Enconder1_Hmi_Sample);
