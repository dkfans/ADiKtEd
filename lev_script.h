/******************************************************************************/
// lev_script.h - Dungeon Keeper Tools.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  24 Jan 2008

// Purpose:
//   Header file. Defines exported routines from lev_script.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_LEVSCRIPT_H
#define ADIKT_LEVSCRIPT_H

struct LEVEL;

//Command types

enum cmd_groups {
    // The main commands - at start of the line
    CMD_UNKNOWN = 0x000,
    CMD_CONDIT  = 0x001,
    CMD_PARTY   = 0x002,
    CMD_AVAIL   = 0x003,
    CMD_CUSTOBJ = 0x004,  //custom objective/info
    CMD_SETUP   = 0x005,
    CMD_TRIGER  = 0x006,
    CMD_CRTRADJ = 0x007,
    CMD_COMMNT  = 0x008,  // REMs and empty lines
    CMD_OBSOLT  = 0x009,  // commands that worked only in beta or before

    // Command parameters
    CMD_PAROBJ  = 0x010,  //party objective
    CMD_FLAG    = 0x012,
    CMD_TIMER   = 0x013,
    CMD_DOOR    = 0x014,
    CMD_TRAP    = 0x015,
    CMD_SPELL   = 0x016,
    CMD_VARIBL  = 0x017,
    CMD_OPERATR = 0x018,
    CMD_CREATR  = 0x019,
    CMD_ROOM    = 0x01a,
    CMD_COMP    = 0x01b,
    CMD_PLAYER  = 0x01c,
    CMD_SPECIAL = 0x01d,
    CMD_OBJTYPE = 0x01e, // type of object; is it spell, room or what

    // Adikted - specific
    CMD_ORIENT  = 0x0fe,
    CMD_ADIKTED = 0x0ff,
};

//Specific commands

// A = Argument (text value). E.g: PLAYER0, TREASURE...
// N = Numerical (number value). E.g: 200, 10...

//Commands - conditions
//Setup commands
enum cmnds_condit {
// 5 - IF([player],[variable][comparison][a]) AAAN
    COND_IF             = 0x001,
// 65 - IF_AVAILABLE([player],[name][comparison][a]) AAAN
    IF_AVAILABLE        = 0x002,
// 18 - IF_ACTION_POINT([action point],[player]) NA
    IF_ACTNPT           = 0x003,
// 6 - ENDIF
    COND_ENDIF          = 0x004,
};

//Commands - partys
enum cmnds_party {
// 0 - CREATE_PARTY([party name]) A
    CREATE_PARTY             = 0x001,
// 19 - ADD_TUNNELLER_TO_LEVEL([player],[action point],[head for],[action point/target player],[experience],[gold]) AAANNN
    ADD_TUNNELER_TOLEV       = 0x002,
// 1 - ADD_TO_PARTY([party name],[creature],[experience],[gold],[objective],[Countdown]) AANNAN
    ADD_TO_PARTY             = 0x003,
// 2 - ADD_PARTY_TO_LEVEL([player],[name],[action point],[a]) AAAN
    ADD_PARTY_TOLEV          = 0x004,
// 3 - ADD_CREATURE_TO_LEVEL([player],[creature],[action point],[a],[experience],[gold]) AAANNN
    ADD_CREATR_TOLEV         = 0x005,
//39 - ADD_TUNNELLER_PARTY_TO_LEVEL([player],[party name],[action point],[head for],[action point/target player],[experience],[gold]) AAAANNN
    ADD_TUNNELLER_PARTY_TOLEV= 0x006,
};

//Commands - availability
enum cmnds_avail {
// 11 - ROOM_AVAILABLE([player],[room],[can be available],[is available]) AANN
    ROOM_AVAIL          = 0x001,
// 12 - CREATURE_AVAILABLE([player],[creature],[can be available],[is available]) AANN
    CREATR_AVAIL        = 0x002,
// 13 - MAGIC_AVAILABLE([player],[spell],[can be available],[is available]) AANN
    MAGIC_AVAIL         = 0x003,
// 14 - TRAP_AVAILABLE([player],[trap],[can be available],[number available]) AANN
    TRAP_AVAIL          = 0x004,
// 29 - DOOR_AVAILABLE([player],[door],[can be available],[number available]) AANN
    DOOR_AVAIL          = 0x005,
};

