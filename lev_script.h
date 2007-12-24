/*
 * lev_script.h header file for lev_script.c
 */

#ifndef ADIKT_LEVSCRIPT_H
#define ADIKT_LEVSCRIPT_H

struct LEVEL;

//Command types

#define CMD_UNKNOWN  0x000
#define CMD_OBJCTV   0x001
#define CMD_FLAG     0x002
#define CMD_TIMER    0x003
#define CMD_CUSTOBJ  0x004
#define CMD_SETUP    0x005
#define CMD_CRTRADJ  0x006
#define CMD_ADD      0x007
#define CMD_CONDIT   0x008
#define CMD_AVAIL    0x009
#define CMD_DOOR     0x00a
#define CMD_TRAP     0x00b
#define CMD_SPELL    0x00c
#define CMD_VARIBL   0x00d
#define CMD_OPERATR  0x00e
#define CMD_PLAYER   0x00f
#define CMD_CREATR   0x010
#define CMD_ROOM     0x011
#define CMD_COMP     0x012

#define CMD_ORIENT   0x0fe
#define CMD_ADIKTED  0x0ff

//Specific commands

//Objectives of creatures in partys
#define OBJCTV_DEFEND_PARTY   0x009
#define OBJCTV_ATTACK_ROOMS   0x008
#define OBJCTV_ATTACK_DNHEART 0x007
#define OBJCTV_ENEMIES        0x006
#define OBJCTV_STEAL_SPELLS   0x005
#define OBJCTV_STEAL_GOLD     0x004
//Heading objectives for tunneller partys
#define OBJCTV_APPROP_DUNG    0x003
#define OBJCTV_DNHEART        0x002
#define OBJCTV_DUNGEON        0x001
#define OBJCTV_ACTNPT         0x000
//Variables - Flags
#define FLAG7  0x007
#define FLAG6  0x006
#define FLAG5  0x005
#define FLAG4  0x004
#define FLAG3  0x003
#define FLAG2  0x002
#define FLAG1  0x001
#define FLAG0  0x000
//Variables - Timers
#define TIMER7 0x007
#define TIMER6 0x006
#define TIMER5 0x005
#define TIMER4 0x004
#define TIMER3 0x003
#define TIMER2 0x002
#define TIMER1 0x001
#define TIMER0 0x000
//Custom objectives
#define CREATR_SWAP          0x008
#define QUICK_INFO           0x007
#define QUICK_OBJECTV        0x006
#define BONUS_LEV_TIME       0x005
#define TUTORIAL_FLASH_BTN   0x004
#define DISPLAY_INFO_WPOS    0x003
#define DISPLAY_INFO         0x002
#define DISPLAY_OBJECTV_WPOS 0x001
#define DISPLAY_OBJECTV      0x000
//Setup commands
#define DEAD_CREATURES_RET_TO_POOL 0x005
#define ALLY_PLAYERS         0x004
#define SET_COMPUTER_PROCESS 0x003
#define SET_COMPUTER_EVENT   0x002
#define SET_COMPUTER_CHECKS  0x001
#define SET_COMPUTER_GLOBALS 0x000
//Creatures adjusting
#define SET_CREATR_FEAR      0x004
#define SET_CREATR_ARMOUR    0x003
#define SET_CREATR_HEALTH    0x002
#define SET_CREATR_STRENGTH  0x001
#define SET_CREATR_MAX_LEVEL 0x000
//Commands - add
#define ADD_CREATR_TO_POOL   0x006
#define ADD_TUNNELLER_PARTY_TOLEV 0x005
#define ADD_CREATR_TOLEV     0x004
#define ADD_PARTY_TOLEV      0x003
#define ADD_TO_PARTY         0x002
#define ADD_TUNNELER_TOLEV   0x001
#define CREATE_PARTY         0x000

//Commands - conditions
#define IF_AVAILABLE         0x002
#define COND_ENDIF           0x001
#define IF_ACTNPT            0x000

//Commands - availability
#define DOOR_AVAIL           0x004
#define TRAP_AVAIL           0x003
#define MAGIC_AVAIL          0x002
#define CREATR_AVAIL         0x001
#define ROOM_AVAIL           0x000

