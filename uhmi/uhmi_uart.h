#ifndef __UART_H__
#define __UART_H__

extern int uart_init(void);

extern int uart_rx(char *buf, int buf_size);
extern int uart_tx(char *data, int data_len);

extern void uart_close(void);

#endif
