/**
  @file   dcasm.c
  @brief  dc.b, dc.w, dc.l 命令のみのアセンブラ
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
            "  バイナリ・データ生成を目的としたdcディレクティブのみのアセンブラ\n"
            "  ファイル名が複数指定された場合、一連のテキストとして順に読みこむ.\n"
            "  出力ファイル名は最初のファイル名の拡張子を.binにしたもの\n"
            "  -?        このヘルプ\n"
            "  -iDIR     ヘッダファイルの入力ディレクトリの指定\n"
            "  -oFILE    結果を FILE に出力.\n"
            "  -eFILE    エラーを FILE に出力\n"
            "  -b<B|L>   デフォルトのエンディアンをB=ビッグ, L=リトルにする\n"
            "  -a<0|2|4|8> アライメント最大バイト数(0:調整無) ex)-a2ならdc.l,dc.qも2byte単位\n"
            "  -rFILE    一番目に読込むファイルを指定(ルール/マクロ定義の記述ファイルを想定)\n"
            "  -dNAME[=STR]  #define名定義\n"
            "  -uNAME    #undefする\n"
            "  -hFILE    xdefされたラベルを#define形式でファイルに出力する\n"
            "  -y        文字列中, C言語の\\エスケープシーケンスを有効  -y- 無効\n"
            "  -s        ;をコメント開始文字でなく複文用のセパレータとして扱う\n"
            "  -m        命令(文法)ヘルプを標準エラー出力\n"
            "  -cUTF8    入力テキストをutf8として解釈.\n"
            "  -cMBC     入力テキストをマルチバイト文字(SJIS)として解釈.\n"
            "  -v[-]     途中経過メッセージを表示する -v-しない\n"
            "  @FILE     レスポンス・ファイル指定\n"
    );
    exit(1);
}


void ManHelp(void)
{
    err_printf(
            "dcasm v0.50 の主な仕様\n"
            "10進数数値       0..9か_で構成される文字列. _は無視.\n"
            "16進数数値       C言語風0x0, インテル風0h, モトローラ風$0\n"
            " 2進数数値       C言語風拡張0b011、インテル風 011h 表記. モトローラ風表記%011\n"
            " 8進数数値       指定できない\n"
            "演算子(Cに同じ)  () + - ! ~   * / % + - << >>  > >= < <= == != & ^ | && ||\n"
            "行頭アドレス値   $ または 単項*\n"
            "コメント         ; か * か // か rem で始まり行末までか /* */ で囲まれた範囲\n"
            "LBL:             ラベル LBL を定義\n"
            "LBL: equ N       ラベル LBL の値を N にする\n"
            "xdef LBL         ラベル LBL を .h ファイルに#define定義で出力.\n"
            "dc.<b|w|l|q> ... 1バイト/2バイト/4バイト整数列のデータ生成\n"
            "db dw dd dq      〃\n"
            "ds.<b|w|l> N     Nバイトの領域を確保(0埋め)\n"
            "org  N           アドレスを Nにする(隙間は0埋め)\n"
            "even             アドレスが偶数になるよう調整(隙間は0埋め)\n"
            "align N          アドレスがNバイト単位になるよう調整\n"
            "auto_align 2|4|8 アライメント最大バイト数. ex)2 なら dc.l, dc.q でも2バイト単位\n"
            "big_endian       ビッグ・エンディアンにする\n"
            "little_endian    リトル・エンディアンにする\n"
            "end              ソースの終わり(なくてもよい)\n"
            "#if,#define,#macro,#rept,#ipr 等の条件アセンブルやマクロ命令あり\n"
        );
    exit(1);
}


/* ------------------------------------------------------------------------ */
SLIST*      fileListTop = NULL;
char*       outname     = NULL;
char*       errname     = NULL;
char*       rulename    = NULL;
char*       hdrname     = NULL;
static int  cescMode;           /* cの \ エスケープシーケンスを用いるか */
static int  endianMode;         /* エンディアンの指定                   */
static int  autoAlignMode = 3;  /* dc.w, dc.l, dc.q での、自動アライメントの方法 */
static int  vmsgFlg;
static int  useSep;
static int  mbcMode = 0;


/** オプションの処理 
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
        if (errname && errname[0]) {    /* エラー出力の準備 */
            if (freopen(errname,"at",stderr) == NULL)
                err_exit("%s をオープンできません\n", errname);
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

/** ソース入力でエラーがあったとき
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


/** ソース行をメモリに貯めるためリストにする。
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


/** ソース行の開放
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


/** セパレータ';' で区切られた一行を複数行に分ける 
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

/** マクロ展開済みのソースを作成する 
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
        s = Filn_Gets();    /* マクロ展開後のソースを取得 */
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
            p = LinSep(s);  /* 実はpはlinbufをさしている */
            while (p) {
                srcl_add(&srcl, fnm, l, p);
                p = LinSep(NULL);
            }
        }
        freeE(s);           /* 取得したメモリを開放 */
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



/** TREE ルーチンで、新しい要素を造るときに呼ばれる 
 */
