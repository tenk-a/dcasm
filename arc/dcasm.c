/*
	dc.b, dc.w, dc.l 命令のみのアセンブラ
 */

#include <stdarg.h>
#include "subr.h"
#include "tree.h"
#include "filn.h"


void Usage(void)
{
	err_printf(
			"usage> dcasm [-opt] filename(s)   // v0.40 " __DATE__ "  " __TIME__ "  by tenk\n"
			"  バイナリ・データ生成を目的としたdcディレクティブのみのアセンブラ\n"
			"  ファイル名が複数指定された場合、一連のテキストとして順に読みこむ.\n"
			"  出力ファイル名は最初のファイル名の拡張子を.binにしたもの\n"
			"  -?        このヘルプ\n"
			"  -iDIR     ヘッダファイルの入力ディレクトリの指定\n"
			"  -oFILE    結果を FILE に出力.\n"
			"  -eFILE    エラーを FILE に出力\n"
			"  -b<B|L>   デフォルトのエンディアンをB=ビッグ, L=リトルにする\n"
			"  -a<0|2|4> 2:dc.w,dc.lでアドレスを偶数, 4:dc.w偶数 dc.l四の倍数 0:調整無\n"
			"  -rFILE    一番目に読込むファイルを指定(ルール/マクロ定義の記述ファイルを想定)\n"
			"  -dNAME[=STR]  #define名定義\n"
			"  -uNAME    #undefする\n"
			"  -hFILE    xdefされたラベルを#define形式でファイルに出力する\n"
			"  -y        文字列中, C言語の\\エスケープシーケンスを有効  -y- 無効\n"
			"  -s        ;をコメント開始文字でなく複文用のセパレータとして扱う\n"
			"  -m        命令(文法)ヘルプを標準エラー出力\n"
			"  -v[-]     途中経過メッセージを表示する -v-しない\n"
			"  @FILE     レスポンス・ファイル指定\n"
	);
	exit(1);
}


void ManHelp(void)
{
	err_printf(
			"dcasm v0.40 の主な仕様\n"
			"10進数数値      0..9か_で構成される文字列. _は無視.\n"
			"16進数数値      C言語風0x0, インテル風0h, モトローラ風$0\n"
			" 2進数数値      C言語風拡張0b011、インテル風 011h 表記. モトローラ風表記%011\n"
			" 8進数数値      指定できない\n"
			"演算子(Cに同じ) () + - ! ~   * / % + - << >>  > >= < <= == != & ^ | && ||\n"
			"行頭アドレス値   $ または 単項*\n"
			"コメント        ; か * か // か rem で始まり行末までか /* */ で囲まれた範囲\n"
			"LBL:            ラベル LBL を定義\n"
			"LBL: equ N      ラベル LBL の値を N にする\n"
			"xdef LBL        ラベル LBL を .h ファイルに#define定義で出力.\n"
			"dc.<b|w|l> ...  1バイト/2バイト/4バイト整数列のデータ生成\n"
			"db dw dd        〃\n"
			"ds.<b|w|l> N    Nバイトの領域を確保(0埋め)\n"
			"org  N          アドレスを Nにする(隙間は0埋め)\n"
			"even            アドレスが偶数になるよう調整(隙間は0埋め)\n"
			"align N         アドレスがNバイト単位になるよう調整\n"
			"auto_align 2|4  2:dc.w,dc.lでアドレスを偶数, 4:dc.w 偶数 dc.l 4の倍数\n"
			"big_endian      ビッグ・エンディアンにする\n"
			"little_endian   リトル・エンディアンにする\n"
			"end             ソースの終わり(なくてもよい)\n"
			"#if,#define,#macro,#rept,#ipr 等の条件アセンブルやマクロ命令あり\n"
		);
	exit(1);
}

/* ------------------------------------------------------------------------ */
SLIST	*fileListTop = NULL;
char	*outname  = NULL;
char	*errname  = NULL;
char	*rulename = NULL;
char	*hdrname  = NULL;
static int cescMode;		/* cの \ エスケープシーケンスを用いるか */
static int endianMode;		/* エンディアンの指定					*/
static int autoAlignMode;	/* dc.w, dc.l での、自動アライメントの方法 */
static int vmsgFlg;
static int useSep;



