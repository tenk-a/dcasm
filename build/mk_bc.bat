rem bcc5.5.1�ŃR���p�C���ł��邪 printf �� ll �w�肪�����Ȃ��̂Ńe�L�X�g�o�͂��o�O��
bcc32 -Ox -d -I..\src\old_vc -I..\src -edcasm.exe ..\src\dcasm.c ..\src\filn.c ..\src\subr.c ..\src\strexpr.c ..\src\tree.c ..\src\mbc.c
del *.obj
del ..\*.tds
