/**
  @file   dcasm.c
  @brief  dc.b, dc.w, dc.l ���߂݂̂̃A�Z���u��
  @author Masashi KITAMURA    (tenka@6809.net)
  @date   2000,2017
  @note
    see license.txt
 */

#include "subr.h"
#include <stdarg.h>
#include "tree.h"
#include "filn.h"
#include "strexpr.h"
#include "mbc.h"


#if defined(_WIN32) || defined(_MSC_VER)
#define strcasecmp  stricmp
#endif


#if defined(_MSC_VER) && _MSC_VER < 1600
#define S_FMT_LL            "I64"
#else
#define S_FMT_LL            "ll"
#endif
#define S_FMT_X             "%" S_FMT_LL "x"
#define S_FMT_08X           "%08" S_FMT_LL "x"
#define S_FMT_10D           "%10" S_FMT_LL "d"

//typedef long          val_t;
typedef int64_t         val_t;



void Usage(void)
{
    err_printf(
            "usage> dcasm [-opt] filename(s)   // v0.50 " __DATE__ "  " __TIME__ "  by tenk*\n"
            "  �o�C�i���E�f�[�^������ړI�Ƃ���dc�f�B���N�e�B�u�݂̂̃A�Z���u��\n"
            "  �t�@�C�����������w�肳�ꂽ�ꍇ�A��A�̃e�L�X�g�Ƃ��ď��ɓǂ݂���.\n"
            "  �o�̓t�@�C�����͍ŏ��̃t�@�C�����̊g���q��.bin�ɂ�������\n"
            "  -?        ���̃w���v\n"
            "  -iDIR     �w�b�_�t�@�C���̓��̓f�B���N�g���̎w��\n"
            "  -oFILE    ���ʂ� FILE �ɏo��.\n"
            "  -eFILE    �G���[�� FILE �ɏo��\n"
            "  -b<B|L>   �f�t�H���g�̃G���f�B�A����B=�r�b�O, L=���g���ɂ���\n"
            "  -a<0|2|4|8> �A���C�����g�ő�o�C�g��(0:������) ex)-a2�Ȃ�dc.l,dc.q��2byte�P��\n"
            "  -rFILE    ��ԖڂɓǍ��ރt�@�C�����w��(���[��/�}�N����`�̋L�q�t�@�C����z��)\n"
            "  -dNAME[=STR]  #define����`\n"
            "  -uNAME    #undef����\n"
            "  -hFILE    xdef���ꂽ���x����#define�`���Ńt�@�C���ɏo�͂���\n"
            "  -y        ������, C�����\\�G�X�P�[�v�V�[�P���X��L��  -y- ����\n"
            "  -s        ;���R�����g�J�n�����łȂ������p�̃Z�p���[�^�Ƃ��Ĉ���\n"
            "  -m        ����(���@)�w���v��W���G���[�o��\n"
            "  -cUTF8    ���̓e�L�X�g��utf8�Ƃ��ĉ���.\n"
            "  -cMBC     ���̓e�L�X�g���}���`�o�C�g����(SJIS)�Ƃ��ĉ���.\n"
            "  -v[-]     �r���o�߃��b�Z�[�W��\������ -v-���Ȃ�\n"
            "  @FILE     ���X�|���X�E�t�@�C���w��\n"
    );
    exit(1);
}


void ManHelp(void)
{
    err_printf(
            "dcasm v0.50 �̎�Ȏd�l\n"
            "10�i�����l       0..9��_�ō\������镶����. _�͖���.\n"
            "16�i�����l       C���ꕗ0x0, �C���e����0h, ���g���[����$0\n"
            " 2�i�����l       C���ꕗ�g��0b011�A�C���e���� 011h �\�L. ���g���[�����\�L%011\n"
            " 8�i�����l       �w��ł��Ȃ�\n"
            "���Z�q(C�ɓ���)  () + - ! ~   * / % + - << >>  > >= < <= == != & ^ | && ||\n"
            "�s���A�h���X�l   $ �܂��� �P��*\n"
            "�R�����g         ; �� * �� // �� rem �Ŏn�܂�s���܂ł� /* */ �ň͂܂ꂽ�͈�\n"
            "LBL:             ���x�� LBL ���`\n"
            "LBL: equ N       ���x�� LBL �̒l�� N �ɂ���\n"
            "xdef LBL         ���x�� LBL �� .h �t�@�C����#define��`�ŏo��.\n"
            "dc.<b|w|l|q> ... 1�o�C�g/2�o�C�g/4�o�C�g������̃f�[�^����\n"
            "db dw dd dq      �V\n"
            "ds.<b|w|l> N     N�o�C�g�̗̈���m��(0����)\n"
            "org  N           �A�h���X�� N�ɂ���(���Ԃ�0����)\n"
            "even             �A�h���X�������ɂȂ�悤����(���Ԃ�0����)\n"
            "align N          �A�h���X��N�o�C�g�P�ʂɂȂ�悤����\n"
            "auto_align 2|4|8 �A���C�����g�ő�o�C�g��. ex)2 �Ȃ� dc.l, dc.q �ł�2�o�C�g�P��\n"
            "big_endian       �r�b�O�E�G���f�B�A���ɂ���\n"
            "little_endian    ���g���E�G���f�B�A���ɂ���\n"
            "end              �\�[�X�̏I���(�Ȃ��Ă��悢)\n"
            "#if,#define,#macro,#rept,#ipr ���̏����A�Z���u����}�N�����߂���\n"
        );
    exit(1);
}


