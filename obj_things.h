/*
 * obj_things.h
 */

#ifndef ADIKT_OBJTHINGS_H
#define ADIKT_OBJTHINGS_H

//Things - most of the game objects is stored as thing
#define THING_TYPE_NONE        0x00
#define THING_TYPE_ITEM        0x01
#define THING_TYPE_CREATURE    0x05
#define THING_TYPE_ROOMEFFECT  0x07
#define THING_TYPE_TRAP        0x08
#define THING_TYPE_DOOR        0x09
//Note: game engine informs of more thing types:
//effect_element, effect_generator, ambient_sound,
//shot, dead_creature, cave_in
//There is no evidence that these items can be created
//in maps (I've tried)

#define ITEM_SUBTYPE_NULL      0x00
#define ITEM_SUBTYPE_BARREL    0x01
#define ITEM_SUBTYPE_TORCH     0x02
#define ITEM_SUBTYPE_GOLDCHEST 0x03
#define ITEM_SUBTYPE_TEMPLESTA 0x04
#define ITEM_SUBTYPE_DNHEART   0x05
#define ITEM_SUBTYPE_GOLD      0x06
#define ITEM_SUBTYPE_TORCHUN   0x07
#define ITEM_SUBTYPE_STATUEWO  0x08
#define ITEM_SUBTYPE_CHICKNGRW 0x09
#define ITEM_SUBTYPE_CHICKN    0x0a
#define ITEM_SUBTYPE_SPELLHOE  0x0b
#define ITEM_SUBTYPE_SPELLIMP  0x0c
#define ITEM_SUBTYPE_SPELLMUST 0x0d
#define ITEM_SUBTYPE_SPELLSLAP 0x0e
#define ITEM_SUBTYPE_SPELLSOE  0x0f
#define ITEM_SUBTYPE_SPELLCTA  0x10
#define ITEM_SUBTYPE_SPELLCAVI 0x11
#define ITEM_SUBTYPE_SPELLHEAL 0x12
#define ITEM_SUBTYPE_SPELLHLDA 0x13
#define ITEM_SUBTYPE_SPELLLIGH 0x14
#define ITEM_SUBTYPE_SPELLSPDC 0x15
#define ITEM_SUBTYPE_SPELLPROT 0x16
#define ITEM_SUBTYPE_SPELLCONC 0x17

#define ITEM_SUBTYPE_NULL1     0x18
#define ITEM_SUBTYPE_NULL2     0x19

#define ITEM_SUBTYPE_ANVIL     0x1a
#define ITEM_SUBTYPE_PRISONBAR 0x1b
#define ITEM_SUBTYPE_CANDLSTCK 0x1c //candle with candlestick
#define ITEM_SUBTYPE_GRAVSTONE 0x1d
#define ITEM_SUBTYPE_STATUHORN 0x1e
#define ITEM_SUBTYPE_TRAINPOST 0x1f
#define ITEM_SUBTYPE_TORTSPIKE 0x20
#define ITEM_SUBTYPE_TEMPLESPN 0x21 //Temple Spangle

#define ITEM_SUBTYPE_POTION1   0x22
#define ITEM_SUBTYPE_POTION2   0x23
#define ITEM_SUBTYPE_POTION3   0x24
#define ITEM_SUBTYPE_PWHAND    0x25
#define ITEM_SUBTYPE_PWHANDGRB 0x26
#define ITEM_SUBTYPE_PWHANDWHP 0x27
#define ITEM_SUBTYPE_CHICKNSTB 0x28
#define ITEM_SUBTYPE_CHICKNWOB 0x29
#define ITEM_SUBTYPE_CHICKNCRK 0x2a
#define ITEM_SUBTYPE_CHICKN2   0x2b //not sure about this one
#define ITEM_SUBTYPE_SPINNKEY  0x2c
#define ITEM_SUBTYPE_SPELLDISE 0x2d
#define ITEM_SUBTYPE_SPELLCHKN 0x2e
#define ITEM_SUBTYPE_SPELLDWAL 0x2f
#define ITEM_SUBTYPE_SPELLTBMB 0x30
#define ITEM_SUBTYPE_HEROGATE  0x31
#define ITEM_SUBTYPE_UNKN32    0x32
        //UNTESTED - indices may differ by one
