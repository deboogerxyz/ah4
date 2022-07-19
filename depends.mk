ah4.o: interfaces.h memory.h sdk.h netvars.h
interfaces.o: interfaces.h sdk.h
utils.o: utils.h
memory.o: memory.h sdk.h
netvars.o: netvars.h interfaces.h sdk.h utils.h
hooks.o: hooks.h
ubsan.o: ubsan.h
