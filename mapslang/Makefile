# Project: mapslang
# Makefile created by KeeperFX/dkfans team

# Instructions:
# To create the Windows executable run:
# make all
# To create the linux executable run:
# make all BUILD_FOR_LINUX=1 (not fully implemented)
# To create a debug executable, run with the command:
# make all DEBUG=1
# To clean up the built files (objects, library, and resource file) run:
# make clean

# Tested and working with:
# - Windows : MSYS2 (MinGW 32-bit) - make
# - Windows : MSYS2 (MinGW 32-bit) - mingw32-make
# - Windows : MinGW (MSYS) - make

# Executable files extension on host environment
ifneq (,$(findstring Windows,$(OS)))
  CROSS_EXEEXT = .exe
else
  CROSS_EXEEXT =
  CROSS_COMPILE = i686-w64-mingw32-
  SLANG_HOST = mingw32
endif
# Executable files extension on target environment
BUILD_FOR_LINUX ?= 0
ifeq ($(BUILD_FOR_LINUX), 1)
  EXEEXT =
  OTHEREXEEXT = .exe
  LIBEXT = .so
  OTHERLIBEXT = .dll
  SLANGPLATFORM =
else
  EXEEXT = .exe
  OTHEREXEEXT =
  LIBEXT = .dll
  OTHERLIBEXT = .so
  SLANGPLATFORM = WIN32 MINGW32 DLL
endif
# Allow debug executables to be created
DEBUG ?= 0
ifeq ($(DEBUG), 1)
  DEBUGME = yes
else
  DEBUGME = no
endif
# Names of utility commands
SLANG_URL          = https://www.jedsoft.org/snapshots/slang-pre2.3.3-24.tar.gz
SLANG_NAME         = slang-pre2.3.3-24
WARNFLAGS          = -Wall -Wextra -Wno-pedantic -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare
PLATFORMFLAGS      = -march=i486
CSTANDARD          = -std=c18
CC                 = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT) $(CSTANDARD) $(WARNFLAGS)
CFLAGS             = -c -O -I. -I../lib/$(SLANG_NAME)/src -DSLANG_DLL=1 $(PLATFORMFLAGS)
CFLAGS-DEBUG       = -c -g -I. -I../lib/$(SLANG_NAME)/src -DSLANG_DLL=1 $(PLATFORMFLAGS) -DDEBUG
LINK               = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT)
WINDRES            = $(CROSS_COMPILE)windres$(CROSS_EXEEXT)
RES                = obj/map_private.res
RC                 = map_private.rc
LFLAGS             = $(PLATFORMFLAGS)
LFLAGS-DEBUG       = $(PLATFORMFLAGS) -DDEBUG
LIBS               = -L. -L../libadikted/bin -L../lib/$(SLANG_NAME)/src/gw32objs -lslang -ladikted
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
OBJ                = $(addprefix obj/,$(SRC:.c=.o))
BIN                = bin/map$(EXEEXT)
LIBSLANG           = libslang$(LIBEXT)
LIBADIKTED         = adikted$(LIBEXT)
RM                 = rm -f
MKDIR              = mkdir -p
CP                 = cp -f

.PHONY: pre-build post-build all clean

all: make-directories pre-build $(SRC) $(BIN) fill-pkg-directory post-build

make-directories:
	$(MKDIR) bin obj pkg/docs

fill-pkg-directory:
	$(CP) $(BIN) pkg/
	$(CP) ../libadikted/bin/$(LIBADIKTED) pkg/
	$(CP) ../lib/$(SLANG_NAME)/src/gw32objs/$(LIBSLANG) pkg/
	$(CP) map.hlp pkg/
	$(CP) map.ini pkg/
	$(CP) ../docs/* pkg/docs/

post-build:

pre-build: $(LIBSLANG) $(LIBADIKTED)

clean:
	$(RM) $(OBJ) $(BIN) $(BIN:$(EXEEXT)=$(OTHEREXEEXT)) $(RES)
	$(RM)r pkg/*
	cd ../libadikted && make clean

$(BIN): $(OBJ) $(RES)
	$(LINK) $(if $(filter yes, $(DEBUGME)),$(LFLAGS-DEBUG),$(LFLAGS)) $(LIBS) -o $@ $(OBJ) $(RES)

obj/%.o: %.c
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS-DEBUG),$(CFLAGS)) $< -o $@

$(RES): $(RC)
	$(WINDRES) -i $< --input-format=rc -o $@ -O coff

../lib/slang.tgz:
	curl -L -o $@ $(SLANG_URL)

$(LIBSLANG): ../lib/slang.tgz
	-tar -C ../lib/ -xzf $<
	cd ../lib/$(SLANG_NAME) && mkfiles/build.sh $(SLANGPLATFORM)
	-${RM} ../lib/$(SLANG_NAME)/src/gw32objs/libslang.a

$(LIBADIKTED):
	cd ../libadikted && make
