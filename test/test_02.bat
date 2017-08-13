dcasm.exe -bl -otest_02_le.bin -htest_02_le.xref.hh test_02.s
dcasm.exe -bb -otest_02_be.bin -htest_02_be.xref.hh test_02.s

fc /b smp_bin\test_02_be.bin test_02_be.bin
fc /b smp_bin\test_02_le.bin test_02_le.bin
fc smp_bin\test_02_be.xref.hh test_02_be.xref.hh
fc smp_bin\test_02_le.xref.hh test_02_le.xref.hh
