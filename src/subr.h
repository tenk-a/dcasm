/**
 *  @file   subr.h
 *  @brief  subrutines
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *      Boost Software License Version 1.0
 */
#ifndef SUBR_H
#define SUBR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define STDERR          stderr

#define FIL_NMSZ        1024

#define ISDIGIT(c)      (((unsigned)(c) - '0') < 10U)
#define ISLOWER(c)      (((unsigned)(c)-'a') < 26U)
#define TOUPPER(c)      (ISLOWER(c) ? (c) - 0x20 : (c) )
#define ISKANJI(c)      ((uint8_t)(c) >= 0x81 && ((uint8_t)(c) <= 0x9F || ((uint8_t)(c) >= 0xE0 && (uint8_t)(c) <= 0xFC)))
#define ISKANJI2(c)     ((uint8_t)(c) >= 0x40 && (uint8_t)(c) <= 0xfc && (c) != 0x7f)
#define STREND(p)       ((p)+strlen(p))
#define STRINS(d,s)     (memmove((d)+strlen(s),(d),strlen(d)+1),memcpy((d),(s),strlen(s)))

#define MAX(x, y)       ((x) > (y) ? (x) : (y)) /* 最大値 */
#define MIN(x, y)       ((x) < (y) ? (x) : (y)) /* 最小値 */
#define ABS(x)          ((x) < 0 ? -(x) : (x))  /* 絶対値 */

#define REVW(a)         ((((a) >> 8) & 0xff)|(((a) & 0xff) << 8))
#define REVL(a)         ( (((a) & 0xff000000) >> 24)|(((a) & 0x00ff0000) >>  8)|(((a) & 0x0000ff00) <<  8)|(((a) & 0x000000ff) << 24) )

#define BB(a,b)         ((((uint8_t)(a))<<8)|(uint8_t)(b))
#define WW(a,b)         ((((uint16_t)(a))<<16)|(uint16_t)(b))
#define BBBB(a,b,c,d)   ((((uint8_t)(a))<<24)|(((uint8_t)(b))<<16)|(((uint8_t)(c))<<8)|((uint8_t)(d)))

#define GLB(a)          ((uint8_t)(a))
#define GHB(a)          GLB(((uint16_t)(a))>>8)
#define GLLB(a)         GLB(a)
#define GLHB(a)         GHB(a)
#define GHLB(a)         GLB(((uint32_t)(a))>>16)
#define GHHB(a)         GLB(((uint32_t)(a))>>24)
#define GLW(a)          ((uint16_t)(a))
#define GHW(a)          GLW(((uint32_t)(a))>>16)
#define GLD(a)          ((uint32_t)(a))
#define GHD(a)          GLD(((uint64_t)(a))>>32)

#define PEEKB(a)        (*(uint8_t  *)(a))
#define PEEKW(a)        (*(uint16_t *)(a))
#define PEEKD(a)        (*(uint32_t *)(a))
#define PEEKQ(a)        (*(uint64_t *)(a))
#define POKEB(a,b)      (*(uint8_t  *)(a) = (b))
#define POKEW(a,b)      (*(uint16_t *)(a) = (b))
#define POKED(a,b)      (*(uint32_t *)(a) = (b))
#define POKEQ(a,b)      (*(uint64_t *)(a) = (b))
#define PEEKiW(a)       ( PEEKB(a) | (PEEKB((char const*)(a)+1)<< 8) )
#define PEEKiD(a)       ( PEEKiW(a) | (PEEKiW((char const*)(a)+2) << 16) )
#define PEEKiQ(a)       ( PEEKiD(a) | ((uint64_t)PEEKiW((char const*)(a)+4) << 32) )
#define PEEKmW(a)       ( (PEEKB(a)<<8) | PEEKB((char const*)(a)+1) )
#define PEEKmD(a)       ( (PEEKmW(a)<<16) | PEEKmW((char const*)(a)+2) )
#define PEEKmQ(a)       ( ((uint64_t)PEEKmD(a)<<32) | PEEKmD((uint32_t)(a)+4) )
#define POKEiW(a,b)     (POKEB( (a),GLB(b)), POKEB( (char*)(a)+1,GHB(b)))
#define POKEiD(a,b)     (POKEiW((a),GLW(b)), POKEiW((char*)(a)+2,GHW(b)))
#define POKEiQ(a,b)     (POKEiD((a),GLD(b)), POKEiD((char*)(a)+4,GHD(b)))
#define POKEmW(a,b)     (POKEB( (a),GHB(b)), POKEB( (char*)(a)+1,GLB(b)))
#define POKEmD(a,b)     (POKEmW((a),GHW(b)), POKEmW((char*)(a)+2,GLW(b)))
#define POKEmQ(a,b)     (POKEmD((a),GHD(b)), POKEmD((char*)(a)+4,GLD(b)))


#define SETBTS(d,s,msk,pos) (  (d) = ((d) & ~(msk << pos)) | (((s) & msk) << pos)  )

#define DB              if (debugflag)
extern int              debugflag;


