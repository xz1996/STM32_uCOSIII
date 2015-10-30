#ifndef _ZPH01_H
#define _ZPH01_H

#include "stm32f10x.h"
#include "misc.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdbool.h"

#include "includes.h"
//宏定义
/*#define USART2_BUFFER_LEN 100    //串口2缓冲区大小（用于串口2打印）*/

#define USART1_Tx_Size	7		//usart1发送缓冲区的大小
#define USART2_Rx_Size	9		//usart2接收DMA缓冲区大小

#define StartFlag	0xaa		//数据帧头
#define	ModuleID	0x03		//模块ID
#define Mode		0x03		//模式

//变量声明

extern OS_TCB	 ZPH01_Task_TCB;


//函数声明


//函数声明


/*************	  配置函数声明		**************/
void ZPH01_RCC_Configuration(void);
void ZPH01_GPIO_Configuration(void);
void ZPH01_NVIC_Configuration(void);
void ZPH01_USART_Configuration(void);
void ZPH01_DMA_Configuration(void);

void ZPH01_START(void);				//用于启动ZPH01粉尘传感器



//void Usart2Printf(char *fmt, ...);	//串口2打印函数


/********************	  处理函数声明		****************************/
bool isValid(u8 *Data,u16 Data_Length);//判断所接收的数据帧是否正确

u8 SumCheck(u8 *Data,u16 Data_Length);//生成ZPH01粉的协议所规定的和校验序列

float CalculateDustRate(u8 *Data);	//用于计算空气中的灰尘浓度

void DataHandler(void);				//用于数据处理并向上位机发送处理后的信息



#endif
