#To create a debug executable, run with the command:
# make DEBUG

CC				= gcc
CFLAGS			= -c -O -I. -DBUILDING_DLL=1
CFLAGS-DEBUG	= -c -g -I. -DBUILDING_DLL=1
LINK				= gcc
LFLAGS			= 
LFLAGS-DEBUG	= 
LIBS				= -L.
SRC				= bulcommn.c dernc.c draw_map.c graffiti.c graffiti_font.c lev_column.c lev_data.c lev_files.c lev_script.c lev_things.c memfile.c obj_actnpts.c obj_column.c obj_column_def.c obj_column_per.c obj_slabs.c obj_things.c xcubtxtr.c xtabdat8.c xtabjty.c msg_log.c arr_utils.c
OBJ				= $(SRC:.c=.o)
DLLWRAP				= dllwrap.exe
DEFFILE				= libadikted.def
STATICLIB			= libadikted.a
BIN				= libadikted.dll
RM				= rm -f

all: $(SRC) $(BIN)

$(BIN): $(OBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --implib $(STATICLIB) $(OBJ) $(LIBS) -o $(BIN)

.c.o:
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS-DEBUG),$(CFLAGS)) $< -o $@

clean:
	${RM} $(OBJ)
	find . -name \*~ -print | xargs ${RM}
	find . -name \*.a -print | xargs ${RM}
	find . -name \*.dep -print | xargs ${RM}
	${RM} $(BIN)
