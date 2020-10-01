# Project: mapslang
# Makefile created by KeeperFX/dkfans team

# Instructions:
# To create the Windows executable run:
# make all
# To create the linux executable run:
# make all BUILD_FOR_LINUX=1 (not fully implemented)
# To create a debug executable, run with the command:
# make all DEBUG=1
# To clean up map and libadikted run:
# make clean
# To perform a normal clean, and a clean of libslang run:
# make clean-inc-libs

# Windows version can be built with:
# - Windows   : MSYS2 (MinGW 32-bit) - make
# - Windows   : MSYS2 (MinGW 32-bit) - mingw32-make
# - Windows   : MinGW (MSYS) - make
# - WSL/Linux : i686-w64-mingw32-gcc - make

# -static-libgcc is used to remove DLL dependancies (e.g. libgcc_s_dw2-1.dll, libwinpthread-1.dll, and libgcc_s_sjlj-1.dll)

# Host environment
ifneq (,$(findstring Windows,$(OS)))
  CROSS_EXEEXT = .exe
  ifneq ($(BUILD_FOR_LINUX), 1)
    SLANG_BUILDCOMMAND =
  else
    SLANG_BUILDCOMMAND =
  endif
else
  CROSS_EXEEXT =
  CROSS_COMPILE = i686-w64-mingw32-
  ifneq ($(BUILD_FOR_LINUX), 1)
    ## build slang-pre2.3.3-24/stc/mkfiles/mkmake.c for linux, only the .exe is provided in the slang package
    ## This is used to enable mingw32 DLL builds in linux
    SLANG_BUILDCOMMAND = cp ./src/slconfig.h ./src/config.h && \
                         gcc -I./src ./src/mkfiles/mkmake.c ./src/slprepr.c -o ./src/mkfiles/mkmake &&
  else
   SLANG_BUILDCOMMAND =
  endif
endif
# Executable file extensions on target environment
BUILD_FOR_LINUX ?= 0
ifeq ($(BUILD_FOR_LINUX), 1)
  EXEEXT =
  OTHEREXEEXT = .exe
  LIBEXT = .so
  OTHERLIBEXT = .dll
else
  EXEEXT = .exe
  OTHEREXEEXT =
  LIBEXT = .dll
  OTHERLIBEXT = .so
  # S-Lang MinGW32 Makefile creation:
  # -static-libgcc is also needed by libslang. So the 3 makefile.all files are patched.
  # (Along with tweaking 'del'/'rm' to 'rm -f' for the clean process, and allowing TOOL_PREFIX, 
  # CFLAGS, and LFLAGS to be passed to the compiler/linker from this Makefile.)
  SLANG_PLATFORM = WIN32 MINGW32 DLL
  SLANG_CFLAGS = -std=gnu89
  SLANG_LFLAGS = -static-libgcc
  SLANG_BUILDCOMMAND += \
  patch -r - -N src/mkfiles/makefile.all ../../mapslang/libslang-patches/slang-src-mingw32-makefile.patch && \
  patch -r - -N slsh/mkfiles/makefile.all ../../mapslang/libslang-patches/slang-slsh-mingw32-makefile.patch && \
  patch -r - -N modules/mkfiles/makefile.all ../../mapslang/libslang-patches/slang-modules-mingw32-makefile.patch && \
  TOOL_PREFIX='$(CROSS_COMPILE)' CFLAGS='$(SLANG_CFLAGS)' LFLAGS='$(SLANG_LFLAGS)' mkfiles/build.sh $(SLANG_PLATFORM)
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
CSTANDARD          = -std=gnu89
CC                 = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT) $(CSTANDARD) $(WARNFLAGS)
CFLAGS             = -c -O -I. -I../lib/$(SLANG_NAME)/src -DSLANG_DLL=1 $(PLATFORMFLAGS)
CFLAGS-DEBUG       = -c -g -I. -I../lib/$(SLANG_NAME)/src -DSLANG_DLL=1 $(PLATFORMFLAGS) -DDEBUG
LINK               = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT)
WINDRES            = $(CROSS_COMPILE)windres$(CROSS_EXEEXT)
RES                = obj/map_private.res
RC                 = map_private.rc
LFLAGS             = -static-libgcc $(PLATFORMFLAGS)
LFLAGS-DEBUG       = -static-libgcc $(PLATFORMFLAGS) -DDEBUG
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
STRIP              = strip --strip-all

.PHONY: pre-build post-build all clean

all: make-directories pre-build $(SRC) $(BIN) post-build

make-directories:
	$(MKDIR) ../lib bin obj

create-package: fill-pkg-directory

strip-executables:
	$(STRIP) pkg/map$(EXEEXT) pkg/$(LIBSLANG) pkg/$(LIBADIKTED)

fill-pkg-directory:
	$(MKDIR) pkg/docs
	$(CP) $(BIN) pkg/
	$(CP) ../libadikted/bin/$(LIBADIKTED) pkg/
	$(CP) ../lib/$(SLANG_NAME)/src/gw32objs/$(LIBSLANG) pkg/
	$(CP) map.hlp pkg/
	$(CP) map.ini pkg/
	$(CP) ../docs/* pkg/docs/

post-build: create-package

pre-build: $(LIBSLANG) $(LIBADIKTED)

clean-inc-libs: clean
	cd ../lib/$(SLANG_NAME) && make clean

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
	cd ../lib/$(SLANG_NAME) && $(SLANG_BUILDCOMMAND)
	-$(RM) ../lib/$(SLANG_NAME)/src/gw32objs/libslang.a

$(LIBADIKTED):
	cd ../libadikted && make
