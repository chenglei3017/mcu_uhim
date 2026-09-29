#ifndef __MYIIC_H
#define __MYIIC_H

#include "main.h"
#include "uhmi_common.h" 

//IO方向设置宏
#define SDA_IN()  {GPIOB->MODER &= ~(3<<(7*2));GPIOB->MODER |= 0<<(7*2);}	  //PB7输入模式
#define SDA_OUT() {GPIOB->MODER &= ~(3<<(7*2));GPIOB->MODER |= 1<<(7*2);}         //PB7输出模式

//IO操作宏
#define IIC_SCL(n)    HAL_GPIO_WritePin(GPIOB, IIC_SCL_Pin, \
                                        n == 0?GPIO_PIN_RESET:GPIO_PIN_SET) //write SCL,PB8
#define IIC_SDA(n)    HAL_GPIO_WritePin(GPIOB, IIC_SDA_Pin, \
                                        n == 0?GPIO_PIN_RESET:GPIO_PIN_SET) //write SDA,PB7	 
#define READ_SDA      HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin)  //read SDA,PB7

//IIC所有操作函数
void IIC_Init(void);                   //初始化IIC的IO口	
void IIC_DeInit(void);                 //释放IIC的IO口资源
void IIC_Start(void);		        //发送IIC开始信号
void IIC_Stop(void);	  		//发送IIC停止信号
void IIC_Send_Byte(u8 txd);		//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);   //IIC读取一个字节
u8 IIC_Wait_Ack(void); 			//IIC等待ACK信号
void IIC_Ack(void);			//IIC发送ACK信号
void IIC_NAck(void);			//IIC不发送ACK信号 

extern void delay_us(u32 i);
extern void delay_ms(u32 i);
#endif
