/* ------------------------------------------------------------------------ */
SLIST*      fileListTop = NULL;
char*       outname     = NULL;
char*       errname     = NULL;
char*       rulename    = NULL;
char*       hdrname     = NULL;
static int  cescMode;           /* c�� \ �G�X�P�[�v�V�[�P���X��p���邩 */
static int  endianMode;         /* �G���f�B�A���̎w��                   */
static int  autoAlignMode = 3;  /* dc.w, dc.l, dc.q �ł́A�����A���C�����g�̕��@ */
static int  vmsgFlg;
static int  useSep;
static int  mbcMode = 0;


/** �I�v�V�����̏��� 
 */
int Opts(char* a)
{
    char*   p;
    int     c;

    p = a;
    p++, c = *p++, c = toupper(c);
    switch (c) {
    case 'D':
        Filn_SetLabel(p, NULL);
        break;
    case 'U':
        Filn_UndefLabel(p);
        break;
    case 'I':
        Filn_AddIncDir(strdupE(p));
        break;
    case 'H':
        hdrname = strdupE(p);
        break;
    case 'O':
        outname = strdupE(p);
        break;
    case 'R':
        rulename = strdupE(p);
        break;
    case 'Y':
        cescMode = (*p == '-')  ? 0 : 1;
        break;
    case 'E':
        errname = strdupE(p);
        if (errname && errname[0]) {    /* �G���[�o�͂̏��� */
            if (freopen(errname,"at",stderr) == NULL)
                err_exit("%s ���I�[�v���ł��܂���\n", errname);
        }
        break;
    case 'B':
        c = *p++, c = toupper(c);
        if (c == 'L')
            endianMode = 0;
        else if (c == 'B')
            endianMode = 1;
        break;
    case 'A':
        c = *p;
        if (c == '0')
            autoAlignMode  = 0;
        else if (c == '2')
            autoAlignMode  = 1;
        else if (c == '4')
            autoAlignMode  = 2;
        else if (c == '8')
            autoAlignMode  = 3;
        else
            goto OPT_ERR;
        break;
    case 'S':
        useSep = (*p == '-') ? 0 : 1;
        Filn->cmt_chr[0] = (*p == '-') ? ';' : 0;
        break;
    case 'V':
        vmsgFlg = (*p == '-') ? 0 : 1;
        break;
    case 'M':
        ManHelp();
        break;
    case 'Z':
        debugflag = (*p == '-') ? 0 : 1;
        break;
    case 'C':
        if (strcasecmp(p, "mbc") == 0) {
            mbs_setEnv(NULL);
            mbcMode = 1;
            Filn->opt_kanji = 1;
            StrExpr_SetMbcMode(1);
        } else if (strcasecmp(p, "utf8") == 0 || strcasecmp(p, "utf-8") == 0) {
            mbs_setEnv("ja_JP.utf-8");
            mbcMode = 2;
            Filn->opt_kanji = 2;
            StrExpr_SetMbcMode(2);
        }
        break;
    case '\0':
        break;
    case '?':
        Usage();
    default:
  OPT_ERR:
        err_exit("Incorrect command line option : %s\n", a);
    }
    return 0;
}



