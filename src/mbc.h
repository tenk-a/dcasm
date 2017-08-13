/**
 *  @file   mbc.h
 *  @brief  マルチバイト文字の処理.
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *   -  utf8対応.
 *   -  2バイトコードは、winでは基本、api任せ.
 *   -  win-api以外では SJIS,EUC-JP,EUC(基本部分),BIG5,GBK(gb18030)を考慮.
 *   -  半角全角を想定した表示の桁数を指定可能に(かなり大雑把).(Width)
 *   -  ライセンス
 *      Boost Software License Version 1.0
 */
#ifndef MBC_H_INCLUDE
#define MBC_H_INCLUDE

#define MBC_USE_DEFAULT_ENV

#include <stddef.h>
#include <assert.h>


#if !defined(inline) && !defined(__cplusplus) && (__STDC_VERSION__ < 199901L)
#define inline      __inline
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct Mbc_Env {
    unsigned (*isLead)(unsigned c);                         // Cがマルチバイト文字の1バイト目か?
    unsigned (*chkC)(unsigned c);
    unsigned (*getC)(const char** str);                     // 1字取り出し＆ポインタ更新.
    unsigned (*peekC)(const char* str);                     // 一字取り出し
    char*    (*setC)(char*  dst, unsigned c);               // 1字書き込み.
    unsigned (*len1)(const char* pChr);                     // 1文字のchar数を返す.
    unsigned (*chrLen)(unsigned chr);                       // 1文字のchar数を返す.
    unsigned (*chrWidth)(unsigned chr);                     // 半角全角を考慮して文字の幅を返す.
    //size_t (*adjust_size)(const char* s, unsigned size);  // 半角全角を考慮して文字の幅を返す.

  #ifdef __cplusplus    //気力があれば、関数ポインタ化.
    char*    inc   (const char* str)     const { return (char*)str + this->len1(str) ; }
    void     putc  (char** d,unsigned c) const { *d = this->setC(*d, c); }

    size_t   strLen(const char* src) const;
    size_t   adjust_size(const char* src, size_t sz) const;
    size_t   sizeToWidth(const char* str, size_t size) const ;
    size_t   sizeToChrs (const char* str, size_t size) const ;
    size_t   chrsToWidth(const char* str, size_t chrs) const ;
    size_t   chrsToSize (const char* str, size_t chrs) const ;
    size_t   widthToSize(const char* str, size_t width) const ;
    size_t   widthToChrs(const char* str, size_t width) const ;
    char*    cpy(char dst[], size_t size, const char* src) const ;
    char*    cat(char dst[], size_t size, const char* src) const ;
    char*    cpyNC(char dst[], size_t size, const char* src, size_t nc) const ;
    char*    catNC(char dst[], size_t size, const char* src, size_t nc) const ;
    char*    cpyWidth(char dst[], size_t size, const char* src, size_t width) const ;
    char*    catWidth(char dst[], size_t size, const char* src, size_t width) const ;
  #endif
} Mbc_Env;

#ifdef __cplusplus
#define MBC_INL     inline
#else
#define MBC_INL     static inline
#endif

const Mbc_Env*   mbc_env_create(const char* lang_enc);      // "ja_JP.SJIS等で環境設定. NULLでデフォルト取得.
MBC_INL unsigned mbc_islead (const Mbc_Env* e, char c)              { return e->isLead(c) ; }
MBC_INL unsigned mbc_getc   (const Mbc_Env* e, char const** ppStr)  { return e->getC(ppStr) ; }
MBC_INL unsigned mbc_peekc  (const Mbc_Env* e, char const* str)     { return e->peekC(str); }
MBC_INL char*    mbc_setc   (const Mbc_Env* e, char*  d,unsigned c) { return e->setC(d,c); }
MBC_INL unsigned mbc_len1   (const Mbc_Env* e, char const* pChr)    { return e->len1(pChr); }
MBC_INL unsigned mbc_chrLen (const Mbc_Env* e, unsigned chr)        { return e->chrLen(chr); }
MBC_INL size_t   mbc_chrWidth(const Mbc_Env* e, unsigned chr)       { return e->chrWidth(chr); }

