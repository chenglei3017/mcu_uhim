#include "CY8CMBR3108.h"

u8 CY8CMBR3108_config[128] = {
    0x0Du, 0x00u, 0x0Du, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x7Fu, 0x80u, 0x80u,
    0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
    0x05u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Eu, 0x00u, 0x00u,
    0x00u, 0x1Eu, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u,
    0x04u, 0x0Fu, 0x0Fu, 0xFFu, 0x0Fu, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x50u, 0x03u, 0x01u, 0x58u,
    0x00u, 0x37u, 0x01u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x29u, 0x16u
};
u8 CY8CMBR3116_config[128] = {
    0x78u, 0x00u, 0x78u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x7Fu, 0x7Fu, 0x7Fu, 0x80u,
    0x80u, 0x80u, 0x80u, 0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu,
    0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu, 0x03u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
    0x05u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u,
    0x04u, 0xFFu, 0xFFu, 0xFFu, 0x0Fu, 0x0Fu, 0x0Fu, 0x0Fu,
    0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x01u, 0x48u,
    0x00u, 0x38u, 0x06u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2Du, 0x87u
};

u8 CY8CMBR3108_config_new[PARA_LEN] = {0};

//init touch IC, return 0 if success, else return  1
u8 Button_IC_Init(void)
{
  //IIC init, GPIO
  //CY8CMBR3108_Init();IIC init, completed in GPIO_Init
  u8 i;

  //read something to get attention
  u8 data = 0;
  u8 count = 15;
  for (i = 0;i < count;i++)
  {
    data = CY8CMBR3108_WakeUp();
    if (data == 1)
    {
      UART_SendData(data);
      break;
    }
  }
  do
  {
    data = CY8CMBR3108_ReadOneByte(0x51);
    UART_SendData(data);
    delay_ms(10);
  }while((data != 0x37) && --count);
  if (!count)
  {
     CY8CMBR3108_DeInit();
     return 1;
   }

  //read the paras in flash
  CY8CMBR3108_ReadLenByte(0x00, PARA_LEN, CY8CMBR3108_config_new);
//  for (i = 0;i < PARA_LEN;i++)
//    UART_SendData(CY8CMBR3108_config_new[i]);

  //compare it with new config
  if (memcmp(CY8CMBR3108_config_new, CY8CMBR3108_config, PARA_LEN))
  {
    UART_SendData(0x99);
    //write paras into register
    CY8CMBR3108_WriteLenByte(0x00, CY8CMBR3108_config, PARA_LEN);
    //load to flash and reboot
    CY8CMBR3108_WriteOneByte(0x86, 0x02);
    delay_ms(200);
    //reboot
    CY8CMBR3108_WriteOneByte(0x86, 0xff);
  }
  else
    UART_SendData(0xcc);
  delay_ms(20);

  //set read registar address at 0xaa, BUTTON STATE,CS0~7
  CY8CMBR3108_SetRegAddr(0xaa);
  //release GPIO resource
  //CY8CMBR3108_DeInit();
  return 0;
}

void CY8CMBR3108_DeInit()
{
  IIC_DeInit();
}
//初始化IIC接口
void CY8CMBR3108_Init(void)
{
  IIC_Init();//IIC初始化,已在main.c的GPIO_Init中完成
}
//success return 1, else 0
u8 CY8CMBR3108_WakeUp(void)
{
  u8 ret = 0;
  u8 try_count = 10;
  u8 i;
  IIC_Start();
  //send addr and R
  for (i = 0;i < try_count;i++)
  {
    delay_ms(1);
    IIC_Send_Byte(SLAVE1_READ);
    if (!IIC_Wait_Ack()) //get AcK  break
    {
      UART_SendData(0x11);
      ret = 1;
      break;
    }
  }
  IIC_Stop();
  return ret;
}

