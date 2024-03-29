# Libraries
LIBS = -lpthread -lcjson -lGLEW -lSDL2

# Flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
#CFLAGS   = -g -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -O0 -fPIC -march=native ${CPPFLAGS} -fsanitize=undefined
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -Ofast -march=native -flto=auto -fipa-pta -fdevirtualize-at-ltrans -fno-plt -fPIC ${CPPFLAGS}
LDFLAGS  = -shared ${LIBS}

# Compiler and linker
CC = cc
