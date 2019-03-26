/******************************************************************************
						STM32F103RCT6 

功能：ME3616-C1A COAP通信流程

硬件原理图:  	TXD1    -->   PA.9
							RXD1    -->   PA.10

							TXD2    -->   PA.2
							RXD2    -->   PA.3

硬件连接：串口1通过USB转串口芯片接到PC，串口2通过电平转换电路接到单片机上
功能说明：PC可以通过串口助手发送数据给串口1，串口1透传给串口2，串口2发送给NB-IOT模块，
		  NB-IOT数据发送到串口2再透传到串口1，从而实现NB模块通讯的同时PC还能进项控制

波特率：115200
******************************************************************************/

#include "delay.h"
#include "sys.h"
#include "usart.h"

int main(void)
{	
	u8 i;
	
	delay_init();	    	 		// 延时函数初始化	
	NVIC_Configuration();		// 设置中断优先级分组
	
	Uart1_Init(115200);	 		// 串口初始化为115200
	Uart2_Init(115200);	 		// 串口初始化为115200
	 
	Uart1_SendString("Doiot System Start.\r\n"); 	// PC显示系统启动
	for(i=0; i<60; i++)
	{
		delay_ms(1000);		// 确保系统稳定
	}

	Uart2_SendString("AT\r\n");		
	delay_ms(100);	
	Uart2_SendString("ATI\r\n");		
	delay_ms(100);	
	Uart2_SendString("AT+CGSN=1\r\n");		
	delay_ms(100);		
	Uart2_SendString("AT+CSQ\r\n");		
	delay_ms(100);	
	Uart2_SendString("AT+CIMI\r\n");		
	delay_ms(100);	
	Uart2_SendString("AT+CEREG?\r\n");		
	delay_ms(100);	
	Uart2_SendString("AT+M2MCLINEW=180.101.147.115,5683,\"868613030006275\",90\r\n");		
	delay_ms(100);	
	for(i=0; i<20; i++)
	{
		delay_ms(1000);		// 确保系统注册上IOT平台
	}
	Uart2_SendString("AT+M2MCLISEND=000101\r\n");		
	delay_ms(100);
	
	while(1)
	{
		Uart1_Method();				// 串口1处理函数
		Uart2_Method();				// 串口2处理函数
	}
}


