#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef SUBR_H
#define SUBR_H

#define STDERR		stderr

#define FIL_NMSZ	1024

typedef unsigned char  UCHAR;
typedef signed   char  SCHAR;
typedef unsigned       UINT;
typedef unsigned short USHORT;
typedef unsigned long  ULONG;
#if 1
typedef int 			Int;
typedef signed char 	Sint8;
typedef short			Sint16;
typedef long			Sint32;
typedef unsigned		Uint;
typedef unsigned char	Uint8;
typedef unsigned short	Uint16;
typedef unsigned long	Uint32;
#else
typedef int 			int_t;
typedef signed char 	int8_t;
typedef short			int16_t;
typedef long			int32_t;
typedef unsigned		uint_t;
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned long	uint32_t;
#endif


#define MEMBER_OFFSET(t,m)	((long)&(((t*)0)->m))	/* 構造体メンバ名の、オフセット値を求める */

#define ISDIGIT(c)  (((unsigned)(c) - '0') < 10U)
#define ISLOWER(c)	(((unsigned)(c)-'a') < 26U)
#define TOUPPER(c)  (ISLOWER(c) ? (c) - 0x20 : (c) )
//#define ISKANJI(c)	((unsigned)((c)^0x20) - 0xA1 < 0x3C)
#define ISKANJI(c)		((unsigned char)(c) >= 0x81 && ((unsigned char)(c) <= 0x9F || ((unsigned char)(c) >= 0xE0 && (unsigned char)(c) <= 0xFC)))
#define ISKANJI2(c) ((UCHAR)(c) >= 0x40 && (UCHAR)(c) <= 0xfc && (c) != 0x7f)
#define STREND(p)	((p)+strlen(p))
#define STRINS(d,s) (memmove((d)+strlen(s),(d),strlen(d)+1),memcpy((d),(s),strlen(s)))

#define MAX(x, y)		((x) > (y) ? (x) : (y)) /* 最大値 */
#define MIN(x, y)		((x) < (y) ? (x) : (y)) /* 最小値 */
#define ABS(x)			((x) < 0 ? -(x) : (x))	/* 絶対値 */

#define REVW(a)			((((a) >> 8) & 0xff)|(((a) & 0xff) << 8))
#define REVL(a)			( (((a) & 0xff000000) >> 24)|(((a) & 0x00ff0000) >>  8)|(((a) & 0x0000ff00) <<  8)|(((a) & 0x000000ff) << 24) )

#define	BB(a,b)			((((unsigned char)(a))<<8)+(unsigned char)(b))
#define	WW(a,b)			((((unsigned short)(a))<<16)+(unsigned short)(b))
#define	BBBB(a,b,c,d)	((((unsigned char)(a))<<24)+(((unsigned char)(b))<<16)+(((unsigned char)(c))<<8)+((unsigned char)(d)))

#define	GLB(a)			((unsigned char)(a))
#define	GHB(a)			GLB(((unsigned short)(a))>>8)
#define	GLLB(a)			GLB(a)
#define	GLHB(a)			GHB(a)
#define	GHLB(a)			GLB(((unsigned long)(a))>>16)
#define	GHHB(a)			GLB(((unsigned long)(a))>>24)
#define	GLW(a)			((unsigned short)(a))
#define	GHW(a)			GLW(((unsigned long)(a))>>16)

#define	PEEKB(a)		(*(unsigned char  *)(a))
#define	PEEKW(a)		(*(unsigned short *)(a))
#define	PEEKD(a)		(*(unsigned long  *)(a))
#define	POKEB(a,b)		(*(unsigned char  *)(a) = (b))
#define	POKEW(a,b)		(*(unsigned short *)(a) = (b))
#define	POKED(a,b)		(*(unsigned long  *)(a) = (b))
#define	PEEKiW(a)		( PEEKB(a) | (PEEKB((unsigned long)(a)+1)<< 8) )
#define	PEEKiD(a)		( PEEKiW(a) | (PEEKiW((unsigned long)(a)+2) << 16) )
#define	PEEKmW(a)		( (PEEKB(a)<<8) | PEEKB((unsigned long)(a)+1) )
#define	PEEKmD(a)		( (PEEKmW(a)<<16) | PEEKmW((unsigned long)(a)+2) )
#define	POKEmW(a,b)		(POKEB((a),GHB(b)), POKEB((ULONG)(a)+1,GLB(b)))
#define	POKEmD(a,b)		(POKEmW((a),GHW(b)), POKEmW((ULONG)(a)+2,GLW(b)))
#define	POKEiW(a,b)		(POKEB((a),GLB(b)), POKEB((ULONG)(a)+1,GHB(b)))
#define	POKEiD(a,b)		(POKEiW((a),GLW(b)), POKEiW((ULONG)(a)+2,GHW(b)))

