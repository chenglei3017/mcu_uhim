#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <fcntl.h>   /*File control*/
#include <termio.h>  /*provide the common interface*/

#include "uhmi_debug.h"

#define UART1 "/dev/ttyS1"

int uart_fd = -1;

pthread_mutex_t uart_wr_mut = PTHREAD_MUTEX_INITIALIZER;

static int uart_open(const char *dev)
{
    int port_fd = -1;
    int ret = -1;

    if (NULL == dev)
        return port_fd;

    port_fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (-1 == port_fd)
    {
        perror(dev);
        return port_fd;
    }

    /*set the serial port is block and waitting*/
    ret = fcntl(port_fd, F_SETFL, 0);
    if (ret < 0)
    {
        DPRINT(DBG_LEVEL_ERROR, "fcntl failed!\n");
    }
    else
    {
        DPRINT(DBG_LEVEL_NORMAL, "fcntl = %d\n", fcntl(port_fd, F_SETFL, 0));
    }/*end if*/


    if(0 == isatty(STDIN_FILENO))
    {
        DPRINT(DBG_LEVEL_ERROR, "standard input is not a terminal device\n");

    }
    else
    {
        DPRINT(DBG_LEVEL_ERROR, "isatty success!\n");
    }

    return port_fd;
}

static int uart_cfg(int fd, int speed, int dataBits, char parity, int stopBits, int flowCtrl)
{
    struct termios options;

    if(0 != tcgetattr(fd, &options))
    {
        perror("uart_init");
        return -1;
    }

    switch(speed)
    {
        case 2400:
            cfsetispeed(&options,B2400);
            cfsetospeed(&options,B2400);
            break;
        case 4800:
            cfsetispeed(&options,B4800);
            cfsetospeed(&options,B4800);
            break;
        case 9600:
            cfsetispeed(&options,B9600);
            cfsetospeed(&options,B9600);
            break;
        case 57600:
            cfsetispeed(&options,B57600);
            cfsetospeed(&options,B57600);
            break;
        case 115200:
            cfsetispeed(&options,B115200);
            cfsetospeed(&options,B115200);
            break;
        case 460800:
            cfsetispeed(&options,B460800);
            cfsetospeed(&options,B460800);
            break;
        case 1152000:
            cfsetispeed(&options,B1152000);
            cfsetospeed(&options,B1152000);
            break;
        default:
            DPRINT(DBG_LEVEL_ERROR, "invalid speed!\n");
            return -1;
    }

    options.c_cflag |= CLOCAL|CREAD;

    options.c_cflag &= ~CSIZE;

    options.c_lflag &= ~ICANON; // raw mode

    options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);

    options.c_oflag &= ~OPOST;

    options.c_oflag &= ~(ONLCR | OCRNL);

    options.c_iflag &= ~(ICRNL | INLCR);

    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    //newtio.c_lflag |= ICANON; // standard mode

    switch(dataBits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            DPRINT(DBG_LEVEL_ERROR, "invalid databits!\n");
            return -1;
    }

    switch(flowCtrl)
    {

        case 0:
            options.c_cflag &= ~CRTSCTS;
            break;
        case 1:
            options.c_cflag |= CRTSCTS;
            break;
        case 2:
            options.c_cflag |= IXON | IXOFF | IXANY;
            break;
        default:
            DPRINT(DBG_LEVEL_ERROR, "invalid flowCtrl!\n");
            return -1;
    }

    switch (parity)
    {
        case 'N':
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            options.c_iflag &= ~INPCK;     /* Enable parity checking */
            break;
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;             /* Disnable parity checking */
            break;
        case 'E':
            options.c_cflag |= PARENB;     /* Enable parity */
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S':
            options.c_cflag &= ~PARENB;     /*as no parity*/
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            DPRINT(DBG_LEVEL_ERROR, "invalid parity\n");
            return -1;
    }

    switch (stopBits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            DPRINT(DBG_LEVEL_ERROR, "invalid stop bits\n");
            return -1;
    }

    options.c_oflag &= ~OPOST;

    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;

    tcflush(fd, TCIFLUSH);
    if(0 != (tcsetattr(fd, TCSANOW, &options)))
    {
        perror("com set error");
        return -1;
    }

    return 0;
}

int uart_init(void)
{
    int ret = 0;

    uart_fd = uart_open(UART1);
    if (uart_fd < 0)
    {
        DPRINT(DBG_LEVEL_ERROR, "Can't open %s!\n", UART1);
        ret = -1;
    }
    else if (-1 == uart_cfg(uart_fd, 115200, 8, 'N', 1, 0))
    {
        DPRINT(DBG_LEVEL_ERROR, "UART init fail!\n");
        uart_close();
        ret = -1;
    }
    return ret;
}

int uart_rx(char *buf, int buf_size)
{
    int total_read = 0, nread = 0, retry = 3;
    fd_set rd;

    int i = 0;

    FD_ZERO(&rd);
    FD_SET(uart_fd, &rd);

    if(select(uart_fd+1,&rd,NULL,NULL,NULL) < 0)
    {
        perror("select error!\n");
    }
    else
    {
        //pthread_mutex_lock(&uart_wr_mut);
        while((buf_size - total_read) && retry)
        {
            if ((nread = read(uart_fd, (buf + total_read), (buf_size - total_read)))>0)
            {
                total_read += nread;
            }
            else
            {
                retry--;
            }
        }
        //pthread_mutex_unlock(&uart_wr_mut);

        if (dump_mode & DUMP_PACKET_RX)
        {
            printf("uart_rx[%d]:",total_read);
            for (i = 0; i < total_read; i++)
            {
                if (i%16 == 0)
                    printf("\n");
                printf("%02x", buf[i]);
            }
            printf("\n");
        }
    }

    return total_read;
}


int uart_tx(char *data, int data_len)
{
    int i = 0;
    //pthread_mutex_lock(&uart_wr_mut);
    write(uart_fd, data, data_len);
    //pthread_mutex_unlock(&uart_wr_mut);

    if (dump_mode & DUMP_PACKET_TX)
    {
        printf("uart_tx[%d]:",data_len);
        for (i = 0; i < data_len; i++)
        {
            if (i%16 == 0)
                printf("\n");
            printf("%02x", data[i]);
        }
        printf("\n");
    }
}

void uart_close(void)
{
    if (-1 != uart_fd)
        if (-1 == close(uart_fd))
            perror("uart_close");
}