//Commands
#define SET_MUSIC            0x000
#define RESET_ACTNPT         0x000
#define NEXT_CMD_REUSABLE    0x000
#define MAX_CREATURES        0x000
#define SET_FLAG             0x000
#define GAME_LOSE            0x000
#define GAME_WIN             0x000
#define SET_TIMER            0x000
#define COMP_PLAYER          0x000
#define START_MONEY          0x000
#define REM                  0x000
#define SET_GEN_SPEED        0x000
#define SET_HATE             0x000
#define CREATURE             0x000
#define ROOM                 0x000

//Note: Doors, Traps, Spells are defined elswhere

//Variables
#define ALL_DNGNS_DESTROYED  0x000
#define CRTRS_SCAVNG_GAINED  0x000
#define CRTRS_SCAVNG_LOST    0x000
#define DOORS_DESTROYED      0x000
#define TOTAL_GOLD_MINED     0x000
#define DNGN_DESTROYED       0x000
#define GOLD_POTS_STOLEN     0x000
#define ITEMS_BROKEN_INTO    0x000
#define SPELLS_STOLEN        0x000
#define ROOMS_DESTROYED      0x000
#define BATTLES_WON          0x000
#define BATTLES_LOST         0x000
#define CREATRS_ANNOYED      0x000
#define TOTAL_CREATRS_LEFT   0x000
#define TOTAL_AREA           0x000
#define TOTAL_DOORS          0x000
#define TOTAL_RESEARCH       0x000
#define TOTAL_CREATRS        0x000
#define TOTAL_IMPS           0x000
#define BREAK_IN             0x000
#define GAME_TURN            0x000
#define MONEY                0x000
//Operators
#define OPER_EQU             0x000
#define OPER_NEQ             0x000
#define OPER_LESS            0x000
#define OPER_GRTR            0x000
#define OPER_LESSEQ          0x000
#define OPER_GRTREQ          0x000

//Note: Players, Creatures, Rooms are defined elswhere

//Computer player tweaks (undocumented)
#define COMP_SELL_TRAPSDOORS 0x000
#define COMP_ATTACK_MAGIC    0x000
#define COMP_WAIT_FOR_BRIDGE 0x000
#define COMP_MAGIC_SPEED_UP  0x000
#define COMP_DIG_TO_NEUTRAL  0x000
#define COMP_SLAP_IMPS       0x000
#define COMP_MOVE_CREATRS_TODEFENS 0x000
#define COMP_MOVE_CREATRS_TOPOS    0x000
#define COMP_MOVE_CREATRS_TOROOM   0x000
#define COMP_PICKUP_FOR_ATTACK     0x000
#define COMP_MAGIC_CALLARMS  0x000
#define COMP_DIG_TOATTACK    0x000
#define COMP_DIG_TOGOLD      0x000
#define COMP_DIG_TOENTRNCE   0x000
#define COMP_PLACE_ROOM      0x000
#define COMP_CHECK_ROOM_DUG  0x000
#define COMP_DIG_ROOM        0x000
#define COMP_DIG_ROOM_PASSAGE 0x000

//ADiKtEd specific commands
#define CUSTOM_COLUMN        0x001
#define DEFINE_GRAFFITI      0x002

struct DK_SCRIPT_COMMAND {
    int group;
    int index;
    unsigned char **params;
    int param_count;
  };

//Functions for script analysis
short decompose_script_command(struct DK_SCRIPT_COMMAND *cmd,char *text);
//Functions for Adikted script execution
short execute_script_line(struct LEVEL *lvl,char *line,char *err_msg);

//Working with text files
void text_file_free(char **lines,int lines_count);
int text_file_line_add(char ***lines,int *lines_count,char *text);
int text_file_linecp_add(char ***lines,int *lines_count,char *text);

//Lower level - for obtaining index of a command
int adikted_cmd_index(char *cmdtext);
const char *adikted_cmd_text(int cmdidx);
int orient_cmd_index(char *cmdtext);
const char *orient_cmd_text(int cmdidx);

//Other lower level functions
short script_command_param_add(struct DK_SCRIPT_COMMAND *cmd,char *param);
void script_command_clear(struct DK_SCRIPT_COMMAND *cmd);
void script_command_free(struct DK_SCRIPT_COMMAND *cmd);
char *get_orientation_shortname(unsigned short orient);
char *get_font_longname(unsigned short font);

//Lower level - executing commands
short execute_adikted_command(struct LEVEL *lvl,struct DK_SCRIPT_COMMAND *cmd,char *err_msg);

#endif // ADIKT_LEVSCRIPT_H