//Custom objectives and info boxes
enum cmnds_custobj {
// 36 - DISPLAY_OBJECTIVE([a],[player]) NA
    DISPLAY_OBJECTV    = 0x001,
// 64 - DISPLAY_OBJECTIVE_WITH_POS([a],[X],[Y]) NNN
    DISPLAY_OBJECTV_WPOS=0x002,
//37 - DISPLAY_INFORMATION([a]) N
    DISPLAY_INFO       = 0x003,
//37 (mistake?) - DISPLAY_INFORMATION_WITH_POS([a],[x],[y]) NNN
    DISPLAY_INFO_WPOS  = 0x004,
// 74 - BONUS_LEVEL_TIME([a]) N
    BONUS_LEV_TIME     = 0x005,
// 53 - TUTORIAL_FLASH_BUTTON([button],[player]) NN
    TUTORIAL_FLASH_BTN = 0x006,
// 43 - QUICK_OBJECTIVE([a],["objective"],[player]) NAA
    QUICK_OBJECTV      = 0x007,
// 44 - QUICK_INFORMATION([a],["information"]) NA
    QUICK_INFO         = 0x008,
};

//Setup commands
enum cmnds_setup {
// 8 - SET_GENERATE_SPEED([a]) N
    SET_GEN_SPEED       = 0x001,
// 10 - START_MONEY([player],[a]) AN
    START_MONEY         = 0x002,
// 15 - RESEARCH([player],[research type],[room or spell],[a]) AAAN
    RESEARCH            = 0x003,
// 16 - COMPUTER_PLAYER([player],[a]) AN
    COMP_PLAYER         = 0x004,
// 71 - ALLY_PLAYERS([player],[player]) AA
    ALLY_PLAYERS        = 0x005,
// 7 - SET_HATE([a],[a],[a]) NNN
    SET_HATE            = 0x006,
// 67 - SET_COMPUTER_GLOBALS([player],[a],[a],[a],[a],[a],[a]) ANNNNNN
    SET_COMPUTER_GLOBALS= 0x007,
// 68 - SET_COMPUTER_CHECKS([player],["ChecksName"],[CheckEvery],[AttackPercent],[CTADuration],[MinForAttack],[LastCheck]) AANNNNN
    SET_COMPUTER_CHECKS = 0x008,
// 69 - SET_COMPUTER_EVENT([player],["EventName"],[Data1],[Data2]) AANN
    SET_COMPUTER_EVENT  = 0x009,
// 70 - SET_COMPUTER_PROCESS([player],["ProcessName"],[Priority],[Data1],[Data2],[Data3],[Data4]) AANNNNN
    SET_COMPUTER_PROCESS= 0x00a,
// 25 - MAX_CREATURES([player],[a]) AN
    MAX_CREATURES       = 0x00b,
};

// Game action control/trigger commands
enum cmnds_triger {
// 41 - RESET_ACTION_POINT([action point]) N
    RESET_ACTNPT        = 0x001,
// 26 - NEXT_COMMAND_REUSABLE
    NEXT_CMD_REUSABLE   = 0x002,
// 24 - SET_FLAG([player],[flag],[a]) AAN
    SET_FLAG            = 0x003,
// 17 - SET_TIMER([player],[timer]) AA
    SET_TIMER           = 0x004,
// 21 - LOSE_GAME
    GAME_LOSE           = 0x005,
// 20 - WIN_GAME
    GAME_WIN            = 0x006,
// 59 - SET_MUSIC([a]) N
    SET_MUSIC           = 0x007,
};

//Creatures adjusting
enum cmnds_crtradj {
// 72 - DEAD_CREATURES_RETURN_TO_POOL([a]) N
    DEAD_CREATURES_RET_TO_POOL=0x001,
// 40 - ADD_CREATURE_TO_POOL([creature],[a]) AN
    ADD_CREATR_TO_POOL = 0x002,
// 58 - SET_CREATURE_MAX_LEVEL([player],[creature],[max experience]) AAN
    SET_CREATR_MAX_LEVEL=0x003,
// 61 - SET_CREATURE_STRENGTH([creature],[a]) AN
    SET_CREATR_STRENGTH= 0x004,
// 60 - SET_CREATURE_HEALTH([creature],[a]) AN
    SET_CREATR_HEALTH  = 0x005,
// 62 - SET_CREATURE_ARMOUR([creature],[a]) AN
    SET_CREATR_ARMOUR  = 0x006,
// 63 - SET_CREATURE_FEAR([creature],[a]) AN
    SET_CREATR_FEAR    = 0x007,
// 76 - SWAP_CREATURE([?],[?]) AA
    CREATR_SWAP        = 0x008,
};

// Comments/skiped commands
enum cmnds_comnt {
// Empty lines will be stored in this category
    EMPTYLN             = 0x001,
// 9 - REM
    CMNT_REM            = 0x002,
};

// Obsolete commands
enum cmnds_obsolt {
    OBSOLT_PRINT        = 0x001,
    CREATE_TEXT         = 0x002,
};

// Special parameters and numbers
enum cmnds_special {
    SPEC_RANDOM        = 0x001,
    SPEC_NUMBER        = 0x002,
};

// Object types
enum cmnds_objtypes {
    OBJTYPE_CREATURE   = 0x001,
    OBJTYPE_ROOM       = 0x003,
    OBJTYPE_SPELL      = 0x004,
};

