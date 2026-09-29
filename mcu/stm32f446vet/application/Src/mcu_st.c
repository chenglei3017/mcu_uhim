#include "mcu_st.h"

GUI_FONT *mcu_font[12]={
   (GUI_FONT *)&GUI_FontWQY14,
   (GUI_FONT *)&GUI_FontWQY16,
   (GUI_FONT *)&GUI_FontWQY18,
   (GUI_FONT *)&GUI_FontCity18,
   (GUI_FONT *)&GUI_FontWQY18,
   (GUI_FONT *)&GUI_FontWQY20,
   (GUI_FONT *)&GUI_FontWQY22,
   (GUI_FONT *)&GUI_FontWQY24,
   (GUI_FONT *)&GUI_FontWQY26,
   (GUI_FONT *)&GUI_FontPF42,
   (GUI_FONT *)&GUI_FontPF48,
};

GUI_BITMAP *mcu_bitmap[]={
    (GUI_BITMAP *)&bmicon_default,
    (GUI_BITMAP *)&bmicon_1jia,
    (GUI_BITMAP *)&bmicon_360,
    (GUI_BITMAP *)&bmicon_asus,
    (GUI_BITMAP *)&bmicon_coolpad,
    (GUI_BITMAP *)&bmicon_dell,
    (GUI_BITMAP *)&bmicon_haier,
    (GUI_BITMAP *)&bmicon_hasee,
    (GUI_BITMAP *)&bmicon_honor,
    (GUI_BITMAP *)&bmicon_hp,
    (GUI_BITMAP *)&bmicon_htc,
    (GUI_BITMAP *)&bmicon_huawei,
    (GUI_BITMAP *)&bmicon_iPhone,
    (GUI_BITMAP *)&bmicon_lenovo,
    (GUI_BITMAP *)&bmicon_letv,
    (GUI_BITMAP *)&bmicon_lg,
    (GUI_BITMAP *)&bmicon_meitu,
    (GUI_BITMAP *)&bmicon_meizu,
    (GUI_BITMAP *)&bmicon_oppo,
    (GUI_BITMAP *)&bmicon_phicomm,
    (GUI_BITMAP *)&bmicon_samsung,
    (GUI_BITMAP *)&bmicon_smartisan,
    (GUI_BITMAP *)&bmicon_sony,
    (GUI_BITMAP *)&bmicon_tcl,
    (GUI_BITMAP *)&bmicon_thinkpad,
    (GUI_BITMAP *)&bmicon_tongfang,
    (GUI_BITMAP *)&bmicon_vivo,
    (GUI_BITMAP *)&bmicon_windowsphone,
    (GUI_BITMAP *)&bmicon_xiaomi,
    (GUI_BITMAP *)&bmicon_zte,
    (GUI_BITMAP *)&bmicon_locationfailed,
    (GUI_BITMAP *)&bmw_cloudy,
    (GUI_BITMAP *)&bmw_dust,
    (GUI_BITMAP *)&bmw_foggy,
    (GUI_BITMAP *)&bmw_haze,
    (GUI_BITMAP *)&bmw_overcast,
    (GUI_BITMAP *)&bmw_rain,
    (GUI_BITMAP *)&bmw_snow,
    (GUI_BITMAP *)&bmw_sun,
    (GUI_BITMAP *)&bmw_windy,
    (GUI_BITMAP *)&bmw_unknown,
    (GUI_BITMAP *)&bmphicomm_logo,
    (GUI_BITMAP *)&bmicon_soft_update,
    (GUI_BITMAP *)&bmicon_two_dimensional_code,
    (GUI_BITMAP *)&bmicon_hint_disconnected,
    (GUI_BITMAP *)&bmicon_upload_white,
    (GUI_BITMAP *)&bmicon_download_white,
    (GUI_BITMAP *)&bmicon_wifi,
    (GUI_BITMAP *)&bmicon_visitor,
    (GUI_BITMAP *)&bmicon_information_background_double,
    (GUI_BITMAP *)&bmicon_information_background_single,
    (GUI_BITMAP *)&bmicon_usb_link,
    (GUI_BITMAP *)&bmicon_apmode,
};

void MCU_SetColor(MCU_COLOR c)
{
    GUI_SetColor(c);
}
void MCU_SetFont(unsigned int font_index)
{
    GUI_SetFont(mcu_font[font_index]);
}
void MCU_SetTextAlign(MCU_TEXTALIGN ta)
{
     GUI_SetTextAlign(ta);
}
void MCU_FillRect(MCU_RECT r)
{
    GUI_FillRect(r.x0,r.y0,r.x1,r.y1);
}
void MCU_SetTextMode(MCU_TEXTMODE tm)
{
    GUI_SetTextMode(tm);
}
void MCU_SetWrapMode(MCU_WRAPMODE wm)
{
    ;
}

void MCU_DrawTextAtPos(const unsigned char *str,unsigned int x,unsigned int y)
{
    GUI_UC_SetEncodeUTF8();
    GUI_DispStringAt((const char *)str,x,y);
}

void MCU_DrawText(const unsigned char *str)
{
    GUI_UC_SetEncodeUTF8();
    GUI_DispString((const char *)str);
}

void MCU_DrawTextInRect(const unsigned char *str ,MCU_RECT rect, MCU_TEXTALIGN ta, MCU_TEXTMODE tm, MCU_WRAPMODE wm)
{
    GUI_RECT r;
    r.x0 = rect.x0;
    r.y0 = rect.y0;
    r.x1 = rect.x1;
    r.y1 = rect.y1;
    MCU_SetTextMode(tm);
    GUI_UC_SetEncodeUTF8();
    GUI_DispStringInRectWrap((char *)str,(GUI_RECT *)&r,(int)ta,(GUI_WRAPMODE)wm);
}
void MCU_SetPenSize(unsigned char psize)
{
    GUI_SetPenSize(psize);
}

void MCU_DrawLine(MCU_LINE l)
{
    GUI_DrawLine(l.x0, l.y0, l.x1, l.y1);
}

void MCU_DrawBitmap(unsigned int bmp_index)
{
    GUI_DrawBitmap(mcu_bitmap[bmp_index],0,0);
}

void MCU_DrawBitmapAtPos(unsigned int bmp_index,unsigned int x,unsigned int y)
{ 
    GUI_DrawBitmap(mcu_bitmap[bmp_index],x,y);
}
/*
 * 获取当前字符的点阵宽度
 */
unsigned int MCU_GetCharWidthByFontSize(unsigned int font_index,unsigned char c)
{
    // 根据不同的字体选择 p.pProp还是p.pPropExt等 ;注：xDist区别于xSize
    return mcu_font[font_index]->p.pPropExt[0].paCharInfo[c-32].XDist;
}

void MCU_AA_DrawLine(MCU_LINE l)
{
    GUI_AA_DrawLine(l.x0,l.y0,l.x1,l.y1);
}
void MCU_AA_SetFactor(unsigned int f)
{
    GUI_AA_SetFactor((int)f);
}
void MCU_DrawRect(MCU_RECT r)
{
    GUI_DrawRect(r.x0,r.y0,r.x1,r.y1);
}

void MCU_Clear()
{
    MCU_RECT rect = {0,0,320,440};
    MCU_SetColor(0x00000000);
    MCU_FillRect(rect);
    //GUI_Clear();
}
