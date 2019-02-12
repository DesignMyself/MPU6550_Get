#include "motor1.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "HDMI.H"
#include "app_uSart.h"
#include "pin_def.h"

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
extern rt_uint8_t ch[14];
float motor_speed_r_min=0;
rt_uint32_t motor_speed_r_GD=0;
static void MX_TIM2_Init(void)
{
	
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_RISING ;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	__HAL_TIM_SET_COUNTER(&htim2,0);
	 HAL_TIM_Base_Init(&htim2);	
}


static void MX_TIM3_Init(void)
{
	
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0xff;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_RISING ;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	__HAL_TIM_SET_COUNTER(&htim3,0);
	 HAL_TIM_Base_Init(&htim3);	
}





/*************TIME2 初始化错误处理代码**********************************/
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  
		rt_kprintf("page0.t5.txt=\"error\"");
		HMI_End();
  
  /* USER CODE END Error_Handler_Debug */ 
	
	
	
}

/************初始化GPIO口和time 口*************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
  
    /**TIM2 GPIO Configuration    
    PA15     ------> TIM2_ETR 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD ;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
  else if(htim_base->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
    /**TIM3 GPIO Configuration    
    PD2     ------> TIM3_ETR 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed =GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
  else if(htim_base->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspInit 0 */

  /* USER CODE END TIM8_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM8_CLK_ENABLE();
  
    /**TIM8 GPIO Configuration    
    PA0-WKUP     ------> TIM8_ETR 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM8_MspInit 1 */

  /* USER CODE END TIM8_MspInit 1 */
  }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();
  
    /**TIM2 GPIO Configuration    
    PA15     ------> TIM2_ETR 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
  
    /**TIM3 GPIO Configuration    
    PD2     ------> TIM3_ETR 
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspDeInit 0 */

  /* USER CODE END TIM8_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM8_CLK_DISABLE();
  
    /**TIM8 GPIO Configuration    
    PA0-WKUP     ------> TIM8_ETR 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

  /* USER CODE BEGIN TIM8_MspDeInit 1 */

  /* USER CODE END TIM8_MspDeInit 1 */
  }

}
#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
	rt_kprintf("page0.t5.txt=\"error\"");
		HMI_End();

}

#endif 
/*******************电机速度数据采集和显示处理任务********************/
rt_uint16_t kk1=0;
rt_uint16_t kk2=0;
extern rt_uint32_t SumMotor1;
extern rt_uint32_t SumMotor2;
float Motor1Speed=0;
float Motor2Speed=0;
void motor1_Dis_Catch()
{
	kk1=__HAL_TIM_GET_COUNTER(&htim2);
	kk2=__HAL_TIM_GET_COUNTER(&htim3);
	SumMotor1=SumMotor1+kk1;
	SumMotor2=SumMotor2+kk2;
	Motor1Speed=(float)SumMotor1/6/124.6f;
	Motor2Speed=(float)SumMotor2/6/474.3f;
		__HAL_TIM_SET_COUNTER(&htim3,0);
		__HAL_TIM_SET_COUNTER(&htim2,0);

}

/*************建立两个任务，一个用来收取数据，一个用来显示数据******************/
/* 线程的TCB控制块 */
static struct rt_thread *Motor1_thread;
static struct rt_thread *HMI2_thread;
rt_uint32_t motor1_pulse=0;
rt_uint32_t motor2_pulse=0;
static void Motor1_entry(void *parameter)
{
	while(1)
	{
		rt_enter_critical();
		motor1_Dis_Catch();
		rt_exit_critical();
		rt_thread_delay(1);
	}
	
}

static void HMI2_entry(void *parameter)
{
	while(1){
				
				HDMI0_2("page0.t1.txt=",motor_speed_r_min*100);
				HDMI0_2("page0.t2.txt=",SumMotor2*100);
				HDMI0_2("page0.t5.txt=",Motor1Speed*100);
				HDMI0_2("page0.t6.txt=",Motor2Speed*100);
				rt_thread_delay(5);
	}
}



