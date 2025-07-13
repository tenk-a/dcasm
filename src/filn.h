/**
    @file   filn.h
    @brief  条件生成、マクロ機能をもったテキストファイル入力ルーチン.
    @author Masashi KITAMURA (tenka@6809.net)
    @date   1996-2025
    @note
     - コマンドラインツールでの利用を前提。
     - 致命的エラーがあると exit(1) で終了する。
     - 使い方
        - まず、最初に Filn_Init() で初期化する。
        - Filn->???? で、オプション要素を好みに設定。
        - include検索パスは、Filn_AddIncDir()で可能。
        - オプション設定は、入力を開始する前に済ますこと。入力中に変更した場合の挙動は不明。
        - エラーは基本stderr出力だが、必要ならば Filn_ErrReOpen()でエラー出力先を代える。
        - Filn_Open(name) で、入力オープン。name=NULL だと標準入力。
        - Filn_Gets() で１行入力。マクロ展開や#ifや#includeの処理後の文字列を得る。
          元ソースをコメントで含める設定にしていれば、そのように読みこまれる。
          戻り値は、取得した文字列を収めた malloc()したメモリへのポインタを返す。
          そのメモリの開放は呼び出し側が行う必要がある。
        - 現在入力中の行が、元のどのファイルの何行目かは、Filn_GetFileLine() で得られる。
        - Filn側でエラーがあれば勝手にエラー出力する。
          入力した行に対応したエラーを、アプリ側で出したい場合は、
          Filn_Error() や Filn_Warning() を使う。エラー数、警告数は Filn_GetErrNum() で取得可能。
        - 入力の終わりは Filn_Gets() がNULLを返したかどうかで判定する。
          includeの都合、自動的にファイル・クローズするので、呼び出し側でクローズの必要はない。
        - 最後に Filn_Term() を呼べば終了。

     - 注意
        - 現状の版では、取得したメモリの開放が不充分でリークする仕様なので、
          Filn_Init/Filn_Termのペアは起動して一回しか使えない。
        - あまり使いこんでいない/使い方に偏りがある、ので、バグはたくさん残っていると思われる。
        - &&や||で、左辺で結果が確定していても右辺を評価するしくみになっている。
            #if defined(M1) && M1(2,3) == 0
          のような記述で M1 未定義のとき、意図通りにはならずエラーになってしまう。
            #if defined(M1)
              #if M1(2,3) == 0
          のように行を分けて対処のこと.
        - #(式) 中では definedを使えないです。

     - license
        Boost Software License Version 1.0
 */
#ifndef FILN_H
#define FILN_H

#include <stdio.h>

typedef struct FILN_DIRS {
    struct FILN_DIRS    *link;
    char                *s;
} FILN_DIRS;