int Opts(char *a)
{
	/* オプションの処理 */
	char *p;
	int c;

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
		if (errname && errname[0]) {	/* エラー出力の準備 */
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



void GetResFile(char *name)
{
	static char buf[1024*16];
	char *p;

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

typedef struct srcl_t {
	struct srcl_t *link;
	char *fname;
	int  line;
	char *s;
} srcl_t;

srcl_t *srcl = NULL, *srcl_cur;
int    srcl_errDisp;
static char linbuf[0x10000];

void srcl_errorSw(int n)
{
	srcl_errDisp = n;
}

void srcl_error(char *fmt, ...)
{
	// ソース入力でエラーがあったとき
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


void srcl_add(srcl_t **p0, char *fnm, int l, char *s)
{
	// ソース行をメモリに貯めるためリストにする。
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


void srcl_free(srcl_t **p0)
{
	// ソース行の開放
	srcl_t *p, *q;

	for (p = *p0; p; p = q) {
		q = p->link;
		freeE(p->s);
		freeE(p->fname);
		freeE(p);
	}
}



char *LinSep(char *st)
{
	/* セパレータ; で区切られた一行を複数行に分ける */
	/*static char linbuf[0x10000];*/
	static char *s;
	char *d = linbuf;
	int k;

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
		} else if (ISKANJI((Uint8)*s) && s[1]) {
			*d++ = *s++;
			*d++ = *s++;
		} else {
			*d++ = *s++;
		}
	}
	*d++ = '\n';
	*d = 0;
	return linbuf;
}

void GetSrcList(char *name)
{
	/* マクロ展開済みのソースを作成する */
	char *s;
	char *fnm;
	int  l;

	if (Filn_Open(name)) {
		exit(1);
	}
	for (; ;) {
		s = Filn_Gets();	/* マクロ展開後のソースを取得 */
		if (s == NULL)
			break;
		Filn_GetFnameLine(&fnm, &l);
		if (useSep == 0) {
			strcpy(linbuf, s);
			StrDelLf(linbuf);
			strcat(linbuf, "\n");
			srcl_add(&srcl, fnm, l, linbuf);
		} else {
			char *p;
			p = LinSep(s);	/* 実はpはlinbufをさしている */
			while (p) {
				srcl_add(&srcl, fnm, l, p);
				p = LinSep(NULL);
			}
		}
		freeE(s);			/* 取得したメモリを開放 */
	}
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

typedef struct label_t {
	char 			*name;
	struct label_t	*link;
	long 			val;
	int				typ;
} label_t;

label_t *label_top, *label_cur;

static TREE	*LBL_tree;



static void *LBL_New(label_t *t)
	/* TREE ルーチンで、新しい要素を造るときに呼ばれる */
{
	label_t *p;
	p = callocE(1,sizeof(label_t));
	memcpy(p, t, sizeof(label_t));
	p->name = strdupE(t->name);
	return p;
}



static void LBL_Del(void *ff)
	/* TREE ルーチンで、メモリ開放のときに呼ばれる */
{
	freeE(ff);
}



static int  LBL_Cmp(label_t *f1, label_t *f2)
	/* TREE ルーチンで、用いられる比較条件 */
{
	return strcmp(f1->name, f2->name);
}



void 	LBL_Init(void)
{
	/* TREE を初期化 */
	LBL_tree = TREE_Make((TREE_NEW) LBL_New, (TREE_DEL) LBL_Del, (TREE_CMP) LBL_Cmp, (TREE_MALLOC) mallocE, (TREE_FREE) freeE);
}



void LBL_Term(void)
{
	/* TREE を開放 */
	TREE_Clear(LBL_tree);
}



label_t *LBL_Search(char *lbl_name)
	/* 現在の名前が木に登録されたラベルかどうか探す */
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



label_t	*LBL_Add(char *lbl_name, int val, int typ)
	/* ラベル(名前)を木に登録する */
{
	label_t t, *p;

	memset (&t, 0, sizeof(label_t));
	t.name  = lbl_name;
	//t.line  = TXT1_line;
	p = TREE_Insert(LBL_tree, &t);
	if (LBL_tree->flag == 0) {			/* 新規登録でなかった */
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
	ODR_DS_B,
	ODR_DS_W,
	ODR_DS_L,
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
	char 			*name;
	int 			id;
} odr_t;

static TREE	*odr_tree;

static void *odr_new(odr_t *t)
	/* TREE ルーチンで、新しい要素を造るときに呼ばれる */
{
	odr_t *p;

	p = callocE(1,sizeof(odr_t));
	memcpy(p, t, sizeof(odr_t));
	p->name = strdupE(t->name);
	return p;
}

static void odr_del(void *ff)
	/* TREE ルーチンで、メモリ開放のときに呼ばれる */
{
	freeE(ff);
}

static int  odr_cmp(odr_t *f1, odr_t *f2)
	/* TREE ルーチンで、用いられる比較条件 */
{
	return strcmp(f1->name, f2->name);
}

int odr_search(char *name)
{
	odr_t t,*p;

	memset (&t, 0, sizeof(odr_t));
	t.name = name;
	p = TREE_Search(odr_tree, &t);
	if (p)
		return p->id;
	return ODR_NON;
}

odr_t	*odr_add(char *name, int id)
	/* ラベル(名前)を木に登録する */
{
	odr_t t, *p;

	memset (&t, 0, sizeof(odr_t));
	t.name  = name;
	p = TREE_Insert(odr_tree, &t);
	if (odr_tree->flag == 0) {			/* 新規登録でなかった */
		/* グローバル宣言されたラベルのとき */
		err_printf( "PRGERR:%s命令定義が多重！\n", name);
		p = NULL;
	} else {
		p->id = id;
	}
	return p;
}


void 	odr_init(void)
{
	/* TREE を初期化 */
	odr_tree = TREE_Make((TREE_NEW) odr_new, (TREE_DEL) odr_del, (TREE_CMP) odr_cmp, (TREE_MALLOC) mallocE, (TREE_FREE) freeE);
	odr_add("org", 			ODR_ORG);
	odr_add("dc.b", 		ODR_DC_B);
	odr_add("dc.w", 		ODR_DC_W);
	odr_add("dc.l", 		ODR_DC_L);
	odr_add("ds.b", 		ODR_DS_B);
	odr_add("ds.w", 		ODR_DS_W);
	odr_add("ds.l", 		ODR_DS_L);
	odr_add("even", 		ODR_EVEN);
	odr_add("align",		ODR_ALIGN);
	odr_add("auto_align",   ODR_AUTOALIGN);
	odr_add("big_endian",   ODR_BIG_ENDIAN);
	odr_add("little_endian",ODR_LITTLE_ENDIAN);
	odr_add("db", 			ODR_DC_B);
	odr_add("dw", 			ODR_DC_W);
	odr_add("dd", 			ODR_DC_L);
	odr_add("end",			ODR_END);
	odr_add("stri",			ODR_STRI);
	odr_add("stri_opt",		ODR_STRI_OPT);
  #if 0
	odr_add("stri_adi",		ODR_STRI_ADI);
  #endif
	odr_add("equ",			ODR_EQU);
	odr_add("xdef",			ODR_XDEF);
	odr_add("rem",			ODR_REM);
}

void odr_term(void)
{
	/* TREE を開放 */
	TREE_Clear(odr_tree);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define	IsLblTop(c)		(isalpha(c) || (c) == '_' || *s == '@' || *s == '.')
#define TOK_NAME_LEN	1024
#define IS_KANJI(c)		(ISKANJI(c))

static int 		passMode;
static int  	g_adr;			/* アドレス */
int  			g_adrLinTop;	/* 行頭アドレス. StrExprから参照される */
static Uint8	*g_mem;			/* メモリ */
static char		tok_name[TOK_NAME_LEN+2];


char *GetName(char *buf, char *s)
{
	int n;

	n = 0;
	if (IsLblTop(*s) || IS_KANJI(*s)) {		//行頭に空白のあるラベル定義チェック
		for (;;) {
			if (IS_KANJI(*s) && s[1]) {
				if (n < TOK_NAME_LEN-1) {
					n += 2;
					*buf++ = *s++;
					*buf++ = *s;
				}
				s++;
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



int  GetCh(char **sp)
{
	char *s;
	UINT c,ch,k,l,i;

	s = *sp;
	ch = *s++;
	if (ch == 0)
		s--;
	if (cescMode && ch == '\\') {
		l = 8;
		c = *s++;
		switch (c) {
		case 'a':	ch = '\a';	break;
		case 'b':	ch = '\b';	break;
		case 'e':	ch = 0x1b;	break;
		case 'f':	ch = '\f';	break;
		case 'n':	ch = '\n';	break;
		case 'N':	ch = 0x0d0a;	break;
		case 'r':	ch = '\r';	break;
		case 't':	ch = '\t';	break;
		case 'v':	ch = '\v';	break;
		case '\\':	ch = '\\';	break;
		case '\'':	ch = '\'';	break;
		case '\"':	ch = '\"';	break;
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
	}
	*sp = s;
	return ch;
}


char expr_err_name[TOK_NAME_LEN+2];

int  GetLblVal4StrExpr(char *name, long *valp)
{
	char buf[TOK_NAME_LEN+2];
	label_t *l;

//printf("%s\n",name);
	strncpyZ(buf, name, TOK_NAME_LEN);
//	strupr(buf);
	l = LBL_Search(buf);
	if (l) {
		*valp = l->val;
//printf("@%d(%x)\n", *valp,*valp);
		return 0;
	}
	strcpy(expr_err_name,buf);
	return -1;
}


long  Expr(char **sp)
{
	int n;
	long val;
	char *s;

	s = *sp;
	n = StrExpr(s, &s, &val);
	if (n) {
		static char *msg[] = {
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



int CountArg(char **sp)
{
	// 文字列 sの ','で区切られた項目の数を数える
	char *s;
	int  i;

	s = *sp;
	for (i = 0;;) {
		s = StrSkipSpc(s);
		if (*s == ';' || *s == 0 || *s == '\n')
			break;
		if (*s == '"') {	/* 文字列は、1文字を1項目としてカウントする */
			++s;
			for (;;) {
				if (*s == 0) {
					break;
				}
				if (*s == '"') {
					s++;
					break;
				}
				GetCh(&s);
				i++;
			}
		} else {
			long val;
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



void GetArg(char **sp, long imm[], int num)
{
	// 文字列 sの ','で区切られた数値を num 個、imm[] に取得
	char *s;
	int i;

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
		g_adr = (g_adr + 3) & ~3;
		return;
	}
}


void putOne(int val, int md)
{
	if (md == 0) {
		POKEB(&g_mem[g_adr], val);
		g_adr++;
		if (val < -128 || val > 255)
			srcl_error("値が 8ビット整数の範囲を超えている(%x)\n", val);
	} else if (md == 1) {
		if (endianMode == 0)
			POKEiW(&g_mem[g_adr], val);
		else
			POKEmW(&g_mem[g_adr], val);
		g_adr += 2;
		if (val < (Sint16)0x8000 || val > 0xFFFF)
			srcl_error("値が16ビット整数の範囲を超えている(%x)\n", val);
	} else {
		if (endianMode == 0)
			POKEiD(&g_mem[g_adr], val);
		else
			POKEmD(&g_mem[g_adr], val);
		g_adr += 4;
	}
}


void gen_dc(char **sp, int md)
{
	char *s;
	long val;

	if (md == 1)
		autoalign2();
	else if (md == 2)
		autoalign4();

	s = *sp;
	for (;;) {
		s = StrSkipSpc(s);
		if (*s == ';' || *s == 0 || *s == '\n')
			break;
		if (*s == '"') {	/* 文字列は、1文字を1項目としてカウントする */
			++s;
			for (;;) {
				if (*s == 0) {
					break;
				}
				if (*s == '"') {
					s++;
					break;
				}
				val = GetCh(&s);
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



void pass1_dc(char **sp, int md)
{
	int n;

	n = CountArg(sp);	// 引数の数を数える
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
		}
	}
}




void gen_ds(int val, int md)
{
	if (val < 0) {
		srcl_error("ds の引数の値が負(この命令を無視).\n");
	} else {
		switch (md) {
		case 0:
			g_adr += val;
			break;
		case 1:
			autoalign2();
			g_adr += val*2;
			break;
		case 2:
			autoalign4();
			g_adr += val*4;
			break;
		}
	}
}


void gen_align(int n)
{
	if (n < 1 || n > 0x100000) {
		srcl_error("align の引数が 0以下か 0x100001 以上が指定されている\n");
		n = 1;
	}
	g_adr = ((g_adr + n - 1) / n) * n;
}


void gen_autoAlign(int n)
{
	if (n == 0)
		autoAlignMode = 0;
	else if (n == 2)
		autoAlignMode = 1;
	else if (n == 4)
		autoAlignMode = 2;
	else
		srcl_error("auto_align の引数がおかしい(%d)\n", n);
}


void gen_org(int val)
{
	if (g_adr > val) {
		srcl_error("org の引数が現在のアドレスより小さい(この指定を無視します)\n");
	} else {
		g_adr = val;
	}
}


void gen_endian(int md)
{
	endianMode = md;
}



/*-------------------------------*/
static int striMode = 0;
static int striOpt  = 1;		/* bit 0:行頭空白を削除1:する/0:しない */
								/* bit 1:改行を削除 1:する/0:しない */
								/* bit 2:空行を削除 1:する/0:しない */
								/* bit 3:全角空白も対象 1:する/0:しない */
static int striAdi  = 0;		/* stri文字列の、きゃらオフセット値(単純な暗号化) */


void GenStri(char *s, int passNo)
{
	int  c;
	//char *d0 = &g_mem[g_adr];

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
		c = *s + striAdi;
		if (passNo == 2) {
			POKEB(&g_mem[g_adr], (Uint8)c);
		}
		if (IS_KANJI(c) && s[1]) {
			s++;
			g_adr++;
			c = *s + striAdi;
			if (passNo == 2) {
				POKEB(&g_mem[g_adr], (Uint8)c);
			}
		}
		s++;
		g_adr++;
	}
	if (passNo == 2) {
		POKEB(&g_mem[g_adr], 0+striAdi);
//printf("%s\n",d0);
	}
}


void GenStriEnd(int passNo)
{
	if (striMode) {
		if (passNo == 2) {
			POKEB(&g_mem[g_adr], 0+striAdi);
			
		}
		g_adr++;
	}
	striMode = 0;
}


/*-------------------------------*/

int Pass_Line(int passNo, char *s0)
{
	long imm[2];
	int c;
	label_t *cl = NULL;
	char *s;

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
		gen_autoAlign(imm[0]);
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
		striOpt  = imm[0];
		break;
  #if 0
	case ODR_STRI_ADI:
		GetArg(&s, imm, 1);
		striAdi  = imm[0];
		break;
  #endif
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




void Pass(int passNo, srcl_t *sr)
{
	srcl_t *p;

	srcl_errorSw(passNo - 1);
	g_adr = 0;
	striAdi = 0;
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
FILE  *gen_hdr_fp = NULL;

void	gen_hdr_sub(void *a)
{
	label_t *l = a;

	if (l->typ == 3)
		fprintf(gen_hdr_fp, "#define %-14s\t0x%08x\t/* %10d */\n", l->name, l->val, l->val);
}


void	gen_hdr(char *hdrname)
{
	gen_hdr_fp = fopenE(hdrname, "wt");
	TREE_DoAll(LBL_tree, gen_hdr_sub);
	fclose(gen_hdr_fp);
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void FilnInit(void)
{
	if (Filn_Init() == NULL) {		/* ソース入力ルーチンの初期化 */
		err_exit("メモリがたりない\n");
	}
	Filn->opt_delspc    = 0;
	Filn->opt_mot_doll  = 1;
	Filn->opt_oct	    = 0;
	Filn->opt_orgSrc    = 0;
	Filn->cmt_chr[0]    = ';';
	Filn->cmt_chrTop[0] = '*';
	Filn->opt_kanji     = 1;
}


int main(int argc, char *argv[])
{
	char	oname[260];
	int i;
	char *p;
	SLIST *fl;

	FilnInit();		/* ソース入力ルーチンの初期化 */

	StrExpr_SetNameChkFunc(GetLblVal4StrExpr);	/* 式関係の準備 */
	if (argc < 2)
		Usage();

	odr_init();					/* 命令表を作る */
	LBL_Init();					/* ラベル表の初期化 */

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
	if (rulename) {								/* ルールファイル指定があったとき */
		if (vmsgFlg) {
			err_printf("\t'%s'",rulename);
		}
		GetSrcList(rulename);
	}
	for (fl = fileListTop; fl; fl = fl->link) {	/* 一連のファイルを連続したものとして処理 */
		if (vmsgFlg) {
			err_printf( "\t'%s'", fl->s);
		}
		GetSrcList(fl->s);
	}
	if (vmsgFlg) {
		err_printf( "\n");
	}
	Filn_Term();						/* ソース入力ルーチンの終了 */

	/* 生成 */
	if (vmsgFlg)
		err_printf( "[PASS1]\n");
	Pass(1, srcl);						/* Pass1:アドレスを得る */
	g_mem = callocE(1, g_adr + 0x20);	/* 必要分メモリ確保. +0x20は保険 */
	if (vmsgFlg)
		err_printf( "[PASS2]\n");
	Pass(2, srcl);						/* Pass2:バイナリを生成 */
	if (vmsgFlg)
		err_printf( "[SAVE] '%s' (%x)\n", oname, g_adr);
	FIL_SaveE(oname, g_mem, g_adr);		/* バイナリファイルをセーブ */
	err_printf( "\n%-14s 0x%x(%d) byte(s)\n", oname, g_adr, g_adr);
	if (hdrname) {
		err_printf( "[header output] %-14s\n", hdrname);
		gen_hdr(hdrname);
	}

	DB err_printf( "[後処理]\n");
	/* 後処理 */
	odr_term();							/* 命令表を削除 */

	return 0;
}
