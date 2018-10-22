#include "sys.h"
#include "usart.h"	  
#include "delay.h"

u8 Uart1_Receive_Mark = 0;
u8 Uart1_Receive_Value_Num = 0;
char Uart1_Receive_Array[100];	

u8 Uart2_Receive_Mark = 0;
u8 Uart2_Receive_Value_Num = 0;
char Uart2_Receive_Array[100];	

/*----------------------------
	串口2初始化
-----------------------------*/
void Uart2_Init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//GPIOA时钟, PA2、PA3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2, PA2、PA3
	USART_DeInit(USART2);  //复位串口2
	
	//USART2_TX  --> PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 							//PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); 									//初始化PA2

	//USART2_RX	 --> PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  								//初始化PA3

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;							//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); 							//初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);					//开启串口2中断
	USART_Cmd(USART2, ENABLE);                    					//使能串口2 
}

/*----------------------------
	串口1初始化
-----------------------------*/
void Uart1_Init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//GPIOA时钟, PA9、PA10
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1, PA9、PA10
	USART_DeInit(USART1);  //复位串口1
	
	//USART1_TX  --> PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 							//PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); 									//初始化PA2

	//USART1_RX	 --> PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  								//初始化PA10

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;							//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); 							//初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//开启串口1中断
	USART_Cmd(USART1, ENABLE);                    					//使能串口1 
}

/*----------------------------
	串口2中断服务程序
----------------------------*/
void USART2_IRQHandler(void)                							
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  	//接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Uart2_Receive_Mark = 1;
		Uart2_Receive_Array[Uart2_Receive_Value_Num++] = USART_ReceiveData(USART2);		
	} 
} 

/*----------------------------
	串口1中断服务程序
----------------------------*/
void USART1_IRQHandler(void)                							
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  	//接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Uart1_Receive_Mark = 1;
		Uart1_Receive_Array[Uart1_Receive_Value_Num++] = USART_ReceiveData(USART1);		
	} 
} 

/*----------------------------
	串口2发送字符串
----------------------------*/
void Uart2_SendString(char *s)
{
    while (*s)   
    {
        Uart2_SendData(*s++);       
    }
}

/*----------------------------
	串口1发送字符串
----------------------------*/
void Uart1_SendString(char *s)
{
    while (*s)   
    {
        Uart1_SendData(*s++);       
    }
}

/*----------------------------
	串口2发送数据
----------------------------*/
void Uart2_SendData(u8 dat)
{
	while((USART2->SR&0X40)==0);//等待发送结束
	USART_SendData(USART2, dat);
}

/*----------------------------
	串口1发送数据
----------------------------*/
void Uart1_SendData(u8 dat)
{
	while((USART1->SR&0X40)==0);//等待发送结束
	USART_SendData(USART1, dat);
}

/*----------------------------
	串口2处理函数
----------------------------*/
void Uart2_Method(void)
{
	if(Uart2_Receive_Mark == 1)
	{
		delay_ms(300);
		Uart2_Receive_Array[Uart2_Receive_Value_Num] = '\0';	
		Uart1_SendString(Uart2_Receive_Array);
		Uart2_Receive_Value_Num = 0;
		Uart2_Receive_Mark = 0;
	}
}

/*----------------------------
	串口1处理函数
----------------------------*/
void Uart1_Method(void)
{
	if(Uart1_Receive_Mark == 1)
	{
		delay_ms(300);
		Uart1_Receive_Array[Uart1_Receive_Value_Num] = '\0';	
		Uart2_SendString(Uart1_Receive_Array);
		Uart1_Receive_Value_Num = 0;
		Uart1_Receive_Mark = 0;
	}
}
