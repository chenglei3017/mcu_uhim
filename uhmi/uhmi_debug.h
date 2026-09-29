#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DBG_LEVEL_NONE      (0x00000000)
#define DBG_LEVEL_INFO      (0x00000001)
#define DBG_LEVEL_NORMAL    (0x00000002)
#define DBG_LEVEL_ERROR     (0x00000004)
#define DBG_LEVEL_DEBUG     (0x00000008)
#define DBG_LEVEL_ALL       (0xFFFFFFFF)

#define DUMP_PACKET_NONE    (0x00)
#define DUMP_PACKET_RX      (0x01)
#define DUMP_PACKET_TX      (0x02)
#define DUMP_PACKET_BOTH    (DUMP_PACKET_RX | DUMP_PACKET_TX)

extern int debug_level;
extern int dump_mode;

#define DPRINT(level, fmt, args...) do{\
    if(debug_level&level)\
    {\
        printf("%s_%d:"fmt, __FILE__, __LINE__, ##args);\
        printf("\n");\
    }\
}while(0);

#endif
