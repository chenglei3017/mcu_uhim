#ifndef  __MCU_ST_H
#define  __MCU_ST_H


#if defined(__cplusplus)
    extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "GUI.h"

typedef GUI_COLOR MCU_COLOR;// unsigned long   0x00FFFFFF  (b,g,r)

typedef struct{
    unsigned int x0;
    unsigned int y0;
    unsigned int x1;
    unsigned int y1;
}MCU_RECT,MCU_LINE;

typedef enum{
    MCU_WRAPMODE_NONE = GUI_WRAPMODE_NONE,
    MCU_WRAPMODE_WORD = GUI_WRAPMODE_WORD,
    MCU_WRAPMODE_CHAR = GUI_WRAPMODE_CHAR,
}MCU_WRAPMODE;

typedef enum{
    MCU_TA_LEFT = GUI_TA_LEFT | GUI_TA_VCENTER,
    MCU_TA_RIGHT = GUI_TA_RIGHT | GUI_TA_VCENTER,
    MCU_TA_CENTER = GUI_TA_CENTER | GUI_TA_VCENTER,
}MCU_TEXTALIGN; //Text alignment

typedef enum{
    MCU_TEXTMODE_NORMAL = GUI_TEXTMODE_NORMAL,
    MCU_TEXTMODE_XOR = GUI_TEXTMODE_XOR,
    MCU_TEXTMODE_TRANS = GUI_TEXTMODE_TRANS,
    MCU_TEXTMODE_REV = GUI_TEXTMODE_REV,
}MCU_TEXTMODE;




extern GUI_CONST_STORAGE GUI_FONT GUI_FontWQY14;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontWQY16;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontWQY18;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontWQY20;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontWQY22;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontWQY24;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontWQY26;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontPF42;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontPF48;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontCity18;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontWeather18;

extern GUI_CONST_STORAGE GUI_BITMAP bmphicomm_logo;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_soft_update;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_two_dimensional_code;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_hint_disconnected;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_upload_white;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_download_white;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_wifi;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_visitor;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_information_background_double;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_information_background_single;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_upgrade;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_recovery;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_usb_link;
extern GUI_CONST_STORAGE GUI_BITMAP bmicon_apmode;

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_locationfailed;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_cloudy;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_dust;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_foggy;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_haze;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_overcast;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_rain;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_snow;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_sun;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_unknown;
extern GUI_CONST_STORAGE GUI_BITMAP bmw_windy;


extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_default;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_1jia;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_360;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_asus;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_coolpad;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_dell;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_haier;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_hasee;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_honor;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_hp;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_htc;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_huawei;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_iPhone;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_lenovo;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_letv;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_lg;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_meitu;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_meizu;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_oppo;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_phicomm;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_samsung;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_smartisan;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_sony;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_tcl;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_thinkpad;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_tongfang;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_vivo;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_windowsphone;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_xiaomi;
extern GUI_CONST_STORAGE GUI_BITMAP  bmicon_zte;


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
void MCU_AA_SetFactor(unsigned int f);//设置狂锯齿级别
void MCU_DrawRect(MCU_RECT r);
void MCU_Clear();

#endif
