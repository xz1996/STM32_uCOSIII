

/***************************	外设配置文件		*****************************/

#include <Peripherals_Cfg.h>

//函数声明
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART_Configuration(void);

//变量声明



//时钟配置
void RCC_Configuration(void)
{
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
}


//端口配置
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//Tx
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//Rx
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
}


//中断向量控制配置（由4位来控制抢占优先级和响应优先级）
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);/*一个工程中只能有一种分组方式,此分组方式抢占优先级有0、1两种（2的1次方），响应优先级有0-7（2的（4-1）次方）*/
														
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//串口配置
void USART_Configuration(void)
{
    USART_InitTypeDef  USART_InitStructure;

	USART_InitStructure.USART_BaudRate=9600;					//波特率设置
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;	//通信数据长度
	USART_InitStructure.USART_StopBits=USART_StopBits_1;		//停止位位数
	USART_InitStructure.USART_Parity=USART_Parity_No;			//无校验位
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;		//无硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;					//收发模式

	USART_Init(USART1,&USART_InitStructure);					//初始化配置
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);				//使能串口中断
	USART_Cmd(USART1,ENABLE);									//使能串口
	USART_ClearFlag(USART1,USART_FLAG_TC);						//清除发送完成标志
	
}


