#include <string.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "uhmi_common.h"
#include "bootloader.h"
#include "bootloader_uart.h"

/* rx and tx buffer */
//orignal RX data, with DLE, SOH, EOT, PROT_VER and msg_type in income frame
u8 rx_buff[RX_BUF_SIZE];
u32 rxlen = 0;

void UART_SendData(uint8_t data)
{
  huart1.Instance->DR = data;
  while (((huart1.Instance->SR) & USART_SR_TC) == RESET);
}

u32 Send_TxBuffer(u8* packet, u32 plen)
{
    u8 ret = 0;
    u32 i = 0;
    if (NULL == packet || (plen > TX_BUF_SIZE - 2))
        return 1;
     //echo all buffer data
     for(i = 0;i < plen; i++)
     {
       UART_SendData(packet[i]);
     } 
    //clear tx_buffer
    memset(packet, 0, plen);
    return ret;
}

/* load rx byte in RX interrupt to RX buffer one by one, 
 * then handle the event in USART_Tasks                    */
static BOOL load_rx_to_packet(u8* data, u32 dlen, u8* packet, u16* plen)
{
    BOOL Escape = FALSE;
    BOOL data_complete = FALSE;
    u16 crc;
    
    while ((dlen > 0) && (FALSE == data_complete))
    {
      dlen--;
      /* if plen greater than packet size limit, reset it */
      if ((*plen) >= (PACKET_SIZE - 2))
      {
        (*plen) = 0;
      }
      
      switch(*data)
      {
        case SOH:
          if (Escape)
          {
            packet[(*plen)++] = *data;
            Escape = FALSE;
          }
          else
          {
            (*plen) = 0;
          }
        break;
        
      case EOT:
        if (Escape)
        {
          packet[(*plen)++] = *data;
          Escape = FALSE;
        }
        else
        {
          if ((*plen) > 2)
          {
            crc = (packet[(*plen)-2]) & 0x00ff;
            crc = crc | ((packet[(*plen)-1] << 8) & 0xFF00);
            if(CalculateCrc((char*)packet, ((*plen)-2)) == crc)
            {
              //2 CRC matches and frame received is valid.
              data_complete = TRUE;
            }
          }
        }
        break;
      case DLE:
        if (Escape)
        {
          packet[(*plen)++] = *data;
          Escape = FALSE;
        }
        else
        {
          Escape = TRUE;
        }
        break;
        
      default:
        packet[(*plen)++] = *data;
        Escape = FALSE;
      break;
      }
      data++;
    }
    return data_complete;
}

void UART_ITConfig(uint32_t IT_Flag, uint8_t action);
void UART_Tasks(void)
{
    //printf("cplt_flag: %d\n", cplt_flag);
    if (TRUE == cplt_flag)
    {

        bootloaderData.usrBufferEventComplete = load_rx_to_packet(rx_buff, rxlen,
                       bootloaderData.data.buffers.buff2, &bootloaderData.cmdBufferLength);

        rxlen = 0;
        cplt_flag = FALSE;
        UART_ITConfig((uint32_t)(USART_CR1_RXNEIE | USART_CR1_IDLEIE), 1);
    }    
}

