#include "uhmi_msg.h"
#include "uhmi_debug.h"

void usage()
{
    printf("\n\
uhmiMsg [parameter...]\n\
        reboot\n\
        reset\n\
        upgrade\n\
        usbplugged\n\
        usbunplugged\n\
        screenset\n\
        update\n\
        runapp\n\
        screenoff\n\
        screenon\n\
        ver            Get mcu fw version\n\
        debug <leve> Set debug level\n\
            0x00000001 DBG_LEVEL_NONE\n\
            0x00000002 DBG_LEVEL_INFO\n\
            0x00000004 DBG_LEVEL_NORMAL\n\
            0x00000008 DBG_LEVEL_ERROR\n\
            0x00000010 DBG_LEVEL_DEBUG\n\
            0xFFFFFFFF DBG_LEVEL_ALL\n\
        dump <packet> Dump packet\n\
            none    not dump packet\n\
            rx      dump rx packet\n\
            tx      dump tx packet\n\
            both    dump both rx and tx packet\n");
}

void main(int argc, char **argv)
{
    key_t   key;
    int     msgid = -1;
    int     msgsz = 0;
    UHMI_MSG_T msgbuf;

    memset(&msgbuf, 0, sizeof(msgbuf));

    if(argc < 2)
    {
        usage();
        exit(EXIT_FAILURE);
    }

    //2  creat msg key
    key = FTOK_KEY;
    if (-1 == key)
    {
        exit(EXIT_FAILURE);
    }

    //2 get msgQ id
    msgid = msgget(key, 0);
    if (-1 == msgid)
    {
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "reboot") == 0)
        msgbuf.msg_type = UHMI_ROUTER_REBOOT;
    else  if (strcmp(argv[1], "reset") == 0)
        msgbuf.msg_type = UHMI_ROUTER_RESET;
    else  if (strcmp(argv[1], "upgrade") == 0)
        msgbuf.msg_type = UHMI_ROUTER_UPGRADE;
    else  if (strcmp(argv[1], "usbplugged") == 0)
        msgbuf.msg_type = UHMI_USB_PLUGGED;
    else  if (strcmp(argv[1], "usbunplugged") == 0)
        msgbuf.msg_type = UHMI_USB_UNPLUGGED;
    else if (strcmp(argv[1], "screenset") == 0)
        msgbuf.msg_type = UHMI_SCREEN_SET;
    else if (strcmp(argv[1], "update") == 0)
        msgbuf.msg_type = UHMI_UPDATE;
    else if (strcmp(argv[1], "runapp") == 0)
        msgbuf.msg_type = UHMI_RUNAPP;
    else if (strcmp(argv[1], "ver") == 0)
        msgbuf.msg_type = UHMI_VER;
	else if (strcmp(argv[1], "screenoff") == 0)
		msgbuf.msg_type = UHMI_SCREEN_OFF;
	else if (strcmp(argv[1], "screenon") == 0)
		msgbuf.msg_type = UHMI_SCREEN_ON;
    else if (strcmp(argv[1], "debug") == 0)
    {
        msgbuf.msg_type = UHMI_DEBUG;
        sscanf(argv[2], "%x", (int *)msgbuf.data);
        msgsz = 4;        
    }
    else if (strcmp(argv[1], "dump") == 0)
    {
        msgbuf.msg_type = UHMI_DUMP;
        if (strcmp(argv[2], "rx") == 0)
            *(int *)msgbuf.data = DUMP_PACKET_RX;
        else if (strcmp(argv[2], "tx") == 0)
            *(int *)msgbuf.data = DUMP_PACKET_TX;
        else if (strcmp(argv[2], "both") == 0)
            *(int *)msgbuf.data = DUMP_PACKET_BOTH;
        else
            *(int *)msgbuf.data = DUMP_PACKET_NONE;

        msgsz = 4; 
    }
    else
    {
        msgbuf.msg_type = UHMI_MSG_INVALID;
        usage();
    }

    if (UHMI_MSG_INVALID != msgbuf.msg_type)
        msgsnd(msgid, &msgbuf, msgsz, IPC_NOWAIT);

    exit(EXIT_SUCCESS);
}


