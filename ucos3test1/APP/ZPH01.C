#include "ZPH01.h"




//变量定义
u8  USART2_Rx_Buffer1[USART2_Rx_Size]={0};				//USART2接收1号缓冲区
u8  USART2_Rx_Buffer2[USART2_Rx_Size]={0};				//USART2接收2号缓冲区
u8  USART1_SendData[USART1_Tx_Size];					//USART1发送号缓冲区			
u8 U2R_BufferNum=1;									    //用于乒乓缓冲时选择第一缓冲区还是第二缓冲区
bool volatile U2R_Buf1_Ok=false,U2R_Buf2_Ok=false;	    //用于标志缓冲区是否已满


void ZPH01_RCC_Configuration(void)
{
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	/* DMA clock enable (USART RX using dma1) */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
}

void ZPH01_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//Tx
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//Rx
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//Tx
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;//Rx
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}


//中断向量控制配置（由4位来控制抢占优先级和响应优先级）
void ZPH01_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);/*一个工程中只能有一种分组方式,此分组方式抢占优先级有0、1两种（2的1次方），响应优先级有0-7（2的（4-1）次方）*/
														
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	 /* Enable DMA channel1 IRQ Channel */
	 NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
}


//DMA配置
void ZPH01_DMA_Configuration()
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel6);	//先初始化为默认值

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART2->DR);	//DMA 外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)USART2_Rx_Buffer1;	// DMA 内存基地址

	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;	//外设作为数据传输的来源
	DMA_InitStructure.DMA_BufferSize=USART2_Rx_Size;	//缓存区大小（与定义的数组大小有关）

	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//用来设定外设地址寄存器递增与否
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;	 //用来设定内存地址寄存器递增与否

	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;	//外设数据宽度16位（半个字）
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;			//内存数据宽度16位（半个字）

	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;	//工作在循环缓存模式	
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;	//设定 DMA 通道 x 的软件优先级
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;	//失能DMA通道的内存到内存传输


	DMA_Init(DMA1_Channel6,&DMA_InitStructure);	//初始化USART2_Rx的DMA，其通道号为6
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);        //DMA通道1传输完成中断
	DMA_Cmd(DMA1_Channel6,DISABLE);	//使能DMA
}


//串口配置
void ZPH01_USART_Configuration(void)
{
    USART_InitTypeDef  USART_InitStructure;

	USART_InitStructure.USART_BaudRate=9600;				//波特率设置
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//通信数据长度
	USART_InitStructure.USART_StopBits=USART_StopBits_1;		//停止位位数
	USART_InitStructure.USART_Parity=USART_Parity_No;			//无校验位
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;		//无硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;					//收发模式

	USART_Init(USART1,&USART_InitStructure);					//初始化配置
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);				//使能串口中断
	USART_Cmd(USART1,ENABLE);									//使能串口
	USART_ClearFlag(USART1,USART_FLAG_TC);						//清除发送完成标志
	
	USART_InitStructure.USART_BaudRate=9600;				//波特率设置
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//通信数据长度
	USART_InitStructure.USART_StopBits=USART_StopBits_1;		//停止位位数
	USART_InitStructure.USART_Parity=USART_Parity_No;			//无校验位
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;		//无硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;					//收发模式

	USART_Init(USART2,&USART_InitStructure);					//初始化配置
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);				//使能串口接收中断
	//USART_ITConfig(USART2, USART_IT_IDLE,DISABLE);				//先失能串口空闲中断
	USART_Cmd(USART2,ENABLE);									//使能串口
	USART_ClearFlag(USART2,USART_FLAG_TC);						//清除发送完成标志
	
	/* Enable or Disable USART DMA RX request */
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

}

void ZPH01_START()
{
	ZPH01_RCC_Configuration();
    ZPH01_GPIO_Configuration();
    ZPH01_NVIC_Configuration();
	ZPH01_USART_Configuration();
	ZPH01_DMA_Configuration();
}

