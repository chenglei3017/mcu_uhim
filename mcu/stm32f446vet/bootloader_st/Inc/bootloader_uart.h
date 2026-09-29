#ifndef __BOOTLOADER_UART_H__
#define __BOOTLOADER_UART_H__

/******************** definition for uart ***************************/
/* definition for uart buffer */
#define PACKET_SIZE     512
#define RX_BUF_SIZE     512
#define TX_BUF_SIZE     512

extern UART_HandleTypeDef huart1;

extern unsigned char rx_buff[RX_BUF_SIZE];
extern unsigned int rxlen;
extern BOOL cplt_flag;

void UART_ITConfig(uint32_t IT_Flag, uint8_t action);
u32 Send_TxBuffer(u8* packet, u32 plen);
void UART_Tasks(void);

#endif