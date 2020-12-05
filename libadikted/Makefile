# Project: libadikted
# Makefile created by KeeperFX/dkfans team

# Instructions:
# To create the Windows library run:
# make all
# To create the linux library run:
# make all BUILD_FOR_LINUX=1 (not fully implemented)
# To create a debug library, run with the command:
# make all DEBUG=1 (un-tested)
# To clean up the built files (objects, library, and resource file) run:
# make clean

# Windows version can be built with:
# - Windows   : MSYS2 (MinGW 32-bit) - make
# - Windows   : MSYS2 (MinGW 32-bit) - mingw32-make
# - Windows   : MinGW (MSYS) - make
# - WSL/Linux : i686-w64-mingw32-gcc - make

# Executable files extension on host environment
ifneq (,$(findstring Windows,$(OS)))
  CROSS_EXEEXT = .exe
else
  CROSS_EXEEXT =
  CROSS_COMPILE = i686-w64-mingw32-
endif
# Executable files extension on target environment
BUILD_FOR_LINUX ?= 0
ifeq ($(BUILD_FOR_LINUX), 1)
  LIBEXT = .so
  OTHER_LIBEXT = .dll
  DLL_FLAG =
else
  LIBEXT = .dll
  OTHER_LIBEXT = .so
  DLL_FLAG = -DBUILD_DLL
endif
# Allow debug executables to be created
DEBUG ?= 0
ifeq ($(DEBUG), 1)
  DEBUGME = yes
else
  DEBUGME = no
endif
# Names of utility commands
WARNFLAGS          = -Wall -Wextra -pedantic -Werror-implicit-function-declaration -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare
PLATFORM_FLAGS      = -march=pentium-mmx -mmmx
CSTANDARD          = -std=gnu99
CC                 = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT) $(CSTANDARD) $(WARNFLAGS)
CFLAGS             = -c -O -I. $(DLL_FLAG) -fexpensive-optimizations -O3 $(PLATFORM_FLAGS)
CFLAGS_DEBUG       = -c -g -I. $(DLL_FLAG) $(PLATFORM_FLAGS) -DDEBUG
LINK               = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT)
WINDRES            = $(CROSS_COMPILE)windres$(CROSS_EXEEXT)
RES                = obj/adikted_private.res
RC                 = adikted_private.rc
LFLAGS             = -shared $(PLATFORM_FLAGS)
LFLAGS_DEBUG       = -shared $(PLATFORM_FLAGS) -DDEBUG
LIBS               = -L.
SRC                = \
arr_utils.c \
bulcommn.c \
dernc.c \
draw_map.c \
graffiti.c \
graffiti_font.c \
lbfileio.c \
lev_column.c \
lev_data.c \
lev_files.c \
lev_script.c \
lev_things.c \
libadi_main.c \
memfile.c \
msg_log.c \
obj_actnpts.c \
obj_column.c \
obj_column_def.c \
obj_column_per.c \
obj_slabs.c \
obj_things.c \
xcubtxtr.c \
xtabdat8.c \
xtabjty.c
OBJ                = $(addprefix obj/,$(SRC:.c=.o))
BIN                = bin/adikted$(LIBEXT)
RM                 = rm -f
MKDIR              = mkdir -p

.PHONY: pre-build post-build all clean

all: make-directories pre-build $(SRC) $(BIN) post-build

make-directories:
	$(MKDIR) bin obj

clean:
	$(RM) $(OBJ) $(BIN) $(BIN:$(LIBEXT)=$(OTHER_LIBEXT)) $(RES)

$(BIN): $(OBJ) $(RES)
	$(LINK) $(if $(filter yes, $(DEBUGME)),$(LFLAGS_DEBUG),$(LFLAGS)) $(LIBS) -o $@ $(OBJ) $(RES)

obj/%.o: %.c
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS_DEBUG),$(CFLAGS)) $< -o $@

$(RES): $(RC)
	$(WINDRES) -i $< --input-format=rc -o $@ -O coff
