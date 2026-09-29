#ifndef __CY8CMBR3108_H
#define __CY8CMBR3108_H

#include "myiic/myiic.h"
#include "uhmi_common.h"
#include "main.h"

#define IIC_READ_BIT    0x01
#define IIC_WRITE_BIT   0x00

#define SLAVE1_ADDRESS          0x37
#define SLAVE1_READ             (SLAVE1_ADDRESS<<1) + IIC_READ_BIT
#define SLAVE1_WRITE            (SLAVE1_ADDRESS<<1) + IIC_WRITE_BIT
#define PARA_LEN        128

u8 CY8CMBR3108_WakeUp(void);
u8 CY8CMBR3108_ReadOneByte(u8 ReadAddr);							//指定地址读取一个字节
void CY8CMBR3108_WriteOneByte(u8 Reg_Addr,u8 DataToWrite);		//指定地址写入一个字节
void CY8CMBR3108_WriteLenByte(u8 Reg_Addr,u8* DataToWrite,u8 Len);//指定地址开始写入指定长度的数据
u8 CY8CMBR3108_ReadLenByte(u8 ReadAddr,u8 Len, u8* Rx_buf);					//指定地址开始读取指定长度数据
void CY8CMBR3108_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度的数据
void CY8CMBR3108_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//从指定地址开始读出指定长度的数据
u8 CY8CMBR3108_ReadButton(void);
u8 CY8CMBR3108_SetRegAddr(u8 RegAddr);
u8 Button_IC_Init(void);
u8 CY8CMBR3108_Check(void);  //检查器件
void CY8CMBR3108_Init(void); //初始化IIC
void CY8CMBR3108_DeInit(void);  //释放IIC资源
#endif
















