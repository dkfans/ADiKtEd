/*
 * obj_utils.h
 */

#ifndef ADIKT_OBJUTILS_H
#define ADIKT_OBJUTILS_H

#include "lev_data.h"

#define SLAB_TYPE_ROCK         0x00
#define SLAB_TYPE_GOLD         0x01
#define SLAB_TYPE_EARTH        0x02
#define SLAB_TYPE_TORCHDIRT    0x03
#define SLAB_TYPE_DRAPE        0x04
#define SLAB_TYPE_TORCHWALL    0x05
#define SLAB_TYPE_WALLWTWINS   0x06
#define SLAB_TYPE_WALLWWOMAN   0x07
#define SLAB_TYPE_WALLPAIRSHR  0x08
#define SLAB_TYPE_PATH         0x0a
#define SLAB_TYPE_CLAIMED      0x0b
#define SLAB_TYPE_LAVA         0x0c
#define SLAB_TYPE_WATER        0x0d
#define SLAB_TYPE_PORTAL       0x0e
#define SLAB_TYPE_TREASURE     0x10
#define SLAB_TYPE_LIBRARY      0x12
#define SLAB_TYPE_PRISONCASE   0x14
#define SLAB_TYPE_TORTURE      0x16
#define SLAB_TYPE_TRAINING     0x18
#define SLAB_TYPE_DUNGHEART    0x1a
#define SLAB_TYPE_WORKSHOP     0x1c
#define SLAB_TYPE_SCAVENGER    0x1e
#define SLAB_TYPE_TEMPLE       0x20
#define SLAB_TYPE_GRAVEYARD    0x22
#define SLAB_TYPE_HATCHERY     0x24
#define SLAB_TYPE_LAIR         0x26
#define SLAB_TYPE_BARRACKS     0x28
#define SLAB_TYPE_DOORWOOD1    0x2a
#define SLAB_TYPE_DOORWOOD2    0x2b
#define SLAB_TYPE_DOORBRACE1   0x2c
#define SLAB_TYPE_DOORBRACE2   0x2d
#define SLAB_TYPE_DOORIRON1    0x2e
#define SLAB_TYPE_DOORIRON2    0x2f
#define SLAB_TYPE_DOORMAGIC1   0x30
#define SLAB_TYPE_DOORMAGIC2   0x31
#define SLAB_TYPE_BRIDGE       0x33
#define SLAB_TYPE_GEMS         0x34
#define SLAB_TYPE_GUARDPOST    0x35

#define SLAB_WIB_STATIC        0x00
#define SLAB_WIB_SKEW          0x01
#define SLAB_WIB_ANIMATE       0x02

#define OWNERS_COUNT           6

#define THING_TYPE_ITEM        0x01
#define THING_TYPE_CREATURE    0x05
#define THING_TYPE_ROOMEFFECT  0x07
#define THING_TYPE_TRAP        0x08
#define THING_TYPE_DOOR        0x09

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

#define ROOMEFC_SUBTP_DRIPWTR  0x02
#define ROOMEFC_SUBTP_ROCKFAL  0x03
#define ROOMEFC_SUBTP_ENTRICE  0x04
#define ROOMEFC_SUBTP_DRYICE   0x05

extern short is_spellbook(unsigned char *thing);
extern short is_action_thing(unsigned char *thing);
extern short is_room_thing (unsigned char *thing);
extern int get_item_category(unsigned char stype_idx);
extern short slab_is_central(LEVEL *lvl,int x,int y);
extern int slab_default_wbi_entry(unsigned char slab_type);
extern int slab_siblings_oftype(LEVEL *lvl,int x,int y,unsigned char slab_type);

extern char *get_thing_type_fullname(unsigned char type_idx);
extern char *get_item_subtype_fullname(unsigned char stype_idx);
extern char *get_item_subtype_shortname(unsigned char stype_idx);
extern char *get_creature_subtype_fullname(unsigned char stype_idx);
extern char *get_creature_subtype_shortname(unsigned char stype_idx);
extern char *get_trap_subtype_fullname(unsigned char stype_idx);
extern char *get_door_subtype_fullname(unsigned char stype_idx);
extern char *get_owner_type_fullname(unsigned char type_idx);
extern char *get_item_category_fullname(unsigned char stype_idx);

#endif // ADIKT_OBJUTILS_H
