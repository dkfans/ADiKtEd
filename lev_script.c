/*
 * lev_script.c
 *
 * Functions for maintaining the level script.
 *
 */

#include "lev_script.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"
#include "obj_column.h"

//Objectives of creatures in partys
const char objctv_defend_party_cmdtext[]="DEFEND_PARTY";
const char objctv_attack_rooms_cmdtext[]="ATTACK_ROOMS";
const char objctv_attack_dnheart_cmdtext[]="ATTACK_DUNGEON_HEART";
const char objctv_enemies_cmdtext[]="ATTACK_ENEMIES";
const char objctv_steal_spells_cmdtext[]="STEAL_SPELLS";
const char objctv_steal_gold_cmdtext[]="STEAL_GOLD";
//Variables - Flags
const char flag7_cmdtext[]="FLAG7";
const char flag6_cmdtext[]="FLAG6";
const char flag5_cmdtext[]="FLAG5";
const char flag4_cmdtext[]="FLAG4";
const char flag3_cmdtext[]="FLAG3";
const char flag2_cmdtext[]="FLAG2";
const char flag1_cmdtext[]="FLAG1";
const char flag0_cmdtext[]="FLAG0";
//Heading objectives for tunneller partys
const char objctv_approp_dung_cmdtext[]="APPROPIATE_DUNGEON";
const char objctv_dnheart_cmdtext[]="DUNGEON_HEART";
const char objctv_dungeon_cmdtext[]="DUNGEON";
const char objctv_actnpt_cmdtext[]="ACTION_POINT";
//Variables - Timers
const char timer7_cmdtext[]="TIMER7";
const char timer6_cmdtext[]="TIMER6";
const char timer5_cmdtext[]="TIMER5";
const char timer4_cmdtext[]="TIMER4";
const char timer3_cmdtext[]="TIMER3";
const char timer2_cmdtext[]="TIMER2";
const char timer1_cmdtext[]="TIMER1";
const char timer0_cmdtext[]="TIMER0";
//Custom objectives
const char creatr_swap_cmdtext[]="SWAP_CREATURE";
const char quick_info_cmdtext[]="QUICK_INFORMATION";
const char quick_objectv_cmdtext[]="QUICK_OBJECTIVE";
const char bonus_lev_time_cmdtext[]="BONUS_LEVEL_TIME";
const char tutorial_flash_btn_cmdtext[]="TUTORIAL_FLASH_BUTTON";
const char display_info_wpos_cmdtext[]="DISPLAY_INFORMATION_WITH_POS";
const char display_info_cmdtext[]="DISPLAY_INFORMATION";
const char display_objectv_wpos_cmdtext[]="DISPLAY_OBJECTIVE_WITH_POS";
const char display_objectv_cmdtext[]="DISPLAY_OBJECTIVE";
//Setup commands
const char dead_creatures_ret_to_pool_cmdtext[]="DEAD_CREATURES_RETURN_TO_POOL";
const char ally_players_cmdtext[]="ALLY_PLAYERS";
const char set_computer_process_cmdtext[]="SET_COMPUTER_PROCESS";
const char set_computer_event_cmdtext[]="SET_COMPUTER_EVENT";
const char set_computer_checks_cmdtext[]="SET_COMPUTER_CHECKS";
const char set_computer_globals_cmdtext[]="SET_COMPUTER_GLOBALS";
//Creatures adjusting
const char set_creatr_fear_cmdtext[]="SET_CREATURE_FEAR";
const char set_creatr_armour_cmdtext[]="SET_CREATURE_ARMOUR";
const char set_creatr_health_cmdtext[]="SET_CREATURE_HEALTH";
const char set_creatr_strength_cmdtext[]="SET_CREATURE_STRENGTH";
const char set_creatr_max_level_cmdtext[]="SET_CREATURE_MAX_LEVEL";
//Commands
const char if_available_cmdtext[]="IF_AVAILABLE";
const char set_music_cmdtext[]="SET_MUSIC";
const char reset_actnpt_cmdtext[]="RESET_ACTION_POINT";
const char add_creatr_to_pool_cmdtext[]="ADD_CREATURE_TO_POOL";
const char add_tunneller_party_tolev_cmdtext[]="ADD_TUNNELLER_PARTY_TO_LEVEL";
const char door_avail_cmdtext[]="DOOR_AVAILABLE";
const char next_cmd_reusable_cmdtext[]="NEXT_COMMAND_REUSABLE";
const char max_creatures_cmdtext[]="MAX_CREATURES";
const char set_flag_cmdtext[]="SET_FLAG";
const char game_lose_cmdtext[]="LOSE_GAME";
const char game_win_cmdtext[]="WIN_GAME";
const char add_tunneler_tolev_cmdtext[]="ADD_TUNNELLER_TO_LEVEL";
const char set_timer_cmdtext[]="SET_TIMER";
const char comp_player_cmdtext[]="COMPUTER_PLAYER";
const char trap_avail_cmdtext[]="TRAP_AVAILABLE";
const char magic_avail_cmdtext[]="MAGIC_AVAILABLE";
const char creatr_avail_cmdtext[]="CREATURE_AVAILABLE";
const char room_avail_cmdtext[]="ROOM_AVAILABLE";
const char start_money_cmdtext[]="START_MONEY";
const char rem_cmdtext[]="REM";
const char set_gen_speed_cmdtext[]="SET_GENERATE_SPEED";
const char set_hate_cmdtext[]="SET_HATE";
const char cond_endif_cmdtext[]="ENDIF";
const char if_actnpt_cmdtext[]="IF_ACTION_POINT";
const char add_creatr_tolev_cmdtext[]="ADD_CREATURE_TO_LEVEL";
const char add_party_tolev_cmdtext[]="ADD_PARTY_TO_LEVEL";
const char add_to_party_cmdtext[]="ADD_TO_PARTY";
const char create_party_cmdtext[]="CREATE_PARTY";
const char creature_cmdtext[]="CREATURE";
const char room_cmdtext[]="ROOM";
//Workshop - Doors
const char door_steel_cmdtext[]="STEEL";
const char door_braced_cmdtext[]="BRACED";
const char door_wood_cmdtext[]="WOOD";
const char door_magic_cmdtext[]="MAGIC";
//Workshop - Traps
const char trap_lava_cmdtext[]="LAVA";
const char trap_wordpwr_cmdtext[]="WORD_OF_POWER";
const char trap_lightng_cmdtext[]="LIGHTNING";
const char trap_gas_cmdtext[]="POISON_GAS";
const char trap_alarm_cmdtext[]="ALARM";
const char trap_boulder_cmdtext[]="BOULDER";
//Spells
const char spell_armag_cmdtext[]="POWER_ARMAGEDDON";
const char spell_possess_cmdtext[]="POWER_POSSESS";
const char spell_destrwalls_cmdtext[]="POWER_DESTROY_WALLS";
const char spell_chickn_cmdtext[]="POWER_CHICKEN";
const char spell_disease_cmdtext[]="POWER_DISEASE";
const char spell_conceal_cmdtext[]="POWER_CONCEAL";
const char spell_protect_cmdtext[]="POWER_PROTECT";
const char spell_speed_cmdtext[]="POWER_SPEED";
const char spell_lightng_cmdtext[]="POWER_LIGHTNING";
const char spell_holdaud_cmdtext[]="POWER_HOLD_AUDIENCE";
const char spell_gealcrtr_cmdtext[]="POWER_HEAL_CREATURE";
const char spell_cavein_cmdtext[]="POWER_CAVE_IN";
const char spell_callarms_cmdtext[]="POWER_CALL_TO_ARMS";
const char spell_sight_cmdtext[]="POWER_SIGHT";
const char spell_slap_cmdtext[]="POWER_SLAP";
const char spell_obey_cmdtext[]="POWER_OBEY";
const char spell_imp_cmdtext[]="POWER_IMP";
const char spell_hand_cmdtext[]="POWER_HAND";
//Variables
const char all_dngns_destroyed_cmdtext[]="ALL_DUNGEONS_DESTROYED";
const char crtrs_scavng_gained_cmdtext[]="CREATURES_SCAVENGED_GAINED";
const char crtrs_scavng_lost_cmdtext[]="CREATURES_SCAVENGED_LOST";
const char doors_destroyed_cmdtext[]="DOORS_DESTROYED";
const char total_gold_mined_cmdtext[]="TOTAL_GOLD_MINED";
const char dngn_destroyed_cmdtext[]="DUNGEON_DESTROYED";
const char gold_pots_stolen_cmdtext[]="GOLD_POTS_STOLEN";
const char items_broken_into_cmdtext[]="TIMES_BROKEN_INTO";
const char spells_stolen_cmdtext[]="SPELLS_STOLEN";
const char rooms_destroyed_cmdtext[]="ROOMS_DESTROYED";
const char battles_won_cmdtext[]="BATTLES_WON";
const char battles_lost_cmdtext[]="BATTLES_LOST";
const char creatrs_annoyed_cmdtext[]="CREATURES_ANNOYED";
const char total_creatrs_left_cmdtext[]="TOTAL_CREATURES_LEFT";
const char total_area_cmdtext[]="TOTAL_AREA";
const char total_doors_cmdtext[]="TOTAL_DOORS";
const char total_research_cmdtext[]="TOTAL_RESEARCH";
const char total_creatrs_cmdtext[]="TOTAL_CREATURES";
const char total_imps_cmdtext[]="TOTAL_IMPS";
const char break_in_cmdtext[]="BREAK_IN";
const char game_turn_cmdtext[]="GAME_TURN";
const char money_cmdtext[]="MONEY";
//Operators
const char oper_equ_cmdtext[]="==";
const char oper_neq_cmdtext[]="!=";
const char oper_less_cmdtext[]="<";
const char oper_grtr_cmdtext[]=">";
const char oper_lesseq_cmdtext[]="<=";
const char oper_grtreq_cmdtext[]=">=";
//Players
const char player_all_cmdtext[]="ALL_PLAYERS";
const char player_good_cmdtext[]="PLAYER_GOOD";
const char player3_cmdtext[]="PLAYER3";
const char player2_cmdtext[]="PLAYER2";
const char player1_cmdtext[]="PLAYER1";
const char player0_cmdtext[]="PLAYER0";
//Evil Creatures
const char creatr_newa_cmdtext[]="NEW_CREATURE_B";
const char creatr_newb_cmdtext[]="NEW_CREATURE_A";
const char creatr_flspirit_cmdtext[]="FLOATING_SPIRIT";
const char creatr_orc_cmdtext[]="ORC";
const char creatr_tentacl_cmdtext[]="TENTACLE";
const char creatr_ghost_cmdtext[]="GHOST";
const char creatr_hound_cmdtext[]="HELL_HOUND";
const char creatr_spider_cmdtext[]="SPIDER";
const char creatr_vampire_cmdtext[]="VAMPIRE";
const char creatr_bug_cmdtext[]="BUG";
const char creatr_imp_cmdtext[]="IMP";
const char creatr_biledm_cmdtext[]="BILE_DEMON";
const char creatr_sorcer_cmdtext[]="SORCEROR";
const char creatr_dkmist_cmdtext[]="DARK_MISTRESS";
const char creatr_fly_cmdtext[]="FLY";
const char creatr_dmspawn_cmdtext[]="DEMONSPAWN";
const char creatr_dragon_cmdtext[]="DRAGON";
const char creatr_troll_cmdtext[]="TROLL";
const char creatr_skeleton_cmdtext[]="SKELETON";
const char creatr_horny_cmdtext[]="HORNY";
//Good Creatures
const char creatr_samurai_cmdtext[]="SAMURAI";
const char creatr_thief_cmdtext[]="THIEF";
const char creatr_fairy_cmdtext[]="FAIRY";
const char creatr_giant_cmdtext[]="GIANT";
const char creatr_witch_cmdtext[]="WITCH";
const char creatr_tunnelr_cmdtext[]="TUNNELLER";
const char creatr_avatar_cmdtext[]="AVATAR";
const char creatr_knight_cmdtext[]="KNIGHT";
const char creatr_dwarfa_cmdtext[]="DWARFA";
const char creatr_monk_cmdtext[]="MONK";
const char creatr_archr_cmdtext[]="ARCHER";
const char creatr_barbarin_cmdtext[]="BARBARIAN";
const char creatr_wizard_cmdtext[]="WIZARD";
//Rooms
const char room_grdpost_cmdtext[]="GUARD_POST";
const char room_brige_cmdtext[]="BRIDGE";
const char room_lair_cmdtext[]="LAIR";
const char room_hatchery_cmdtext[]="GARDEN";
const char room_barracks_cmdtext[]="BARRACKS";
const char room_graveyrd_cmdtext[]="GRAVEYARD";
const char room_temple_cmdtext[]="TEMPLE";
const char room_scavng_cmdtext[]="SCAVENGER";
const char room_workshp_cmdtext[]="WORKSHOP";
const char room_training_cmdtext[]="TRAINING";
const char room_torture_cmdtext[]="TORTURE";
const char room_prison_cmdtext[]="PRISON";
const char room_library_cmdtext[]="RESEARCH";
const char room_treasure_cmdtext[]="TREASURE";
const char room_entrance_cmdtext[]="ENTRANCE";
//Computer player tweaks (undocumented)
const char comp_sell_trapsdoors_cmdtext[]="COMPUTER_SELL_TRAPS_AND_DOORS";
const char comp_attack_magic_cmdtext[]="COMPUTER_ATTACK_MAGIC";
const char comp_wait_for_bridge_cmdtext[]="COMPUTER_WAIT_FOR_BRIDGE";
const char comp_magic_speed_up_cmdtext[]="COMPUTER_MAGIC_SPEED_UP";
const char comp_dig_to_neutral_cmdtext[]="COMPUTER_DIG_TO_NEUTRAL";
const char comp_slap_imps_cmdtext[]="COMPUTER_SLAP_IMPS";
const char comp_move_creatrs_todefens_cmdtext[]="COMPUTER_MOVE_CREATURES_TO_DEFEND";
const char comp_move_creatrs_topos_cmdtext[]="COMPUTER_MOVE_CREATURE_TO_POS";
const char comp_move_creatrs_toroom_cmdtext[]="COMPUTER_MOVE_CREATURE_TO_ROOM";
const char comp_pickup_for_attack_cmdtext[]="COMPUTER_PICKUP_FOR_ATTACK";
const char comp_magic_callarms_cmdtext[]="COMPUTER_MAGIC_CALL_TO_ARMS";
const char comp_dig_toattack_cmdtext[]="COMPUTER_DIG_TO_ATTACK";
const char comp_dig_togold_cmdtext[]="COMPUTER_DIG_TO_GOLD";
const char comp_dig_toentrnce_cmdtext[]="COMPUTER_DIG_TO_ENTRANCE";
const char comp_place_room_cmdtext[]="COMPUTER_PLACE_ROOM";
const char comp_check_room_dug_cmdtext[]="COMPUTER_CHECK_ROOM_DUG";
const char comp_dig_room_cmdtext[]="COMPUTER_DIG_ROOM";
const char comp_dig_room_passage_cmdtext[]="COMPUTER_DIG_ROOM_PASSAGE";