//Note: Doors, Traps, Spells are defined elswhere

//Variables
#define ALL_DNGNS_DESTROYED  0x001
#define CRTRS_SCAVNG_GAINED  0x002
#define CRTRS_SCAVNG_LOST    0x003
#define DOORS_DESTROYED      0x004
#define TOTAL_GOLD_MINED     0x005
#define DNGN_DESTROYED       0x006
#define GOLD_POTS_STOLEN     0x007
#define TIMES_BROKEN_INTO    0x008
#define SPELLS_STOLEN        0x009
#define ROOMS_DESTROYED      0x00a
#define BATTLES_WON          0x00b
#define BATTLES_LOST         0x00c
#define CREATRS_ANNOYED      0x00d
#define TOTAL_CREATRS_LEFT   0x00e
#define TOTAL_AREA           0x00f
#define TOTAL_DOORS          0x010
#define TOTAL_RESEARCH       0x011
#define TOTAL_CREATRS        0x012
#define TOTAL_IMPS           0x013
#define BREAK_IN             0x014
#define GAME_TURN            0x015
#define MONEY                0x016
//Objectives of creatures in partys
#define OBJCTV_DEFEND_PARTY   0x00a
#define OBJCTV_ATTACK_ROOMS   0x009
#define OBJCTV_ATTACK_DNHEART 0x008
#define OBJCTV_ENEMIES        0x007
#define OBJCTV_STEAL_SPELLS   0x006
#define OBJCTV_STEAL_GOLD     0x005
//Heading objectives for tunneller partys
#define OBJCTV_APPROP_DUNG    0x004
#define OBJCTV_DNHEART        0x003
#define OBJCTV_DUNGEON        0x002
#define OBJCTV_ACTNPT         0x001
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
//Operators
#define OPER_EQU             0x001
#define OPER_NEQ             0x002
#define OPER_LESS            0x003
#define OPER_GRTR            0x004
#define OPER_LESSEQ          0x005
#define OPER_GRTREQ          0x006

//Note: Players, Creatures, Rooms are defined elswhere

//Computer player tweaks (undocumented)
#define COMP_SELL_TRAPSDOORS       0x001
#define COMP_ATTACK_MAGIC          0x002
#define COMP_WAIT_FOR_BRIDGE       0x003
#define COMP_MAGIC_SPEED_UP        0x004
#define COMP_DIG_TO_NEUTRAL        0x005
#define COMP_SLAP_IMPS             0x006
#define COMP_MOVE_CREATRS_TODEFENS 0x007
#define COMP_MOVE_CREATRS_TOPOS    0x008
#define COMP_MOVE_CREATRS_TOROOM   0x009
#define COMP_PICKUP_FOR_ATTACK     0x00a
#define COMP_MAGIC_CALLARMS        0x00b
#define COMP_DIG_TOATTACK          0x00c
#define COMP_DIG_TOGOLD            0x00d
#define COMP_DIG_TOENTRNCE         0x00e
#define COMP_PLACE_ROOM            0x00f
#define COMP_CHECK_ROOM_DUG        0x010
#define COMP_DIG_ROOM              0x011
#define COMP_DIG_ROOM_PASSAGE      0x012

#define MAX_PARTYS 16

#define ERR_SCRIPTPARAM_WHOLE      -9
#define ERR_SCRIPTPARAM_NARGS      -2

//ADiKtEd specific commands
enum cmnds_adikted {
    CUSTOM_COLUMN      = 0x001,
    DEFINE_GRAFFITI    = 0x002,
    LEVEL_TIMESTAMP    = 0x003,
    USER_CMNDS_COUNT   = 0x004,
    LEVEL_VERSION      = 0x005,
};

struct DK_SCRIPT_COMMAND {
    int group;  // To which command group this one belongs
    int index;  // Specific command index inside the group
    unsigned char **params; // Parameters, as text
    int param_count;  // Count of the parameters
    int level;  // amount of opened loops; 0 means main block
                // (regulates how much empty spaces to add before the command)
  };

struct SCRIPT_VERIFY_DATA {
    unsigned char *dnhearts;
    int dnhearts_count;
    unsigned char *herogts;
    unsigned int herogts_count;
    unsigned char *actnpts;
    unsigned int actnpts_count;
    int level;
    int total_ifs;
    int total_in_pool;
    char **partys;
  };

struct DK_SCRIPT;
struct IPOINT_2D;

typedef int (*func_cmd_index)(const char *cmdtext);
typedef const char *(*func_cmd_text)(int cmdidx);

extern int script_level_spaces;