static void *LBL_New(label_t const* t)
{
    label_t*    p;
    p = callocE(1,sizeof(label_t));
    memcpy(p, t, sizeof(label_t));
    p->name = strdupE(t->name);
    return p;
}



/** ラベルTREE ルーチンで、メモリ開放のときに呼ばれる 
 */
static void LBL_Del(void *ff)
{
    freeE(ff);
}



/** ラベルTREE ルーチンで、用いられる比較条件 
 */
static int  LBL_Cmp(label_t const* f1, label_t const* f2)
{
    return strcmp(f1->name, f2->name);
}



/** ラベルTREE を初期化 
 */
void    LBL_Init(void)
{
    LBL_tree = TREE_Make((TREE_NEW) LBL_New, (TREE_DEL) LBL_Del, (TREE_CMP) LBL_Cmp, (TREE_MALLOC) mallocE, (TREE_FREE) freeE);
}



/** ラベルTREE を開放 
 */
void LBL_Term(void)
{
    TREE_Clear(LBL_tree);
}



/** 現在の名前がラベルtreeに登録されたラベルかどうか探す
 */
label_t *LBL_Search(char const* lbl_name)
{
    label_t t;

    memset (&t, 0, sizeof(label_t));
    t.name = lbl_name;
    if (t.name == NULL) {
        srcl_error("ラベル登録でメモリがたりない\n");
        exit(1);
    }
    return TREE_Search(LBL_tree, &t);
}



/** ラベル(名前)を木に登録する 
 */
