DCASM   v0.51


バイナリ・データ生成を目的として、
　　68kアセンブラでの dc.b dc.w dc.l (dc.q)
　　x86アセンブラでの db dw dd (dq)
のみをサポートしたマクロアセンブラです。

xdef された外部ラベルを、C言語の .hのごとく、
    #define ラベル　アドレス
の形で生成することができます。

マクロ機能として #include #define #if  #macro #rept #ipr
があります。


アプリのライセンスは Boost Software License です。
