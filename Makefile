#
#	SDL Makefile
#


VERSION = 0.5.0
BIN = game
CC = gcc
CFLAGS = -Wall -g -I'D:\Progs\c\sdl\gin-tonic\include' -L./
LIBS = mingw32 SDL2main SDL2_ttf SDL2_mixer SDL2_image SDL2 GinTonic
DIAS = $(patsubst %.xml,%.dbf,$(wildcard assets/txt/*.xml))

REL_CFLAGS = -Wall -O2 -mwindows

run: build
	@echo -e '\n### Running... ###\n'
	@./${BIN}

build: $(DIAS)
	@echo -e '\n### Building... ###\n'
	${CC} ${CFLAGS} -o ${BIN}.exe main.c src/*.c $(addprefix -l,${LIBS})

clean_dia:
	@echo -e '\n### Cleaning Dialogue Files... ###\n'
	rm -f $(DIAS)

assets/txt/%.dbf:
	@dialogue_comp/xdc.exe $(patsubst %.dbf,%.xml,$@) $@

static:
	@echo -e '\n### Building Static... ###\n'
	${CC} -static ${CFLAGS} -o ${BIN}_static.exe main.c src/*.c $(addprefix -l,${LIBS})