MBC_INL char*   mbc_inc     (const Mbc_Env* e, const char* str)     { return (char*)str + e->len1(str) ; }
MBC_INL void    mbc_putc    (const Mbc_Env* e, char** d,unsigned c) { *d = e->setC(*d, c); }

MBC_INL size_t  mbc_raw_len(const char* s) { const char* p = s; assert(s); --p; do {} while (*++p); return p - s; }

size_t          mbc_adjust_size(const Mbc_Env* e, const char* src, size_t sz);
MBC_INL size_t  mbc_strLen  (const Mbc_Env* e, const char* src) { return mbc_adjust_size(e, src, ~(size_t)0); }

size_t          mbc_sizeToWidth(const Mbc_Env* mbc, const char* str, size_t size);
size_t          mbc_sizeToChrs(const Mbc_Env* mbc, const char* str, size_t size);
size_t          mbc_chrsToSize(const Mbc_Env* mbc, const char* str, size_t chrs);
size_t          mbc_chrsToWidth(const Mbc_Env* mbc, const char* str, size_t chrs);
size_t          mbc_widthToSize(const Mbc_Env* mbc, const char* str, size_t width);
size_t          mbc_widthToChrs(const Mbc_Env* mbc, const char* str, size_t width);

char*           mbc_cpy(const Mbc_Env* e, char dst[], size_t size, const char* src);
char*           mbc_cat(const Mbc_Env* e, char dst[], size_t size, const char* src);
char*           mbc_cpyNC(const Mbc_Env* e, char dst[], size_t size, const char* src, size_t nchr);
char*           mbc_catNC(const Mbc_Env* e, char dst[], size_t size, const char* src, size_t nchr);
char*           mbc_cpyWidth(const Mbc_Env* e, char dst[], size_t size, const char* src, size_t width);
char*           mbc_catWidth(const Mbc_Env* e, char dst[], size_t size, const char* src, size_t width);

#if 0
int             mbc_cmp(const Mbc_Env* e, const char* l, const char* r);

int             mbc_l_cmp(const Mbc_Env* e, const char* l, unsigned llen, const char* r, unsigned rlen);
int             mbc_l_casecmp(const Mbc_Env* e, const char* l, const char* r);

char*           mbc_l_chr (const char* src, size_t len, unsigned chr);
char*           mbc_l_rchr(const char* src, size_t len, unsigned chr);
char*           mbc_l_not_chr (const char* src, size_t len, unsigned chr);
char*           mbc_l_not_rchr(const char* src, size_t len, unsigned chr);

char*           mbc_l_find (const char* src, size_t len, const char* k, size_t kn);
char*           mbc_l_rfind(const char* src, size_t len, const char* k, size_t kn);
size_t          mbc_l_first_of1(const C* src, size_t len, C c) { char t[2]={c,0}; mbc_first_of(a,an,t,1); }
size_t          mbc_first_of      (const C* a, size_t an, size_t ofs, const C* t, size_t tn);
size_t          mbc_last_of   (const C* a, size_t an, size_t ofs, C c);
size_t          mbc_last_of   (const C* a, size_t an, size_t ofs, const C* t, size_t tn);
size_t          mbc_first_not_of(const C* a, size_t an, size_t ofs, C c);
size_t          fnd_first_not_of(const C* a, size_t an, size_t ofs, const C* t, size_t tn);
size_t          fnd_last_not_of (const C* a, size_t an, size_t ofs, C c);
size_t          fnd_last_not_of (const C* a, size_t an, size_t ofs, const C* t, size_t tn);
#endif