void USART2_IRQHandler(void)
{
	OSIntEnter();
	
	/*if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) //如果为空闲总线中断
	{
		u16 data=USART_ReceiveData(USART2);
		DMA1_Channel6->CMAR=(u32)USART2_Rx_Buffer1;		//将DMA1通道6的内存基地址改为第一缓存
		DMA_Cmd(DMA1_Channel6,ENABLE);					//使能DMA1_Channel6
		printf("idle ");
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	}*/
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)	//用于判断开始符号
	{
		if(0xff==USART_ReceiveData(USART2))
		{
			DMA_Cmd(DMA1_Channel6,ENABLE);	//使能DMA
			USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
			printf("u2 ");
		}
		else
		{
			if(U2R_BufferNum==1)
			{
				DMA1_Channel6->CMAR=(u32)USART2_Rx_Buffer1;
			}
			else
			{
				DMA1_Channel6->CMAR=(u32)USART2_Rx_Buffer2;
			}
			printf("u2err ");
		}
	}
	if(USART_GetITStatus(USART2,USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)//出错
	{
		USART_ClearITPendingBit(USART2, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
	
	
	OSIntExit();
}

//USART2接收DMA中断处理函数
void DMA1_Channel6_IRQHandler(void)
{
	OSIntEnter(); 
	
	if(DMA_GetITStatus(DMA1_IT_TC6)!=RESET)						//如果该DMA通道发生中断请求
	{
		OS_ERR err;
		
		DMA_Cmd(DMA1_Channel6,DISABLE);							//关闭DMA1_Channel6
		DMA_ClearITPendingBit(DMA1_IT_TC6);						//清除DMA的中断标志
		
		if(U2R_BufferNum==1)
		{
			DMA1_Channel6->CMAR=(u32)USART2_Rx_Buffer2;		//将缓冲区的地址改为2号
			U2R_BufferNum=2;
			U2R_Buf1_Ok=true;								//将1号缓存区标志位置为1，表示已满可读
			
		}
		else
		{
			DMA1_Channel6->CMAR=(u32)USART2_Rx_Buffer1;		//将缓冲区的地址改为1号
			U2R_BufferNum=1;	
			U2R_Buf2_Ok=true;								//将2号缓存区标志位置为1，表示已满可读
			
		}
		OSTaskSemPost(&ZPH01_Task_TCB,OS_OPT_POST_NONE,&err);//释放任务信号量
		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);		//使能串口2的接收中断
	
	}
	
	OSIntExit();
}


void DataHandler()
{
	u8 *Data=USART2_Rx_Buffer1;
	printf("handler ");
	if(U2R_Buf1_Ok)
	{
		Data=USART2_Rx_Buffer1;
		U2R_Buf1_Ok=false;
	}
	else if(U2R_Buf2_Ok)
	{
			Data=USART2_Rx_Buffer2;
			U2R_Buf2_Ok=false;
	}	
	if(isValid(Data,USART2_Rx_Size))
	{
		int temp=(int)(100*CalculateDustRate(Data)),i=0;
		
		u8 H_rate=H_rate=(temp/100);
		u8 L_rate=L_rate=temp%100;
		
		
		USART1_SendData[0]=StartFlag;	USART1_SendData[1]=ModuleID;
		USART1_SendData[2]=Mode;		USART1_SendData[3]=H_rate;
		USART1_SendData[4]=L_rate;		USART1_SendData[5]=0;
		USART1_SendData[6]=0;
		for(i=0;i<6;i++)
		{
			USART1_SendData[6]+=USART1_SendData[i];
			printf("%x ",USART1_SendData[i]);
		}
		printf("%x\n",USART1_SendData[i]);	
	}	
	
}

//判断所接收的数据帧是否正确
bool isValid(u8 *Data,u16 Data_Length)
{
	if(SumCheck(Data,Data_Length)==(*(Data+Data_Length-1)))
		return true;
	else
		return false;
}


//生成和校验序列
u8 SumCheck(u8 *Data,u16 Data_Length)
{
	u16 cnt;
	u8 sum=0;
	for(cnt=1;cnt<(Data_Length-1);cnt++)
	{
		sum+=*(Data+cnt);
	}
	
	sum=(~sum)+1;
	
	return sum;
}

//用于计算灰尘浓度
float CalculateDustRate(u8 *Data)
{
	float LowPulseRate=*(Data+3)+(*(Data+4))/100.0;
	
	return (LowPulseRate*18);
	
}

//用于计算灰尘粒子数目
int CalculateDustNum(u8 *Data)
{
	float LowPulseRate=*(Data+3)+(*(Data+4))/100;
	return (int)(LowPulseRate*2000/3);
}

/**********   USART2打印函数   ************/
/**********   PA2 TX   ************/
/**********   PA3 Rx   ************/
/*void Usart2Printf(char *fmt, ...)
{
    char buffer[USART2_BUFFER_LEN - 1];
    u8 i = 0;
    u8 len;

    va_list arg_ptr; 		//Define convert parameters variable
    va_start(arg_ptr, fmt); //Init variable
    len = vsnprintf(buffer, USART2_BUFFER_LEN+1, fmt, arg_ptr); //parameters list format to buffer
    
    while ((i < USART2_BUFFER_LEN) && (i < len) && (len > 0))
    {
        USART_SendData(USART2, (u8) buffer[i++]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
    }
    va_end(arg_ptr);
}*/