void GetResFile(char* name)
{
    char    buf[1024*16];
    char*   p;

    TXT1_OpenE(name);
    while (TXT1_GetsE(buf, sizeof buf)) {
        p = strtok(buf, " \t\n");
        do {
            if (*p == '-') {
                Opts(p);
            } else {
                SLIST_Add(&fileListTop, p);
            }
            p = strtok(NULL, " \t\n");
        } while (p);
    }
    TXT1_Close();
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define MBS_ISLEAD(c)       (mbcMode && mbs_islead(c))

typedef struct srcl_t {
    struct srcl_t*  link;
    char*           fname;
    int             line;
    char*           s;
} srcl_t;

srcl_t*         srcl     = NULL;
srcl_t const*   srcl_cur = NULL;
int             srcl_errDisp = 0;
static char     linbuf[0x10000];


void srcl_errorSw(int n)
{
    srcl_errDisp = n;
}

/** �\�[�X���͂ŃG���[���������Ƃ�
 */
void srcl_error(char const* fmt, ...)
{
    va_list app;

    if (srcl_errDisp == 0)
        return;
    va_start(app, fmt);
    if (Filn) {
        if (srcl_cur)
            fprintf(Filn->errFp, "%-12s %5d : ", srcl_cur->fname, srcl_cur->line);
        vfprintf(Filn->errFp, fmt, app);
    } else {
        if (srcl_cur)
            fprintf(stderr, "%-12s %5d : ", srcl_cur->fname, srcl_cur->line);
        vfprintf(stderr, fmt, app);
    }
    va_end(app);
    return;
}


/** �\�[�X�s���������ɒ��߂邽�߃��X�g�ɂ���B
 */
void srcl_add(srcl_t** p0, char const* fnm, int l, char const* s)
{
    srcl_t* p;

    p = *p0;
    if (p == NULL) {
        p = callocE(1, sizeof(srcl_t));
        *p0 = p;
    } else {
        while (p->link != NULL) {
            p = p->link;
        }
        p->link = callocE(1, sizeof(srcl_t));
        p = p->link;
    }
    p->fname = strdupE(fnm);
    p->line  = l;
    p->s = strdupE(s);
}


/** �\�[�X�s�̊J��
 */
void srcl_free(srcl_t** p0)
{
    srcl_t* p;
    srcl_t* q;

    for (p = *p0; p; p = q) {
        q = p->link;
        freeE(p->s);
        freeE(p->fname);
        freeE(p);
    }
}


/** �Z�p���[�^';' �ŋ�؂�ꂽ��s�𕡐��s�ɕ����� 
 */
char*   LinSep(char* st)
{
    /*static char linbuf[0x10000];*/
    static char* s;
    char*        d = linbuf;
    int          k;

    if (st) {
        s = st;
    }
    if (*s == 0) {
        return NULL;
    }
  #if 0
    if (*s == '\n' && s[1] == 0) {
        return NULL;
    }
  #endif
    k = 0;
    while (*s) {
        if (*s == '\n') {
            s++;
            break;
        } if (k == 0 && *s == ';') {
            s++;
            break;
        } else if (*s == '"' && (k&~1) == 0) {
            k ^= 1;
            *d++ = *s++;
        } else if (*s == '\'' && (k&~2) == 0) {
            k ^= 2;
            *d++ = *s++;
        } else if (k && *s == '\\' && s[1]) {
            *d++ = *s++;
            *d++ = *s++;
        } else if (MBS_ISLEAD(*(unsigned char*)s)) {
            unsigned l = mbs_len1(s);
            memcpy(d, s, l);
            d += l;
            s += l;
        } else {
            *d++ = *s++;
        }
    }
    *d++ = '\n';
    *d = 0;
    return linbuf;
}

/** �}�N���W�J�ς݂̃\�[�X���쐬���� 
 */
void GetSrcList(char* name)
{
    char*   s;
    char*   fnm;
    int     l;

    if (Filn_Open(name)) {
        exit(1);
    }
    for (; ;) {
        s = Filn_Gets();    /* �}�N���W�J��̃\�[�X���擾 */
        if (s == NULL)
            break;
        Filn_GetFnameLine((char const**)&fnm, &l);
        if (useSep == 0) {
            strcpy(linbuf, s);
            StrDelLf(linbuf);
            strcat(linbuf, "\n");
            srcl_add(&srcl, fnm, l, linbuf);
        } else {
            char *p;
            p = LinSep(s);  /* ����p��linbuf�������Ă��� */
            while (p) {
                srcl_add(&srcl, fnm, l, p);
                p = LinSep(NULL);
            }
        }
        freeE(s);           /* �擾�������������J�� */
    }
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

typedef struct label_t {
    char const*     name;
    struct label_t* link;
    val_t           val;
    int             typ;
} label_t;

label_t*        label_top;
label_t*        label_cur;

static TREE*    LBL_tree;



/** TREE ���[�`���ŁA�V�����v�f�𑢂�Ƃ��ɌĂ΂�� 
 */
static void *LBL_New(label_t const* t)
{
    label_t*    p;
    p = callocE(1,sizeof(label_t));
    memcpy(p, t, sizeof(label_t));
    p->name = strdupE(t->name);
    return p;
}



/** ���x��TREE ���[�`���ŁA�������J���̂Ƃ��ɌĂ΂�� 
 */
static void LBL_Del(void *ff)
{
    freeE(ff);
}



/** ���x��TREE ���[�`���ŁA�p�������r���� 
 */
static int  LBL_Cmp(label_t const* f1, label_t const* f2)
{
    return strcmp(f1->name, f2->name);
}



/** ���x��TREE �������� 
 */
void    LBL_Init(void)
{
    LBL_tree = TREE_Make((TREE_NEW) LBL_New, (TREE_DEL) LBL_Del, (TREE_CMP) LBL_Cmp, (TREE_MALLOC) mallocE, (TREE_FREE) freeE);
}



/** ���x��TREE ���J�� 
 */
void LBL_Term(void)
{
    TREE_Clear(LBL_tree);
}



/** ���݂̖��O�����x��tree�ɓo�^���ꂽ���x�����ǂ����T��
 */
label_t *LBL_Search(char const* lbl_name)
{
    label_t t;

    memset (&t, 0, sizeof(label_t));
    t.name = lbl_name;
    if (t.name == NULL) {
        srcl_error("���x���o�^�Ń�����������Ȃ�\n");
        exit(1);
    }
    return TREE_Search(LBL_tree, &t);
}



/** ���x��(���O)��؂ɓo�^���� 
 */
label_t *LBL_Add(char const* lbl_name, val_t val, int typ)
{
    label_t     t;
    label_t*    p;

    memset (&t, 0, sizeof(label_t));
    t.name  = lbl_name;
    //t.line  = TXT1_line;
    p = TREE_Insert(LBL_tree, &t);
    if (LBL_tree->flag == 0) {          /* �V�K�o�^�łȂ����� */
        if (p->typ == 1 || typ == 1) {
            p->typ = 3;
            if (typ == 0)
                p->val = val;
        } else {
            srcl_error("%s �����d��`����\n", lbl_name);
            p = NULL;
        }
    } else {
        p->val = val;
        p->typ = typ;
    }
    return p;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
enum {
    ODR_NON,
    ODR_REM,
    ODR_EQU,
    ODR_ORG,
    ODR_DC_B,
    ODR_DC_W,
    ODR_DC_L,
    ODR_DC_Q,
    ODR_DS_B,
    ODR_DS_W,
    ODR_DS_L,
    ODR_DS_Q,
    ODR_EVEN,
    ODR_ALIGN,
    ODR_AUTOALIGN,
    ODR_BIG_ENDIAN,
    ODR_LITTLE_ENDIAN,
    ODR_XDEF,
    ODR_STRI,
    ODR_STRI_OPT,
  #if 0
    ODR_STRI_ADI,
  #endif
    ODR_END,
};

typedef struct odr_t {
    char const* name;
    int         id;
} odr_t;

static TREE*    odr_tree;

/** TREE ���[�`���ŁA�V�����v�f�𑢂�Ƃ��ɌĂ΂��
 */
static void*    odr_new(odr_t const* t)
{
    odr_t*  p;

    p = callocE(1,sizeof(odr_t));
    memcpy(p, t, sizeof(odr_t));
    p->name = strdupE(t->name);
    return p;
}

/** TREE ���[�`���ŁA�������J���̂Ƃ��ɌĂ΂��
 */
static void odr_del(void *ff)
{
    freeE(ff);
}

/** TREE ���[�`���ŁA�p�������r����
 */
static int  odr_cmp(odr_t const* f1, odr_t const* f2)
{
    return strcmp(f1->name, f2->name);
}

int odr_search(char const* name)
{
    odr_t   t;
    odr_t*  p;

    memset (&t, 0, sizeof(odr_t));
    t.name = name;
    p = TREE_Search(odr_tree, &t);
    if (p)
        return p->id;
    return ODR_NON;
}

/** ���x��(���O)��؂ɓo�^����
 */
odr_t*  odr_add(char const* name, int id)
{
    odr_t   t;
    odr_t*  p;

    memset (&t, 0, sizeof(odr_t));
    t.name  = name;
    p = TREE_Insert(odr_tree, &t);
    if (odr_tree->flag == 0) {          /* �V�K�o�^�łȂ����� */
        /* �O���[�o���錾���ꂽ���x���̂Ƃ� */
        err_printf( "PRGERR:%s���ߒ�`�����d�I\n", name);
        p = NULL;
    } else {
        p->id = id;
    }
    return p;
}


void    odr_init(void)
{
    /* TREE �������� */
    odr_tree = TREE_Make((TREE_NEW) odr_new, (TREE_DEL) odr_del, (TREE_CMP) odr_cmp, (TREE_MALLOC) mallocE, (TREE_FREE) freeE);
    odr_add("org",          ODR_ORG);
    odr_add("dc.b",         ODR_DC_B);
    odr_add("dc.w",         ODR_DC_W);
    odr_add("dc.l",         ODR_DC_L);
    odr_add("dc.q",         ODR_DC_Q);
    odr_add("ds.b",         ODR_DS_B);
    odr_add("ds.w",         ODR_DS_W);
    odr_add("ds.l",         ODR_DS_L);
    odr_add("ds.q",         ODR_DS_Q);
    odr_add("even",         ODR_EVEN);
    odr_add("align",        ODR_ALIGN);
    odr_add("auto_align",   ODR_AUTOALIGN);
    odr_add("big_endian",   ODR_BIG_ENDIAN);
    odr_add("little_endian",ODR_LITTLE_ENDIAN);
    odr_add("db",           ODR_DC_B);
    odr_add("dw",           ODR_DC_W);
    odr_add("dd",           ODR_DC_L);
    odr_add("dq",           ODR_DC_Q);
    odr_add("end",          ODR_END);
    odr_add("stri",         ODR_STRI);
    odr_add("stri_opt",     ODR_STRI_OPT);
  #if 0
    odr_add("stri_adi",     ODR_STRI_ADI);
  #endif
    odr_add("equ",          ODR_EQU);
    odr_add("xdef",         ODR_XDEF);
    odr_add("rem",          ODR_REM);
}

void odr_term(void)
{
    /* TREE ���J�� */
    TREE_Clear(odr_tree);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define IsLblTop(c)     (isalpha(c) || (c) == '_' || *s == '@' || *s == '.')
#define TOK_NAME_LEN    1024

static int              passMode;
static ptrdiff_t        g_adr;          /* �A�h���X */
ptrdiff_t               g_adrLinTop;    /* �s���A�h���X. StrExpr����Q�Ƃ���� */
static unsigned char    *g_mem;         /* ������ */
static char             tok_name[TOK_NAME_LEN+2];


char const* GetName(char *buf, char const* s)
{
    unsigned n;

    n = 0;
    if (IsLblTop(*s) || MBS_ISLEAD(*s)) {     //�s���ɋ󔒂̂��郉�x����`�`�F�b�N
        for (;;) {
            if (MBS_ISLEAD(*s)) {
                unsigned l = mbs_len1(s);
                if (n <= TOK_NAME_LEN - l) {
                    n += l;
                    memcpy(buf, s, l);
                    buf += l;
                    s   += l;
                } else {
                    s++;
                }
            } else if (IsLblTop(*s) || isdigit(*s) || *s == '$') {
                if (n < TOK_NAME_LEN) {
                    *buf++ = *s;
                    n++;
                }
                s++;
            } else {
                break;
            }
        }
    }
    *buf = 0;
    return s;
}



int  GetCh(char const** sp, int md)
{
    char const* s;
    unsigned    c,ch,k,l,i;

    s = *sp;
    ch = *(unsigned char*)(s++);
    if (ch == 0)
        s--;
    if (cescMode && ch == '\\') {
        l = 8;
        c = *s++;
        switch (c) {
        case 'a':   ch = '\a';  break;
        case 'b':   ch = '\b';  break;
        case 'e':   ch = 0x1b;  break;
        case 'f':   ch = '\f';  break;
        case 'n':   ch = '\n';  break;
        case 'N':   ch = 0x0d0a;    break;
        case 'r':   ch = '\r';  break;
        case 't':   ch = '\t';  break;
        case 'v':   ch = '\v';  break;
        case '\\':  ch = '\\';  break;
        case '\'':  ch = '\'';  break;
        case '\"':  ch = '\"';  break;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
            l = 3;
            c -= '0';
            for (i = 0; i < l; i++) {
                k = *s;
                if (k >= '0' && k <= '7')
                    c = c * 8 + k-'0';
                else
                    break;
                s++;
            }
            ch = c;
            break;
        case 'x':
            l = 2;
        case 'X':
            c = 0;
            for (i = 0; i < l; i++) {
                k = *s;
                if (isdigit(k))
                    c = c * 16 + k-'0';
                else if (k >= 'A' && k <= 'F')
                    c = c * 16 + 10+k-'A';
                else if (k >= 'a' && k <= 'f')
                    c = c * 16 + 10+k-'a';
                /*else if (k == '_')
                    ;*/
                else
                    break;
                s++;
            }
            ch = c;
            break;
        case 0:
            --s;
            ch = c;
            break;
        default:
            if (c < 0x20 || c == 0x7f || c >= 0xFD) {
                srcl_error("\\�̒���ɃR���g���[���R�[�h������\n");
            }
            ch = c;
        }
    } else if (MBS_ISLEAD(ch) && md > 0) {
        --s;
        ch = mbs_getc(&s);      // utf16�̃T���Q�[�g�y�A�͖��Ή�

    }
    *sp = s;
    return ch;
}


char expr_err_name[TOK_NAME_LEN+2];

int  GetLblVal4StrExpr(char const* name, val_t *valp)
{
    char buf[TOK_NAME_LEN+2];
    label_t *l;

    strncpyZ(buf, name, TOK_NAME_LEN);
    l = LBL_Search(buf);
    if (l) {
        *valp = l->val;
        return 0;
    }
    strcpy(expr_err_name,buf);
    return -1;
}


val_t  Expr(char const**    sp)
{
    int         n;
    val_t       val;
    char const* s;

    s = *sp;
    n = StrExpr(s, &s, &val);
    if (n) {
        static char const* const msg[] = {
            "",
            "�z�肵�Ă��Ȃ�����",
            "���̊��ʂ����Ă��Ȃ�",
            "����0�Ŋ��낤�Ƃ���",
            "�����ɖ���`�̖��O������",
        };
        if (n == 4)
            srcl_error("%s::%s\n", msg[n],expr_err_name);
        else
            srcl_error("%s\n", msg[n]);
    }
    *sp = s;
    return val;
}



/** ������ *sp �� ','�ŋ�؂�ꂽ���ڂ̐��𐔂���
 */
int CountArg(char const** sp, int md)
{
    char const* s;
    int         i;

    s = *sp;
    for (i = 0;;) {
        s = StrSkipSpc(s);
        if (*s == ';' || *s == 0 || *s == '\n')
            break;
        if (*s == '"') {    /* ������́A1������1���ڂƂ��ăJ�E���g���� */
            ++s;
            for (;;) {
                if (*s == 0) {
                    break;
                }
                if (*s == '"') {
                    s++;
                    break;
                }
                GetCh(&s, md);
                i++;
            }
        } else {
            val_t val = 0;
            StrExpr(s, &s, &val);
            i++;
        }
        s = StrSkipSpc(s);
        if (*s != ',')
            break;
        s++;
    }
    --s;
    *sp = s;
    return i;
}



/** ������ *sp �� ','�ŋ�؂�ꂽ���l�� num �Aimm[] �Ɏ擾
 */
void GetArg(char const** sp, val_t imm[], int num)
{
    char const* s;
    int         i;

    s = *sp;
    for (i = 0; i < num; i++) {
        imm[i] = 0;
    }
    for (i = 0; i < num; i++) {
        s = StrSkipSpc(s);
        if (*s == ';' || *s == 0 || *s == '\n')
            break;
        imm[i] = Expr(&s);
        s = StrSkipSpc(s);
        if (*s != ',' && i < num-1)
            srcl_error("�����̐������Ȃ�(%d�̂͂���%d�����Ȃ�)\n", num, i);
        s++;
    }
    if (i >= num && s[-1] == ',') {
        srcl_error("�����̐�������(%d�ȏ゠�邩��)\n", num+1);
    }
    *sp = s;
}


/*---------------------------------------*/
void autoalign2(void)
{
    switch (autoAlignMode) {
    case 0:
        return;
    case 1:
    case 2:
    case 3:
        g_adr = (g_adr + 1) & ~1;
        return;
    }
}


void autoalign4(void)
{
    switch (autoAlignMode) {
    case 0:
        return;
    case 1:
        g_adr = (g_adr + 1) & ~1;
        return;
    case 2:
    case 3:
        g_adr = (g_adr + 3) & ~3;
        return;
    }
}


void autoalign8(void)
{
    switch (autoAlignMode) {
    case 0:
        return;
    case 1:
        g_adr = (g_adr + 1) & ~1;
        return;
    case 2:
        g_adr = (g_adr + 3) & ~3;
        return;
    case 3:
        g_adr = (g_adr + 7) & ~7;
        return;
    }
}


void putOne(val_t val, int md)
{
    if (md == 0) {
        POKEB(&g_mem[g_adr], (uint8_t)val);
        g_adr++;
        if (val < -128 || val > 255)
            srcl_error("�l�� 8�r�b�g�����͈̔͂𒴂��Ă���(" S_FMT_X ")\n", val);
    } else if (md == 1) {
        if (endianMode == 0)
            POKEiW(&g_mem[g_adr], val);
        else
            POKEmW(&g_mem[g_adr], val);
        g_adr += 2;
        if (val < -32768 || val > 0xFFFF)
            srcl_error("�l��16�r�b�g�����͈̔͂𒴂��Ă���(" S_FMT_X ")\n", val);
    } else if (md == 2) {
        if (endianMode == 0)
            POKEiD(&g_mem[g_adr], val);
        else
            POKEmD(&g_mem[g_adr], val);
        g_adr += 4;
        if (val < -(val_t)2147483648 || val > (val_t)0xFFFFFFFF)
            srcl_error("�l��32�r�b�g�����͈̔͂𒴂��Ă���(" S_FMT_X ")\n", val);
    } else if (md == 3) {
        if (endianMode == 0)
            POKEiQ(&g_mem[g_adr], val);
        else
            POKEmQ(&g_mem[g_adr], val);
        g_adr += 8;
    }
}


void gen_dc(char const** sp, int md)
{
    char const* s;
    val_t       val;

    if (md == 1)
        autoalign2();
    else if (md == 2)
        autoalign4();
    else if (md == 3)
        autoalign8();

    s = *sp;
    for (;;) {
        s = StrSkipSpc(s);
        if (*s == ';' || *s == 0 || *s == '\n')
            break;
        if (*s == '"') {    /* ������́A1������1���ڂƂ��ăJ�E���g���� */
            ++s;
            for (;;) {
                if (*s == 0) {
                    break;
                }
                if (*s == '"') {
                    s++;
                    break;
                }
                val = GetCh(&s, md);
                putOne(val,md);
            }
        } else {
            val = Expr(&s);
            putOne(val,md);
        }
        s = StrSkipSpc(s);
        if (*s != ',')
            break;
        s++;
    }
    *sp = s;
}



void pass1_dc(char const** sp, int md)
{
    int     n;

    n = CountArg(sp, md);   // �����̐��𐔂���
    if (n == 0) {
        srcl_error("dc �Ɉ������Ȃ�.\n");
    } else {
        switch (md) {
        case 0:
            g_adr += n;
            break;
        case 1:
            autoalign2();
            g_adr += n*2;
            break;
        case 2:
            autoalign4();
            g_adr += n*4;
            break;
        case 3:
            autoalign8();
            g_adr += n*8;
            break;
        }
    }
}




void gen_ds(val_t val, int md)
{
    if (val < 0) {
        srcl_error("ds �̈����̒l����(���̖��߂𖳎�).\n");
    } else {
        switch (md) {
        case 0:
            g_adr += (ptrdiff_t)val;
            break;
        case 1:
            autoalign2();
            g_adr += (ptrdiff_t)(val*2);
            break;
        case 2:
            autoalign4();
            g_adr += (ptrdiff_t)(val*4);
            break;
        case 8:
            autoalign8();
            g_adr += (ptrdiff_t)(val*8);
            break;
        }
    }
}


void gen_align(val_t n)
{
    if (n < 1 || n > 0x100000) {
        srcl_error("align �̈����� 0�ȉ��� 0x100001 �ȏオ�w�肳��Ă���\n");
        n = 1;
    }
    g_adr = (ptrdiff_t)(((g_adr + n - 1) / n) * n);
}


void gen_autoAlign(int n)
{
    if (n == 0)
        autoAlignMode = 0;
    else if (n == 2)
        autoAlignMode = 1;
    else if (n == 4)
        autoAlignMode = 2;
    else if (n == 8)
        autoAlignMode = 3;
    else
        srcl_error("auto_align �̈�������������(%d)\n", n);
}


void gen_org(val_t val)
{
    if (g_adr > val) {
        srcl_error("org �̈��������݂̃A�h���X��菬����(���̎w��𖳎����܂�)\n");
    } else {
        g_adr = (ptrdiff_t)val;
    }
}


void gen_endian(int md)
{
    endianMode = md;
}



/*-------------------------------*/
static int striMode = 0;
static int striOpt  = 1;        /* bit 0:�s���󔒂��폜1:����/0:���Ȃ� */
                                /* bit 1:���s���폜 1:����/0:���Ȃ� */
                                /* bit 2:��s���폜 1:����/0:���Ȃ� */
                                /* bit 3:�S�p�󔒂��Ώ� 1:����/0:���Ȃ� */


void GenStri(char const* s, int passNo)
{
    int  c;

    if (striOpt & 1) {
        if (striOpt & 8)
            s = StrSkipSpc2(s);
        else
            s = StrSkipSpc(s);
    }
    if (striOpt & 4) {
        if (*s == '\n')
            return;
    }
    while (*s) {
        if (*s == '\n' && (striOpt & 2)) {
            s++;
            continue;
        }
        c = *s;
        if (MBS_ISLEAD(c)) {
            unsigned l = mbs_len1(s);
           if (passNo == 2) {
                memcpy(&g_mem[g_adr], s, l);
            }
            g_adr += l;
            s     += l;
        } else {
            if (passNo == 2) {
                POKEB(&g_mem[g_adr], (uint8_t)c);
            }
            ++g_adr;
            ++s;
        }
    }
    if (passNo == 2) {
        POKEB(&g_mem[g_adr], 0);
    }
}


void GenStriEnd(int passNo)
{
    if (striMode) {
        if (passNo == 2) {
            POKEB(&g_mem[g_adr], 0);
            
        }
        g_adr++;
    }
    striMode = 0;
}


/*-------------------------------*/

int Pass_Line(int passNo, char const* s0)
{
    val_t       imm[2];
    int         c;
    label_t*    cl = NULL;
    char const* s;

    s = StrSkipSpc(s0);
    if (*s == 0 || *s == ';')
        return 0;
    s = GetName(tok_name, s);
    s = StrSkipSpc(s);
    if (*s == ':' && tok_name[0]) {
        if (passNo == 1)
            cl = LBL_Add(tok_name, g_adr, 0);
        s++;
        s = StrSkipSpc(s);
        s = GetName(tok_name, s);
        s = StrSkipSpc(s);
        GenStriEnd(passNo);
    }
    if (tok_name[0] == 0) {
        if (*s == 0 || *s == ';')
            return 0;
        if (striMode) {
            GenStri(s0, passNo);
        } else {
            srcl_error("���߂̈ʒu�ɁA���O�ȊO�̂��̂�����::%s\n",s);
        }
        return 0;
    }
    c = odr_search(tok_name);
    if (c == ODR_NON && striMode) {
        GenStri(s0, passNo);
        return 0;
    } else {
        GenStriEnd(passNo);
    }
    switch (c) {
    case ODR_NON:
        srcl_error("���߂̈ʒu�ɁA���߈ȊO�̖��O������::%s\n",tok_name);
        return 0;
    case ODR_REM:
        return 0;
    case ODR_EQU:
        GetArg(&s, imm, 1);
        if (passNo == 1) {
            if (cl)
                cl->val = imm[0];
            else
                srcl_error("equ��`�Ń��x�����w�肳��Ă��Ȃ�\n");
        }
        break;
    case ODR_DC_B:
        if (passNo == 1)
            pass1_dc(&s, 0);
        else
            gen_dc(&s, 0);
        break;
    case ODR_DC_W:
        if (passNo == 1)
            pass1_dc(&s, 1);
        else
            gen_dc(&s, 1);
        break;
    case ODR_DC_L:
        if (passNo == 1)
            pass1_dc(&s, 2);
        else
            gen_dc(&s, 2);
        break;
    case ODR_DC_Q:
        if (passNo == 1)
            pass1_dc(&s, 3);
        else
            gen_dc(&s, 3);
        break;
    case ODR_DS_B:
        GetArg(&s, imm, 1);
        gen_ds(imm[0], 0);
        break;
    case ODR_DS_W:
        GetArg(&s, imm, 1);
        gen_ds(imm[0], 1);
        break;
    case ODR_DS_L:
        GetArg(&s, imm, 1);
        gen_ds(imm[0], 2);
        break;
    case ODR_DS_Q:
        GetArg(&s, imm, 1);
        gen_ds(imm[0], 3);
        break;
    case ODR_ORG:
        GetArg(&s, imm, 1);
        gen_org(imm[0]);
        break;
    case ODR_EVEN:
        gen_align(2);
        break;
    case ODR_ALIGN:
        GetArg(&s, imm, 1);
        gen_align(imm[0]);
        break;
    case ODR_AUTOALIGN:
        GetArg(&s, imm, 1);
        gen_autoAlign((int)imm[0]);
        break;
    case ODR_BIG_ENDIAN:
        gen_endian(1);
        break;
    case ODR_LITTLE_ENDIAN:
        gen_endian(0);
        break;
    case ODR_STRI:
        striMode = 1;
        break;
    case ODR_STRI_OPT:
        GetArg(&s, imm, 1);
        striOpt  = (int)imm[0];
        break;
    case ODR_XDEF:
        {
            char name[TOK_NAME_LEN+2];
            for (; ;) {
                s = StrSkipSpc(s);
                s = GetName(name, s);
                s = StrSkipSpc(s);
                if (passNo == 1) {
                    if (name[0]) {
                        LBL_Add(name, 0, 1);
                    } else {
                        srcl_error("xdef ��`�Ƀ��x�������Ȃ�\n");
                    }
                }
                if (*s != ',')
                    break;
                s++;
            }
        }
        break;
    case ODR_END:
        return 1;
    default:
        srcl_error("PRGERR:�m��Ȃ�����(%x)\n", c);
        return 0;
    }
    s = StrSkipSpc(s);
    if (s && *s && *s != ';') {
        srcl_error("�s���ɗ]���ȕ����񂪂���... %s\n", s);
    }
    return 0;
}




void Pass(int passNo, srcl_t const* sr)
{
    srcl_t const* p;

    srcl_errorSw(passNo - 1);
    g_adr   = 0;
    for (p = sr; p; p = p->link) {
        srcl_cur = p;
        g_adrLinTop = g_adr;
        if (Pass_Line(passNo, p->s)) {
            /* end �����ꂽ�Ƃ��I�� */
            break;
        }
    }
    GenStriEnd(passNo);
}


/*-------------------------------*/
static FILE  *gen_hdr_fp = NULL;

void    gen_hdr_sub(void *a)
{
    label_t *l = a;

    if (l->typ == 3)
        fprintf(gen_hdr_fp, "#define %-14s\t0x" S_FMT_08X "\t/* " S_FMT_10D " */\n", l->name, l->val, l->val);
}


void    gen_hdr(char *hdrname)
{
    gen_hdr_fp = fopenE(hdrname, "wt");
    TREE_DoAll(LBL_tree, gen_hdr_sub);
    fclose(gen_hdr_fp);
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void FilnInit(void)
{
    if (Filn_Init() == NULL) {      /* �\�[�X���̓��[�`���̏����� */
        err_exit("������������Ȃ�\n");
    }
    Filn->opt_delspc    = 0;
    Filn->opt_mot_doll  = 1;
    Filn->opt_oct       = 0;
    Filn->opt_orgSrc    = 0;
    Filn->cmt_chr[0]    = ';';
    Filn->cmt_chrTop[0] = '*';
    Filn->opt_kanji     = 0;
}


int main(int argc, char *argv[])
{
    char    oname[FIL_NMSZ];
    int     i;
    char*   p;
    SLIST*  fl;

    mbs_init();
    FilnInit();     /* �\�[�X���̓��[�`���̏����� */

    StrExpr_SetNameChkFunc(GetLblVal4StrExpr);  /* ���֌W�̏��� */
    if (argc < 2)
        Usage();

    odr_init();                 /* ���ߕ\����� */
    LBL_Init();                 /* ���x���\�̏����� */

    /* �R�}���h���C����� */
    for (i = 1; i < argc; i++) {
        p = argv[i];
        if (*p == '-') {
            Opts(p);
        } else if (*p == '@') {
            GetResFile(p+1);
        } else {
            SLIST_Add(&fileListTop, p);
        }
    }

    Filn_ErrReOpen(NULL, stderr);

    if (fileListTop == NULL) {
        err_printf("�t�@�C�������w�肵�Ă�������\n");
        Usage();
    }

    if (outname) {
        strcpy(oname, outname);
    } else {
        strcpy(oname, fileListTop->s);
        FIL_ChgExt(oname, "bin");
    }

    /*-- �t�@�C���Ǎ�. ���ׂẴt�@�C����Ǎ��݁A�}�N�������������̂��I���������ŗ��ߍ��� */
    if (vmsgFlg) {
        err_printf("[LOAD] ");
    }
    if (rulename) {                             /* ���[���t�@�C���w�肪�������Ƃ� */
        if (vmsgFlg) {
            err_printf("\t'%s'",rulename);
        }
        GetSrcList(rulename);
    }
    for (fl = fileListTop; fl; fl = fl->link) { /* ��A�̃t�@�C����A���������̂Ƃ��ď��� */
        if (vmsgFlg) {
            err_printf( "\t'%s'", fl->s);
        }
        GetSrcList(fl->s);
    }
    if (vmsgFlg) {
        err_printf( "\n");
    }
    Filn_Term();                        /* �\�[�X���̓��[�`���̏I�� */

    /* ���� */
    if (vmsgFlg)
        err_printf( "[PASS1]\n");
    Pass(1, srcl);                      /* Pass1:�A�h���X�𓾂� */
    g_mem = callocE(1, g_adr + 0x20);   /* �K�v���������m��. +0x20�͕ی� */
    if (vmsgFlg)
        err_printf( "[PASS2]\n");
    Pass(2, srcl);                      /* Pass2:�o�C�i���𐶐� */
    if (vmsgFlg)
        err_printf( "[SAVE] '%s' (%x)\n", oname, g_adr);
    FIL_SaveE(oname, g_mem, g_adr);     /* �o�C�i���t�@�C�����Z�[�u */
    err_printf( "\n%-14s 0x%x(%d) byte(s)\n", oname, g_adr, g_adr);
    if (hdrname) {
        err_printf( "[header output] %-14s\n", hdrname);
        gen_hdr(hdrname);
    }

    DB err_printf( "[�㏈��]\n");
    /* �㏈�� */
    srcl_free(&srcl);
    odr_term();                         /* ���ߕ\���폜 */

    return 0;
}