#ifdef __cplusplus
inline size_t   Mbc_Env::strLen(const char* src) const { return mbc_adjust_size(this, src, ~(size_t)0); }
inline size_t   Mbc_Env::adjust_size(const char* src, size_t sz) const { return mbc_adjust_size(this, src, sz); }

inline size_t   Mbc_Env::sizeToWidth(const char* str, size_t size) const { return mbc_sizeToWidth (this,str,size); }
inline size_t   Mbc_Env::sizeToChrs (const char* str, size_t size) const { return mbc_sizeToChrs  (this,str,size); }
inline size_t   Mbc_Env::chrsToWidth(const char* str, size_t chrs) const { return mbc_chrsToWidth (this,str,chrs); }
inline size_t   Mbc_Env::chrsToSize (const char* str, size_t chrs) const { return mbc_chrsToSize  (this,str,chrs); }
inline size_t   Mbc_Env::widthToSize(const char* str, size_t width) const { return mbc_widthToSize(this,str,width); }
inline size_t   Mbc_Env::widthToChrs(const char* str, size_t width) const { return mbc_widthToChrs(this,str,width); }

inline char*    Mbc_Env::cpy(char dst[], size_t size, const char* src) const { return mbc_cpy(this, dst, size, src); }
inline char*    Mbc_Env::cat(char dst[], size_t size, const char* src) const { return mbc_cat(this, dst, size, src); }
inline char*    Mbc_Env::cpyNC(char dst[], size_t size, const char* src, size_t nc) const { return mbc_cpyNC(this, dst, size, src, nc); }
inline char*    Mbc_Env::catNC(char dst[], size_t size, const char* src, size_t nc) const { return mbc_catNC(this, dst, size, src, nc); }
inline char*    Mbc_Env::cpyWidth(char dst[], size_t size, const char* src, size_t width) const { return mbc_cpyWidth(this, dst, size, src, width); }
inline char*    Mbc_Env::catWidth(char dst[], size_t size, const char* src, size_t width) const { return mbc_catWidth(this, dst, size, src, width); }
#endif



// ===========================================================================
// デフォルトの環境/言語専用.

#ifdef MBC_USE_DEFAULT_ENV
void     mbs_init(void);                                 // cの場合の初期化. c++では自動.
void     mbs_setEnv(char const* lang);
unsigned mbs_islead  (char c)               ;
unsigned mbs_getc    (const char** ppStr)   ;
unsigned mbs_peekc   (const char* str)      ;
char*    mbs_inc     (const char* str)      ;
void     mbs_putc    (char** d,unsigned c)  ;
char*    mbs_setc    (char*  d,unsigned c)  ;
unsigned mbs_len1    (const char* pChr)     ;
unsigned mbs_chrLen  (unsigned chr)         ;
unsigned mbs_chrWidth(unsigned chr)         ;
size_t   mbs_strLen (const char* src);
size_t   mbs_adjust_size(const char* src, size_t sz);

size_t   mbs_sizeToWidth(const char* str, size_t size);
size_t   mbs_widthToSize(const char* str, size_t width);

size_t   mbs_sizeToWidth(const char* str, size_t size);
size_t   mbs_sizeToChrs (const char* str, size_t size);
size_t   mbs_chrsToSize (const char* str, size_t chrs);
size_t   mbs_chrsToWidth(const char* str, size_t chrs);
size_t   mbs_widthToSize(const char* str, size_t width);
size_t   mbs_widthToChrs(const char* str, size_t width);

char*    mbs_cpy(char dst[], size_t size, const char* src);
char*    mbs_cat(char dst[], size_t size, const char* src);
char*    mbs_cpyNC(char dst[], size_t size, const char* src, size_t nc);
char*    mbs_catNC(char dst[], size_t size, const char* src, size_t nc);
char*    mbs_cpyWidth(char dst[], size_t size, const char* src, size_t width);
char*    mbs_catWidth(char dst[], size_t size, const char* src, size_t width);
#endif


#ifdef __cplusplus
}
#endif


#endif  // MBCS_H_INCLUDED