label_t *LBL_Add(char const* lbl_name, val_t val, int typ)
{
    label_t     t;
    label_t*    p;

    memset (&t, 0, sizeof(label_t));
    t.name  = lbl_name;
    //t.line  = TXT1_line;
    p = TREE_Insert(LBL_tree, &t);
    if (LBL_tree->flag == 0) {          /* 新規登録でなかった */
        if (p->typ == 1 || typ == 1) {
            p->typ = 3;
            if (typ == 0)
                p->val = val;
        } else {
            srcl_error("%s が多重定義かも\n", lbl_name);
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

/** TREE ルーチンで、新しい要素を造るときに呼ばれる
 */
static void*    odr_new(odr_t const* t)
{
    odr_t*  p;

    p = callocE(1,sizeof(odr_t));
    memcpy(p, t, sizeof(odr_t));
    p->name = strdupE(t->name);
    return p;
}

/** TREE ルーチンで、メモリ開放のときに呼ばれる
 */
static void odr_del(void *ff)
{
    freeE(ff);
}

/** TREE ルーチンで、用いられる比較条件
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

/** ラベル(名前)を木に登録する
 */
odr_t*  odr_add(char const* name, int id)
{
    odr_t   t;
    odr_t*  p;

    memset (&t, 0, sizeof(odr_t));
    t.name  = name;
    p = TREE_Insert(odr_tree, &t);
    if (odr_tree->flag == 0) {          /* 新規登録でなかった */
        /* グローバル宣言されたラベルのとき */
        err_printf( "PRGERR:%s命令定義が多重！\n", name);
        p = NULL;
    } else {
        p->id = id;
    }
    return p;
}


void    odr_init(void)
{
    /* TREE を初期化 */
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
    /* TREE を開放 */
    TREE_Clear(odr_tree);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define IsLblTop(c)     (isalpha(c) || (c) == '_' || *s == '@' || *s == '.')
#define TOK_NAME_LEN    1024

static int              passMode;
static ptrdiff_t        g_adr;          /* アドレス */
ptrdiff_t               g_adrLinTop;    /* 行頭アドレス. StrExprから参照される */
static unsigned char    *g_mem;         /* メモリ */
static char             tok_name[TOK_NAME_LEN+2];


char const* GetName(char *buf, char const* s)
{
    unsigned n;

    n = 0;
    if (IsLblTop(*s) || MBS_ISLEAD(*s)) {     //行頭に空白のあるラベル定義チェック
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
                srcl_error("\\の直後にコントロールコードがある\n");
            }
            ch = c;
        }
    } else if (MBS_ISLEAD(ch) && md > 0) {
        --s;
        ch = mbs_getc(&s);      // utf16のサロゲートペアは未対応

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
            "想定していない式だ",
            "式の括弧が閉じていない",
            "式で0で割ろうとした",
            "式中に未定義の名前がある",
        };
        if (n == 4)
            srcl_error("%s::%s\n", msg[n],expr_err_name);
        else
            srcl_error("%s\n", msg[n]);
    }
    *sp = s;
    return val;
}



/** 文字列 *sp の ','で区切られた項目の数を数える
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
        if (*s == '"') {    /* 文字列は、1文字を1項目としてカウントする */
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



/** 文字列 *sp の ','で区切られた数値を num 個、imm[] に取得
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
            srcl_error("引数の数が少ない(%d個のはずが%d個しかない)\n", num, i);
        s++;
    }
    if (i >= num && s[-1] == ',') {
        srcl_error("引数の数が多い(%d個以上あるかも)\n", num+1);
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
            srcl_error("値が 8ビット整数の範囲を超えている(" S_FMT_X ")\n", val);
    } else if (md == 1) {
        if (endianMode == 0)
            POKEiW(&g_mem[g_adr], val);
        else
            POKEmW(&g_mem[g_adr], val);
        g_adr += 2;
        if (val < -32768 || val > 0xFFFF)
            srcl_error("値が16ビット整数の範囲を超えている(" S_FMT_X ")\n", val);
    } else if (md == 2) {
        if (endianMode == 0)
            POKEiD(&g_mem[g_adr], val);
        else
            POKEmD(&g_mem[g_adr], val);
        g_adr += 4;
        if (val < -(val_t)2147483648 || val > (val_t)0xFFFFFFFF)
            srcl_error("値が32ビット整数の範囲を超えている(" S_FMT_X ")\n", val);
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
        if (*s == '"') {    /* 文字列は、1文字を1項目としてカウントする */
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

    n = CountArg(sp, md);   // 引数の数を数える
    if (n == 0) {
        srcl_error("dc に引数がない.\n");
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
        srcl_error("ds の引数の値が負(この命令を無視).\n");
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
        srcl_error("align の引数が 0以下か 0x100001 以上が指定されている\n");
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
        srcl_error("auto_align の引数がおかしい(%d)\n", n);
}


void gen_org(val_t val)
{
    if (g_adr > val) {
        srcl_error("org の引数が現在のアドレスより小さい(この指定を無視します)\n");
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
static int striOpt  = 1;        /* bit 0:行頭空白を削除1:する/0:しない */
                                /* bit 1:改行を削除 1:する/0:しない */
                                /* bit 2:空行を削除 1:する/0:しない */
                                /* bit 3:全角空白も対象 1:する/0:しない */


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
            srcl_error("命令の位置に、名前以外のものがある::%s\n",s);
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
        srcl_error("命令の位置に、命令以外の名前がある::%s\n",tok_name);
        return 0;
    case ODR_REM:
        return 0;
    case ODR_EQU:
        GetArg(&s, imm, 1);
        if (passNo == 1) {
            if (cl)
                cl->val = imm[0];
            else
                srcl_error("equ定義でラベルが指定されていない\n");
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
                        srcl_error("xdef 定義にラベル名がない\n");
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
        srcl_error("PRGERR:知らない命令(%x)\n", c);
        return 0;
    }
    s = StrSkipSpc(s);
    if (s && *s && *s != ';') {
        srcl_error("行末に余分な文字列がある... %s\n", s);
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
            /* end が現れたとき終了 */
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
    if (Filn_Init() == NULL) {      /* ソース入力ルーチンの初期化 */
        err_exit("メモリがたりない\n");
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
    FilnInit();     /* ソース入力ルーチンの初期化 */

    StrExpr_SetNameChkFunc(GetLblVal4StrExpr);  /* 式関係の準備 */
    if (argc < 2)
        Usage();

    odr_init();                 /* 命令表を作る */
    LBL_Init();                 /* ラベル表の初期化 */

    /* コマンドライン解析 */
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
        err_printf("ファイル名を指定してください\n");
        Usage();
    }

    if (outname) {
        strcpy(oname, outname);
    } else {
        strcpy(oname, fileListTop->s);
        FIL_ChgExt(oname, "bin");
    }

    /*-- ファイル読込. すべてのファイルを読込み、マクロ処理したものをオンメモリで溜め込む */
    if (vmsgFlg) {
        err_printf("[LOAD] ");
    }
    if (rulename) {                             /* ルールファイル指定があったとき */
        if (vmsgFlg) {
            err_printf("\t'%s'",rulename);
        }
        GetSrcList(rulename);
    }
    for (fl = fileListTop; fl; fl = fl->link) { /* 一連のファイルを連続したものとして処理 */
        if (vmsgFlg) {
            err_printf( "\t'%s'", fl->s);
        }
        GetSrcList(fl->s);
    }
    if (vmsgFlg) {
        err_printf( "\n");
    }
    Filn_Term();                        /* ソース入力ルーチンの終了 */

    /* 生成 */
    if (vmsgFlg)
        err_printf( "[PASS1]\n");
    Pass(1, srcl);                      /* Pass1:アドレスを得る */
    g_mem = callocE(1, g_adr + 0x20);   /* 必要分メモリ確保. +0x20は保険 */
    if (vmsgFlg)
        err_printf( "[PASS2]\n");
    Pass(2, srcl);                      /* Pass2:バイナリを生成 */
    if (vmsgFlg)
        err_printf( "[SAVE] '%s' (%x)\n", oname, g_adr);
    FIL_SaveE(oname, g_mem, g_adr);     /* バイナリファイルをセーブ */
    err_printf( "\n%-14s 0x%x(%d) byte(s)\n", oname, g_adr, g_adr);
    if (hdrname) {
        err_printf( "[header output] %-14s\n", hdrname);
        gen_hdr(hdrname);
    }

    DB err_printf( "[後処理]\n");
    /* 後処理 */
    srcl_free(&srcl);
    odr_term();                         /* 命令表を削除 */

    return 0;
}
