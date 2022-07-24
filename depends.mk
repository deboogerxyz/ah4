ah4.o: interfaces.h memory.h sdk.h netvars.h
sdk.o: sdk.h memory.h netvars.h
interfaces.o: interfaces.h sdk.h
utils.o: utils.h
memory.o: memory.h sdk.h
netvars.o: netvars.h interfaces.h sdk.h utils.h
hooks.o: hooks.h
ubsan.o: ubsan.h
config.o: config.h
nuklear.o: nuklear.h nuklear_sdl_gl3.h
enginePrediction.o: enginePrediction.h interfaces.h memory.h sdk.h