#define	SETBTS(d,s,msk,pos)	(  (d) = ((d) & ~(msk << pos)) | (((s) & msk) << pos)  )

#define	DB			if (debugflag)
extern int		debugflag;


/* memマクロ */
#define MEMCPY(d0,s0,c0) do {		\
	char *d__ = (void*)(d0);		\
	char *s__ = (void*)(s0);		\
	int c__ = (c0);					\
	do {							\
		*d__++ = *s__++;			\
	} while (--c__);				\
} while(0)

#define MEMCPY2(d0,s0,c0) do {		\
	short *d__ = (void*)(d0); 		\
	short *s__ = (void*)(s0); 		\
	int c__ = (c0)>>1;				\
	do {							\
		*d__++ = *s__++;			\
	} while (--c__);				\
} while(0)

#define MEMCPY4(d0,s0,c0) do {		\
	long *d__ = (void*)(d0);		\
	long *s__ = (void*)(s0);		\
	int c__ = (unsigned)(c0)>>2;	\
	do {							\
		*d__++ = *s__++;			\
	} while (--c__);				\
} while(0)

#define MEMCPYW(d0,s0,c0) do {		\
	short *d__ = (void*)(d0); 		\
	short *s__ = (void*)(s0); 		\
	int c__ = (c0);					\
	do {							\
		*d__++ = *s__++;			\
	} while (--c__);				\
} while(0)

#define MEMCPYD(d0,s0,c0) do {		\
	long *d__ = (void*)(d0);		\
	long *s__ = (void*)(s0);		\
	int c__ = (c0);					\
	do {							\
		*d__++ = *s__++;			\
	} while (--c__);				\
} while(0)

#define MEMSET(d0,s0,c0) do {		\
	char *d__ = (void*)(d0);		\
	int c__ = (c0);					\
	do {							\
		*d__++ = (char)(s0);		\
	} while(--c__);					\
} while(0)

#define MEMSETW(d0,s0,c0) do {		\
	short *d__ = (void*)(d0); 		\
	int c__ = (c0);					\
	do {							\
		*d__++ = (short)(s0); 		\
	} while(--c__);					\
} while(0)

#define MEMSETD(d0,s0,c0) do {		\
	long *d__ = (void*)(d0);		\
	int c__ = (c0);					\
	do {							\
		*d__++ = (long)(s0);		\
	} while(--c__);					\
} while(0)



/*------------------------------------------*/
/*------------------------------------------*/
/*------------------------------------------*/

char *strncpyZ(char *dst, char *src, size_t size);
char *StrSkipSpc(char *s);
char *StrSkipSpc2(char *s);
char *StrDelLf(char *s);
char *FIL_BaseName(char *adr);
char *FIL_ExtPtr(char *name);
char *FIL_ChgExt(char filename[], char *ext);
char *FIL_AddExt(char filename[], char *ext);
int  FIL_GetTmpDir(char *t);
char *FIL_DelLastDirSep(char *dir);
char *FIL_DirNameAddExt(char *nam, char *dir, char *name, char *addext);
char *FIL_DirNameChgExt(char *nam, char *dir, char *name, char *chgext);
void FIL_LoadE(char *name, void *buf, int size);
void FIL_SaveE(char *name, void *buf, int size);
void *FIL_LoadM(char *name);
void *FIL_LoadME(char *name);
extern int	FIL_loadsize;

volatile void err_exit(char *fmt, ...);
int  err_printf(char *fmt, ...);
void *mallocE(size_t a);
void *reallocE(void *a, size_t b);
void *callocE(size_t a, size_t b);
char *strdupE(char *p);
int freeE(void *p);
FILE *fopenE(char *name, char *mod);
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

int  TXT1_Open(char *name);
void TXT1_OpenE(char *name);
void TXT1_Close(void);
char *TXT1_GetsE(char *buf, int sz);
void TXT1_Error(char *fmt, ...);
void TXT1_ErrorE(char *fmt, ...);
extern ULONG	TXT1_line;
extern char		TXT1_name[FIL_NMSZ];
extern FILE		*TXT1_fp;

typedef struct SLIST {
	struct SLIST	*link;
	char			*s;
} SLIST;
SLIST *SLIST_Add(SLIST **root, char *s);
void   SLIST_Free(SLIST **p0);

typedef int (*STBL_CMP)(void *s0, void *s1);
STBL_CMP STBL_SetFncCmp(STBL_CMP fncCmp);
int STBL_Add(void *t[], int *tblcnt, void *key);
int STBL_Search(void *tbl[], int nn, void *key);

int StrExpr(char *s_old, char **s_new, long *val);		/* 戻り値0:no error  !0:error */
void StrExpr_SetNameChkFunc(int (*name2valFnc)(char *name, long *valp));
	/* name2valFnc は、名前が渡され、正常なら0を返しその名前の値を *valpにいれる. 異常なら-1を返す関数を設定すること */

#endif	/* SUBR_H */
