dcasm.exe -bl -otest_01_le.bin -htest_01_le.xref.hh test_01.s
dcasm.exe -bb -otest_01_be.bin -htest_01_be.xref.hh test_01.s

fc /b smp_bin\test_01_be.bin test_01_be.bin
fc /b smp_bin\test_01_le.bin test_01_le.bin
fc smp_bin\test_01_be.xref.hh test_01_be.xref.hh
fc smp_bin\test_01_le.xref.hh test_01_le.xref.hh