#define ITEM_SUBTYPE_ARMOUR    0x33
#define ITEM_SUBTYPE_GLDHOARD1 0x34
#define ITEM_SUBTYPE_GLDHOARD2 0x35
#define ITEM_SUBTYPE_GLDHOARD3 0x36
#define ITEM_SUBTYPE_GLDHOARD4 0x37
#define ITEM_SUBTYPE_GLDHOARD5 0x38
        //Creature lairs
#define ITEM_SUBTYPE_LAIRWIZRD 0x39
#define ITEM_SUBTYPE_LAIRBARBR 0x3a
#define ITEM_SUBTYPE_LAIRARCHR 0x3b
#define ITEM_SUBTYPE_LAIRMONK  0x3c
#define ITEM_SUBTYPE_LAIRDWRFA 0x3d
#define ITEM_SUBTYPE_LAIRKNGHT 0x3e
#define ITEM_SUBTYPE_LAIRAVATR 0x3f
#define ITEM_SUBTYPE_LAIRTUNLR 0x40
#define ITEM_SUBTYPE_LAIRWITCH 0x41
#define ITEM_SUBTYPE_LAIRGIANT 0x42
#define ITEM_SUBTYPE_LAIRFAIRY 0x43
#define ITEM_SUBTYPE_LAIRTHEFT 0x44
#define ITEM_SUBTYPE_LAIRSAMUR 0x45
#define ITEM_SUBTYPE_LAIRHORNY 0x46
#define ITEM_SUBTYPE_LAIRSKELT 0x47
#define ITEM_SUBTYPE_LAIRGOBLN 0x48
#define ITEM_SUBTYPE_LAIRDRAGN 0x49
#define ITEM_SUBTYPE_LAIRDEMSP 0x4a
#define ITEM_SUBTYPE_LAIRFLY   0x4b
#define ITEM_SUBTYPE_LAIRDKMIS 0x4c
#define ITEM_SUBTYPE_LAIRSORCR 0x4d
#define ITEM_SUBTYPE_LAIRBILDM 0x4e
#define ITEM_SUBTYPE_LAIRIMP   0x4f
#define ITEM_SUBTYPE_LAIRBUG   0x50
#define ITEM_SUBTYPE_LAIRVAMP  0x51
#define ITEM_SUBTYPE_LAIRSPIDR 0x52
#define ITEM_SUBTYPE_LAIRHLHND 0x53
#define ITEM_SUBTYPE_LAIRGHOST 0x54
#define ITEM_SUBTYPE_LAIRTENTC 0x55

#define ITEM_SUBTYPE_SPREVMAP  0x56
#define ITEM_SUBTYPE_SPRESURCT 0x57
#define ITEM_SUBTYPE_SPTRANSFR 0x58
#define ITEM_SUBTYPE_SPSTEALHR 0x59
#define ITEM_SUBTYPE_SPMULTPLY 0x5a
#define ITEM_SUBTYPE_SPINCLEV  0x5b
#define ITEM_SUBTYPE_SPMKSAFE  0x5c
#define ITEM_SUBTYPE_SPHIDNWRL 0x5d
#define ITEM_SUBTYPE_TBBOULDER 0x5e
#define ITEM_SUBTYPE_TBALARM   0x5f
#define ITEM_SUBTYPE_TBPOISONG 0x60
#define ITEM_SUBTYPE_TBLIGHTNG 0x61
#define ITEM_SUBTYPE_TBWRDOFPW 0x62
#define ITEM_SUBTYPE_TBLAVA    0x63
#define ITEM_SUBTYPE_TBDUMMY2  0x64
#define ITEM_SUBTYPE_TBDUMMY3  0x65
#define ITEM_SUBTYPE_TBDUMMY4  0x66
#define ITEM_SUBTYPE_TBDUMMY5  0x67
#define ITEM_SUBTYPE_TBDUMMY6  0x68
#define ITEM_SUBTYPE_TBDUMMY7  0x69
#define ITEM_SUBTYPE_DBWOOD    0x6a
#define ITEM_SUBTYPE_DBBRACE   0x6b
#define ITEM_SUBTYPE_DBSTEEL   0x6c
#define ITEM_SUBTYPE_DBMAGIC   0x6d
#define ITEM_SUBTYPE_WBITEM    0x6e
#define ITEM_SUBTYPE_HEARTFLMR 0x6f

