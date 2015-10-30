

/***************************	���������ļ�		*****************************/

#include <Peripherals_Cfg.h>

//��������
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART_Configuration(void);

//��������



//ʱ������
void RCC_Configuration(void)
{
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
}


//�˿�����
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//Tx
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//Rx
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
}


//�ж������������ã���4λ��������ռ���ȼ�����Ӧ���ȼ���
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);/*һ��������ֻ����һ�ַ��鷽ʽ,�˷��鷽ʽ��ռ���ȼ���0��1���֣�2��1�η�������Ӧ���ȼ���0-7��2�ģ�4-1���η���*/
														
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//��������
void USART_Configuration(void)
{
    USART_InitTypeDef  USART_InitStructure;

	USART_InitStructure.USART_BaudRate=9600;					//����������
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;	//ͨ�����ݳ���
	USART_InitStructure.USART_StopBits=USART_StopBits_1;		//ֹͣλλ��
	USART_InitStructure.USART_Parity=USART_Parity_No;			//��У��λ
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;		//��Ӳ��������
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;					//�շ�ģʽ

	USART_Init(USART1,&USART_InitStructure);					//��ʼ������
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);				//ʹ�ܴ����ж�
	USART_Cmd(USART1,ENABLE);									//ʹ�ܴ���
	USART_ClearFlag(USART1,USART_FLAG_TC);						//���������ɱ�־
	
}


