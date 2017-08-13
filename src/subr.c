/**
 *  @file   subr.c
 *  @brief  subrutines
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *      Boost Software License Version 1.0
 */

#include "subr.h"
#include <stdarg.h>
#include <io.h>

int     debugflag;

char *strncpyZ(char *dst, char const* src, size_t size)
{
    strncpy(dst, src, size);
    dst[size-1] = 0;
    return dst;
}

char* StrSkipSpc(char const* s)
{
    while ((*s && *(unsigned char *)s <= ' ') || *s == 0x7f) {
        s++;
    }
    return (char*)s;
}


/* �S�p�Ή���StrSkipSpc
 */
char* StrSkipSpc2(char const* s)
{
    while ((*s && *(unsigned char *)s <= ' ') || *s == 0x7f || ((unsigned char)s[0] == 0x81 && s[1] == 0x40)) {
        if ((unsigned char)s[0] == 0x81)
            s++;
        s++;
    }
    return (char*)s;
}



char *StrDelLf(char *s)
{
    char *p;
    p = STREND(s);
    if (p != s && p[-1] == '\n') {
        p[-1] = 0;
    }
    return s;
}

char *FIL_BaseName(char const* adr)
{
    char const* p;

    p = adr;
    while (*p != '\0') {
        if (*p == ':' || *p == '/' || *p == '\\')
            adr = p + 1;
        if (ISKANJI((*(unsigned char *)p)) && *(p+1) )
            p++;
        p++;
    }
    return (char*)adr;
}

char *FIL_ExtPtr(char const* name)
{
    char const* p;

    name = FIL_BaseName(name);
    p = strrchr(name, '.');
    if (p) {
        return (char*)p+1;
    }
    return (char*)STREND(name);
}


char *FIL_ChgExt(char filename[], char *ext)
{
    char *p;

    p = FIL_BaseName(filename);
    p = strrchr( p, '.');
    if (p == NULL) {
        if (ext) {
            strcat(filename,".");
            strcat( filename, ext);
        }
    } else {
        if (ext == NULL)
            *p = 0;
        else
            strcpy(p+1, ext);
    }
    return filename;
}


char *FIL_AddExt(char filename[], char *ext)
{
    if (strrchr(FIL_BaseName(filename), '.') == NULL) {
        strcat(filename,".");
        strcat(filename, ext);
    }
    return filename;
}

char *FIL_DelLastDirSep(char *dir)
{
    char *p;

    if (dir) {
        p = FIL_BaseName(dir);
        if (strlen(p) > 1) {
            p = STREND(dir);
            if (p[-1] == '\\' || p[-1] == '/')
                p[-1] = 0;
        }
    }
    return dir;
}

char *FIL_DirNameChgExt(char *nam, char *dir, char *name, char *chgext)
{
    if (name == NULL || strcmp(name,".") == 0)
        return NULL;
    if (dir && *dir) {
        sprintf(nam, "%s\\%s", dir, name);
    } else {
        sprintf(nam, "%s", name);
    }
    FIL_ChgExt(nam, chgext);
    strupr(nam);
    return nam;
}

char *FIL_DirNameAddExt(char *nam, char *dir, char *name, char *addext)
{
    if (name == NULL || strcmp(name,".") == 0)
        return NULL;
    if (dir && *dir) {
        sprintf(nam, "%s\\%s", dir, name);
    } else {
        sprintf(nam, "%s", name);
    }
    FIL_AddExt(nam, addext);
    strupr(nam);
    return nam;
}


/*--------------------- �G���[�����t���̕W���֐� ---------------------------*/
volatile void err_exit(char const* fmt, ...)
{
    va_list app;

    va_start(app, fmt);
/*  fprintf(STDERR, "%s %5d : ", src_name, src_line);*/
    vfprintf(STDERR, fmt, app);
    va_end(app);
    exit(1);
}


int err_printf(char const* fmt, ...)
{
    va_list app;
    int     n;

    va_start(app, fmt);
    n = vfprintf(STDERR, fmt, app);
    va_end(app);
    fflush(STDERR);
    return n;
}



/* �G���[������Α�exit�� malloc() */
void *mallocE(size_t a)
{
    void *p;
#if 1
    if (a == 0)
        a = 1;
#endif
    p = malloc(a);
//printf("malloc(0x%x)\n",a);
    if (p == NULL) {
        err_exit("������������Ȃ�(%d byte(s))\n",a);
    }
    return p;
}

