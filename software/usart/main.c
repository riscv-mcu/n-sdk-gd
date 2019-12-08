/**
 ************************************************************************************
 * Hong Xin Integrated Circuit, INC.
 *     
 * Copyright (c) 2018 Hong Xin IC.  All Rights Reserved.
 *
 * File Name  : H103_USART_Test_Program01.c
 * By         :	Xu Jianhua
 * Create Date: 2018-5-16
 * Version    : 1.0
 * Function Brief: Data transmission and reception between USART1 and USART2
 **************************************************************************************
 * Description: This program checks the asynchronous communication between USART1 and USART2 with typical configurations.
 * PINS are no remapped. BARD RATE=115200¡£A group of data is first transmitted by USART1 and received by USART2¡£
 * Then the transfer direction is reversed. 
 * A comparison between the source data and the target data helps check the transfer result. 
 *
 * Conditions: It is assumed that system clock is correct. 
 * PA9 must be connected to PA3.
 * PA10 must be connected to PA2.
 * Results justification: R0 = 0XFDB97531 means function OK, R0 != 0XFDB97531 means an error occurred.
 *
 ***************************************************************************************
 */

#include "sys.h"
#include "led.h"
#include "usart.h"
#include "drivers/func.h"
#include "soc/drivers/soc.h"
#include "soc/drivers/board.h"
#include "drivers/riscv_encoding.h"
#include "drivers/timer.h"
#include "drivers/eclic.h"

char data1[512], data2[512], data3[512]; 		//data1作源数据
uint16_t datacount=0;


//USART1接受数据的中断程序
void USART1_IRQHandler(void)
{

	printf("USART1_IRQHandler\n");
	uint8_t res;	
	if(USART1->SR&(1<<5))		//接收到数据
	{	 
		res=USART1->DR; 
		data3[datacount]=res;
		datacount++;
	}
} 


////USART2接受数据的中断程序
void USART2_IRQHandler(void)
{
	uint8_t res;
	printf("USART2_IRQHandler\n");	
	if(USART2->SR&(1<<5))		//接收到数綵
	{	 
		res=USART2->DR; 
		data2[datacount]=res;
		datacount++;
	}
} 
	
/*
*		USART初始化函数
*		pclk1: APB1 clock
*		pclk2: APB2 clock
*		baud:	Baud Rate
*/


void uart_init(uint32_t pclk1,uint32_t pclk2,uint32_t baud)
{  
	float temp;
	uint16_t mantissa1,mantissa2;
	uint16_t fraction1,fraction2;	   
	temp=(float)(pclk1*1000000)/(baud*16);	//得到USARTDIV
	mantissa1=temp;				 									//得到整数部分
	fraction1=(temp-mantissa1)*16; 					//得到小数部分	 
    mantissa1<<=4;
	mantissa1+=fraction1; 
	temp=(float)(pclk2*1000000)/(baud*16);	//得到USARTDIV
	mantissa2=temp;				 									//得到整数部分
	fraction2=(temp-mantissa2)*16; 					//得到小数部分	 
    mantissa2<<=4;
	mantissa2+=fraction2; 
	RCC->APB2ENR|=1<<2;   									//使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  									//使能USART1
	RCC->APB1ENR|=1<<17;  									//使能USART2
	GPIOA->CRH&=0XFFFFF00F;									//IO状态设置
	GPIOA->CRH|=0X000004B0;									//IO状态设置 
	GPIOA->CRL&=0XFFFF00FF;									//IO状态设置
	GPIOA->CRL|=0X00004B00;									//IO状态设置 
	RCC->APB2RSTR|=1<<14;   								//Reset USART1
	RCC->APB2RSTR&=~(1<<14);		   	   
	RCC->APB1RSTR|=1<<17;   								//Reset USART2
	RCC->APB1RSTR&=~(1<<17);
	//波特率设置
 	USART1->BRR=mantissa1; 									// Set USART1 Baud Rate
	USART2->BRR=mantissa2; 									// Set USART1 Baud Rate
	USART1->CR1|=0X200C;  									//1位停止,无校验位.
	USART1->CR1|=1<<5;    									//接收缓冲区非空中断使能	
  USART2->CR1|=0X200C;  									//1位停止,无校验位.
	USART2->CR1|=1<<5;    									//接收缓冲区非空中断使能				
//	MY_NVIC_PriorityGroupConfig(2);					//ÅäÖÃÖÐ¶Ï
//	MY_NVIC_Init(3,3,USART1_IRQn,2);				//×é2£¬×îµÍÓÅÏÈ¼¶ 
//	MY_NVIC_Init(3,3,USART2_IRQn,2);				//×é2£¬×îµÍÓÅÏÈ¼¶ 

  eclic_enable_interrupt(ECLIC_INT_MTIP);
  eclic_enable_interrupt(USART1_IRQn);
  eclic_enable_interrupt(USART2_IRQn);


  eclic_set_nlbits(3);

  eclic_set_irq_lvl_abs(ECLIC_INT_MTIP, 1);
  eclic_set_irq_lvl_abs(USART1_IRQn, 2);
  eclic_set_irq_lvl_abs(USART2_IRQn, 2);

  set_csr(mstatus, MSTATUS_MIE);

  //  The USART1_IRQn using Vector-Mode
 // eclic_set_vmode(USART1_IRQn);


}



