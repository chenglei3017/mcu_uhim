
#ifndef __HZ2PY_H__
#define __HZ2PY_H__

#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <string.h>

/*MACRO*/
#define MAXLEN 256
#define NAMELEN 64

/*struct*/
typedef struct h2p {
  char *py;
  char *py_shengdiao;
  unsigned shengdiao;
  unsigned char *hz;
} pyhz_tab;


/*
 * convert hanzi string to pinyin string
 * @parameter:
 *  char *hz: input hanzi string
 *	char *py:output pinyin string
 *	size_t hz_len: input hanzi string length
 *	size_t py_len: the max available pinyin string length 
 *	@return:
 *	 int: 0 if success, -1 if failed
 *	support only 2 encode methods, GB2312 and UTF8 , auto detect
 */
extern int hz2py(char *hz, char *py, size_t hz_len, size_t py_len);

#endif
