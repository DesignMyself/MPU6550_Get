/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2018-04-21
  * 功    能: 编码器接口例程
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "enconder.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "HDMI.H"
#include "app_uSart.h"
#include "pin_def.h"
int32_t OverflowCount = 0;//定时器溢出次数
/* Timer handler declaration */
TIM_HandleTypeDef    htimx_Encoder;
uint32_t Encoder1=32767;//用于记录编码器脉冲数
	uint32_t Dis_Rec1=0;
	uint32_t kk=0;//获取芯片寄存器中的值
	uint8_t sign=0;//正负判断标志
	uint32_t sum_dis=0;//编码器距离统计
	uint32_t gd=0;//用于存储变化量
	uint8_t a=0;//标志正反向
	uint8_t c=0;//正负标志
	float wheel=50;//编码器轮子大小
	rt_uint16_t resolution=50;//分辨率
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 通用定时器初始化并配置通道PWM输出
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void ENCODER_TIMx_Init(void)
{
  /* Timer Encoder Configuration Structure declaration */
  TIM_Encoder_InitTypeDef sEncoderConfig;

  ENCODER_TIM_RCC_CLK_ENABLE();//开启时钟
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

//  __HAL_TIM_CLEAR_IT(&htimx_Encoder, TIM_IT_UPDATE);  // 清除更新中断标志位
//  __HAL_TIM_URS_ENABLE(&htimx_Encoder);               // 仅允许计数器溢出才产生更新中断
//  __HAL_TIM_ENABLE_IT(&htimx_Encoder,TIM_IT_UPDATE);  // 使能更新中断
//  
  HAL_NVIC_SetPriority(ENCODER_TIM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ENCODER_TIM_IRQn);
}

/**
  * 函数功能: 基本定时器硬件初始化配置
  * 输入参数: htim_base：基本定时器句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_base)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==ENCODER_TIMx)
  {
    /* 基本定时器外设时钟使能 */
    ENCODER_TIM_GPIO_CLK_ENABLE();

    /* 定时器通道1功能引脚IO初始化 */
    GPIO_InitStruct.Pin = ENCODER_TIM_CH1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Alternate = ENCODER_TIM_GPIO_AF;
    HAL_GPIO_Init(ENCODER_TIM_CH1_GPIO, &GPIO_InitStruct);
    
    /* 定时器通道2功能引脚IO初始化 */
    GPIO_InitStruct.Pin = ENCODER_TIM_CH2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Alternate = ENCODER_TIM_GPIO_AF;
    HAL_GPIO_Init(ENCODER_TIM_CH2_GPIO, &GPIO_InitStruct);
  }
}

/**
  * 函数功能: 基本定时器硬件反初始化配置
  * 输入参数: htim_base：基本定时器句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==ENCODER_TIMx)
  {
    /* 基本定时器外设时钟禁用 */
    ENCODER_TIM_RCC_CLK_DISABLE();
    
    HAL_GPIO_DeInit(ENCODER_TIM_CH1_GPIO, ENCODER_TIM_CH1_PIN);
    HAL_GPIO_DeInit(ENCODER_TIM_CH2_GPIO, ENCODER_TIM_CH2_PIN);
  }
} 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htimx_Encoder))
    OverflowCount--;       //向下计数溢出
  else
    OverflowCount++;       //向上计数溢出
}
/*******************编码器数据采集和显示处理任务********************/
int Encoder1_Catch()
{
	kk=__HAL_TIM_GET_COUNTER(&htimx_Encoder);
		
	if(kk==Encoder1)
		{
			Dis_Rec1=Dis_Rec1;
			
			return 0;
		}
	/*******正向行走判断*******/
	if(kk>Encoder1)//当正向行走时
		{
			a=0;
			gd=kk-Encoder1;
			if(sign==0)//此时Dis为正，正向行走
			{
				Dis_Rec1=Dis_Rec1+gd;
				return 0;
			}
			if(sign==1)//此时DIS为负，正向行走
			{
				if(gd>Dis_Rec1)//由负变正，结果为正
				{
					Dis_Rec1=gd-Dis_Rec1;
					sign=0;
					c=0;
					return 0;
				}
				else{
					Dis_Rec1=Dis_Rec1-gd;//仍然为负
					c=1;
					return 0;
				}
			}
			
	}
		/*******反向行走判断*******/
	if(kk<Encoder1)//当反向行走时
{
			a=1;
			gd=Encoder1-kk;
			if(sign==1)
			{
				Dis_Rec1=Dis_Rec1+gd;
				return 0;
			}
			if(sign==0)//此时Dis为正，反向行走
			{
				if(gd>Dis_Rec1)//当反向行走大于Dis时,此时为负的结果
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
/*************建立两个任务，一个用来收取数据，一个用来显示数据******************/
/* 线程的TCB控制块 */
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
/****************编码器距离清零任务***************/
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
/****************编码器前进后退距离设定任务***************/
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
			rt_pin_write(PB3,1);//开启电机
			h=1;
		}
		if(h==1)
		{
		if((a==0&&sign==0)|(a==1&&sign==0&&c==1))
		{
			if((Enc_auto_ahead*100)<=sum_dis)
			{
				rt_pin_write(PA7,0);//前进显示灯
				rt_pin_write(PB3,0);//开启电机
				h=0;
			}
			
		}
		if((a==0&&sign==1&&c==1)|(a==1&&sign==1))
		{
			if((Enc_auto_back*100)<=sum_dis)
			{
				
				rt_pin_write(PA6,0);//后退显示灯
				rt_pin_write(PB3,0);//开启电机
				h=0;
			}
		}
	}
		rt_thread_delay(2);
	}
}

