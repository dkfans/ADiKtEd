# Project: libadikted
# Makefile created by KeeperFX/dkfans team

# Instructions:
# To create the Windows library run:
# make all
# To create the linux library run:
# make all BUILD_FOR_LINUX=1 (not fully implemented)
# To create a debug library, run with the command:
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
endif
# Executable files extension on target environment
BUILD_FOR_LINUX ?= 0
ifneq (,$(BUILD_FOR_LINUX))
  LIBEXT = .so
  OTHERLIBEXT = .dll
else
  LIBEXT = .dll
  OTHERLIBEXT = .so
endif
# Allow debug executables to be created
DEBUG ?= 0
ifeq ($(DEBUG), 1)
  DEBUGME = yes
else
  DEBUGME = no
endif
# Names of utility commands
WARNFLAGS          = -Wall -Wextra -Wno-pedantic -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare
PLATFORMFLAGS      = -march=pentium-mmx -mmmx
CSTANDARD          = -std=c89
CC                 = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT) $(CSTANDARD) $(WARNFLAGS)
CFLAGS             = -c -O -I. -DBUILD_DLL -fexpensive-optimizations -O3 $(PLATFORMFLAGS)
CFLAGS-DEBUG       = -c -g -I. -DBUILD_DLL $(PLATFORMFLAGS) -DDEBUG
LINK               = $(CROSS_COMPILE)gcc$(CROSS_EXEEXT)
WINDRES            = $(CROSS_COMPILE)windres$(CROSS_EXEEXT)
RES                = obj/adikted_private.res
RC                 = adikted_private.rc
LFLAGS             = -shared $(PLATFORMFLAGS)
LFLAGS-DEBUG       = -shared $(PLATFORMFLAGS) -DDEBUG
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
	$(RM) $(OBJ) $(BIN) $(BIN:$(LIBEXT)=$(OTHERLIBEXT)) $(RES)

$(BIN): $(OBJ) $(RES)
	$(LINK) $(if $(filter yes, $(DEBUGME)),$(LFLAGS-DEBUG),$(LFLAGS)) $(LIBS) -o $@ $(OBJ) $(RES)

obj/%.o: %.c
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS-DEBUG),$(CFLAGS)) $< -o $@

$(RES): $(RC)
	$(WINDRES) -i $< --input-format=rc -o $@ -O coff
