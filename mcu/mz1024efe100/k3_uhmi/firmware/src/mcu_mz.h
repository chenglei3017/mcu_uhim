/* 
 * File:   mcu_mz.h
 * Author: flipped
 *
 * Created on August 11, 2017, 2:07 PM
 */

#ifndef MCU_MZ_H
#define	MCU_MZ_H

#ifdef	__cplusplus
extern "C" {
#endif



#include "gfx_hgc_definitions.h"

extern const GFX_RESOURCE_HDR icon_download_white;
extern const GFX_RESOURCE_HDR icon_upload_white;
extern const GFX_RESOURCE_HDR icon_1jia;
extern const GFX_RESOURCE_HDR icon_360;
extern const GFX_RESOURCE_HDR icon_asus;
extern const GFX_RESOURCE_HDR icon_coolpad;
extern const GFX_RESOURCE_HDR icon_dell;
extern const GFX_RESOURCE_HDR icon_haier;
extern const GFX_RESOURCE_HDR icon_hasee;
extern const GFX_RESOURCE_HDR icon_honor;
extern const GFX_RESOURCE_HDR icon_hp;
extern const GFX_RESOURCE_HDR icon_htc;
extern const GFX_RESOURCE_HDR icon_huawei;
extern const GFX_RESOURCE_HDR icon_iPhone;
extern const GFX_RESOURCE_HDR icon_lenovo;
extern const GFX_RESOURCE_HDR icon_letv;
extern const GFX_RESOURCE_HDR icon_lg;
extern const GFX_RESOURCE_HDR icon_meitu;
extern const GFX_RESOURCE_HDR icon_meizu;
extern const GFX_RESOURCE_HDR icon_oppo;
extern const GFX_RESOURCE_HDR icon_phicomm;
extern const GFX_RESOURCE_HDR icon_samsung;
extern const GFX_RESOURCE_HDR icon_smartisan;
extern const GFX_RESOURCE_HDR icon_sony;
extern const GFX_RESOURCE_HDR icon_tcl;
extern const GFX_RESOURCE_HDR icon_thinkpad;
extern const GFX_RESOURCE_HDR icon_tongfang;
extern const GFX_RESOURCE_HDR icon_vivo;
extern const GFX_RESOURCE_HDR icon_windowsphone;
extern const GFX_RESOURCE_HDR icon_xiaomi;
extern const GFX_RESOURCE_HDR icon_zte;
extern const GFX_RESOURCE_HDR icon_hint_disconnected;
extern const GFX_RESOURCE_HDR icon_information_background_double;
extern const GFX_RESOURCE_HDR icon_information_background_single;
extern const GFX_RESOURCE_HDR icon_recovery;
extern const GFX_RESOURCE_HDR icon_wifi_close;
extern const GFX_RESOURCE_HDR icon_wifi_open;
extern const GFX_RESOURCE_HDR icon_default;
extern const GFX_RESOURCE_HDR icon_two_dimensional_code;
extern const GFX_RESOURCE_HDR phicomm;
extern const GFX_RESOURCE_HDR w_cloudy;
extern const GFX_RESOURCE_HDR w_dust;
extern const GFX_RESOURCE_HDR w_foggy;
extern const GFX_RESOURCE_HDR w_haze;
extern const GFX_RESOURCE_HDR w_overcast;
extern const GFX_RESOURCE_HDR w_rain;
extern const GFX_RESOURCE_HDR w_windy;
extern const GFX_RESOURCE_HDR w_sun;
extern const GFX_RESOURCE_HDR w_snow;
extern const GFX_RESOURCE_HDR w_unknown;
extern const GFX_RESOURCE_HDR icon_update;
extern const GFX_RESOURCE_HDR w_locationfailed;
extern const GFX_RESOURCE_HDR icon_wifi;
extern const GFX_RESOURCE_HDR icon_usb_link;
extern const GFX_RESOURCE_HDR icon_visitor;
extern const GFX_RESOURCE_HDR icon_apmode;
extern const GFX_RESOURCE_HDR font18;
extern const GFX_RESOURCE_HDR font26;
extern const GFX_RESOURCE_HDR font14;
extern const GFX_RESOURCE_HDR font20;
extern const GFX_RESOURCE_HDR font48;
extern const GFX_RESOURCE_HDR font16;
extern const GFX_RESOURCE_HDR font22;
extern const GFX_RESOURCE_HDR alert_font24;
extern const GFX_RESOURCE_HDR city18;
extern const GFX_RESOURCE_HDR weather18;
extern const GFX_RESOURCE_HDR font_pf42;
    
typedef unsigned long  MCU_COLOR;    

typedef struct{
    int x0;
    int y0;
    int x1;
    int y1;
}MCU_RECT,MCU_LINE;

typedef enum{
    MCU_WRAPMODE_NONE = 0,
    MCU_WRAPMODE_WORD,
    MCU_WRAPMODE_CHAR,
}MCU_WRAPMODE;

typedef enum{
    MCU_TA_LEFT = GFX_ALIGN_LEFT,
    MCU_TA_RIGHT = GFX_ALIGN_RIGHT,
    MCU_TA_CENTER = GFX_ALIGN_HCENTER,
}MCU_TEXTALIGN; //Text alignment

typedef enum{
    MCU_TEXTMODE_NORMAL = 0,  
    MCU_TEXTMODE_XOR,     
    MCU_TEXTMODE_TRANS ,   
    MCU_TEXTMODE_REV ,
}MCU_TEXTMODE;

void MCU_SetColor(MCU_COLOR c);
void MCU_SetFont(unsigned int font_index);
void MCU_SetTextAlign(MCU_TEXTALIGN ta);
void MCU_SetTextMode(MCU_TEXTMODE tm);
void MCU_FillRect(MCU_RECT r);
void MCU_SetWrapMode(MCU_WRAPMODE wm);
void MCU_DrawText(const unsigned char *str);
void MCU_DrawTextAtPos(const unsigned char *str,unsigned int x,unsigned int y);
void MCU_DrawTextInRect(const unsigned char *str ,MCU_RECT rect, MCU_TEXTALIGN ta, MCU_TEXTMODE tm, MCU_WRAPMODE wm);
void MCU_SetPenSize(unsigned char psize);
void MCU_DrawLine(MCU_LINE l);
void MCU_DrawBitmap(unsigned int bmp_index);
void MCU_DrawBitmapAtPos(unsigned int  bmp_index,unsigned int x,unsigned int y);
unsigned int MCU_GetCharWidthByFontSize(unsigned int font_index,unsigned char c);
void MCU_AA_DrawLine(MCU_LINE l);
void MCU_AA_SetFactor(unsigned int f);//设置抗锯齿级别
void MCU_DrawRect(MCU_RECT r);
void MCU_Clear();


unsigned int get_font_data_num(unsigned int font_index);
void set_gfxxchar_value_by_font_data(unsigned char font_data[][7],unsigned char *str);
void set_gfxxchar_value(unsigned char *str); 




#ifdef	__cplusplus
}
#endif

#endif	/* MCU_MZ_H */

