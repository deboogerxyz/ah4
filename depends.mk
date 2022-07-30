ah4.o: interfaces.h memory.h sdk.h netvars.h
sdk.o: sdk.h memory.h netvars.h utils.h
interfaces.o: interfaces.h sdk.h
utils.o: utils.h
memory.o: memory.h sdk.h
netvars.o: netvars.h interfaces.h sdk.h utils.h
hooks.o: hooks.h hacks/enginePrediction.h hacks/backtrack.h hacks/misc.h hacks/skinChanger.h interfaces.h memory.h sdk.h utils.h
ubsan.o: ubsan.h
config.o: config.h
libs/nuklear.o: libs/nuklear.h libs/nuklear_sdl_gl3.h
gui.o: gui.h libs/nuklear.h libs/nuklear_sdl_gl3.h hacks/skinChanger.h keyBinds.h config.h
keyBinds.o: keyBinds.h interfaces.h sdk.h config.h
hacks/enginePrediction.o: hacks/enginePrediction.h interfaces.h memory.h netvars.h sdk.h
hacks/backtrack.o: hacks/backtrack.h config.h interfaces.h memory.h netvars.h sdk.h libs/cvector.h
hacks/misc.o: hacks/misc.h hacks/enginePrediction.h config.h keyBinds.h memory.h netvars.h sdk.h
hacks/skinChanger.o: hacks/skinChanger.h config.h interfaces.h memory.h netvars.h sdk.h