/* �G���[������Α�exit�� calloc() */
void *callocE(size_t a, size_t b)
{
    void *p;

#if 1
    if (a== 0 || b == 0)
        a = b = 1;
#endif
    p = calloc(a,b);
//printf("calloc(0x%x,0x%x)\n",a,b);
    if (p == NULL) {
        err_exit("������������Ȃ�(%d*%d byte(s))\n",a,b);
    }
    return p;
}

/* �G���[������Α�exit�� calloc() */
void *reallocE(void *m, size_t a)
{
    void *p;
    if (a == 0)
        a = 1;
    p = realloc(m, a);
//printf("realloc(0x%x,0x%x)\n",m,a);
    if (p == NULL) {
        err_exit("������������Ȃ�(%d byte(s))\n",a);
    }
    return p;
}

/* �G���[������Α�exit�� strdup() */
char *strdupE(char const* s)
{
    char *p;
    if (s == NULL)
        return callocE(1,1);
  #if 1
    p = calloc(1,strlen(s)+8);
    if (p)
        strcpy(p, s);
  #else
    p = strdup(s);
  #endif
    if (p == NULL) {
        err_exit("������������Ȃ�(����%d+1)\n",strlen(s));
    }
    return p;
}

int freeE(void *p)
{
    if (p)
        free(p);
    return 0;
}

/* ------------------------------------------------------------------------ */
/* �G���[������Α�exit�� fopen() */
FILE *fopenE(char const* name, char const* mod)
{
    FILE*   fp;
    fp = fopen(name,mod);
    if (fp == NULL) {
        err_exit("�t�@�C�� %s ���I�[�v���ł��܂���\n",name);
    }
    setvbuf(fp, NULL, _IOFBF, 1024*1024);
    return fp;
}

/* �G���[������Α�exit�� fwrite() */
size_t  fwriteE(void *buf, size_t sz, size_t num, FILE *fp)
{
    size_t l;

    l = fwrite(buf, sz, num, fp);
    if (ferror(fp)) {
        err_exit("�t�@�C�������݂ŃG���[����\n");
    }
    return l;
}

/* �G���[������Α�exit�� fread() */
size_t  freadE(void *buf, size_t sz, size_t num, FILE *fp)
{
    size_t l;

    l = fread(buf, sz, num, fp);
    if (ferror(fp)) {
        err_exit("�t�@�C���Ǎ��݂ŃG���[����\n");
    }
    return l;
}

/* ------------------------------------------------------------------------ */

/* fp��� 1�o�C�g(0..255) �ǂݍ���. �G���[������Α��I�� */
int fgetcE(FILE *fp)
{
  #if 1
    static uint8_t buf[1];
    freadE(buf, 1, 1, fp);
    return (uint8_t)buf[0];
  #else
    int c;
    c = fgetc(fp);
    if (c == EOF) {
        err_exit("�t�@�C���ǂݍ��݂ŃG���[����\n");
    }
    return (uint8_t)c;
  #endif
}

/* fp��� ��ٴ��ި�݂� 2�o�C�g�ǂݍ���. �G���[������Α��I�� */
int fgetc2iE(FILE *fp)
{
    int c;
    c = fgetcE(fp);
    return (uint16_t)(c | (fgetcE(fp)<<8));
}

/* fp��� ��ٴ��ި�݂� 4�o�C�g�ǂݍ���. �G���[������Α��I�� */
int fgetc4iE(FILE *fp)
{
    int c;
    c = fgetc2iE(fp);
    return c + (fgetc2iE(fp)<<16);
}

/* fp��� big���ި�݂� 2�o�C�g�ǂݍ���. �G���[������Α��I�� */
int fgetc2mE(FILE *fp)
{
    int c;
    c = fgetcE(fp);
    return (uint16_t)((c<<8) | fgetcE(fp));
}

/* fp��� big���ި�݂� 4�o�C�g�ǂݍ���. �G���[������Α��I�� */
int fgetc4mE(FILE *fp)
{
    int c;
    c = fgetc2mE(fp);
    return (c<<16) | fgetc2mE(fp);
}

/* fp�� 1�o�C�g(0..255) ��������. �G���[������Α��I�� */
void fputcE(int c, FILE *fp)
{
    uint8_t buf[1];
    buf[0] = (uint8_t)c;
    fwriteE(buf, 1, 1, fp);
}

