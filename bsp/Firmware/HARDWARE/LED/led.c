#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������V3
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
	// RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��	   	 
	// RCC->APB2ENR|=1<<6;    //ʹ��PORTEʱ��	
	   	 
	// GPIOB->CRL&=0XFF0FFFFF; 
	// GPIOB->CRL|=0X00300000;//PB.5 �������   	 
    // GPIOB->ODR|=1<<5;      //PB.5 �����
											  
	// GPIOE->CRL&=0XFF0FFFFF;
	// GPIOE->CRL|=0X00300000;//PE.5�������
	// GPIOE->ODR|=1<<5;      //PE.5����� 


	// RCC->APB2ENR|=1<<6;    //ʹ��PORTBʱ��	   	 
	RCC->APB2ENR|=1<<6;    //ʹ��PORTEʱ��
	RCC->APB2ENR|=1<<4;    //ʹ��PORTEʱ��	
	   	 
	GPIOE->CRL&=0XFFFFFFF0; 
	GPIOE->CRL|=0X00000003;//PB.5 �������   	 
    GPIOE->ODR|=1<<0;      //PB.5 �����
											  
	GPIOE->CRL&=0XFFFFFF0F;
	GPIOE->CRL|=0X00000030;//PE.5�������
	GPIOE->ODR|=1<<1;      //PE.5����� 


	GPIOC->CRL&=0XFFFFFFF0; 
	GPIOC->CRL|=0X00000003;//PC.5 �������   	 
    GPIOC->ODR|=1<<0;      //PC.5 �����

	GPIOC->CRL&=0XFFFFF0FF; 
	GPIOC->CRL|=0X00000300;//PC.5 �������   	 
    GPIOC->ODR|=1<<2;      //PC.5 �����


}