void setup_mtime (){

    // Set the machine timer to go off in 2 seconds.
    volatile uint64_t * mtime    = (uint64_t*) (TIMER_CTRL_ADDR + TIMER_MTIME);
    volatile uint64_t * mtimecmp = (uint64_t*) (TIMER_CTRL_ADDR + TIMER_MTIMECMP);
    uint64_t now = *mtime;
  #ifdef CFG_SIMULATION
    // Bob: In simulation, we need to make it more quick
    uint64_t then = now + 0.001 * TIMER_FREQ;
  #else
    uint64_t then = now + 2 * TIMER_FREQ;
  #endif
    *mtimecmp = then;

}

/*Entry Point for Machine Timer Interrupt Handler*/
  //  Can only enable this attribute if the toolchain support it
//void __attribute__ ((interrupt)) MTIME_HANDLER(){
void MTIME_HANDLER(){

  #ifdef CFG_SIMULATION
    // Use write functions instead of printf because it will be much faster in simulation
  write (STDOUT_FILENO, "Begin mtime handler\n", strlen("Begin mtime handler\n"));
  #else
  printf ("%s","Begin mtime handler----NonVector mode\n");
  #endif
  GPIO_REG(GPIO_OUTPUT_VAL) ^= (0x1 << RED_LED_GPIO_OFFSET);

  volatile uint64_t * mtime       = (uint64_t*) (TIMER_CTRL_ADDR + TIMER_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (TIMER_CTRL_ADDR + TIMER_MTIMECMP);
  uint64_t now = *mtime;
  #ifdef CFG_SIMULATION
    // In simulation, we need to make it more quick
  uint64_t then = now + 0.01 * TIMER_FREQ;
  #else
  uint64_t then = now + 0.5 * TIMER_FREQ;// Here we set 1 second, but we need to wait 5 cycles to get out from this handler, so the blink will not toggle as 1 cycle
  #endif
  *mtimecmp = then;

  #ifdef CFG_SIMULATION
  write (STDOUT_FILENO, "End mtime handler\n", strlen("End mtime handler\n"));
  #else
  wait_seconds(5);// Wait for a while
  
  printf ("%s","End mtime handler\n");
  #endif

}

void wait_seconds(size_t n)
{
   unsigned long start_mtime, delta_mtime;

  // Don't start measuruing until we see an mtime tick
   unsigned long tmp = mtime_lo();
  do {
    start_mtime = mtime_lo();
  } while (start_mtime == tmp);

  do {
    delta_mtime = mtime_lo() - start_mtime;
  } while (delta_mtime < (n * TIMER_FREQ));
  printf("-----------------Waited %d seconds.\n", n);
}

int main(void)
{
	uint16_t i=0, same=0;	
	uint32_t result=0;
	Stm32_Clock_Init(9);  				//把Sysclk设置到72MHz
	LED_Init();										//LED初始化
	uart_init(72,36,115200);			//USART1和USART2初始化
	
	for (i=1; i<512; i++) data1[i]=i;		//源数据初始化
			
	USART1->SR;			//先读一下SR，避免第一个字节丢失
	for(i = 0; i < 512; i++)
	{
		USART1->DR=data1[i];
		while((USART1->SR&0X40)==0);			//等待TC置位后再写下一个数据
	}
	//数据的接收由中断程序完成
	
	//检查data1和data2中的数据是否一致，一致则点亮LED0。
	for(i = 0; i < 512 && same; i++)
	{
		if(data2[i] != data1[i]) same=1;
	}
	
	LED0 = same;
	
	if(!same) result|=(uint32_t)0XFDB9<<16;
	
	datacount = 0;
	same = 0;
	
		
	for(i = 0; i < 512; i++)
	{
		USART2->DR=data2[i];
		while((USART2->SR&0X40)==0);		//等待TC置位后再写下一个数据
	}
	//数据的接收由中断程序完成
	
	//检查data1和data3中的数据是否一致，一致则点亮LED1。
		for(i = 0; i < 512 && same; i++)
	{
		if(data3[i] != data1[i]) same=1;
	}
	
	LED1 = same;
	
	if(!same) result|=0X7531;
	


    while(1)
	{
   //PE.5�����
		GPIOE->ODR|=1<<0;
		delay_1ms(1000);
	//	printf("hello_world\n");
		//PE.5����� 
		GPIOE->ODR &=~1<<0;
		delay_1ms(1000);
	};

	return 0;
	
	//	SETR0(result);			//把result传送给R0
}