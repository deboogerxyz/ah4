ah4.o: interfaces.h memory.h sdk.h netvars.h
sdk.o: sdk.h memory.h netvars.h
interfaces.o: interfaces.h sdk.h
utils.o: utils.h
memory.o: memory.h sdk.h
netvars.o: netvars.h interfaces.h sdk.h utils.h
hooks.o: hooks.h hacks/enginePrediction.h interfaces.h memory.h sdk.h
ubsan.o: ubsan.h
config.o: config.h
libs/nuklear.o: libs/nuklear.h libs/nuklear_sdl_gl3.h
hacks/enginePrediction.o: hacks/enginePrediction.h interfaces.h memory.h sdk.h