/****************编码器轮子直径和分辨率设定***************/

static struct rt_thread  * Encoder_Para_thread;
static void Encoder_Para_entry(void *parameter)
{	
		
	while(1)
	{
		
		if(ch[1]==0x04)
		{
			wheel=(float)((ch[2]*256+ch[3])/(ch[4]*256+ch[5]));
			resolution=ch[6]*256+ch[7];
			ch[1]=0;//断开标志位
		}
		
		rt_thread_delay(2);
	}
}

/******************任务初始化*****************/
void  Enconder1_Hmi_Init()
{
	ENCODER_TIMx_Init();
	HAL_TIM_Encoder_Start(&htimx_Encoder, TIM_CHANNEL_ALL);
	__HAL_TIM_SET_COUNTER(&htimx_Encoder,Encoder1);	
	
	/********编码器参数获取任务控制块定义**************/
	Encoder1_thread= rt_thread_create("Encoder1",//任务的名称
														Encoder1_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														4,//优先级大小
														5);//时间片，在同一优先级时使用
		if(Encoder1_thread!= RT_NULL)
		{
			
			rt_thread_startup(Encoder1_thread);
			
		}
		HMI1_thread= rt_thread_create("HMI1",//任务的名称
														HMI1_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														10,//优先级大小
														10);//时间片，在同一优先级时使用
		if(HMI1_thread!= RT_NULL)
		{
			
			rt_thread_startup(HMI1_thread);
			
		}
		
		Encoder_cls_thread= rt_thread_create("Encoder_cls",//任务的名称
														Encoder_cls_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														11,//优先级大小
														10);//时间片，在同一优先级时使用
		if(Encoder_cls_thread!=RT_NULL)
		{
			
			rt_thread_startup(Encoder_cls_thread);
			
		}
		
		/********************************************/
		Encoder_autorun_thread= rt_thread_create("Encoder_autorun",//任务的名称
														Encoder_autorun_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														2,//优先级大小
														5);//时间片，在同一优先级时使用
		if(Encoder_autorun_thread!=RT_NULL)
		{
			
			rt_thread_startup(Encoder_autorun_thread);
			
		}
	/********************************************/
		Encoder_Para_thread= rt_thread_create("Encoder_Para",//任务的名称
														Encoder_Para_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														12,//优先级大小
														10);//时间片，在同一优先级时使用
		if(Encoder_Para_thread!=RT_NULL)
		{
			
			rt_thread_startup(Encoder_Para_thread);
			
		}
	
}
/* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(Enconder1_Hmi_Init);
#endif
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Enconder1_Hmi_Init, Enconder1_Hmi_Sample);