#define ITEM_SUBTYPE_DISEASE   0x70
#define ITEM_SUBTYPE_SCAVNGEYE 0x71
#define ITEM_SUBTYPE_WRKSHPMCH 0x72
#define ITEM_SUBTYPE_GURDFLAGR 0x73
#define ITEM_SUBTYPE_GURDFLAGB 0x74
#define ITEM_SUBTYPE_GURDFLAGG 0x75
#define ITEM_SUBTYPE_GURDFLAGY 0x76
#define ITEM_SUBTYPE_FLAGPOST  0x77
#define ITEM_SUBTYPE_HEARTFLMB 0x78
#define ITEM_SUBTYPE_HEARTFLMG 0x79
#define ITEM_SUBTYPE_HEARTFLMY 0x7a //isn't it purple?!
#define ITEM_SUBTYPE_PWSIGHT   0x7b
#define ITEM_SUBTYPE_PWLIGHTNG 0x7c
#define ITEM_SUBTYPE_TORTURER  0x7d
#define ITEM_SUBTYPE_LAIRORC   0x7e
#define ITEM_SUBTYPE_PWHANDGLD 0x7f
#define ITEM_SUBTYPE_COIN      0x80
#define ITEM_SUBTYPE_STATUE2   0x81
#define ITEM_SUBTYPE_STATUE3   0x82
#define ITEM_SUBTYPE_STATUE4   0x83
#define ITEM_SUBTYPE_STATUE5   0x84
#define ITEM_SUBTYPE_STATUE6   0x85
#define ITEM_SUBTYPE_SPELLARMG 0x86

#define ITEM_CATEGR_NULL       0x00
#define ITEM_CATEGR_VARIOUS    0x01
#define ITEM_CATEGR_ROOMEQUIP  0x02
#define ITEM_CATEGR_SPELLBOOK  0x03
#define ITEM_CATEGR_STATUES    0x04
#define ITEM_CATEGR_GOLD       0x05
#define ITEM_CATEGR_FOOD       0x06
#define ITEM_CATEGR_CREATLAIR  0x07
#define ITEM_CATEGR_SPECIALBOX 0x08
#define ITEM_CATEGR_WRKSHOPBOX 0x09
#define ITEM_CATEGR_PWHNDEFFCT 0x0a
#define ITEM_CATEGR_LIGHTS     0x0b
#define ITEM_CATEGR_UNKNOWN    0x10

//Effects (all listed)
#define ROOMEFC_SUBTP_LAVA     0x01
#define ROOMEFC_SUBTP_DRIPWTR  0x02
#define ROOMEFC_SUBTP_ROCKFAL  0x03
#define ROOMEFC_SUBTP_ENTRICE  0x04
#define ROOMEFC_SUBTP_DRYICE   0x05

//Creature types (all listed)
#define CREATR_SUBTP_WIZRD     0x01
#define CREATR_SUBTP_BARBARIN  0x02
#define CREATR_SUBTP_ARCHER    0x03
#define CREATR_SUBTP_MONK      0x04
#define CREATR_SUBTP_DWAFT     0x05
#define CREATR_SUBTP_KNIGHT    0x06
#define CREATR_SUBTP_AVATAR    0x07
#define CREATR_SUBTP_TUNELER   0x08
#define CREATR_SUBTP_WITCH     0x09
#define CREATR_SUBTP_GIANT     0x0A
#define CREATR_SUBTP_FAIRY     0x0B
#define CREATR_SUBTP_THEFT     0x0C
#define CREATR_SUBTP_SMURI     0x0D
#define CREATR_SUBTP_REAPER    0x0E
#define CREATR_SUBTP_SKELETON  0x0F
#define CREATR_SUBTP_TROLL     0x10
#define CREATR_SUBTP_DRAGON    0x11
#define CREATR_SUBTP_SPAWN     0x12
#define CREATR_SUBTP_FLY       0x13
#define CREATR_SUBTP_MISTRESS  0x14
#define CREATR_SUBTP_WARLOCK   0x15
#define CREATR_SUBTP_BILEDEMN  0x16
#define CREATR_SUBTP_IMP       0x17
#define CREATR_SUBTP_BEETLE    0x18
#define CREATR_SUBTP_VAMPIRE   0x19
#define CREATR_SUBTP_SPIDER    0x1A
#define CREATR_SUBTP_HOUND     0x1B
#define CREATR_SUBTP_GHOST     0x1C
#define CREATR_SUBTP_TENTCL    0x1D
#define CREATR_SUBTP_ORC       0x1E
#define CREATR_SUBTP_FLOAT     0x1F

