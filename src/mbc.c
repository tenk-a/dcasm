/**
 *  @file   mbc.c
 *  @brief  マルチバイト文字の処理.
 *  @note
 *  -   文字コード変換はしない.
 */
#include "mbc.h"
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#define strncasecmp     _strnicmp
#endif

#ifdef _MSC_VER
#pragma warning (disable:4244)
#endif



// ---------------------------------------------------------------------------

/** 1字書き込み.
 */
static char*    dbc_setc(char*  d, unsigned c) {
    if (c > 0xff) {
        *d++ = c >> 8;
    }
    *d++ = c;
    return d;
}



/** 1文字のchar数を返す.
 */
static unsigned dbc_chrLen(unsigned chr) {
    return 1 + (chr > 0xff);
}



/** 半角全角を考慮して文字の幅を返す.
 */
static unsigned dbc_chrWidth(unsigned chr) {
    // とりあえず特殊なNEC半角は無視...
    return 1 + (chr > 0xff);
}



// ---------------------------------------------------------------------------

#ifdef _WIN32
/** 全角の1バイト目か?
 */
static unsigned dbc_islead(unsigned c) {
    return IsDBCSLeadByte((unsigned char)c);
}



static unsigned dbc_istrail(unsigned c) {
    return ((c >= 0x30) & (c <= 0xFE)) && c != 0x7f;
}