//在CY8CMBR3108指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
u8 CY8CMBR3108_ReadOneByte(u8 Reg_Addr)
{
  u8 temp=0;
  u8 try_count = 10;
  u8 i;

  IIC_Start();
  //send addr and W
  for (i = 0;i < try_count;i++)
  {
    delay_ms(1);
    IIC_Send_Byte(SLAVE1_WRITE);
    if (!IIC_Wait_Ack()) //get AcK or try_cout = 0, break
    {
      break;
    }
  }
  //set register address
  for (i = 0;i < try_count; i++)
  {
    delay_ms(1);
    IIC_Send_Byte(Reg_Addr);
   if (!IIC_Wait_Ack()) //get AcK or try_cout = 0, break
    {
      break;
    }
  }
  IIC_Stop();

  delay_ms(5);

  IIC_Start();
  delay_ms(1);
  //set read mode
  for (i = 0;i < try_count;i++)
  {
    delay_ms(1);
    IIC_Send_Byte(SLAVE1_READ);
    if (!IIC_Wait_Ack()) //get AcK or try_cout = 0, break
    {
      break;
    }
  }
  temp=IIC_Read_Byte(0);
  delay_ms(1);
  IIC_Stop();
  return temp;
}
u8 CY8CMBR3108_SetRegAddr(u8 Reg_Addr)
{
  u8 try_count = 10;
  u8 i;

  IIC_Start();
  //send addr and W
  for (i = 0;i < try_count;i++)
  {
    delay_ms(1);
    IIC_Send_Byte(SLAVE1_WRITE);
    if (!IIC_Wait_Ack()) //get AcK or try_cout = 0, break
    {
      break;
    }
  }
  //set register address
  for (i = 0;i < try_count; i++)
  {
    delay_ms(1);
    IIC_Send_Byte(Reg_Addr);
   if (!IIC_Wait_Ack()) //get AcK or try_cout = 0, break
    {
      break;
    }
  }
  IIC_Stop();
  delay_ms(1);

  return 0;
}

u8 CY8CMBR3108_ReadButton(void)
{
  u8 temp=0;
  u8 try_count = 10;
  u8 i;

  IIC_Start();
  delay_ms(1);
  //set read mode
  for (i = 0;i < try_count;i++)
  {
    delay_ms(1);
    IIC_Send_Byte(SLAVE1_READ);
    if (!IIC_Wait_Ack()) //get AcK or try_cout = 0, break
    {
      break;
    }
  }
  temp=IIC_Read_Byte(0);
  delay_ms(1);
  IIC_Stop();
  return temp;
}

//在CY8CMBR3108指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
void CY8CMBR3108_WriteOneByte(u8 Reg_Addr,u8 DataToWrite)
{
  u8 try_count = 5;
  IIC_Start();
  //send addr and W
  do
  {
    IIC_Send_Byte(SLAVE1_WRITE);
    try_count--;
  }while(IIC_Wait_Ack() && try_count);
  //send register addr
  try_count = 5;
  do
  {
    IIC_Send_Byte(Reg_Addr);
    try_count--;
  }while(IIC_Wait_Ack() && try_count);
  IIC_Send_Byte(DataToWrite);
  if (!IIC_Wait_Ack())  UART_SendData(DataToWrite);
  IIC_Stop();
  delay_us(20);
}
//在CY8CMBR3108里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void CY8CMBR3108_WriteLenByte(u8 Reg_Addr,u8* DataToWrite,u8 len)
{

  u8 i;
  for (i = 0; i < len; i++)
    CY8CMBR3108_WriteOneByte(Reg_Addr++, *DataToWrite++);
}

//在CY8CMBR3108里面的指定地址开始读出长度为Len的数据
//该函数用于读出全部128字节的数据.
//ReadAddr   :开始读出的地址，一般为0x00
//Rx_buf    :数据
//Len        :要读出数据的长度，一般为128
//u8    ：返回值，0-读取成功，1-读取失败
u8 CY8CMBR3108_ReadLenByte(u8 Reg_Addr,u8 len, u8* Rx_buf)
{
  u8 ret = 0;
  u8 i;
  for (i = 0;i < len; i++)
  {
    *Rx_buf++ = CY8CMBR3108_ReadOneByte(Reg_Addr++);
  }
  return ret;
}
//检查CY8CMBR3108是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 CY8CMBR3108_Check(void)
{
  u8 temp;
  temp=CY8CMBR3108_ReadOneByte(255);//避免每次开机都写CY8CMBR3108
  if(temp==0X55)return 0;
  else//排除第一次初始化的情况
  {
    CY8CMBR3108_WriteOneByte(255,0X55);
    temp=CY8CMBR3108_ReadOneByte(255);
    if(temp==0X55)return 0;
  }
  return 1;
}

//在CY8CMBR3108里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void CY8CMBR3108_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
  while(NumToRead)
  {
    *pBuffer++=CY8CMBR3108_ReadOneByte(ReadAddr++);
    NumToRead--;
  }
}
//在CY8CMBR3108里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void CY8CMBR3108_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
  while(NumToWrite--)
  {
    CY8CMBR3108_WriteOneByte(WriteAddr,*pBuffer);
    WriteAddr++;
    pBuffer++;
  }
}








