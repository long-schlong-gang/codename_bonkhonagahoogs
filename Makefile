#
#	SDL Makefile
#


VERSION = 0.2.2
BIN = game
CC = gcc
CFLAGS = -Wall -g -I'D:\Progs\c\sdl\gin-tonic\include' -L./
LIBS = mingw32 SDL2main SDL2_ttf SDL2_mixer SDL2_image SDL2 GinTonic

REL_CFLAGS = -Wall -O2 -mwindows

run: build
	@echo -e '### Running... ###\n'
	@./${BIN}

build:
	@echo '### Building... ###\n'
	${CC} ${CFLAGS} -o ${BIN}.exe main.c src/*.c $(addprefix -l,${LIBS})

static:
	@echo '### Building Static... ###\n'
	${CC} -static ${CFLAGS} -o ${BIN}_static.exe main.c src/*.c $(addprefix -l,${LIBS})