// ADiKtEd-specific commands

const char custom_column_cmdtext[]="CUSTOM_COLUMN";
const char graffiti_cmdtext[]="GRAFFITI";

const char orient_ns_cmdtext[]="ORIENT_NS";
const char orient_we_cmdtext[]="ORIENT_WE";
const char orient_sn_cmdtext[]="ORIENT_SN";
const char orient_ew_cmdtext[]="ORIENT_EW";
const char orient_tp_cmdtext[]="ORIENT_TOP";

const char font_none_cmdtext[]="FONT_NONE";
const char font_adiclssc_cmdtext[]="FONT_ADICLSSC";

//Command arrays

const char *cmd_adikted_arr[]={
        "",custom_column_cmdtext,graffiti_cmdtext  //0x000,0x001,0x002
        };

const char *cmd_orient_arr[]={
        orient_ns_cmdtext,orient_we_cmdtext,orient_sn_cmdtext,  //0x000,0x001,0x002
        orient_ew_cmdtext,orient_tp_cmdtext                     //0x003, 0x004
        };

const char *cmd_orient_shortnames[]={
        "NS","WE","SN","EW","Top" };

const char *cmd_font_longnames[]={
        "none","Classic" };

/*
 * Executes command from the CMD_ADIKTED group
 */
short execute_adikted_command(struct LEVEL *lvl,struct DK_SCRIPT_COMMAND *cmd,char *err_msg)
{
    char text[READ_BUFSIZE];
    switch (cmd->index)
    {
    case CUSTOM_COLUMN:
       if (cmd->param_count<10)
       {
         sprintf(err_msg,"%s requires more parameters",custom_column_cmdtext);
         return false;
       }
       int sx=-1,sy=-1;
       if (sscanf(cmd->params[0],"%d",&sx)<1)
       {
         sprintf(err_msg,"Cannot read map subtile X coordinate");
         return false;
       }
       if (sscanf(cmd->params[1],"%d",&sy)<1)
       {
         sprintf(err_msg,"Cannot read map subtile Y coordinate");
         return false;
       }
         //TODO
       return true;
    case DEFINE_GRAFFITI:
      {
       if (cmd->param_count<7)
       {
         sprintf(err_msg,"%s requires more parameters",graffiti_cmdtext);
         return false;
       }
       int tx=-1,ty=-1,height=0;
       if (sscanf(cmd->params[0],"%d",&tx)<1)
       {
         sprintf(err_msg,"Cannot read map tile X coordinate");
         return false;
       }
       if (sscanf(cmd->params[1],"%d",&ty)<1)
       {
         sprintf(err_msg,"Cannot read map tile Y coordinate");
         return false;
       }
       if (sscanf(cmd->params[2],"%d",&height)<1)
       {
         sprintf(err_msg,"Cannot read height in tile");
         return false;
       }
       int orient;
       orient=orient_cmd_index(cmd->params[3]);
       int font;
       //TODO: make different fonts support
       font=GRAFF_FONT_ADICLSSC;//font_cmd_index(cmd->params[4]);
       if (orient<0) orient=ORIENT_NS;
       if (font<0) font=GRAFF_FONT_ADICLSSC;
       int i,textlen;
       unsigned int cube=0x0184;
       if (sscanf(cmd->params[5],"%d",&cube)<1)
       {
         sprintf(err_msg,"Cannot read filler cube index");
         return false;
       }
       textlen=strlen(cmd->params[6]);
       if (textlen>=READ_BUFSIZE) textlen=READ_BUFSIZE-1;
       for (i=0;i<textlen;i++)
       {
         char c=cmd->params[6][i];
         if ((c<=32)||(c=='_')) text[i]=' '; else
         if (c=='.') text[i]=','; else
         if (c=='{') text[i]='('; else
         if (c=='}') text[i]=')'; else
           text[i]=c;
       }
       text[textlen]='\0';
       int graf_idx=graffiti_add(lvl,tx,ty,height,text,font,orient,cube);
       if (graf_idx<0) return false;
      };return true;
    default:
      {
      };return false;
    }
}