#if 0
static unsigned dbc_istrailp(char const* p) {
    return dbc_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned dbc_chkc(unsigned c)
{
    if (c > 0xff) {
        return IsDBCSLeadByte(c >> 8) && dbc_istrail((unsigned char)c);
    }
    return 1;
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned dbc_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (IsDBCSLeadByte(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char*)s;
    return c;
}



/** 一字取り出し
 */
static unsigned dbc_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (IsDBCSLeadByte(c) && *s) {
        c = (c << 8) | *s;
    }
    return c;
}



/** 1文字のchar数を返す.
 */
static unsigned dbc_len1(const char* pChr) {
    return (pChr[0] == 0) ? 0 : 1 + (IsDBCSLeadByte(pChr[0]) && pChr[1]);
}



static const Mbc_Env mbc_win = {
        dbc_islead,                 // Cがマルチバイト文字の1バイト目か?
        dbc_chkc,                   // 文字コードが正しい範囲にあるかチェック.
        dbc_getc,                   // 1字取り出し＆ポインタ更新.
        dbc_peekc,                  // 一字取り出し
        dbc_setc,                   // 1字書き込み.
        dbc_len1,                   // 1文字のchar数を返す.
        dbc_chrLen,                 // 1文字のchar数を返す.
        dbc_chrWidth,               // 半角全角を考慮して文字の幅を返す.
};

#endif



// ---------------------------------------------------------------------------

/** 全角の1バイト目か?
 */
static unsigned sjis_islead(unsigned c) {
    return (c >= 0x81) && ((c <= 0x9F) || ((c >= 0xE0) & (c <= 0xFC)));
}



static unsigned sjis_istrail(unsigned c) {
    return ((c >= 0x40 && c <= 0x7e) || (c >= 0x81 && c <= 0xFC));
}


#if 0
static unsigned sjis_istrailp(char const* p) {
    return sjis_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned sjis_chkc(unsigned c)
{
    if (c > 0xff) {
        return sjis_islead(c >> 8) && sjis_istrail((unsigned char)c);
    }
    return 1;
}




/** 1字取り出し＆ポインタ更新.
 */
static unsigned sjis_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (sjis_islead(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char *)s;
    return c;
}



/** 一字取り出し
 */
static unsigned sjis_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (sjis_islead(c) && *s) {
        c = (c << 8) | *s;
    }
    return c;
}



/** 1文字のchar数を返す.
 */
static unsigned sjis_len1(const char* pChr) {
    return (pChr[0] == 0) ? 0 : 1 + (sjis_islead(pChr[0]) && pChr[1]);
}



static const Mbc_Env mbc_sjis = {
        sjis_islead,                    // Cがマルチバイト文字の1バイト目か?
        sjis_chkc,                      // 文字コードが正しい範囲にあるかチェック.
        sjis_getc,                      // 1字取り出し＆ポインタ更新.
        sjis_peekc,                     // 一字取り出し
        dbc_setc,                       // 1字書き込み.
        sjis_len1,                      // 1文字のchar数を返す.
        dbc_chrLen,                     // 1文字のchar数を返す.
        dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
};




// ---------------------------------------------------------------------------

/** 全角の1バイト目か?
 */
static unsigned euc_islead(unsigned c) {
    return (c >= 0xA1 && c <= 0xFE);
}



static unsigned euc_istrail(unsigned c) {
    return (c >= 0xA1 && c <= 0xFE);
}


#if 0
static unsigned euc_istrailp(char const* p) {
    return euc_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned euc_chkc(unsigned c)
{
    if (c > 0xff) {
        return euc_islead(c >> 8) && euc_istrail((unsigned char)c);
    }
    return 1;
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned euc_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (euc_islead(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char*)s;
    return c;
}



/** 一字取り出し
 */
static unsigned euc_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (euc_islead(c) && *s) {
        c = (c << 8) | *s;
    }
    return c;
}



/** 1文字のchar数を返す.
 */
static unsigned euc_len1(const char* pChr) {
    return (pChr[0] == 0) ? 0 : 1 + (euc_islead(pChr[0]) && pChr[1]);
}



static const Mbc_Env mbc_euc = {
        euc_islead,                 // Cがマルチバイト文字の1バイト目か?
        euc_chkc,                   // 文字コードが正しい範囲にあるかチェック.
        euc_getc,                   // 1字取り出し＆ポインタ更新.
        euc_peekc,                  // 一字取り出し
        dbc_setc,                   // 1字書き込み.
        euc_len1,                   // 1文字のchar数を返す.
        dbc_chrLen,                 // 1文字のchar数を返す.
        dbc_chrWidth,               // 半角全角を考慮して文字の幅を返す.
};




// ---------------------------------------------------------------------------

/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned eucjp_chkc(unsigned c)
{
    if (c > 0xff) {
        if (c > 0xffff) {
            if ((c >> 16) != 0x8f)
                return 0;
            c = (unsigned short)c;
        }
        return euc_islead(c >> 8) && euc_istrail((unsigned char)c);
    }
    return 1;
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned eucjp_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (euc_islead(c) && *s) {
        unsigned   k  = c;
        c = (c << 8) | *s++;
        if (k == 0x8f && *s) {
            c = (c << 8) | *s++;
        }
    }
    *pStr = (const char*)s;
    return c;
}


/** 一字取り出し
 */
static unsigned eucjp_peekc(const char* pStr) {
    const unsigned char* s = (unsigned char*)pStr;
    unsigned       c       = *s++;
    if (euc_islead(c) && *s) {
        unsigned   k  = c;
        c = (c << 8) | *s++;
        if (k == 0x8f && *s) {
            c = (c << 8) | *s++;
        }
    }
    return c;
}


/** 1文字のchar数を返す.
 */
static unsigned eucjp_len1(const char* s) {
    unsigned       c       = *(const unsigned char*)s;
    if (euc_islead(c) && s[1]) {
        if (c == 0x8f && s[2])
            return 3;
        return 2;
    }
    return (s[0] != 0);
}


/** 1字書き込み.
 */
static char*    eucjp_setc(char*  d, unsigned c) {
    if (c > 0xff) {
        if (c > 0xffff)
            *d++ = c >> 16;
        *d++ = c >> 8;
    }
    *d++ = c;
    return d;
}


/** 1文字のchar数を返す.
 */
static unsigned eucjp_chrLen(unsigned chr) {
    return (chr > 0) + (chr > 0xff);
}


/** 半角全角を考慮して文字の幅を返す.
 */
static unsigned eucjp_chrWidth(unsigned chr) {
    unsigned h = chr >> 8;
    if (h == 0 || h == 0x8E) {
        return 1;
    }
    return 2;
}



static const Mbc_Env mbc_eucjp = {
        euc_islead,                     // Cがマルチバイト文字の1バイト目か?
        eucjp_chkc,                     // 文字コードが正しい範囲にあるかチェック.
        eucjp_getc,                     // 1字取り出し＆ポインタ更新.
        eucjp_peekc,                    // 一字取り出し
        eucjp_setc,                     // 1字書き込み.
        eucjp_len1,                     // 1文字のchar数を返す.
        eucjp_chrLen,                   // 1文字のchar数を返す.
        eucjp_chrWidth,                 // 半角全角を考慮して文字の幅を返す.
};


// ---------------------------------------------------------------------------

/** 全角の1バイト目か?
 */
static unsigned big5_islead(unsigned c) {
    return (c >= 0xA1) && ((c <= 0xC6) || ((c >= 0xC9) & (c <= 0xF9)));
}


static unsigned big5_istrail(unsigned c) {
    return ((c >= 0x40 && c <= 0x7e) || (c >= 0xA1 && c <= 0xFE));
}


#if 0
static unsigned big5_istrailp(char const* p) {
    return big5_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned big5_chkc(unsigned c)
{
    if (c > 0xff) {
        return big5_islead(c >> 8) && big5_istrail((unsigned char)c);
    }
    return 1;
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned big5_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned             c = *s++;
    if (big5_islead(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char *)s;
    return c;
}



/** 一字取り出し
 */
static unsigned big5_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (big5_islead(c) && *s) {
        c = (c << 8) | *(unsigned char*)s;
    }
    return c;
}



/** 1文字のchar数を返す.
 */
static unsigned big5_len1(const char* pChr) {
    return (pChr[0] != 0) + (big5_islead(*(const unsigned char*)pChr) && pChr[1]);
}



static const Mbc_Env mbc_big5 = {
        big5_islead,                    // Cがマルチバイト文字の1バイト目か?
        big5_chkc,                      // 文字コードが正しい範囲にあるかチェック.
        big5_getc,                      // 1字取り出し＆ポインタ更新.
        big5_peekc,                     // 一字取り出し
        dbc_setc,                       // 1字書き込み.
        big5_len1,                      // 1文字のchar数を返す.
        dbc_chrLen,                     // 1文字のchar数を返す.
        dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
};



// ---------------------------------------------------------------------------
// gbk, gb18030

/** 全角の1バイト目か?
 */
static unsigned gbk_islead(unsigned c) {
    return ((c >= 0x81) & (c <= 0xFE));
}


static unsigned gbk_istrail(unsigned c) {
    return (c >= 0x40 && c <= 0xFE) && c != 0x7f;
}


#if 0
static unsigned gbk_istrailp(char const* p) {
    return gbk_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック. 手抜きでかなり甘くしてる.
 */
static unsigned gbk_chkc(unsigned c)
{
    if (c <= 0xff) {
        return 1;
    } else if (c <= 0xffff) {
        return gbk_islead(c >> 8) && gbk_istrail((unsigned char)c);
    } else {
        unsigned a = c >> 24;
        unsigned b = c >> 16;
        unsigned x = c >>  8;
        unsigned y = (unsigned char)c;
        return (gbk_islead(a) && b >= 0x30 && b <= 0x39 && gbk_islead(x) && y >= 0x30 && y <= 0x39);
    }
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned gbk_getc(const char** ppStr) {
    const unsigned char* s = (unsigned char*)*ppStr;
    unsigned       c       = *s++;
    if (gbk_islead(c) && *s) {
        unsigned k = *s++;
        if (k >= 0x30 && k <= 0x39 && gbk_islead(*s) && s[1]) {
            c = (c << 24) | (k << 16) | (*s << 8) | s[1];
            s += 2;
        } else {
            c = (c << 8) | k;
        }
    }
    *ppStr = (const char*)s;
    return c;
}



/** 一字取り出し.
 */
static unsigned gbk_peekc(const char* pStr) {
    const unsigned char* s = (unsigned char*)pStr;
    unsigned             c = *s++;
    if (gbk_islead(c) && *s) {
        unsigned k = *s++;
        if (k >= 0x30 && k <= 0x39 && gbk_islead(*s) && s[1]) {
            c = (c << 24) | (k << 16) | (*s << 8) | s[1];
        } else {
            c = (c << 8) | k;
        }
    }
    return c;
}



/** 1文字のchar数を返す.
 */
static unsigned gbk_len1(const char* pStr) {
    const unsigned char* s = (unsigned char*)pStr;
    unsigned       c       = *s++;
    if (gbk_islead(c) && *s) {
        unsigned k = *s++;
        if (k >= 0x30 && k <= 0x39 && gbk_islead(*s) && s[1]) {
            return 4;
        } else {
            return 2;
        }
    }
    return c != 0;
}



/** 1字書き込み.
 */
static char*    gbk_setc(char* d, unsigned c) {
    if (c > 0xff) {
        if (c > 0xffff) {
            //if (c > 0xffffff)
                *d++ = c >> 24;
            *d++ = c >> 16;
        }
        *d++ = c >> 8;
    }
    *d++ = c;
    return d;
}



/** 1文字のchar数を返す.
 */
static unsigned gbk_chrLen(unsigned chr) {
    // return 1 + (c > 0xff) + (c > 0xffff) + (c > 0xffffff);
    return 1 + (chr > 0xff) + (chr > 0xffff) * 2;
}



/** 半角全角を考慮して文字の幅を返す... くわしいことわからないので1バイト文字のみ半角扱い.
 */
static unsigned gbk_chrWidth(unsigned chr) {
    return 1 + (chr > 0xff);
}



static const Mbc_Env mbc_gbk = {
        gbk_islead,                 // Cがマルチバイト文字の1バイト目か?
        gbk_chkc,                   // 文字コードが正しい範囲にあるかチェック.
        gbk_getc,                   // 1字取り出し＆ポインタ更新.
        gbk_peekc,                  // 一字取り出し
        gbk_setc,                   // 1字書き込み.
        gbk_len1,                   // 1文字のchar数を返す.
        gbk_chrLen,                 // 1文字のchar数を返す.
        gbk_chrWidth,               // 半角全角を考慮して文字の幅を返す.
};



// ---------------------------------------------------------------------------
// uhc

#define UHC_ISLEAD(c)   ((c >= 0x81) & (c <= 0xFE))


/** 全角の1バイト目か?
 */
static unsigned uhc_islead(unsigned c) {
    return UHC_ISLEAD(c);
}



static unsigned uhc_istrail(unsigned c) {
    if (c >= 0x40 && c <= 0xFE) {
        if (c >= 0x81 || c <= 0x5a)
            return 1;
        if (c >= 0x61 && c <= 0x7a)
            return 1;
    }
    return 0;
}


#if 0
static unsigned uhc_istrailp(char const* p) {
    return uhc_istrail(*(unsigned char*)p);
}
#endif

/** 文字コードが正しい範囲にあるかチェック. 手抜きでかなり甘くしてる.
 */
static unsigned uhc_chkc(unsigned c)
{
    if (c <= 0xff) {
        return 1;
    } else {
        return UHC_ISLEAD(c >> 8) && uhc_istrail((unsigned char)c);
    }
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned uhc_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned             c = *s++;
    if (UHC_ISLEAD(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char *)s;
    return c;
}



/** 一字取り出し
 */
static unsigned uhc_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (UHC_ISLEAD(c) && *s) {
        c = (c << 8) | *(unsigned char*)s;
    }
    return c;
}



/** 1文字のchar数を返す.
 */
static unsigned uhc_len1(const char* pChr) {
    unsigned char c = *(unsigned char*)pChr;
    return (c != 0) + (UHC_ISLEAD(c) && pChr[1]);
}



static const Mbc_Env mbc_uhc = {
        uhc_islead,                     // Cがマルチバイト文字の1バイト目か?
        uhc_chkc,                       // 文字コードが正しい範囲にあるかチェック.
        uhc_getc,                       // 1字取り出し＆ポインタ更新.
        uhc_peekc,                      // 一字取り出し
        dbc_setc,                       // 1字書き込み.
        uhc_len1,                       // 1文字のchar数を返す.
        dbc_chrLen,                     // 1文字のchar数を返す.
        dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
};



// ---------------------------------------------------------------------------
// utf8

/** 全角の1バイト目か?
 */
static unsigned utf8_islead(unsigned c) {
    return c >= 0x80;
}



/** 文字コードが正しい範囲にあるかチェック. (\0もBOMもOKとする)
 */
static unsigned utf8_chkc(unsigned c)
{
    c;
    return 1;
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned utf8_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;

    if (c < 0x80) {
        ;
    } else if (*s) {
        int c2 = *s++;
        c2 &= 0x3F;
        if (c < 0xE0) {
            c = ((c & 0x1F) << 6) | c2;
        } else if (*s) {
            int c3 = *s++;
            c3 &= 0x3F;
            if (c < 0xF0) {
                c = ((c & 0xF) << 12) | (c2 << 6) | c3;
            } else if (*s) {
                int c4 = *s++;
                c4 &= 0x3F;
                if (c < 0xF8) {
                    c = ((c&7)<<18) | (c2<<12) | (c3<<6) | c4;
                } else if (*s) {
                    int c5 = *s++;
                    c5 &= 0x3F;
                    if (c < 0xFC) {
                        c = ((c&3)<<24) | (c2<<18) | (c3<<12) | (c4<<6) | c5;
                    } else if (*s) {
                        int c6 = *s++;
                        c6 &= 0x3F;
                        c = ((c&1)<<30) |(c2<<24) | (c3<<18) | (c4<<12) | (c5<<6) | c6;
                    }
                }
            }
        }
    }

    *pStr = (const char*)s;
    return c;
}



/** 一字取り出し
 */
static unsigned utf8_peekc(const char* s) {
    return utf8_getc(&s);
}



/** 1文字のchar数を返す.
 */
static unsigned utf8_len1(const char* pChr) {
    const unsigned char* s = (unsigned char*)pChr;
    unsigned       c       = *s;
    if (c && c < 0x80) {
        return 1;
    } else if (*++s) {
        if (c < 0xE0) {
            return 2;
        } else if (*++s) {
            if (c < 0xF0) {
                return 3;
            } else if (*++s) {
                if (c < 0xF8) {
                    return 4;
                } else if (*++s) {
                    if (c < 0xFC) {
                        return 5;
                    } else if (*++s) {
                        return 6;
                    }
                }
            }
        }
    }
    return (const char*)s - pChr;
}



/** 1字書き込み.
 */
static char*    utf8_setc(char*  dst, unsigned c) {
    char* d = dst;
    if (c < 0x80) {
        *d++ = c;
    } else {
        if (c <= 0x7FF) {
            *d++ = 0xC0|(c>>6);
            *d++ = 0x80|(c&0x3f);
        } else if (c <= 0xFFFF) {
            *d++ = 0xE0|(c>>12);
            *d++ = 0x80|(c>>6)&0x3f;
            *d++ = 0x80|(c&0x3f);
            //if (c >= 0xff60 && c <= 0xff9f) {--(*adn); }  // 半角カナなら、半角文字扱い.
        } else if (c <= 0x1fFFFF) {
            *d++ = 0xF0|(c>>18);
            *d++ = 0x80|(c>>12)&0x3f;
            *d++ = 0x80|(c>>6)&0x3f;
            *d++ = 0x80|(c&0x3f);
        } else if (c <= 0x3fffFFFF) {
            *d++ = 0xF8|(c>>24);
            *d++ = 0x80|(c>>18)&0x3f;
            *d++ = 0x80|(c>>12)&0x3f;
            *d++ = 0x80|(c>>6)&0x3f;
            *d++ = 0x80|(c&0x3f);
        } else {
            *d++ = 0xFC|(c>>30);
            *d++ = 0x80|(c>>24)&0x3f;
            *d++ = 0x80|(c>>18)&0x3f;
            *d++ = 0x80|(c>>12)&0x3f;
            *d++ = 0x80|(c>>6)&0x3f;
            *d++ = 0x80|(c&0x3f);
        }
    }
    return d;
}



/** 1文字のchar数を返す.
 */
static unsigned utf8_chrLen(unsigned c) {
 #if 0  // あとで
    if (c <= 0x7FF) {
        if (c < 0x80)
            return 1;
        return 2;
    }
    if (c <= 0xFFFF)
        return 3;
    if (c <= 0x1fFFFF)
        return 4;
    if (c <= 0x3fffFFFF)
        return 5;
    return 6;
 #else
    if (c < 0x80)
        return 1;
    if (c <= 0x7FF)
        return 2;
    if (c <= 0xFFFF)
        return 3;
    if (c <= 0x1fFFFF)
        return 4;
    if (c <= 0x3fffFFFF)
        return 5;
    return 6;
 #endif
}



/** 半角全角を考慮して文字の幅を返す.(面倒なんですべて同一サイズ扱い)
 */
static unsigned utf8_chrWidth(unsigned chr) {
    //chr;
    return 1;
}



/** 半角全角を考慮して文字の幅を返す. (日本語フォントを想定).
 */
static unsigned utf8_jp_chrWidth(unsigned c) {
    if (c < 0x370)
        return 1;
    if (c >= 0xff60 && c <= 0xff9f)
        return 1;
    return 2;
}



static const Mbc_Env mbc_utf8 = {
        utf8_islead,                // Cがマルチバイト文字の1バイト目か?
        utf8_chkc,                  // 文字コードが正しい範囲にあるかチェック.
        utf8_getc,                  // 1字取り出し＆ポインタ更新.
        utf8_peekc,                 // 一字取り出し
        utf8_setc,                  // 1字書き込み.
        utf8_len1,                  // 1文字のchar数を返す.
        utf8_chrLen,                // 1文字のchar数を返す.
        utf8_chrWidth,              // 半角全角を考慮して文字の幅を返す.
};



static const Mbc_Env mbc_utf8_jp = {
        utf8_islead,                // Cがマルチバイト文字の1バイト目か?
        utf8_chkc,                  // 文字コードが正しい範囲にあるかチェック.
        utf8_getc,                  // 1字取り出し＆ポインタ更新.
        utf8_peekc,                 // 一字取り出し
        utf8_setc,                  // 1字書き込み.
        utf8_len1,                  // 1文字のchar数を返す.
        utf8_chrLen,                // 1文字のchar数を返す.
        utf8_jp_chrWidth,           // 半角全角を考慮して文字の幅を返す.
};




// ---------------------------------------------------------------------------

static const Mbc_Env*  mbc_env_default_ptr = 0;


#ifdef __cplusplus
struct MbcInit {
    MbcInit() { mbc_env_create(0); }
};
static MbcInit  s_mbcInit;
#endif



const Mbc_Env*  mbc_env_create(const char* lang_enc)
{
    const Mbc_Env*  pEnv;
    const char*     p;
    if (lang_enc == 0) {
        if (mbc_env_default_ptr) {
            return mbc_env_default_ptr;
        }
      #ifdef _WIN32
        return mbc_env_default_ptr = &mbc_win;
      #else
        lang_enc = getenv("LANG");
      #endif
    }

  #ifdef _WIN32
    pEnv = &mbc_win;
  #else
    pEnv = &mbc_std_c;
  #endif
    p = strrchr(lang_enc, '.');
    if (p) {
        ++p;
        if (strncasecmp(p, "utf-8", 5) == 0 || strncasecmp(p, "utf8", 4) == 0 || strncasecmp(p,"65001",5) == 0) {
            if (strncasecmp(lang_enc, "ja_jp", 5) == 0 || strncasecmp(lang_enc, "japan", 5) == 0)
                pEnv = &mbc_utf8_jp;
            else
                pEnv = &mbc_utf8;
        } else if (strncasecmp(p, "sjis", 4) == 0 || strncasecmp(p,"932", 3) == 0) {
            pEnv = &mbc_sjis;
        } else if (strncasecmp(p, "euc-jp", 6) == 0 || strncasecmp(p, "eucjp", 5) == 0) {
            pEnv = &mbc_eucjp;
        } else if (strncasecmp(p, "euc", 3) == 0 || strncasecmp(p, "gb2312", 6) == 0) {
            pEnv = &mbc_euc;
        } else if (strncasecmp(p, "big5", 4) == 0) {
            pEnv = &mbc_big5;
        } else if (strncasecmp(p, "gbk", 4) == 0 || strncasecmp(p, "gb18030", 7) == 0) {
            pEnv = &mbc_gbk;
        } else if (strncasecmp(p, "uhc", 3) == 0 || strncasecmp(p,"949", 5) == 0) {
            pEnv = &mbc_uhc;
        }
    }
    if (mbc_env_default_ptr == 0) {
        mbc_env_default_ptr = pEnv;
    }
    return pEnv;
}



// ---------------------------------------------------------------------------


size_t    mbc_adjust_size(const Mbc_Env* mbc, const char* str, size_t size) {
    const char* s = str;
    const char* b = s;
    const char* e = s + size;
    assert(str != 0 && size > 0);
    if (e < s)
        e = (const char*)(~(size_t)0);
    while (s < e) {
        if (*s == 0)
            return s - str;
        b = s;
        s += mbc->len1(s);
    }
    return b - str;
}


/// 領域サイズから(意味的な)文字数を求める
size_t  mbc_sizeToChrs(const Mbc_Env* mbc, const char* str, size_t size) {
    const char* s = str;
    const char* e = s + size;
    const char* b = s;
    size_t      l = 0;
    if (e < s)
        e = (const char*)(~(size_t)0);
    assert(str != 0 && size > 0);
    while (s < e) {
        unsigned c;
        b  = s;
        c  = mbc->getc(&s);
        if (c == 0)
            break;
        ++l;
    }
    if (s > e)
        --l;
    return l;
}


/// 領域サイズから半角文字単位の幅を求める.
size_t  mbc_sizeToWidth(const Mbc_Env* mbc, const char* str, size_t size) {
    const char* s = str;
    const char* e = s + size;
    size_t      b = 0;
    size_t      w = 0;
    if (e < s)
        e = (const char*)(~(size_t)0);
    assert(str != 0 && size > 0);
    while (s < e) {
        unsigned c  = mbc->getc(&s);
        if (c == 0)
            break;
        b = w;
        w += mbc->chrWidth(c);
    }
    if (s == e)
        return w;
    return b;
}


/// 文字数から半角文字単位の幅を求める.
size_t  mbc_chrsToWidth(const Mbc_Env* mbc, const char* str, size_t chrs) {
    const char* s = str;
    size_t      w = 0;
    assert(str != 0);
    while (chrs) {
        unsigned c  = mbc->getc(&s);
        if (c == 0)
            break;
        w += mbc->chrWidth(c);
        --chrs;
    }
    return w;
}


/// 文字数から半角文字単位の領域サイズを求める.
size_t  mbc_chrsToSize(const Mbc_Env* mbc, const char* str, size_t chrs) {
    const char* s  = str;
    size_t      sz = 0;
    assert(str != 0);
    while (chrs) {
        unsigned c  = mbc->getc(&s);
        if (c == 0)
            break;
        sz += mbc->chrLen(c);
        --chrs;
    }
    return sz;
}


/// 半角文字単位の幅から領域サイズを求める.
size_t  mbc_widthToSize(const Mbc_Env* mbc, const char* str, size_t width) {
    const char* s = str;
    const char* b = s;
    size_t      w = 0;
    assert(str != 0);
    while (w < width) {
        unsigned c;
        b  = s;
        c  = mbc->getc(&s);
        if (c == 0)
            break;
        w += mbc->chrWidth(c);
    }
    if (w > width)
        s = b;
    return s - str;
}


/// 半角文字単位の幅から文字数を求める.
size_t  mbc_widthToChrs(const Mbc_Env* mbc, const char* str, size_t width) {
    const char* s = str;
    const char* b = s;
    size_t      w = 0;
    size_t      n = 0;
    assert(str != 0);
    while (w < width) {
        unsigned c;
        b  = s;
        c  = mbc->getc(&s);
        if (c == 0)
            break;
        ++n;
        w += mbc->chrWidth(c);
    }
    if (w > width)
        --n;
    return n;
}



/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
char*   mbc_cpy(const Mbc_Env* mbc, char dst[], size_t dstSz, const char* src)
{
    size_t    l;
    assert(dst != NULL && dstSz > 0 && src != NULL);

    l = mbc_adjust_size(mbc, src, dstSz);

    // アドレスが同じなら、長さをあわせるのみ.
    if (dst == src) {
        dst[l] = 0;
        return dst;
    }

    // コピー.
    {
        const char* s = src;
        const char* e = s + l;
        char*       d = dst;
        while (s < e)
            *d++ = *s++;
        *d = 0;
    }

    return dst;
}



/** 文字列の連結.
 */
char*   mbc_cat(const Mbc_Env* mbc, char dst[], size_t dstSz, const char* src)
{
    size_t l;
    assert(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return mbc_cpy(mbc, dst, dstSz, dst);
    }
    mbc_cpy(mbc, dst+l, dstSz - l, src);
    return dst;
}



/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
char*   mbc_cpyNC(const Mbc_Env* mbc, char dst[], size_t dstSz, const char* src, size_t nc)
{
    size_t    l;
    assert(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_chrsToSize(mbc, src, nc) + 1;
    l = dstSz < l ? dstSz : l;
    return mbc_cpy(mbc, dst, l, src);
}



/** 文字列の連結.
 */
char*   mbc_catNC(const Mbc_Env* mbc, char dst[], size_t dstSz, const char* src, size_t nc)
{
    size_t l, l2;
    assert(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return mbc_cpy(mbc, dst, dstSz, dst);
    }
    dstSz -= l;
    l2  = mbc_chrsToSize(mbc, src, nc) + 1;
    l2  = dstSz < l2 ? dstSz : l2;
    mbc_cpy(mbc, dst+l, l2, src);
    return dst;
}



/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
char*   mbc_cpyWidth(const Mbc_Env* mbc, char dst[], size_t dstSz, const char* src, size_t width)
{
    size_t    l = mbc_widthToSize(mbc, src, width) + 1;
    l = dstSz < l ? dstSz : l;
    return mbc_cpy(mbc, dst, l, src);
}



/** 文字列の連結.
 */
char*   mbc_catWidth(const Mbc_Env* mbc, char dst[], size_t dstSz, const char* src, size_t width)
{
    size_t l, l2;
    assert(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return mbc_cpy(mbc, dst, dstSz, dst);
    }
    dstSz -= l;
    l2  = mbc_widthToSize(mbc, src, width) + 1;
    l2  = dstSz < l2 ? dstSz : l2;
    mbc_cpy(mbc, dst+l, l2, src);
    return dst;
}



/** '\0'終端文字列比較. 文字値が intの正数範囲に収まることに依存.
 */
int mbc_cmp(const Mbc_Env* mbc, const char* lp, const char* rp) {
    int lc, rc;
    int d;
    assert(lp != NULL);
    assert(rp != NULL);
    do {
        lc = mbc->getc(&lp);
        rc = mbc->getc(&rp);
        d  = lc - rc;
    } while (d == 0 && lc);
    return d;
}




// ---------------------------------------------------------------------------
// デフォルト・環境バージョン (スレッドセーフでないので注意)

#ifdef MBC_USE_DEFAULT_ENV

void mbs_init(void)
{
    mbc_env_create(NULL);
}


void     mbs_setEnv(char const* lang_enc)
{
    mbc_env_default_ptr = mbc_env_create(lang_enc);
}


unsigned mbs_islead  (char c)               { return mbc_env_default_ptr->islead(c)  ; }
unsigned mbs_getc    (const char** ppStr)   { return mbc_env_default_ptr->getc(ppStr)  ; }
unsigned mbs_peekc   (const char* str)      { return mbc_env_default_ptr->peekc(str) ; }
char*    mbs_inc     (const char* str)      { return (char*)str + mbc_env_default_ptr->len1(str) ; }
void     mbs_putc    (char** d,unsigned c)  { *d = mbc_env_default_ptr->setc(*d, c)  ; }
char*    mbs_setc    (char*  d,unsigned c)  { return mbc_env_default_ptr->setc(d,c)  ; }
unsigned mbs_len1    (const char* pChr)     { return mbc_env_default_ptr->len1(pChr) ; }
unsigned mbs_chrLen  (unsigned chr)         { return mbc_env_default_ptr->chrLen(chr)    ; }
unsigned mbs_chrWidth(unsigned chr)         { return mbc_env_default_ptr->chrWidth(chr); }

size_t  mbs_strLen  (const char* src)       { return mbc_strLen(mbc_env_default_ptr, src); }
size_t  mbs_adjust_size(const char* src, size_t sz) { return mbc_adjust_size(mbc_env_default_ptr, src, sz); }

size_t  mbs_sizeToWidth(const char* str, size_t size ) { return mbc_sizeToWidth(mbc_env_default_ptr, str, size ); }
size_t  mbs_sizeToChrs (const char* str, size_t size ) { return mbc_sizeToChrs (mbc_env_default_ptr, str, size ); }
size_t  mbs_chrsToSize (const char* str, size_t chrs ) { return mbc_chrsToSize (mbc_env_default_ptr, str, chrs ); }
size_t  mbs_chrsToWidth(const char* str, size_t chrs ) { return mbc_chrsToWidth(mbc_env_default_ptr, str, chrs ); }
size_t  mbs_widthToSize(const char* str, size_t width) { return mbc_widthToSize(mbc_env_default_ptr, str, width); }
size_t  mbs_widthToChrs(const char* str, size_t width) { return mbc_widthToChrs(mbc_env_default_ptr, str, width); }

char*   mbs_cpy(char dst[], size_t size, const char* src) { return mbc_cpy(mbc_env_default_ptr, dst, size, src); }
char*   mbs_cat(char dst[], size_t size, const char* src) { return mbc_cat(mbc_env_default_ptr, dst, size, src); }
char*   mbs_cpyNC(char dst[], size_t size, const char* src, size_t nc) { return mbc_cpyNC(mbc_env_default_ptr, dst, size, src, nc); }
char*   mbs_catNC(char dst[], size_t size, const char* src, size_t nc) { return mbc_catNC(mbc_env_default_ptr, dst, size, src, nc); }
char*   mbs_cpyWidth(char dst[], size_t size, const char* src, size_t width) { return mbc_cpyWidth(mbc_env_default_ptr, dst, size, src, width); }
char*   mbs_catWidth(char dst[], size_t size, const char* src, size_t width) { return mbc_catWidth(mbc_env_default_ptr, dst, size, src, width); }

#endif
