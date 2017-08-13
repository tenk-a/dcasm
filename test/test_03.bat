dcasm.exe -cMBC -bl -otest_03_le.bin test_03.s
dcasm.exe -cMBC -bb -otest_03_be.bin test_03.s

fc /b smp_bin\test_03_be.bin test_03_be.bin
fc /b smp_bin\test_03_le.bin test_03_le.bin