short execute_script_line(struct LEVEL *lvl,char *line,char *err_msg)
{
    struct DK_SCRIPT_COMMAND *cmd;
    cmd=malloc(sizeof(struct DK_SCRIPT_COMMAND));
    if (cmd==NULL) die("Cannot allocate memory to execute script command");
    script_command_clear(cmd);
    int result;
    if (decompose_script_command(cmd,line))
    {
      switch (cmd->group)
      {
      case CMD_ADIKTED:
          result=execute_adikted_command(lvl,cmd,err_msg);
          break;
      default:
          sprintf(err_msg,"Command is not executable in ADiKtEd");
          result=false;
          break;
      }
    } else
    {
      //Check if it's just empty line
      if ((line==NULL)||(strlen(line)<1))
      {
        result=true;
      } else
      {
        sprintf(err_msg,"Cannot identify command");
        result=false;
      }
    }
    script_command_free(cmd);
    return result;
}

short decompose_script_command(struct DK_SCRIPT_COMMAND *cmd,char *text)
{
  if ((cmd==NULL)||(text==NULL)) return false;
  //Decomposing the string into single parameters - getting first (command name)
  char *wordtxt;
  wordtxt = strtok (text," \t,(");
  cmd->index=-1;
  int cmd_idx;
  if (cmd->index<0)
  {
    cmd_idx=adikted_cmd_index(wordtxt);
    if (cmd_idx>=0)
    {
      cmd->group=CMD_ADIKTED;
      cmd->index=cmd_idx;
    }
  }
  if (cmd->index<0) return false;
  //Decomposing the string into single parameters - getting the rest of parameters
  while (wordtxt != NULL)
  {
    wordtxt = strtok(NULL, " \t,)");
    script_command_param_add(cmd,wordtxt);
  }
  return true;
}

