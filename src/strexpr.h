/**
 *  @file   strexpr.c
 *  @brief  式文字列を計算する
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @date   1996-2017
 */
#ifndef STREXPER_H
#define STREXPER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef long long strexpr_val_t;
//typedef long strexpr_val_t;

int  StrExpr(char const *s, char const** s_nxt, strexpr_val_t* val);

void StrExpr_SetNameChkFunc(int (*name2valFnc)(char const* name, strexpr_val_t* valp));
    /* name2valFnc は、名前が渡され、正常なら0を返しその名前の値を *valpにいれる. 異常なら-1を返す関数を設定すること */

void StrExpr_SetMbcMode(int mode);


#ifdef __cplusplus
}
#endif

#endif  // STREXPER_H
