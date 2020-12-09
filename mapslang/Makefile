# Project: mapslang
# Makefile created by KeeperFX/dkfans team

# Instructions:
# To create the Windows executable run:
# make all
# To create the linux executable run:
# make all BUILD_FOR_LINUX=1 (not fully implemented)
# To create a debug executable, run with the command:
# make all DEBUG=1 (un-tested)
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
else
  CROSS_EXEEXT =
  CROSS_COMPILE = i686-w64-mingw32-
  ifneq ($(BUILD_FOR_LINUX), 1)
    ## build slang-pre2.3.3-24/stc/mkfiles/mkmake.c for linux, only the .exe is provided in the slang package
    ## This is used to enable mingw32 DLL builds in linux
    SLANG_BUILDCOMMAND = cp ./src/slconfig.h ./src/config.h && \
                         gcc -I./src ./src/mkfiles/mkmake.c ./src/slprepr.c -o ./src/mkfiles/mkmake &&
  else
    SLANG_BUILDCOMMAND = ./configure && make
  endif
endif
# Executable file extensions on target environment
BUILD_FOR_LINUX ?= 0
ifeq ($(BUILD_FOR_LINUX), 1)
  EXEEXT =
  OTHER_EXEEXT = .exe
  LIBEXT = .so
  OTHER_LIBEXT = .dll
  DLL_FLAG =
  SLANG_OBJ_FOLDER = elfobjs
else
  EXEEXT = .exe
  OTHER_EXEEXT =
  LIBEXT = .dll
  OTHER_LIBEXT = .so
  DLL_FLAG = -DSLANG_DLL=1
  SLANG_OBJ_FOLDER = gw32objs
  # S-Lang MinGW32 Makefile creation:
  # -static-libgcc is also needed by libslang. So the 3 makefile.all files are patched.
  # (Along with tweaking 'del'/'rm' to 'rm -f' for the clean process, and allowing TOOL_PREFIX, 
  # CFLAGS, and LFLAGS to be passed to the compiler/linker from this Makefile.)
  SLANG_PLATFORM = WIN32 MINGW32 DLL
  SLANG_CFLAGS = -std=gnu89
  SLANG_LFLAGS = -static-libgcc
  SLANG_RC_FOLDER = ../../../mapslang/libslang-patches
  SLANG_BUILDCOMMAND += \
  patch -r - -N src/mkfiles/makefile.all ../../mapslang/libslang-patches/slang-src-mingw32-makefile.patch && \
  patch -r - -N slsh/mkfiles/makefile.all ../../mapslang/libslang-patches/slang-slsh-mingw32-makefile.patch && \
  patch -r - -N modules/mkfiles/makefile.all ../../mapslang/libslang-patches/slang-modules-mingw32-makefile.patch && \
  TOOL_PREFIX='$(CROSS_COMPILE)' CFLAGS='$(SLANG_CFLAGS)' LFLAGS='$(SLANG_LFLAGS)' SLANG_RC_FOLDER='$(SLANG_RC_FOLDER)' mkfiles/build.sh $(SLANG_PLATFORM)
endif
# Allow debug executables to be created
DEBUG ?= 0
ifeq ($(DEBUG), 1)
  DEBUGME = yes
else
  DEBUGME = no
endif
# Names of utility commands
SLANG_URL          = https://www.jedsoft.org/releases/slang/slang-2.3.2.tar.bz2
SLANG_NAME         = slang-2.3.2
SLANG_ARCHIVE      = ../lib/slang.tar.bz2
SLANG_BIN_FOLDER   = /lib/$(SLANG_NAME)/src/$(SLANG_OBJ_FOLDER)
WARNFLAGS          = -Wall -Wextra -Werror-implicit-function-declaration -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare
PLATFORM_FLAGS     = -march=i486
CSTANDARD          = -std=gnu99
CC                 = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT) $(CSTANDARD) $(WARNFLAGS)
CFLAGS             = -c -O -I. -I../lib/$(SLANG_NAME)/src $(DLL_FLAG) $(PLATFORM_FLAGS)
CFLAGS_DEBUG       = -c -g -I. -I../lib/$(SLANG_NAME)/src $(DLL_FLAG) $(PLATFORM_FLAGS) -DDEBUG
LINK               = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT)
WINDRES            = $(CROSS_COMPILE)windres$(CROSS_EXEEXT)
RES                = obj/map_private.res
RC                 = map_private.rc
LFLAGS             = -static-libgcc $(PLATFORM_FLAGS)
LFLAGS_DEBUG       = -static-libgcc $(PLATFORM_FLAGS) -DDEBUG
LIBS               = -L. -L../libadikted/bin -L..$(SLANG_BIN_FOLDER) -lslang -ladikted
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
LIB_SLANG          = libslang$(LIBEXT)
LIB_ADIKTED        = adikted$(LIBEXT)
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
	$(STRIP) pkg/map$(EXEEXT) pkg/$(LIB_SLANG) pkg/$(LIB_ADIKTED)

fill-pkg-directory:
	$(MKDIR) pkg/docs
	$(CP) $(BIN) pkg/
	$(CP) ../libadikted/bin/$(LIB_ADIKTED) pkg/
	$(CP) ..$(SLANG_BIN_FOLDER)/$(LIB_SLANG) pkg/
	$(CP) map.hlp pkg/
	$(CP) map.ini pkg/
	$(CP) ../docs/* pkg/docs/

post-build: create-package

pre-build: $(LIB_SLANG) $(LIB_ADIKTED)

clean-inc-libs: clean
	if [ -e ../lib/$(SLANG_NAME) ]; then cd ../lib/$(SLANG_NAME) && make clean; fi

clean:
	$(RM) $(OBJ) $(BIN) $(BIN:$(EXEEXT)=$(OTHER_EXEEXT)) $(RES)
	$(RM)r pkg/*
	cd ../libadikted && make clean

$(BIN): $(OBJ) $(RES)
	$(LINK) $(if $(filter yes, $(DEBUGME)),$(LFLAGS_DEBUG),$(LFLAGS)) $(LIBS) -o $@ $(OBJ) $(RES)

obj/%.o: %.c
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS_DEBUG),$(CFLAGS)) $< -o $@

$(RES): $(RC)
	$(WINDRES) -i $< --input-format=rc -o $@ -O coff

$(SLANG_ARCHIVE):
	curl -f -L -o $@ $(SLANG_URL)

$(LIB_SLANG): $(SLANG_ARCHIVE)
	-tar -C ../lib/ -xf $<
	cd ../lib/$(SLANG_NAME) && $(SLANG_BUILDCOMMAND)
	-$(RM) ..$(SLANG_BIN_FOLDER)/libslang.a

$(LIB_ADIKTED):
	cd ../libadikted && make