//Functions for script analysis
short decompose_script(struct DK_SCRIPT *script);
short recompose_script(struct DK_SCRIPT *script);
short recompute_script_levels(struct DK_SCRIPT *script);
short decompose_script_command(struct DK_SCRIPT_COMMAND *cmd,const char *text);
char *recompose_script_command(const struct DK_SCRIPT_COMMAND *cmd);
//Functions for Adikted script execution
short execute_script_line(struct LEVEL *lvl,char *line,char *err_msg);
short add_stats_to_script(char ***lines,int *lines_count,struct LEVEL *lvl);
short add_graffiti_to_script(char ***lines,int *lines_count,struct LEVEL *lvl);
//Functions - verification
short dkscript_verify(const struct LEVEL *lvl, char *err_msg,int *err_line,int *err_param);
short txt_verify(const struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt);

//Working with text files
void text_file_free(char **lines,int lines_count);
int text_file_line_add(char ***lines,int *lines_count,char *text);
int text_file_linecp_add(char ***lines,int *lines_count,char *text);

//Lower level - for obtaining index of a command/parameter - adikted specific
int adikted_cmd_index(const char *cmdtext);
const char *adikted_cmd_text(int cmdidx);
int orient_cmd_index(const char *cmdtext);
const char *orient_cmd_text(int cmdidx);
unsigned short get_orientation_next(unsigned short orient);
int font_cmd_index(const char *cmdtext);
const char *font_cmd_text(int cmdidx);
//Lower level - for obtaining index of a command - dk commands
int condit_cmd_index(const char *cmdtext);
const char *condit_cmd_text(int cmdidx);
int party_cmd_index(const char *cmdtext);
const char *party_cmd_text(int cmdidx);
int avail_cmd_index(const char *cmdtext);
const char *avail_cmd_text(int cmdidx);
int custobj_cmd_index(const char *cmdtext);
const char *custobj_cmd_text(int cmdidx);
int setup_cmd_index(const char *cmdtext);
const char *setup_cmd_text(int cmdidx);
int triger_cmd_index(const char *cmdtext);
const char *triger_cmd_text(int cmdidx);
int crtradj_cmd_index(const char *cmdtext);
const char *crtradj_cmd_text(int cmdidx);
int obsolt_cmd_index(const char *cmdtext);
const char *obsolt_cmd_text(int cmdidx);
int commnt_cmd_index(const char *cmdtext);
const char *commnt_cmd_text(int cmdidx);
//Lower level - for obtaining index of a parameter - dk command parameters
int special_cmd_index(const char *cmdtext);
const char *special_cmd_text(int cmdidx,const char *param);
int operator_cmd_index(const char *cmdtext);
const char *operator_cmd_text(int cmdidx);
int comp_plyr_cmd_index(const char *cmdtext);
const char *comp_plyr_cmd_text(int cmdidx);
int players_cmd_index(const char *cmdtext);
const char *players_cmd_text(int cmdidx);
int creatures_cmd_index(const char *cmdtext);
const char *creatures_cmd_text(int cmdidx);
int room_cmd_index(const char *cmdtext);
const char *room_cmd_text(int cmdidx);
int spell_cmd_index(const char *cmdtext);
const char *spell_cmd_text(int cmdidx);
int trap_cmd_index(const char *cmdtext);
const char *trap_cmd_text(int cmdidx);
int door_cmd_index(const char *cmdtext);
const char *door_cmd_text(int cmdidx);
int objtype_cmd_index(const char *cmdtext);
const char *objtype_cmd_text(int cmdidx);
int operator_cmd_index(const char *cmdtext);
const char *operator_cmd_text(int cmdidx);
int variabl_cmd_index(const char *cmdtext);
const char *variabl_cmd_text(int cmdidx);
int timer_cmd_index(const char *cmdtext);
const char *timer_cmd_text(int cmdidx);
int flag_cmd_index(const char *cmdtext);
const char *flag_cmd_text(int cmdidx);
int party_objectv_cmd_index(const char *cmdtext);
const char *party_objectv_cmd_text(int cmdidx);

//Other lower level functions
short script_command_param_add(struct DK_SCRIPT_COMMAND *cmd,char *param);
void script_command_clear(struct DK_SCRIPT_COMMAND *cmd);
struct DK_SCRIPT_COMMAND *script_command_create(void);
void script_command_renew(struct DK_SCRIPT_COMMAND **cmd);
void script_command_free(struct DK_SCRIPT_COMMAND *cmd);
char *get_orientation_shortname(unsigned short orient);
char *get_font_longname(unsigned short font);
short script_param_to_int(int *val,const char *param);
short script_param_to_ulong(unsigned long *val,const char *param);
const char *script_cmd_text(int group,int cmdidx);
short is_no_bracket_command(int group,int cmdidx);
char *script_strword( const char *str, const short whole_rest );

//Lower level - executing commands
short execute_adikted_command(struct LEVEL *lvl,struct DK_SCRIPT_COMMAND *cmd,char *err_msg);

#endif // ADIKT_LEVSCRIPT_H