/* memマクロ */
#define MEMCPY(d0,s0,c0) do {       \
    char*  d__ = (char*)(d0);       \
    char*  s__ = (char*)(s0);       \
    size_t c__ = (c0);              \
    do {                            \
        *d__++ = *s__++;            \
    } while (--c__);                \
} while(0)

#define MEMCPY2(d0,s0,c0) do {      \
    short *d__ = (short*)(d0);      \
    short *s__ = (short*)(s0);      \
    size_t c__ = (c0)>>1;           \
    do {                            \
        *d__++ = *s__++;            \
    } while (--c__);                \
} while(0)

#define MEMCPY4(d0,s0,c0) do {         \
    uint32_t *d__ = (uint32_t*)(d0);   \
    uint32_t *s__ = (uint32_t*)(s0);   \
    size_t    c__ = (unsigned)(c0)>>2; \
    do {                               \
        *d__++ = *s__++;               \
    } while (--c__);                   \
} while(0)

#define MEMCPYW(d0,s0,c0) do {      \
    short *d__ = (short*)(d0);      \
    short *s__ = (short*)(s0);      \
    size_t c__ = (c0);              \
    do {                            \
        *d__++ = *s__++;            \
    } while (--c__);                \
} while(0)

#define MEMCPYD(d0,s0,c0) do {       \
    uint32_t *d__ = (uint32_t*)(d0); \
    uint32_t *s__ = (uint32_t*)(s0); \
    size_t    c__ = (c0);            \
    do {                             \
        *d__++ = *s__++;             \
    } while (--c__);                 \
} while(0)

#define MEMSET(d0,s0,c0) do {       \
    char * d__ = (char*)(d0);       \
    size_t c__ = (c0);              \
    do {                            \
        *d__++ = (char)(s0);        \
    } while(--c__);                 \
} while(0)

#define MEMSETW(d0,s0,c0) do {      \
    short *d__ = (short*)(d0);      \
    size_t c__ = (c0);              \
    do {                            \
        *d__++ = (short)(s0);       \
    } while(--c__);                 \
} while(0)

#define MEMSETD(d0,s0,c0) do {       \
    uint32_t *d__ = (uint32_t*)(d0); \
    size_t    c__ = (c0);            \
    do {                             \
        *d__++ = (long)(s0);         \
    } while(--c__);                  \
} while(0)



/*------------------------------------------*/
/*------------------------------------------*/
/*------------------------------------------*/

char *strncpyZ(char *dst, char const* src, size_t size);
char *StrSkipSpc(char const* s);
char *StrSkipSpc2(char const* s);
char *StrDelLf(char *s);
char *FIL_BaseName(char const* adr);
char *FIL_ExtPtr(char const* name);
char *FIL_ChgExt(char filename[], char *ext);
char *FIL_AddExt(char filename[], char *ext);
int  FIL_GetTmpDir(char *t);
char *FIL_DelLastDirSep(char *dir);
char *FIL_DirNameAddExt(char *nam, char *dir, char *name, char *addext);
char *FIL_DirNameChgExt(char *nam, char *dir, char *name, char *chgext);
void FIL_LoadE(char *name, void *buf, size_t size);
void FIL_SaveE(char *name, void *buf, size_t size);
void *FIL_LoadM(char *name);
void *FIL_LoadME(char *name);
extern size_t FIL_loadsize;

volatile void err_exit(char const* fmt, ...);
int  err_printf(char const* fmt, ...);
void *mallocE(size_t a);
void *reallocE(void *a, size_t b);
void *callocE(size_t a, size_t b);
char *strdupE(char const* p);
int freeE(void *p);
FILE *fopenE(char const* name, char const* mod);
size_t  fwriteE(void *buf, size_t sz, size_t num, FILE *fp);
size_t  freadE(void *buf, size_t sz, size_t num, FILE *fp);
int fgetcE(FILE *fp);
int fgetc2iE(FILE *fp);
int fgetc4iE(FILE *fp);
int fgetc2mE(FILE *fp);
int fgetc4mE(FILE *fp);
void fputcE(int c, FILE *fp);
void fputc2mE(int c, FILE *fp);
void fputc4mE(int c, FILE *fp);
void *fputsE(char *s, FILE *fp);
void fputc2iE(int c, FILE *fp);
void fputc4iE(int c, FILE *fp);

int  TXT1_Open(char const* name);
void TXT1_OpenE(char const* name);
void TXT1_Close(void);
char *TXT1_GetsE(char *buf, int sz);
void TXT1_Error(char const* fmt, ...);
void TXT1_ErrorE(char const* fmt, ...);
extern uint32_t TXT1_line;
extern char     TXT1_name[FIL_NMSZ];
extern FILE*    TXT1_fp;

typedef struct SLIST {
    struct SLIST*   link;
    char*           s;
} SLIST;
SLIST *SLIST_Add(SLIST** root, char const* s);
void   SLIST_Free(SLIST** p0);

typedef int (*STBL_CMP)(void const* s0, void const* s1);
STBL_CMP STBL_SetFncCmp(STBL_CMP fncCmp);
int STBL_Add(void const *t[], int *tblcnt, void const* key);
int STBL_Search(void const* const tbl[], int nn, void const* key);


#endif  /* SUBR_H */
