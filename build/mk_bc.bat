bcc32 -Ox -d -Iold_vc -edcasm.exe dcasm.c filn.c subr.c strexpr.c tree.c mbc.c
del *.obj
del ..\*.tds
