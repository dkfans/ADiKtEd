# Project: adikted

#To create a debug executable, run with the command:
# make DEBUG

#ON_LINUX = 1

ifneq (,$(ON_LINUX))
  CROSS_COMPILE = i686-w64-mingw32-
  SLANG_HOST=mingw32
endif

SLANG_URL = https://www.jedsoft.org/snapshots/slang-pre2.3.3-24.tar.gz
SLANG_NAME=slang-pre2.3.3-24

CC                = $(CROSS_COMPILE)gcc
CPP                = $(CROSS_COMPILE)g++
CFLAGS                          = -c -O -I. -I../lib/$(SLANG_NAME)/src -DSLANG_DLL=1
CFLAGS-DEBUG                    = -c -g -I. -I../lib/$(SLANG_NAME)/src -DSLANG_DLL=1
LINK                = $(CROSS_COMPILE)gcc
WINDRES                         = $(CROSS_COMPILE)windres
LFLAGS                          = 
LFLAGS-DEBUG                    = 
LIBS                = -L. -L../libadikted -L../lib/$(SLANG_NAME)/src/gw32objs -lslang -ladikted
SRC                = \
input_kb.c \
main.c \
output_scr.c \
scr_actn.c \
scr_clm.c \
scr_cube.c \
scr_help.c \
scr_list.c \
scr_rwrk.c \
scr_slab.c \
scr_thing.c \
scr_txted.c \
scr_txtgen.c \
textmenu.c \
var_utils.c

OBJ                = $(SRC:.c=.o)
RES                = map_private.res
BIN                = map
RM                 = rm -f
ECHO               = echo

.PHONY: pre-build post-build all clean

all: pre-build $(SRC) $(BIN) post-build

$(BIN): $(OBJ) $(RES)
	$(LINK) $(if $(filter yes, $(DEBUGME)),$(LFLAGS-DEBUG),$(LFLAGS)) $(LIBS) -o $@ $(OBJ) $(RES)

.c.o:
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS-DEBUG),$(CFLAGS)) $< -o $@

post-build:

pre-build: libslang.dll adikted.dll

../lib/slang.tgz:
	curl -L -o $@ $(SLANG_URL)

libslang.dll: ../lib/slang.tgz
	-tar -C ../lib/ -xzf $<
	cd ../lib/$(SLANG_NAME) && mkfiles/build.sh WIN32 MINGW32 DLL
	-${RM} ../lib/$(SLANG_NAME)/src/gw32objs/libslang.a
	-cp ../lib/$(SLANG_NAME)/src/gw32objs/libslang.dll ./

adikted.dll:
	cd ../libadikted && make
	-cp ../libadikted/adikted.dll ./

map_private.res: map_private.rc
	$(WINDRES) -i $< --input-format=rc -o $@ -O coff

clean:
	${RM} $(OBJ)
	${RM} $(BIN)
	${RM} *.dll