/* fp�� �ޯ�޴��ި�݂� 2�o�C�g��������. �G���[������Α��I�� */
void fputc2mE(int c, FILE *fp)
{
    uint8_t buf[4];
    buf[0] = (uint8_t)(c>> 8);
    buf[1] = (uint8_t)(c);
    fwriteE(buf, 1, 2, fp);
}

/* fp�� �ޯ�޴��ި�݂� 4�o�C�g��������. �G���[������Α��I�� */
void fputc4mE(int c, FILE *fp)
{
    uint8_t buf[4];
    buf[0] = (uint8_t)(c>>24);
    buf[1] = (uint8_t)(c>>16);
    buf[2] = (uint8_t)(c>> 8);
    buf[3] = (uint8_t)(c);
    fwriteE(buf, 1, 4, fp);
}

void *fputsE(char *s, FILE *fp)
{
    size_t n;
    n = strlen(s);
    fwriteE(s, 1, n, fp);
    return s;
}

/* fp�� ��ٴ��ި�݂� 2�o�C�g��������. �G���[������Α��I�� */
void fputc2iE(int c, FILE *fp)
{
    uint8_t buf[4];
    buf[0] = (uint8_t)(c);
    buf[1] = (uint8_t)(c>> 8);
    fwriteE(buf, 1, 2, fp);
}

/* fp�� ��ٴ��ި�݂� 4�o�C�g��������. �G���[������Α��I�� */
void fputc4iE(int c, FILE *fp)
{
    uint8_t buf[4];
    buf[0] = (uint8_t)(c);
    buf[1] = (uint8_t)(c>> 8);
    buf[2] = (uint8_t)(c>> 16);
    buf[3] = (uint8_t)(c>> 24);
    fwriteE(buf, 1, 4, fp);
}

/* ------------------------------------------------------------------------ */
int FIL_GetTmpDir(char *t)
{
    char *p;
    char nm[FIL_NMSZ+2];

    if (*t) {
        strcpy(nm, t);
        p = STREND(nm);
    } else {
        p = getenv("TMP");
        if (p == NULL) {
            p = getenv("TEMP");
            if (p == NULL) {
              #if 0
                p = ".\\";
              #else
                err_exit("���ϐ�TMP��TEMP�������إ�ިڸ�؂��w�肵�Ă�������\n");
              #endif
            }
        }
        strcpy(nm, p);
        p = STREND(nm);
    }
    if (p[-1] != '\\' && p[-1] != ':' && p[-1] != '/')
        strcat(nm,"\\");
    strcat(nm,"*.*");
    _fullpath(t, nm, FIL_NMSZ);
    p = FIL_BaseName(t);
    *p = 0;
    if (p[-1] == '\\')
        p[-1] = 0;
    return 0;
}

#if 0
char *FIL_DirNameAddExtM(char *dir, char *name, char *addext)
{
    char *p;

    if (name == NULL || strcmp(name,".") == 0)
        return NULL;
    if (dir && *dir) {
        p = mallocE(strlen(dir) + strlen(name) + (32));
        sprintf(p, "%s\\%s", dir, name);
    } else {
        p = mallocE(strlen(name) + (32));
        sprintf(p, "%s", name);
    }
    FIL_AddExt(p, addext);
    return p;
}
#endif

void FIL_SaveE(char *name, void *buf, size_t size)
{
    FILE *fp;

    fp = fopenE(name,"wb");
    if (size)
        fwriteE(buf, 1, size, fp);
    fclose(fp);
}

void FIL_LoadE(char *name, void *buf, size_t size)
{
    FILE *fp;

    if (size) {
        fp = fopenE(name,"rb");
        if (size)
            freadE(buf, 1, size, fp);
        fclose(fp);
    }
}

size_t  FIL_loadsize;

void *FIL_LoadME(char *name)
{
    FILE*   fp;
    char*   buf;
    size_t  l;

    fp = fopenE(name,"rb");
    l  = filelength(fileno(fp));
    FIL_loadsize = l;
    if (l) {
        l = (l + 15) & ~15;
        buf = callocE(1,l);
        freadE(buf, 1, l, fp);
    } else {
        buf = mallocE(16);
    }
    fclose(fp);
    return buf;
}

