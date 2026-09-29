#include "myiic.h"

void delay_us(u32 i)
{
	u32 temp;
	SysTick->LOAD=225*i/10;         //设置重装数值, 22 = SYSCLK/8
	SysTick->CTRL=0X01;         //使能，减到零是无动作，采用外部时钟源
	SysTick->VAL=0;                //清零计数器
	do
	{
		temp=SysTick->CTRL;           //读取当前倒计数值
	}
	while((temp&0x01)&&(!(temp&(1<<16))));     //等待时间到达
	SysTick->CTRL=0;    //关闭计数器
	SysTick->VAL=0;        //清空计数器
}

void delay_ms(u32 i)
{
	u32 temp;
	SysTick->LOAD=22500*i;         //设置重装数值, 22 = SYSCLK/8
	SysTick->CTRL=0X01;         //使能，减到零是无动作，采用外部时钟源
	SysTick->VAL=0;                //清零计数器
	do
	{
		temp=SysTick->CTRL;           //读取当前倒计数值
	}
	while((temp&0x01)&&(!(temp&(1<<16))));     //等待时间到达
	SysTick->CTRL=0;    //关闭计数器
	SysTick->VAL=0;        //清空计数器
}

//初始化IIC
void IIC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();//使能GPIOB时钟

	GPIO_InitStruct.Pin = IIC_SCL_Pin|IIC_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(IIC_SCL_GPIO_Port, &GPIO_InitStruct);

	IIC_SCL(1);
	IIC_SDA(1);
}

void IIC_DeInit(void)
{
	HAL_GPIO_DeInit(IIC_SDA_GPIO_Port, IIC_SDA_Pin);
	HAL_GPIO_DeInit(IIC_SCL_GPIO_Port, IIC_SCL_Pin);
}

//产生IIC起始信号,SCL高时的下降沿
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA(1);
	IIC_SCL(1);
	delay_us(2);
	IIC_SDA(0);		//START:when CLK is high,DATA change form high to low
	delay_us(2);
	IIC_SCL(0);//钳住I2C总线，准备发送或接收数据
	delay_us(2);
}
//产生IIC停止信号，SCL高时的上升沿
void IIC_Stop(void)
{
	SDA_OUT();		//sda线输出
	IIC_SCL(0);
	IIC_SDA(0);		//STOP:when CLK is high DATA change form low to high
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SDA(1);		//发送I2C总线结束信号
	delay_us(2);
}
//等待应答信号到来，一般下一个时钟就会得到ACK，等待250个CLOCK超时
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入
	IIC_SDA(1);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	while (READ_SDA)
	{
		ucErrTime++;
		delay_us(2);
		if (ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0);//时钟输出0
	return 0;
}

//产生ACK应答，SCL时钟高时，SDA保持低
void IIC_Ack(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(0);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
}

//不产生ACK应答，SCL时钟高时，SDA保持高
void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(1);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txd)
{
	u8 t;
	SDA_OUT();
	IIC_SCL(0);//拉低时钟开始数据传输
	for (t = 0; t < 8; t++)
	{
		IIC_SDA((txd&0x80)>>7);
		txd<<=1;
		delay_us(2);
		IIC_SCL(1);
		delay_us(2);
		IIC_SCL(0);
		delay_us(2);
	}
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();	//SDA设置为输入
	for (i = 0; i < 8; i++ )
	{
		IIC_SCL(0);
		delay_us(2);
		IIC_SCL(1);
		receive<<=1;
		if (READ_SDA)
		{
			receive++;
		}
		delay_us(2);
	}
	if (!ack)
	{
		IIC_NAck();//发送nACK
	}
	else
	{
		IIC_Ack(); //发送ACK
	}
	return receive;
}
