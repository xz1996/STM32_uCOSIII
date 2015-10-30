/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>

/********** 	用于配置应用程序的头文件，设定任务的优先级、栈空间的大小以及其他一些应用相关的参数	********/
#include <app_cfg.h>



/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;			//创建任务时，必须为该任务分配一个任务控制块

OS_TCB	 ZPH01_Task_TCB;

static  OS_TCB   Task1TCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];		//为任务分配栈空间

static  CPU_STK  ZPH01_TaskStk[ZPH01_TASK_STK_SIZE];

static  CPU_STK  Task1Stk[TASK1_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate (void);
static  void  AppObjCreate  (void);
static  void  AppTaskStart  (void *p_arg);
static  void  ZPH01TaskStart (void *p_arg);
static  void  Task1Start (void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

//重载printf函数
int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

int  main (void)
{
    OS_ERR  err;												/*		错误类型可在os.h中查看其意义	*/

    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

    OSInit(&err);                                               /* Init uC/OS-III. 通过检查err是否为0来验证是否初始化正确，创建了4个内部任务*/

	if(err!=OS_ERR_NONE)
	{
		/*		未正常初始化		*/
		/*	到os.h中检查其错误	*/
		
	}
	
	AppObjCreate();                                             /* Create Application Objects								*/
	
	 /* 		Create the start task           				*/
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /*		分配给该任务的控制块的地址							*/
                 (CPU_CHAR   *)"App Task Start",				/*		任务名												*/
                 (OS_TASK_PTR ) AppTaskStart,					/*		任务代码的起始地址									*/
                 (void       *) 0,								/*		任务第一次运行时会接收到的参数						*/
                 (OS_PRIO     ) APP_TASK_START_PRIO,			/*		任务优先级（数字越小优先级越高），避免使用0和MAX-1		*/
                 (CPU_STK    *)&AppTaskStartStk[0],				/*		分配给该任务的栈空间的基地址			*/
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,	/*		任务栈的深度标记，数值代表栈溢出警告前栈内剩余空间大小	*/
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,		/*		定义任务的栈的大小									*/
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);							/*		指向一个接收错误码的变量								*/
				 
	if(err!=OS_ERR_NONE)
	{
		/*		任务创建出错		*/
		/*	到os.h中检查其错误	*/
	}

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
	
	if(err!=OS_ERR_NONE)
	{
		/*		任务创建出错		*/
		printf("false ");
		/*	到os.h中检查其错误	*/
	}
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;									/*	用于得到系统时钟源频率								*/
    CPU_INT32U  cnts;											/*	传给SysTick的Load寄存器来决定系统节拍					*/
    OS_ERR      err;


   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions ，初始化外设                 */
    CPU_Init();													/* 初始化uc/CPU的服务，包括初始化测量中断禁止时间，时间戳以及其他一些服务的内部变量*/

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize Memory Management Module                  */

//#if OS_CFG_STAT_TASK_EN > 0u
//    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
//#endif

//    CPU_IntDisMeasMaxCurReset();								/* 重置当前禁止中断的时间								*/
    
    AppTaskCreate();                                            /* Create Application Tasks                             */
                               

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
		
        OSTimeDlyHMSM(0, 0, 0, 100,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }                          

}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
	
}





/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
	/*****************************		这些任务只能被操作系统使用，用户不能调用这些任务		*********************************/
	 OS_ERR      err;
	 OSTaskCreate((OS_TCB     *)&ZPH01_Task_TCB,                /*		分配给该任务的控制块的地址							*/
                 (CPU_CHAR   *)"ZPH01 Task Start",				/*		任务名												*/
                 (OS_TASK_PTR ) ZPH01TaskStart,					/*		任务代码的起始地址									*/
                 (void       *) 0,								/*		任务第一次运行时会接收到的参数						*/
                 (OS_PRIO     ) ZPH01_TASK_START_PRIO,			/*		任务优先级（数字越小优先级越高），避免使用0和MAX-1		*/
                 (CPU_STK    *)&ZPH01_TaskStk[0],				/*		分配给该任务的栈空间的基地址			*/
                 (CPU_STK_SIZE) ZPH01_TASK_STK_SIZE / 10,		/*		任务栈的深度标记，数值代表栈溢出警告前栈内剩余空间大小	*/
                 (CPU_STK_SIZE) ZPH01_TASK_STK_SIZE,			/*		定义任务的栈的大小									*/
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);							/*		指向一个接收错误码的变量								*/
				 
				 
				 
	OSTaskCreate((OS_TCB     *)&Task1TCB,               		 /*		分配给该任务的控制块的地址							*/
                 (CPU_CHAR   *)"Task1 Start",					 /*		任务名												*/
                 (OS_TASK_PTR ) Task1Start,						 /*		任务代码的起始地址									*/
                 (void       *) 0,								 /*		任务第一次运行时会接收到的参数						*/
                 (OS_PRIO     ) TASK1_START_PRIO,				 /*		任务优先级（数字越小优先级越高），避免使用0和MAX-1		*/
                 (CPU_STK    *)&Task1Stk[0],					 /*		分配给该任务的栈空间的基地址			*/
                 (CPU_STK_SIZE) TASK1_STK_SIZE / 10,			 /*		任务栈的深度标记，数值代表栈溢出警告前栈内剩余空间大小	*/
                 (CPU_STK_SIZE) TASK1_STK_SIZE,					 /*		定义任务的栈的大小									*/
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);							 /*		指向一个接收错误码的变量								*/	
				 
	if(err!=OS_ERR_NONE)
	{
		/*		任务创建出错		*/
		printf("false ");
		/*	到os.h中检查其错误	*/
	}
}




/********************************************************************************************************
*
*									APP'S FUNCTION
*
*********************************************************************************************************
*/


static void  ZPH01TaskStart (void *p_arg)
{
	OS_ERR      err;
	CPU_TS      ts;
	
	ZPH01_START();				//用于启动ZPH01粉尘传感器
	
	while(1)
	{
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);		//等待任务信号量，并将自己阻塞
		DataHandler();				//用于数据处理并向上位机发送处理后的信息
		//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		if(err!=OS_ERR_NONE)
		{
			/*		任务创建出错		*/
			printf("ZPH01Start false ");
			/*	到os.h中检查其错误	*/
		}
	}
}

static  void  Task1Start (void *p_arg)
{
	OS_ERR      err;
	while(1)
	{
		printf("Task1Start:500ms\n");
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
		if(err!=OS_ERR_NONE)
		{
			/*		任务创建出错		*/
			printf("Task1Start false ");
			/*	到os.h中检查其错误	*/
		}
	}
	
}


