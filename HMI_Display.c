#include "HDMI.H"
#include "app_uSart.h"
void HMI_End()
{
	uart_putchar(0xff);
	uart_putchar(0xff);
	uart_putchar(0xff);
}

/***精度为0.1*****/
void HDMI0_1(char *str1,uint32_t num)
{
	uint8_t ch1[8];
	uint8_t i=0;
	ch1[0]=num/1000000+48;
	ch1[1]=num%1000000/100000+48;
	ch1[2]=num%1000000%100000/10000+48;
	ch1[3]=num%1000000%100000%10000/1000+48;
	ch1[4]=num%1000000%100000%10000%1000/100+48;
	ch1[5]=num%1000000%100000%10000%1000%100/10+48;
	ch1[6]=44;
	ch1[7]=(num-num/1)*10/1+48;
	rt_kprintf(str1);
	uart_putchar(34);//双引号输入ASII码，括号内为十进制
	for(i=0;i<8;i++)
	{
		
		uart_putchar(ch1[i]);
		
	}
		uart_putchar(34);//双引号输入ASII码，括号内为十进制
	HMI_End();
}


/********正数精度为0.01*************/
void HDMI0_2(char *str1,uint32_t num)
{
	uint8_t ch1[8];
	uint8_t i=0;
	ch1[0]=num/1000000+48;
	ch1[1]=num%1000000/100000+48;
	ch1[2]=num%1000000%100000/10000+48;
	ch1[3]=num%1000000%100000%10000/1000+48;
	ch1[4]=num%1000000%100000%10000%1000/100+48;
	ch1[5]=44;
	ch1[6]=num%1000000%100000%10000%1000%100/10+48;
	ch1[7]=num%1000000%100000%10000%1000%100%10+48;
	rt_kprintf(str1);
	uart_putchar(34);//双引号输入ASII码，括号内为十进制
	for(i=0;i<8;i++)
	{
		
		uart_putchar(ch1[i]);
		
	}
		uart_putchar(34);//双引号输入ASII码
	HMI_End();
}

/**********负数精度为0.01***************/
/****显示负数精度2位********/
void HDMI_0_2(char *str1,uint32_t num)
{
	uint8_t ch1[9];
	uint8_t i=0;
	ch1[0]=45;
	ch1[1]=num/1000000+48;
	ch1[2]=num%1000000/100000+48;
	ch1[3]=num%1000000%100000/10000+48;
	ch1[4]=num%1000000%100000%10000/1000+48;
	ch1[5]=num%1000000%100000%10000%1000/100+48;
	ch1[6]=44;
	ch1[7]=num%1000000%100000%10000%1000%100/10+48;
	ch1[8]=num%1000000%100000%10000%1000%100%10+48;
	rt_kprintf(str1);
	uart_putchar(34);//双引号输入ASII码
	for( i=0;i<9;i++)
	{
		
		uart_putchar(ch1[i]);
		
	}
	uart_putchar(34);//双引号输入ASII码
	HMI_End();
}
