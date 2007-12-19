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
const char research_cmdtext[]="RESEARCH";
const char treasure_cmdtext[]="TREASURE";
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

