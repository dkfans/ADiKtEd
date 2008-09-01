# Project: adikted

#To create a debug executable, run with the command:
# make DEBUG

CC				= gcc
CFLAGS                          = -c -O -I.
CFLAGS-DEBUG                    = -c -g -I.
LINK				= gcc
LFLAGS                          = 
LFLAGS-DEBUG                    = 
LIBS				= -L. -lslang -ladikted
SRC				= main.c input_kb.c scr_help.c output_scr.c scr_thing.c scr_list.c scr_slab.c scr_clm.c scr_actn.c scr_txted.c scr_rwrk.c scr_cube.c var_utils.c
OBJ				= $(SRC:.c=.o)
BIN				= map
RM				= rm -f

all: $(SRC) $(BIN)

$(BIN): $(OBJ)
	$(LINK) $(if $(filter yes, $(DEBUGME)),$(LFLAGS-DEBUG),$(LFLAGS)) -o $@ $(OBJ) $(LIBS)

.c.o:
	$(CC) $(if $(filter yes, $(DEBUGME)),$(CFLAGS-DEBUG),$(CFLAGS)) $< -o $@

clean:
	${RM} $(OBJ)
	${RM} $(BIN)
