# Project: libadikted

#To create a debug executable, run with the command:
# make DEBUG

#CROSS_COMPILE = i686-w64-mingw32-


CC           = $(CROSS_COMPILE)gcc
LINK         = $(CROSS_COMPILE)gcc
SRC          = \
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

OBJ          = $(SRC:.c=.o)
LIBS         = -L. 
# --no-export-all-symbols --add-stdcall-alias
CFLAGS       = -c -O -I. -DBUILD_DLL
CFLAGS-DEBUG = -c -g -I. -DBUILD_DLL
LFLAGS       = -shared
LFLAGS-DEBUG = -shared
BIN          = adikted.dll
RM           = rm -f

all: $(SRC) $(BIN)

clean:
	${RM} $(OBJ)
	${RM} $(BIN)

$(BIN): $(OBJ)
	$(LINK) $(if $(filter yes, $(DEBUGME)),$(LFLAGS-DEBUG),$(LFLAGS)) $(LIBS) -o $@ $(OBJ)

.c.o:
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS-DEBUG),$(CFLAGS)) $< -o $@