/****************电机前进后退距离设定任务***************/

static struct rt_thread  * motor1_autorun_thread;
rt_uint8_t signM1=0;
rt_uint8_t signM2=0;
rt_uint32_t motor1_auto_ahead=0;
rt_uint32_t motor1_auto_back=0;
rt_uint32_t motor2_auto_ahead=0;
rt_uint32_t motor2_auto_back=0;
static void motor1_autorun_entry(void *parameter)
{	
		
	while(1)
	{
		/*********电机1判断*******************/
		if(ch[3]==0x05&&ch[9]==0xE1)
		{
			motor1_auto_ahead=ch[5]*256+ch[6];
			motor1_auto_back=ch[7]*256+ch[8];
			ch[9]=0;
			signM1=1;
		}
		if(signM1==1)
		{
			if((motor1_auto_ahead)>Motor1Speed)
			{
				rt_pin_write(PB3,1);//开启电机
				
			}
			else
			{
				
				rt_pin_write(PB3,0);//关闭电机
				signM1=0;
			}
			
		}
		/*********电机2判断*******************/
		if(ch[3]==0x05&&ch[9]==0xE2)
		{
			motor2_auto_ahead=ch[5]*256+ch[6];
			motor2_auto_back=ch[7]*256+ch[8];
			ch[9]=0;
			signM2=1;
		}
		if(signM2==1)
		{
			if((motor2_auto_ahead)>Motor2Speed)
			{
				rt_pin_write(PC0,0);//开启电机
				
			}
			else
			{
				
				rt_pin_write(PC0,1);//关闭电机
				signM2=0;
			}
			
		}
		rt_thread_delay(1);
	}
}

/********************电机速度测试任务*************/
static struct rt_thread  * motor1_speed_thread;

static void motor1_speed_entry(void *parameter)
{
	while(1)
	{
		motor_speed_r_GD=SumMotor1-motor_speed_r_GD;
		motor_speed_r_min=(float)motor_speed_r_GD*60/6/125/0.5f;
		motor_speed_r_GD=SumMotor1;
		rt_thread_delay(50);
		
	}
	
	
}
/******************任务初始化*****************/
void  Motor1_Hmi_Init()
{
	MX_TIM2_Init();
	 MX_TIM3_Init();
	HAL_TIM_Base_MspInit(&htim2);
	HAL_TIM_Base_Start(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2,0);	
	SumMotor1=0;//初始化电机速度 为0
	HAL_TIM_Base_MspInit(&htim3);
	HAL_TIM_Base_Start(&htim3);
	__HAL_TIM_SET_COUNTER(&htim3,0);	
	
	
	/********电机1参数获取任务控制块定义**************/
	Motor1_thread= rt_thread_create("Motor1",//任务的名称
														Motor1_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														13,//优先级大小
														10);//时间片，在同一优先级时使用
		if(Motor1_thread!= RT_NULL)
		{
			
			rt_thread_startup(Motor1_thread);
			
		}
		HMI2_thread= rt_thread_create("HMI2",//任务的名称
														HMI2_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														14,//优先级大小
														10);//时间片，在同一优先级时使用
		if(HMI2_thread!= RT_NULL)
		{
			
			rt_thread_startup(HMI2_thread);
			
		}
		motor1_autorun_thread= rt_thread_create("MOTOR1",//任务的名称
														motor1_autorun_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														15,//优先级大小
														10);//时间片，在同一优先级时使用
		if(motor1_autorun_thread!= RT_NULL)
		{
			
			rt_thread_startup(motor1_autorun_thread);
			
		}
		motor1_speed_thread= rt_thread_create("MOTOR1Speed",//任务的名称
														motor1_speed_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														16,//优先级大小
														10);//时间片，在同一优先级时使用
		if(motor1_speed_thread!= RT_NULL)
		{
			
			rt_thread_startup(motor1_speed_thread);
			
		}
}
/* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(Motor1_Hmi_Init);
#endif
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Motor1_Hmi_Init, Enconder1_Hmi_Sample);
