#
#	SDL Makefile
#


VERSION = 1.0.0
BIN = Astral_Fermata
CC = gcc
CFLAGS = -Wall -g -I'D:\Progs\c\sdl\gin-tonic\include' -L./
LIBS = mingw32 SDL2main SDL2_ttf SDL2_mixer SDL2_image SDL2 GinTonic
DIAS = $(patsubst %.xml,%.dbf,$(wildcard assets/txt/*.xml))

REL_CFLAGS = -Wall -O2 -I'D:\Progs\c\sdl\gin-tonic\include' -L./ -mwindows

run: build
	@echo -e '\n### Running... ###\n'
	@./${BIN}

build: $(DIAS)
	@echo -e '\n### Building... ###\n'
	${CC} ${CFLAGS} -o ${BIN}.exe main.c src/*.c $(addprefix -l,${LIBS})

release: $(DIAS)
	@echo -e '\n### Building Release... ###\n'
	${CC} ${REL_CFLAGS} -o ${BIN}_v${VERSION}.exe main.c src/*.c $(addprefix -l,${LIBS})

clean:
	@echo -e '\n### Cleaning Dialogue Files... ###\n'
	rm -f $(DIAS)
	#@echo -e '\n### Cleaning Userdata... ###\n'
	#rm -f userdata.dbf

assets/txt/%.dbf:
	@dialogue_comp/xdc.exe $(patsubst %.dbf,%.xml,$@) $@

static:
	@echo -e '\n### Building Static... ###\n'
	${CC} -static ${CFLAGS} -o ${BIN}_static.exe main.c src/*.c $(addprefix -l,${LIBS})