void *FIL_LoadM(char *name)
{
    FILE *fp;
    char  *buf;
    int   l;

    fp = fopen(name,"rb");
    if (fp == NULL)
        return NULL;
    l  = filelength(fileno(fp));
    FIL_loadsize = l;
    if (l) {
        l = (l + 15) & ~15;
        buf = calloc(1,l);
        if (buf)
            freadE(buf, 1, l, fp);
    } else {
        buf = NULL/*mallocE(16)*/;
    }
    fclose(fp);
    return buf;
}


/* ------------------------------------------------------------------------ */
uint32_t    TXT1_line;
char        TXT1_name[FIL_NMSZ];
FILE*       TXT1_fp;

void TXT1_Error(char const* fmt, ...)
{
    va_list app;

    va_start(app, fmt);
    fprintf(STDERR, "%-12s %5d : ", TXT1_name, TXT1_line);
    vfprintf(STDERR, fmt, app);
    va_end(app);
    return;
}

void TXT1_ErrorE(char const* fmt, ...)
{
    va_list app;

    va_start(app, fmt);
    fprintf(STDERR, "%-12s %5d : ", TXT1_name, TXT1_line);
    vfprintf(STDERR, fmt, app);
    va_end(app);
    exit(1);
}

int TXT1_Open(char const* name)
{
    TXT1_fp = fopen(name,"rt");
    if (TXT1_fp == 0)
        return -1;
    strcpy(TXT1_name, name);
    TXT1_line = 0;
    return 0;
}

void TXT1_OpenE(char const* name)
{
    TXT1_fp = fopenE(name,"rt");
    strcpy(TXT1_name, name);
    TXT1_line = 0;
}


char *TXT1_GetsE(char *buf, int sz)
{
    char *p;

    p = fgets(buf, sz, TXT1_fp);
    if (ferror(TXT1_fp)) {
        TXT1_Error("file read error\n");
        exit(1);
    }
    TXT1_line++;
    return p;
}

void TXT1_Close(void)
{
    fclose(TXT1_fp);
}


/* ------------------------------------------------------------------------ */

SLIST *SLIST_Add(SLIST **p0, char const* s)
{
    SLIST* p;
    p = *p0;
    if (p == NULL) {
        p = callocE(1, sizeof(SLIST));
        p->s = strdupE(s);
        *p0 = p;
    } else {
        while (p->link != NULL) {
            p = p->link;
        }
        p->link = callocE(1, sizeof(SLIST));
        p = p->link;
        p->s = strdupE(s);
    }
    return p;
}

void SLIST_Free(SLIST **p0)
{
    SLIST *p, *q;

    for (p = *p0; p; p = q) {
        q = p->link;
        freeE(p->s);
        freeE(p);
    }
}

/* ------------------------------------------------------------------------ */
static STBL_CMP STBL_cmp = (STBL_CMP)strcmp;

STBL_CMP STBL_SetFncCmp(STBL_CMP cmp)
{
    if (cmp)
        STBL_cmp = cmp;
    return STBL_cmp;
}

/*
 *  t     : ������ւ̃|�C���^�������߂��z��
 *  tblcnt: �o�^�ό�
 *  key   : �ǉ����镶����
 *  ���A�l: 0:�ǉ� -1:���łɓo�^��
 */
int STBL_Add(void const* t[], int* tblcnt, void const* key)
{
    int  low, mid, f, hi;

    hi = *tblcnt;
    mid = low = 0;
    while (low < hi) {
        mid = (low + hi - 1) / 2;
        if ((f = STBL_cmp(key, t[mid])) < 0) {
            hi = mid;
        } else if (f > 0) {
            mid++;
            low = mid;
        } else {
            return -1;  /* �������̂��݂������̂Œǉ����Ȃ� */
        }
    }
    (*tblcnt)++;
    for (hi = *tblcnt; --hi > mid;) {
        t[hi] = t[hi-1];
    }
    t[mid] = key;
    return 0;
}

/*
 *  key:������������ւ̃|�C���^
 *  tbl:������ւ̃|�C���^�������߂��z��
 *  nn:�z��̃T�C�Y
 *  ���A�l:��������������̔ԍ�(0���)  �݂���Ȃ������Ƃ�-1
 */
int STBL_Search(void const* const tbl[], int nn, void const* key)
{
    int     low, mid, f;

    low = 0;
    while (low < nn) {
        mid = (low + nn - 1) / 2;
        if ((f = STBL_cmp(key, tbl[mid])) < 0)
            nn = mid;
        else if (f > 0)
            low = mid + 1;
        else
            return mid;
    }
    return -1;
}