#define DOOR_SUBTYPE_WOOD      0x01
#define DOOR_SUBTYPE_BRACED    0x02
#define DOOR_SUBTYPE_IRON      0x03
#define DOOR_SUBTYPE_MAGIC     0x04

//Traps (all listed; dummy doesn't really exist)
#define TRAP_SUBTYPE_BOULDER   0x01
#define TRAP_SUBTYPE_ALARM     0x02
#define TRAP_SUBTYPE_GAS       0x03
#define TRAP_SUBTYPE_LIGHTNG   0x04
#define TRAP_SUBTYPE_WORDPWR   0x05
#define TRAP_SUBTYPE_LAVA      0x06
#define TRAP_SUBTYPE_DUMMY2    0x07
#define TRAP_SUBTYPE_DUMMY3    0x08
#define TRAP_SUBTYPE_DUMMY4    0x09
#define TRAP_SUBTYPE_DUMMY5    0x0A
#define TRAP_SUBTYPE_DUMMY6    0x0B
#define TRAP_SUBTYPE_DUMMY7    0x0C

#define DOOR_ORIENT_NSPASS     0x00
#define DOOR_ORIENT_EWPASS     0x01
#define DOOR_PASS_UNLOCKED     0x00
#define DOOR_PASS_LOCKED       0x01

//Other defines
#define THING_SENSITILE_NONE   0x0ffff


struct LEVEL;

extern short obj_auto_update;

unsigned char get_thing_type(unsigned char *thing);
short set_thing_type(unsigned char *thing,unsigned char type_idx);
unsigned char get_thing_subtype(unsigned char *thing);
short set_thing_subtype(unsigned char *thing,unsigned char stype_idx);
unsigned char get_thing_owner(unsigned char *thing);
short set_thing_owner(unsigned char *thing,unsigned char plyr_idx);

unsigned char get_thing_tilepos_x(unsigned char *thing);
short set_thing_tilepos_x(unsigned char *thing,unsigned char pos_x);
unsigned char get_thing_tilepos_y(unsigned char *thing);
short set_thing_tilepos_y(unsigned char *thing,unsigned char pos_y);
unsigned char get_thing_tilepos_h(unsigned char *thing);
short set_thing_tilepos_h(unsigned char *thing,unsigned char pos_h);
short set_thing_tilepos(unsigned char *thing,unsigned char pos_x,unsigned char pos_y);
unsigned char get_thing_subtpos_x(unsigned char *thing);
short set_thing_subtpos_x(unsigned char *thing,unsigned char pos_x);
unsigned char get_thing_subtpos_y(unsigned char *thing);
short set_thing_subtpos_y(unsigned char *thing,unsigned char pos_y);
unsigned char get_thing_subtpos_h(unsigned char *thing);
short set_thing_subtpos_h(unsigned char *thing,unsigned char pos_h);
short set_thing_subtpos(unsigned char *thing,unsigned char pos_x,unsigned char pos_y);
unsigned char get_thing_level(unsigned char *thing);
short set_thing_level(unsigned char *thing,unsigned char lev_num);
unsigned short get_thing_sensitile(unsigned char *thing);
short set_thing_sensitile(unsigned char *thing,unsigned short til_num);

unsigned char get_door_orientation(unsigned char *thing);
short set_door_orientation(unsigned char *thing,unsigned char orient);

