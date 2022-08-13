# FreeType
FREETYPEINC = /usr/include/freetype2
FREETYPELIB = -lfreetype

# ImGui
IMGUIINC = libs/imgui

# Includes and libraries
INCS = -I${FREETYPEINC} -I${IMGUIINC}
LIBS = -lpthread -lcjson -lGLEW -lSDL2 ${FREETYPELIB}

# Flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS   = -g -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -O0 -fPIC ${INCS} ${CPPFLAGS} -fsanitize=undefined
#CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -O2 -fPIC ${INCS} ${CPPFLAGS}
CXXFLAGS = -g -std=c++11 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -O0 -fPIC ${INCS} ${CPPFLAGS} -fsanitize=undefined
#CXXFLAGS = -std=c++11 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -O2 -fPIC ${INCS} ${CPPFLAGS}
LDFLAGS  = -shared ${LIBS}

# Compilers and linkers
CC = cc
CXX = c++
