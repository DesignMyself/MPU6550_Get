#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"
#include "app_uSart.h"
#include "Display.h"
/********°?êy×?×a?aASCII*********/
/*****************°?êy?μ×a?aASII****/
void Value_Asii(uint8_t uart_value,float value)
{

	uint8_t ch1[12];
	uint8_t i=0;
	int32_t num;
	uint16_t num1;
	num=(int32_t)fabs(value);

			
			num1=(value-num)*100;
			ch1[0]='-';
			ch1[1]=num/1000000+48;
			ch1[2]=num%1000000/100000+48;
			ch1[3]=num%1000000%100000/10000+48;
			ch1[4]=num%1000000%100000%10000/1000+48;
			ch1[5]=num%1000000%100000%10000%1000/100+48;
			ch1[6]=num%1000000%100000%10000%1000%100/10+48;
			ch1[7]=num%1000000%100000%10000%1000%100%10+48;
			ch1[8]=46;
			ch1[9]=num1/10+48;
			ch1[10]=num1%10+48;
			
			if(uart_value==3)
				{
					if(value<0)
					{
						uart3_putchar(ch1[0]);
					}
						for(i=1;i<10;i++)
					{
							if(ch1[i]!=48||(ch1[i-1]!=48&&i>=2))
							{
								uart3_putchar(ch1[i]);
							}
					}
				}
			if(uart_value==1)
				{
					if(value<0)
					{
						uart1_putchar(ch1[0]);
					}
						for(i=1;i<10;i++)
					{
							if(ch1[i]!=48||(ch1[i-1]!=48&&i>=2))
							{
								uart1_putchar(ch1[i]);
							}
					}
				}
			if(uart_value==2)
				{
					if(value<0)
					{
						uart2_putchar(ch1[0]);
					}
						for(i=1;i<10;i++)
					{
							if(ch1[i]!=48||(ch1[i-1]!=48&&i>=2))
							{
								uart2_putchar(ch1[i]);
							}
					}
				}
}


void send_Angle(uint8_t axix_sign,uint8_t uart_value,uint8_t data[4])//0xf1为X轴角度，0xf2为Y轴角度，0xf3为Z轴角度，
{
	uint8_t i;
	/******X轴数据发送*****************/

		
		switch(uart_value){
			
			case 1:
			uart1_putchar(0xBB);//数据发送标志位
			uart1_putchar(axix_sign);
			for(i=0;i<4;i++)
				{
					uart1_putchar(data[i]);

				}	
				break;
			
			case 2:
			uart2_putchar(0xBB);//数据发送标志位
			uart2_putchar(axix_sign);
			for(i=0;i<4;i++)
				{
					uart2_putchar(data[i]);

				}	
				break;
				
			case 3:
			uart3_putchar(0xBB);//数据发送标志位
			uart3_putchar(axix_sign);
			for(i=0;i<4;i++)
				{
					uart3_putchar(data[i]);

				}	
			break;
				default:
					rt_kprintf("no uart value\n");
			
		
	}
	
		
}
	