unsigned char *create_thing_empty();
unsigned char *create_thing(unsigned int sx, unsigned int sy);
unsigned char *create_thing_copy(unsigned int sx, unsigned int sy,unsigned char *src);
unsigned char *create_item(unsigned int sx, unsigned int sy, unsigned char stype_idx);

short is_spellbook(unsigned char *thing);
short is_spellbook_stype(unsigned char stype_idx);
unsigned char get_spellbook_next(unsigned char stype_idx);
unsigned char get_spellbook_prev(unsigned char stype_idx);
short is_dngspecbox(unsigned char *thing);
short is_dngspecbox_stype(unsigned char stype_idx);
unsigned char get_dngspecbox_next(unsigned char stype_idx);
unsigned char get_dngspecbox_prev(unsigned char stype_idx);
short is_crtrlair(unsigned char *thing);
short is_crtrlair_stype(unsigned char stype_idx);
unsigned char get_crtrlair_next(unsigned char stype_idx);
unsigned char get_crtrlair_prev(unsigned char stype_idx);
short is_trapbox(unsigned char *thing);
short is_trapbox_stype(unsigned char stype_idx);
unsigned char get_trapbox_next(unsigned char stype_idx);
unsigned char get_trapbox_prev(unsigned char stype_idx);
short is_trap(unsigned char *thing);
unsigned char get_trap_next(unsigned char stype_idx);
unsigned char get_trap_prev(unsigned char stype_idx);
short is_creature(unsigned char *thing);
unsigned char get_creature_next(unsigned char stype_idx);
unsigned char get_creature_prev(unsigned char stype_idx);
short is_door(unsigned char *thing);
unsigned char get_door_next(unsigned char stype_idx);
unsigned char get_door_prev(unsigned char stype_idx);
short is_roomeffect(unsigned char *thing);
unsigned char get_roomeffect_next(unsigned char stype_idx);
unsigned char get_roomeffect_prev(unsigned char stype_idx);
short is_statue(unsigned char *thing);
short is_statue_stype(unsigned char stype_idx);
unsigned char get_statue_next(unsigned char stype_idx);
unsigned char get_statue_prev(unsigned char stype_idx);
short is_furniture(unsigned char *thing);
short is_furniture_stype(unsigned char stype_idx);
unsigned char get_furniture_next(unsigned char stype_idx);
unsigned char get_furniture_prev(unsigned char stype_idx);
short is_food(unsigned char *thing);
short is_food_stype(unsigned char stype_idx);
unsigned char get_food_next(unsigned char stype_idx);
unsigned char get_food_prev(unsigned char stype_idx);
short is_gold(unsigned char *thing);
short is_gold_stype(unsigned char stype_idx);
unsigned char get_gold_next(unsigned char stype_idx);
unsigned char get_gold_prev(unsigned char stype_idx);
short is_torch(unsigned char *thing);
short is_torch_stype(unsigned char stype_idx);
unsigned char get_torch_next(unsigned char stype_idx);
unsigned char get_torch_prev(unsigned char stype_idx);
short is_herogate(unsigned char *thing);
short is_dnheart(unsigned char *thing);
short is_room_thing(unsigned char *thing);
short is_clmaffective_thing(unsigned char *thing);

int get_item_category(unsigned char stype_idx);
unsigned char get_usual_item_slab(unsigned char stype_idx);
unsigned char get_usual_thing_slab(unsigned char *thing);

char *get_thing_type_fullname(unsigned short type_idx);
char *get_thing_type_shortname(unsigned short type_idx);
char *get_item_subtype_fullname(unsigned short stype_idx);
char *get_item_subtype_shortname(unsigned short stype_idx);
char *get_creature_subtype_fullname(unsigned short stype_idx);
char *get_creature_subtype_shortname(unsigned short stype_idx);
char *get_trap_subtype_fullname(unsigned short stype_idx);
char *get_door_subtype_fullname(unsigned short stype_idx);
char *get_item_category_fullname(unsigned short stype_idx);

short thing_verify(unsigned char *thing, char *err_msg);

#endif // ADIKT_OBJTHINGS_H
