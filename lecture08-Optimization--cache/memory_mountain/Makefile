
COMPILER=gcc

ifeq ($(COMPILER),gcc)
CC = gcc
CFLAGS = -Wall -O3 -march=native -mtune=native -ftree-vectorize -D__i686__ 
#CFLAGS = -Wall -ggdb3 -lm
SUFFIX = .gcc
LIBM = -lm
endif

ifeq ($(COMPILER),intel)
CC = icx
CFLAGS = -Wall -O3 -fast -axCORE-AVX2 -axCORE-AVX512 -xHost -ipo
SUFFIX = .icx
LIBM =
endif

ifeq ($(COMPILER),clang)
CC = clang-19
CFLAGS = -Wall -O3 -march=native -mtune=native
SUFFIX = .clang
LIBM = -lm
endif

mountain: mountain.c ftime.c ftime.h clock.c clock.h
	$(CC) $(CFLAGS) -o mountain$(SUFFIX) mountain.c ftime.c clock.c $(LIBM)

clean:
	rm -f mountain$(SUFFIX) *.o *~


