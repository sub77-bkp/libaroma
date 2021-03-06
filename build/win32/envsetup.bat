@ECHO OFF
REM
REM TOOLCHAIN AND ADB
REM
setx LIBAROMA_GDB "D:\DevelTools\linaronew\bin\arm-linux-gnueabihf-gdb.exe"
setx LIBAROMA_NM "D:\DevelTools\linaronew\bin\arm-linux-gnueabihf-nm.exe"
setx LIBAROMA_ADB "D:\DevelTools\adt\sdk\platform-tools\adb.exe"
setx LIBAROMA_GCC "D:\DevelTools\linaronew\bin\arm-linux-gnueabihf-gcc.exe"
setx LIBAROMA_GPP "D:\DevelTools\linaronew\bin\arm-linux-gnueabihf-g++.exe"

REM
REM PRODUCTION CFLAGS
REM
setx LIBAROMA_CFLAGS "-O3 -ftree-vectorize -mvectorize-with-neon-quad -fprefetch-loop-arrays -funsafe-math-optimizations -fopenmp -mfloat-abi=hard -mfpu=neon -D__ARM_HAVE_NEON -DLIBAROMA_CONFIG_OPENMP=1 -DLIBAROMA_CONFIG_DEBUG_TRACE=0"

REM
REM DEBUG CFLAGS
REM
setx LIBAROMA_DEBUG_CFLAGS "-ggdb -fopenmp -mfloat-abi=hard -mfpu=neon -D__ARM_HAVE_NEON -DLIBAROMA_CONFIG_OPENMP=1 -DLIBAROMA_CONFIG_DEBUG_TRACE=0"

REM
REM LIBAROMA CONFIGS
REM
setx LIBAROMA_CONFIG_DEBUG "3"
setx LIBAROMA_CONFIG_DEBUG_FILE "0"
setx LIBAROMA_CONFIG_DEBUG_MEMORY "0"
setx LIBAROMA_CONFIG_COMPILER_MESSAGE "1"
setx LIBAROMA_CONFIG_SHMEMFB "0"
