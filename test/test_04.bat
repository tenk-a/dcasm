rem dcasm.exe -cUTF8 -bl -otest_04_le.bin test_04.s
rem dcasm.exe -cUTF8 -bb -otest_04_be.bin test_04.s
dcasm.exe -bl -otest_04_le.bin test_04.s
dcasm.exe -bb -otest_04_be.bin test_04.s

fc /b smp_bin\test_04_be.bin test_04_be.bin
fc /b smp_bin\test_04_le.bin test_04_le.bin
