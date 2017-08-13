DCASM   v0.50


バイナリ・データ生成を目的として、  
　　68kアセンブラでの dc.b dc.w dc.l (dc.q)  
　　x86アセンブラでの db dw dd (dq)  
のみをサポートしたマクロアセンブラです。

xdef された外部ラベルを、C言語の .hのごとく、  
    #define ラベル　アドレス  
の形で生成することができます。

マクロ機能は、#include #define #if  #macro #rept #ipr 等  
で、拙作 acpp のプリプロセス機能を内臓しています。


アプリのライセンスは 二条項BSDライセンス です。