short script_command_param_add(struct DK_SCRIPT_COMMAND *cmd,char *param)
{
    if ((cmd==NULL)||(param==NULL)) return false;
    int param_idx=cmd->param_count;
    cmd->params=realloc(cmd->params,(param_idx+1)*sizeof(char *));
    cmd->params[param_idx]=strdup(param);
    cmd->param_count=param_idx+1;
    return true;
}

int adikted_cmd_index(char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_adikted_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (strcmp(cmd_adikted_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *adikted_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_adikted_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "X";
    return cmd_adikted_arr[cmdidx];
}

int orient_cmd_index(char *cmdtext)
{
    if ((cmdtext==NULL)||(strlen(cmdtext)<2)) return -1;
    int i=0;
    int array_count=sizeof(cmd_orient_arr)/sizeof(char *);
    while (i<array_count)
    {
      if (strcmp(cmd_orient_arr[i],cmdtext)==0)
        return i;
      i++;
    }
    return -1;
}

const char *orient_cmd_text(int cmdidx)
{
    int array_count=sizeof(cmd_orient_arr)/sizeof(char *);
    if ((cmdidx<0)||(cmdidx>=array_count)) return "X";
    return cmd_orient_arr[cmdidx];
}

/*
 * Clears values in given DK_SCRIPT_COMMAND without deallocating memory
 */
void script_command_clear(struct DK_SCRIPT_COMMAND *cmd)
{
    cmd->group=CMD_UNKNOWN;
    cmd->index=-1;
    cmd->params=NULL;
    cmd->param_count=0;
}

/*
 * Frees memory allocated for given DK_SCRIPT_COMMAND
 */
void script_command_free(struct DK_SCRIPT_COMMAND *cmd)
{
    int i;
    for (i=cmd->param_count-1;i>=0;i--)
        free(cmd->params[i]);
    free(cmd);
}

/*
 * Frees all lines in text file given in parameters
 */
void text_file_free(char **lines,int lines_count)
{
     int i;
     for (i=lines_count-1;i>=0;i--)
       free(lines[i]);
     free(lines);
}

/*
 * Adds a text line at end of given text file. Adds directly,
 * without making copy of the text. Returns index of the new line.
 */
int text_file_line_add(char ***lines,int *lines_count,char *text)
{
    int line_idx=*lines_count;
    *lines=realloc(*lines,(line_idx+1)*sizeof(char *));
    if ((*lines)==NULL) die("Cannot allocate text file lines");
    (*lines)[line_idx]=text;
    (*lines_count)++;
    return line_idx;
}

/*
 * Adds a text line at end of given text file. Adds copy of text
 * variable. Returns index of the new line.
 */
int text_file_linecp_add(char ***lines,int *lines_count,char *text)
{
    int line_idx=*lines_count;
    *lines=realloc(*lines,(line_idx+1)*sizeof(char *));
    if ((*lines)==NULL) die("Cannot allocate text file lines");
    (*lines)[line_idx]=strdup(text);
    (*lines_count)++;
    return line_idx;
}

short add_graffiti_to_script(char ***lines,int *lines_count,struct LEVEL *lvl)
{
    int i,k;
    struct DK_GRAFFITI *graf;
    char *line;
    line=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    char *graftxt;
    graftxt=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    for (i=0; i < lvl->graffiti_count; i++)
    {
      graf = lvl->graffiti[i];
      if (graf==NULL) continue;
      int graf_len=strlen(graf->text);
      for (k=0;k<graf_len;k++)
      {
        char c=graf->text[k];
        if (c<=32) graftxt[k]='_'; else
        if (c==',') graftxt[k]='.'; else
        if (c=='(') graftxt[k]='{'; else
        if (c==')') graftxt[k]='}'; else
          graftxt[k]=c;
      }
      graftxt[graf_len]='\0';
      sprintf(line,"%s(%d,%d,%d,%s,%s,0x%04x,%s)",graffiti_cmdtext,graf->tx,graf->ty,graf->height,
                orient_cmd_text(graf->orient),font_adiclssc_cmdtext,graf->cube,graftxt);
      text_file_linecp_add(lines,lines_count,line);
    }
    free(line);
    free(graftxt);
    return true;
}

short add_custom_clms_to_script(char ***lines,int *lines_count,struct LEVEL *lvl)
{
    int i;
    struct DK_CUSTOM_CLM *cclm;
    char *line;
    line=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    for (i=0; i < lvl->cust_clm_count; i++)
    {
      cclm = lvl->cust_clm[i];
      sprintf(line,"%s(%d,%d,...)",custom_column_cmdtext,cclm->sx,cclm->sy);
      text_file_linecp_add(lines,lines_count,line);
    }
    free(line);
    return true;
}

/*
 * Returns orientation name string
 */
char *get_orientation_shortname(unsigned short orient)
{
     int types_count=sizeof(cmd_orient_shortnames)/sizeof(char *);
     if (orient<types_count)
       return (char *)cmd_orient_shortnames[orient];
     else
       return "unkn";
}

/*
 * Returns font name string
 */
char *get_font_longname(unsigned short font)
{
     int types_count=sizeof(cmd_font_longnames)/sizeof(char *);
     if (font<types_count)
       return (char *)cmd_font_longnames[font];
     else
       return "unkn";
}
