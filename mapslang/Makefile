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

CC				= $(CROSS_COMPILE)gcc
CPP				= $(CROSS_COMPILE)g++
CFLAGS                          = -c -O -I. -I../lib/$(SLANG_NAME)/src
CFLAGS-DEBUG                    = -c -g -I. -I../lib/$(SLANG_NAME)/src
LINK				= $(CROSS_COMPILE)gcc
LFLAGS                          = 
LFLAGS-DEBUG                    = 
LIBS				= -L. -L../libadikted -L../lib/$(SLANG_NAME)/src/gw32objs -lslang -ladikted
SRC				= \
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

OBJ				= $(SRC:.c=.o)
BIN				= map
RM				= rm -f
ECHO                            = echo

all: pre-build $(SRC) $(BIN)

$(BIN): $(OBJ)
	$(LINK) $(if $(filter yes, $(DEBUGME)),$(LFLAGS-DEBUG),$(LFLAGS)) $(LIBS) -o $@ $(OBJ)

.c.o:
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS-DEBUG),$(CFLAGS)) $< -o $@

pre-build: ../lib/slang.ok

../lib/slang.tgz:
	curl -L -o $@ $(SLANG_URL)

../lib/slang.ok: ../lib/slang.tgz
	-tar -C ../lib/ -xzf $<
	cd ../lib/$(SLANG_NAME) && mkfiles/build.sh WIN32 MINGW32 && make
	touch $@

clean:
	${RM} $(OBJ)
	${RM} $(BIN)