typedef struct filn_t {
/* public:*/ /* ユーザが設定を変更することのある変数 */
    int         opt_kanji;      /* 0以外ならば ascii 以外もラベルに対応 */
    int         opt_cp;         /* code page */
    int         opt_sscomment;  /* 0以外ならば//コメントを削除する */
    int         opt_blkcomment; /* 0以外ならば／＊コメント＊／を削除する */
    int         opt_dellf;      /* 0以外ならば￥改行による行連結を行う */
    int         opt_delspc;     /* 0以外ならば空白の圧縮を許す */
    int         opt_oct;        /* 1: 0から始まる数字は８進数  0:10進 */
    int         opt_yen;        /* \ 文字をCのように 0:扱わない. 1:する 2:'"中のみ  3,4:変換もしちゃう(実験) */
    int         opt_sq_mode;    /* ' を文字定数して　0:扱わない 1:扱う 2:扱うがペアにしない(モトローラ汗) */
    int         opt_wq_mode;    /* " を文字列定数して0:扱わない 1:扱う */
    int         opt_mot_doll;   /* $ を モトローラな 16進数定数開始文字として扱う */

    int         opt_orgSrc;     /* 1:元のソースもコメントにして出力 2:TAG JUMP形式  0:出力しない */
    char        *orgSrcPre;     /* 元ソース出力時の行頭につける文字列 */
    char        *orgSrcPost;    /* 元ソース出力時の行末につける文字列 */

    int         sym_chr_doll;   /* 名前の一部として $を使うなら'$'を, そうでなければ0を設定する */
    int         sym_chr_atmk;   /* 名前の一部として @を使うなら'@'を, そうでなければ0を設定する */
    int         sym_chr_qa;     /* 名前の一部として ?を使うなら'?'を, そうでなければ0を設定する */
    int         sym_chr_shp;    /* 名前の一部として #を使うなら'#'を, そうでなければ0を設定する */
    int         sym_chr_prd;    /* 名前の一部として .を使うなら'.'を, そうでなければ0を設定する */

    int         macErrFlg;      /* マクロ中のエラー行番号も表示 1:する 0:しない */
    int         mac_chr;        /* マクロ行開始文字 */
    int         mac_chr2;       /* マクロの特殊展開指定文字.  */
    int         immMode;        /* 即値の出力 0:まま  1:10進 2:符号無 3:0xFF 4:0FFh 5:$FF */
    char        cmt_chr[2];     /* コメント開始文字になる文字 */
    char        cmt_chrTop[2];  /* 行頭コメント開始文字になる文字 */
    char        *localPrefix;   /* #localで生成するラベルのプレフィックス*/

    int         getsAddSiz;     /* Filn_Gets()で返す mallocサイズで、少なくともこのバイト数分余分に確保する */
/*private:*/ /* 以下は参照していいが、書き換えてはだめ */
    FILN_DIRS   *dir;           /* include 時に検索するディレクトリ一覧 */
    FILE        *errFp;         /* エラー出力先 */
} filn_t;


extern  filn_t *Filn;

filn_t *Filn_Init(void);                                /* Filnの初期化ルーチン。真っ先に呼び出すこと*/
void Filn_Term(void);                                   /* Filnの終了. メモリ開放など. Filn_ErrCloseは行わない */

int  Filn_GetEnc(void);                                 /* テキスト・エンコーディングを取得 */
void Filn_SetEnc(int cp);                               /* テキスト・エンコーディングを設定 */

int  Filn_Open(char const* name);                       /* ソースファイルをオープンする */
char *Filn_Gets(void);                                  /* マクロ展開付１行入力. malloc したメモリを返すので、呼出側で開放のこと*/

void Filn_AddIncDir(char const* dir);                   /* (複数の)入力ディレクトリを設定する */
void Filn_GetFnameLine(char const** s, int* line);      /* 現在入力中のファイル名と行番号を得る */
int  Filn_UndefLabel(char const* p);                    /* マクロ名の削除. 登録されてなければなにもしない */
int  Filn_SetLabel(char const* name, char const* st);   /* マクロ名の登録. st=NULLならばnameを Cコンパイラの-Dname と同様の処理 */
int  Filn_GetLabel(char const* name,char const** strp); /* name がdefine されているか調べ、されていれば0以外を返す。*/
                                                        /* そのとき *strpに定義文字列へのポインタを入れて返す. *strpの破壊は不可。*/

FILE *Filn_ErrReOpen(char const* name, FILE *fp);       /* エラー出力先fpをnameで再オープン. */
                                                        /* name=NULLだと単にfpにし、fp=NULLだと新規にファイルへ出力 */
void Filn_ErrClose(void);                               /* エラー出力先のclose */
int  Filn_Error(char const* fmt, ...);                  /* ファイル名行番号付のエラー出力printf */
volatile int Filn_Exit(char const* fmt, ...);           /* Filn_Error と同じことをしたのちexit(1)する */
void Filn_GetErrNum(int *errNum, int *warnNum);         /* エラーと警告の数を返す */

#endif /* FILN_H */
