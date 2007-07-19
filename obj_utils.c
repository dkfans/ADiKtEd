/*
 * obj_utils.c
 */

#include "obj_utils.h"
#include "lev_data.h"
#include "globals.h"

const char *things[]={"Nothing", "Item/decoration", "unknown 2",
    "unknown 3", "unknown 4", "Creature", "unknown 6", "Room effect",
    "Trap", "Door"};
const char *traps[]={"", "Boulder", "Alarm", "Poison gas",
    "Lightning", "Word of power", "Lava", "Dummy 2", "Dummy 3",
    "Dummy 4", "Dummy 5", "Dummy 6", "Dummy 7"};
const char *doors[]={"", "Wooden", "Braced", "Iron", "Magic"};
const char *creature_names[]={"", "Wizrd", "Barb ", "Archr",
    "Monk ", "Dwarf", "Knigt", "Avtar", "Tunlr", "Witch",
    "Giant", "Fairy", "Thief", "Smuri", "Horny", "Skel ",
    "Troll", "Drgon", "Spawn", " Fly ", "DrkMs", "Warlk",
    "BileD", " Imp ", "Beetl", "Vamp ", "Spder", "Hound",
    "Ghost", "Tntcl", " Orc ", "Float"};
const char *creature_fullnames[]={"", "Wizard", "Barbarian",
    "Archer", "Monk", "Dwarf", "Knight", "Avatar", "Tunneller",
    "Witch", "Giant", "Fairy", "Thief", "Samurai", "Horned Reaper",
    "Skeleton", "Troll", "Dragon", "Demon Spawn", "Fly", "Dark Mistress",
    "Warlock", "Bile Demon", "Imp", "Beetle", "Vampire", "Spider",
    "Hell Hound", "Ghost", "Tentacle", "Orc", "Floating spirit"};
const char *owners[]={"Keeper 0 (Human)", "Keeper 1", "Keeper 2",
    "Keeper 3", "Heroes", "Unclaimed"};
const char *roomeffects[]={"unknown 0", "unknown 1", "Dripping water",
    "Rock Fall", "Entrance Ice", "Dry ice", "unknown 6", "unknown 7",
    "unknown 8"};

char *get_thing_type_fullname(unsigned char type_idx)
{
     int types_count=sizeof(things)/sizeof(char *);
     if (type_idx<types_count)
       return (char *)things[type_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns if the thing is a spellbook
 */
short is_spellbook(unsigned char *thing)
{
    //All spells are items
    if (thing[6] != THING_TYPE_ITEM)
      return false;
    //and they belong to SPELLBOOK category
    int cat_idx=get_item_category(thing[7]);
    if (cat_idx==ITEM_CATEGR_SPELLBOOK)
      return true;
    return false;
}

short is_action_thing(unsigned char *thing)
{
    int i;
    for (i=4; i < 17; i++)
      if (thing[i]!=0xff)
          return 0;
    return 1;
}

short is_room_thing(unsigned char *thing)
{
  switch (thing[6])
  {
  case THING_TYPE_ROOMEFFECT:
      if (thing[7]==ROOMEFC_SUBTP_ENTRICE) // Entrance ice room effect
        return true;
      return false;
  case THING_TYPE_DOOR:
      return true;
  case THING_TYPE_ITEM:
    { // Everything from ROOMEQUIP category is room_thing
      int cat_idx=get_item_category(thing[7]);
      if (cat_idx==ITEM_CATEGR_ROOMEQUIP)
        return true;
      // Also some other items which are not ROOMEQUIP
      switch (thing[7])
      {
      case ITEM_SUBTYPE_TORCH:     //the torch on claimed wall
      case ITEM_SUBTYPE_SPINNKEY:  //Key marking closed doors
        return true;
      }
    };return false;
  default:
    return false;
  }
}

/*
 * Returns if the object is crucial for the game. Such object
 * is not easily removed by the automatic placers.
 */
short is_crucial_thing(unsigned char *thing)
{
  switch (thing[6])
  {
  case THING_TYPE_ITEM:
      switch (thing[7])
      {
      case ITEM_SUBTYPE_DNHEART:
      case ITEM_SUBTYPE_HEROGATE:
        return true;
      }
    return false;
  default:
    return false;
  }
}

/*
 * Returns object type name as text
 */
char *get_item_subtype_fullname(unsigned char stype_idx)
{
      switch (stype_idx)
      {
        case ITEM_SUBTYPE_NULL:
          return "Null";
        case ITEM_SUBTYPE_BARREL:
          return "Barrel";
        case ITEM_SUBTYPE_TORCH:
          return "Torch";
        case ITEM_SUBTYPE_GOLDCHEST:
          return "Gold Chest (500)";
        case ITEM_SUBTYPE_TEMPLESTA:
          return "Temple statue";
        case ITEM_SUBTYPE_DNHEART:
          return "Dungeon heart";
        case ITEM_SUBTYPE_GOLD:
          return "Gold (250)";
        case ITEM_SUBTYPE_TORCHUN:
          return "Unlit torch";
        case ITEM_SUBTYPE_STATUEWO:
          return "Statue wo";
        case ITEM_SUBTYPE_CHICKNGRW:
          return "Growing Chicken";
        case ITEM_SUBTYPE_CHICKN:
          return "Chicken";
        //Spells
        case ITEM_SUBTYPE_SPELLHOE:
          return "Hand of evil spell";
        case ITEM_SUBTYPE_SPELLIMP:
          return "Create imp spell";
        case ITEM_SUBTYPE_SPELLMUST:
          return "Must obey spell";
        case ITEM_SUBTYPE_SPELLSLAP:
          return "Slap spell";
        case ITEM_SUBTYPE_SPELLSOE:
          return "Sight of evil spell";
        case ITEM_SUBTYPE_SPELLCTA:
          return "Call to arms spell";
        case ITEM_SUBTYPE_SPELLCAVI:
          return "Cave in spell";
        case ITEM_SUBTYPE_SPELLHEAL:
          return "Heal creature spell";
        case ITEM_SUBTYPE_SPELLHLDA:
          return "Hold audience spell";
        case ITEM_SUBTYPE_SPELLLIGH:
          return "Lightning spell";
        case ITEM_SUBTYPE_SPELLSPDC:
          return "Speed creature spell";
        case ITEM_SUBTYPE_SPELLPROT:
          return "Protect creature spell";
        case ITEM_SUBTYPE_SPELLCONC:
          return "Conceal creature spell";

        case ITEM_SUBTYPE_NULL1:
          return "Null1";
        case ITEM_SUBTYPE_NULL2:
          return "Null2";

        case ITEM_SUBTYPE_ANVIL:
          return "Anvil";
        case ITEM_SUBTYPE_PRISONBAR:
          return "Prison bar";
        case ITEM_SUBTYPE_CANDLSTCK:
          return "Candlestick";
        case ITEM_SUBTYPE_GRAVSTONE:
          return "Gravestone";
        case ITEM_SUBTYPE_STATUHORN:
          return "Horny Statue";
        case ITEM_SUBTYPE_TRAINPOST:
          return "Training post";
        case ITEM_SUBTYPE_TORTSPIKE:
          return "Torture spike";
        case ITEM_SUBTYPE_TEMPLESPN:
          return "Temple Spangle";

        case ITEM_SUBTYPE_POTION1:
          return "Potion1";
        case ITEM_SUBTYPE_POTION2:
          return "Potion2";
        case ITEM_SUBTYPE_POTION3:
          return "Potion3";
        case ITEM_SUBTYPE_PWHAND:
          return "Power hand";
        case ITEM_SUBTYPE_PWHANDGRB:
          return "Power hand grab";
        case ITEM_SUBTYPE_PWHANDWHP:
          return "Power hand whip";
        case ITEM_SUBTYPE_CHICKNSTB:
          return "Chicken stable";
        case ITEM_SUBTYPE_CHICKNWOB:
          return "Chicken wobble";
        case ITEM_SUBTYPE_CHICKNCRK:
          return "Chicken crack";
        case ITEM_SUBTYPE_CHICKN2:
          return "Chicken";
        case ITEM_SUBTYPE_SPINNKEY:
          return "Spinning key";
        //Spells
        case ITEM_SUBTYPE_SPELLDISE:
          return "Disease spell";
        case ITEM_SUBTYPE_SPELLCHKN:
          return "Chicken spell";
        case ITEM_SUBTYPE_SPELLDWAL:
          return "Destroy walls spell";
        case ITEM_SUBTYPE_SPELLTBMB:
          return "Time bomb spell";

        case ITEM_SUBTYPE_HEROGATE:
          return "Hero gate";
        case ITEM_SUBTYPE_UNKN32: 
          return "unknown32";

        //UNTESTED - indices may differ by one
        case ITEM_SUBTYPE_ARMOUR: 
          return "Armour";
        case ITEM_SUBTYPE_GLDHOARD1: 
          return "Gold Hoarde 1";
        case ITEM_SUBTYPE_GLDHOARD2: 
          return "Gold Hoarde 2";
        case ITEM_SUBTYPE_GLDHOARD3: 
          return "Gold Hoarde 3";
        case ITEM_SUBTYPE_GLDHOARD4: 
          return "Gold Hoarde 4";
        case ITEM_SUBTYPE_GLDHOARD5: 
          return "Gold Hoarde 5";

        //Creature lairs
        case ITEM_SUBTYPE_LAIRWIZRD: 
          return "Wizard Lair";
        case ITEM_SUBTYPE_LAIRBARBR: 
          return "Barbarian Lair";
        case ITEM_SUBTYPE_LAIRARCHR: 
          return "Archer Lair";
        case ITEM_SUBTYPE_LAIRMONK: 
          return "Monk Lair";
        case ITEM_SUBTYPE_LAIRDWRFA: 
          return "Dwarfa Lair";
        case ITEM_SUBTYPE_LAIRKNGHT: 
          return "Knight Lair";
        case ITEM_SUBTYPE_LAIRAVATR: 
          return "Avatar Lair";
        case ITEM_SUBTYPE_LAIRTUNLR: 
          return "Tunneller Lair";
        case ITEM_SUBTYPE_LAIRWITCH: 
          return "Witch Lair";
        case ITEM_SUBTYPE_LAIRGIANT: 
          return "Giant Lair";
        case ITEM_SUBTYPE_LAIRFAIRY:
          return "Fairy Lair";
        case ITEM_SUBTYPE_LAIRTHEFT: 
          return "Thief Lair";
        case ITEM_SUBTYPE_LAIRSAMUR: 
          return "Samurai Lair";
        case ITEM_SUBTYPE_LAIRHORNY: 
          return "Horny Lair";
        case ITEM_SUBTYPE_LAIRSKELT: 
          return "Skeleton Lair";
        case ITEM_SUBTYPE_LAIRGOBLN: 
          return "Goblin Lair";
        case ITEM_SUBTYPE_LAIRDRAGN: 
          return "Dragon Lair";
        case ITEM_SUBTYPE_LAIRDEMSP: 
          return "Demonspawn Lair";
        case ITEM_SUBTYPE_LAIRFLY: 
          return "Fly Lair";
        case ITEM_SUBTYPE_LAIRDKMIS: 
          return "Dark Mistress Lair";
        case ITEM_SUBTYPE_LAIRSORCR: 
          return "Sorceror Lair";
        case ITEM_SUBTYPE_LAIRBILDM: 
          return "Bile Demon Lair";
        case ITEM_SUBTYPE_LAIRIMP: 
          return "Imp Lair";
        case ITEM_SUBTYPE_LAIRBUG: 
          return "Bug Lair";
        case ITEM_SUBTYPE_LAIRVAMP: 
          return "Vampire Lair";
        case ITEM_SUBTYPE_LAIRSPIDR: 
          return "Spider Lair";
        case ITEM_SUBTYPE_LAIRHLHND: 
          return "Hell Hound Lair";
        case ITEM_SUBTYPE_LAIRGHOST: 
          return "Ghost Lair";
        case ITEM_SUBTYPE_LAIRTENTC: 
          return "Tentacle Lair";

        //Dungeon Specials
        case ITEM_SUBTYPE_SPREVMAP: 
          return "Reveal map";
        case ITEM_SUBTYPE_SPRESURCT:
          return "Resurrect creature";
        case ITEM_SUBTYPE_SPTRANSFR:
          return "Transfer creature";
        case ITEM_SUBTYPE_SPSTEALHR:
          return "Steal hero";
        case ITEM_SUBTYPE_SPMULTPLY:
          return "Multiply creatures";
        case ITEM_SUBTYPE_SPINCLEV:
          return "Increase level";
        case ITEM_SUBTYPE_SPMKSAFE:
          return "Make safe";
        case ITEM_SUBTYPE_SPHIDNWRL:
          return "Reveal hidden world";
        //Traps
        case ITEM_SUBTYPE_TBBOULDER:
          return "Boulder trap box";
        case ITEM_SUBTYPE_TBALARM:
          return "Alarm trap box";
        case ITEM_SUBTYPE_TBPOISONG:
          return "Poison gas trap box";
        case ITEM_SUBTYPE_TBLIGHTNG:
          return "Lightning trap box";
        case ITEM_SUBTYPE_TBWRDOFPW:
          return "Word of Power trap box";
        case ITEM_SUBTYPE_TBLAVA:
          return "Lava trap box";
        case ITEM_SUBTYPE_TBDUMMY2:
          return "Trap Dummy 2";
        case ITEM_SUBTYPE_TBDUMMY3:
          return "Trap Dummy 3";
        case ITEM_SUBTYPE_TBDUMMY4:
          return "Trap Dummy 4";
        case ITEM_SUBTYPE_TBDUMMY5:
          return "Trap Dummy 5";
        case ITEM_SUBTYPE_TBDUMMY6:
          return "Trap Dummy 6";
        case ITEM_SUBTYPE_TBDUMMY7:
          return "Trap Dummy 7";

        //Workshop items
        case ITEM_SUBTYPE_DBWOOD:
          return "Door Wood box";
        case ITEM_SUBTYPE_DBBRACE:
          return "Door Brace box";
        case ITEM_SUBTYPE_DBSTEEL:
          return "Door Steel box";
        case ITEM_SUBTYPE_DBMAGIC:
          return "Door Magic box";
        case ITEM_SUBTYPE_WBITEM:
          return "Workshop Item";
        //Room equipment
        case ITEM_SUBTYPE_HEARTFLMR:
          return "Heart flame (red)";
        case ITEM_SUBTYPE_DISEASE:
          return "Disease";
        case ITEM_SUBTYPE_SCAVNGEYE:
          return "Scavenger eye";
        case ITEM_SUBTYPE_WRKSHPMCH:
          return "Workshop machine";
        case ITEM_SUBTYPE_GURDFLAGR:
          return "Guard Flag (red)";
        case ITEM_SUBTYPE_GURDFLAGB:
          return "Guard Flag (blue)";
        case ITEM_SUBTYPE_GURDFLAGG:
          return "Guard Flag (green)";
        case ITEM_SUBTYPE_GURDFLAGY:
          return "Guard Flag (yellow)";
        case ITEM_SUBTYPE_FLAGPOST:
          return "Flagpost";
        case ITEM_SUBTYPE_HEARTFLMB:
          return "Heart flame (blue)";
        case ITEM_SUBTYPE_HEARTFLMG:
          return "Heart flame (green)";
        case ITEM_SUBTYPE_HEARTFLMY:
          return "Heart flame (yellow)";

        //Special effects
        case ITEM_SUBTYPE_PWSIGHT:
          return "Power Sight";
        case ITEM_SUBTYPE_PWLIGHTNG:
          return "Power Lightning";
        //Other, mostly untested
        case ITEM_SUBTYPE_TORTURER:
          return "Torturer";
        case ITEM_SUBTYPE_LAIRORC:
          return "Orc Lair";
        case ITEM_SUBTYPE_PWHANDGLD:
          return "Power Hand Gold";
        case ITEM_SUBTYPE_COIN:
          return "Coin";

        case ITEM_SUBTYPE_STATUE2:
          return "Statue 2";
        case ITEM_SUBTYPE_STATUE3:
          return "Statue 3";
        case ITEM_SUBTYPE_STATUE4:
          return "Statue 4";
        case ITEM_SUBTYPE_STATUE5:
          return "Statue 5";
        case ITEM_SUBTYPE_STATUE6:
          return "Statue 6";
        case ITEM_SUBTYPE_SPELLARMG:
          return "Armageddon Spell";
        default:
          return "Unknown";
      }
}

/*
 * Returns object type name which is max 5 chars long
 */
char *get_item_subtype_shortname(unsigned char stype_idx)
{
      switch (stype_idx)
      {
        case ITEM_SUBTYPE_NULL:
        case ITEM_SUBTYPE_NULL1:
        case ITEM_SUBTYPE_NULL2:
          return "Null";
        case ITEM_SUBTYPE_BARREL:
          return "Barrl";
        case ITEM_SUBTYPE_TORCH:
        case ITEM_SUBTYPE_TORCHUN:
          return "Torch";
        case ITEM_SUBTYPE_GOLDCHEST:
          return "GldCh";
        case ITEM_SUBTYPE_TEMPLESTA:
        case ITEM_SUBTYPE_STATUEWO:
        case ITEM_SUBTYPE_STATUHORN:
        case ITEM_SUBTYPE_STATUE2:
        case ITEM_SUBTYPE_STATUE3:
        case ITEM_SUBTYPE_STATUE4:
        case ITEM_SUBTYPE_STATUE5:
        case ITEM_SUBTYPE_STATUE6:
          return "Statu";
        case ITEM_SUBTYPE_DNHEART:
          return "Heart";
        case ITEM_SUBTYPE_GOLD:
          return "Gold";
        case ITEM_SUBTYPE_CHICKNGRW:
        case ITEM_SUBTYPE_CHICKN:
        case ITEM_SUBTYPE_CHICKNSTB:
        case ITEM_SUBTYPE_CHICKNWOB:
        case ITEM_SUBTYPE_CHICKNCRK:
        case ITEM_SUBTYPE_CHICKN2:
          return "Food";
        //Spells
        case ITEM_SUBTYPE_SPELLHOE:
        case ITEM_SUBTYPE_SPELLIMP:
        case ITEM_SUBTYPE_SPELLMUST:
        case ITEM_SUBTYPE_SPELLSLAP:
        case ITEM_SUBTYPE_SPELLSOE:
        case ITEM_SUBTYPE_SPELLCTA:
        case ITEM_SUBTYPE_SPELLCAVI:
        case ITEM_SUBTYPE_SPELLHEAL:
        case ITEM_SUBTYPE_SPELLHLDA:
        case ITEM_SUBTYPE_SPELLLIGH:
        case ITEM_SUBTYPE_SPELLSPDC:
        case ITEM_SUBTYPE_SPELLPROT:
        case ITEM_SUBTYPE_SPELLCONC:
        case ITEM_SUBTYPE_SPELLDISE:
        case ITEM_SUBTYPE_SPELLCHKN:
        case ITEM_SUBTYPE_SPELLDWAL:
        case ITEM_SUBTYPE_SPELLTBMB:
        case ITEM_SUBTYPE_SPELLARMG:
          return "Spell";

        case ITEM_SUBTYPE_ANVIL:
          return "Anvil";
        case ITEM_SUBTYPE_PRISONBAR:
          return "PrBar";
        case ITEM_SUBTYPE_CANDLSTCK:
          return "Candl";
        case ITEM_SUBTYPE_GRAVSTONE:
          return "Grave";
        case ITEM_SUBTYPE_TRAINPOST:
          return "Trinr";
        case ITEM_SUBTYPE_TORTSPIKE:
          return "Spike";
        case ITEM_SUBTYPE_TEMPLESPN:
          return "Spangl";

        case ITEM_SUBTYPE_POTION1:
        case ITEM_SUBTYPE_POTION2:
        case ITEM_SUBTYPE_POTION3:
          return "Potin";
        case ITEM_SUBTYPE_PWHAND:
        case ITEM_SUBTYPE_PWHANDGRB:
        case ITEM_SUBTYPE_PWHANDWHP:
        case ITEM_SUBTYPE_PWHANDGLD:
          return "PHand";
        case ITEM_SUBTYPE_SPINNKEY:
          return "SpKey";

        case ITEM_SUBTYPE_HEROGATE:
          return "HGate";
        case ITEM_SUBTYPE_UNKN32: 
          return "unk32";

        case ITEM_SUBTYPE_ARMOUR: 
          return "Armor";
        case ITEM_SUBTYPE_GLDHOARD1: 
        case ITEM_SUBTYPE_GLDHOARD2: 
        case ITEM_SUBTYPE_GLDHOARD3: 
        case ITEM_SUBTYPE_GLDHOARD4: 
        case ITEM_SUBTYPE_GLDHOARD5: 
          return "GldHr";
        //Creature lairs
        case ITEM_SUBTYPE_LAIRWIZRD: 
        case ITEM_SUBTYPE_LAIRBARBR: 
        case ITEM_SUBTYPE_LAIRARCHR: 
        case ITEM_SUBTYPE_LAIRMONK: 
        case ITEM_SUBTYPE_LAIRDWRFA: 
        case ITEM_SUBTYPE_LAIRKNGHT: 
        case ITEM_SUBTYPE_LAIRAVATR: 
        case ITEM_SUBTYPE_LAIRTUNLR: 
        case ITEM_SUBTYPE_LAIRWITCH: 
        case ITEM_SUBTYPE_LAIRGIANT: 
        case ITEM_SUBTYPE_LAIRFAIRY:
        case ITEM_SUBTYPE_LAIRTHEFT: 
        case ITEM_SUBTYPE_LAIRSAMUR: 
        case ITEM_SUBTYPE_LAIRHORNY: 
        case ITEM_SUBTYPE_LAIRSKELT: 
        case ITEM_SUBTYPE_LAIRGOBLN: 
        case ITEM_SUBTYPE_LAIRDRAGN: 
        case ITEM_SUBTYPE_LAIRDEMSP: 
        case ITEM_SUBTYPE_LAIRFLY: 
        case ITEM_SUBTYPE_LAIRDKMIS: 
        case ITEM_SUBTYPE_LAIRSORCR: 
        case ITEM_SUBTYPE_LAIRBILDM: 
        case ITEM_SUBTYPE_LAIRIMP: 
        case ITEM_SUBTYPE_LAIRBUG: 
        case ITEM_SUBTYPE_LAIRVAMP: 
        case ITEM_SUBTYPE_LAIRSPIDR: 
        case ITEM_SUBTYPE_LAIRHLHND: 
        case ITEM_SUBTYPE_LAIRGHOST: 
        case ITEM_SUBTYPE_LAIRTENTC: 
        case ITEM_SUBTYPE_LAIRORC:
          return "Lair";
        //Dungeon Specials
        case ITEM_SUBTYPE_SPREVMAP: 
        case ITEM_SUBTYPE_SPRESURCT:
        case ITEM_SUBTYPE_SPTRANSFR:
        case ITEM_SUBTYPE_SPSTEALHR:
        case ITEM_SUBTYPE_SPMULTPLY:
        case ITEM_SUBTYPE_SPINCLEV:
        case ITEM_SUBTYPE_SPMKSAFE:
        case ITEM_SUBTYPE_SPHIDNWRL:
          return "SpcBx";
        //Traps
        case ITEM_SUBTYPE_TBBOULDER:
        case ITEM_SUBTYPE_TBALARM:
        case ITEM_SUBTYPE_TBPOISONG:
        case ITEM_SUBTYPE_TBLIGHTNG:
        case ITEM_SUBTYPE_TBWRDOFPW:
        case ITEM_SUBTYPE_TBLAVA:
        case ITEM_SUBTYPE_TBDUMMY2:
        case ITEM_SUBTYPE_TBDUMMY3:
        case ITEM_SUBTYPE_TBDUMMY4:
        case ITEM_SUBTYPE_TBDUMMY5:
        case ITEM_SUBTYPE_TBDUMMY6:
        case ITEM_SUBTYPE_TBDUMMY7:
          return "TrpBx";
        //Workshop items
        case ITEM_SUBTYPE_DBWOOD:
        case ITEM_SUBTYPE_DBBRACE:
        case ITEM_SUBTYPE_DBSTEEL:
        case ITEM_SUBTYPE_DBMAGIC:
          return "DorBx";
        case ITEM_SUBTYPE_WBITEM:
          return "WshBx";
        //Room equipment
        case ITEM_SUBTYPE_HEARTFLMR:
        case ITEM_SUBTYPE_HEARTFLMB:
        case ITEM_SUBTYPE_HEARTFLMG:
        case ITEM_SUBTYPE_HEARTFLMY:
          return "Flame";
        case ITEM_SUBTYPE_DISEASE:
          return "Dises";
        case ITEM_SUBTYPE_SCAVNGEYE:
          return "ScEye";
        case ITEM_SUBTYPE_WRKSHPMCH:
          return "Machn";
        case ITEM_SUBTYPE_GURDFLAGR:
        case ITEM_SUBTYPE_GURDFLAGB:
        case ITEM_SUBTYPE_GURDFLAGG:
        case ITEM_SUBTYPE_GURDFLAGY:
          return "Flag";
        case ITEM_SUBTYPE_FLAGPOST:
          return "FlagP";
        case ITEM_SUBTYPE_PWSIGHT:
          return "PwSig";
        case ITEM_SUBTYPE_PWLIGHTNG:
          return "PwLgh";
        case ITEM_SUBTYPE_TORTURER:
          return "Tortr";
        case ITEM_SUBTYPE_COIN:
          return "Coin";
        default:
          return "thing";
      }
}

/*
 * Returns itam category for given subtype
 */
int get_item_category(unsigned char stype_idx)
{
      switch (stype_idx)
      {
        case ITEM_SUBTYPE_NULL:
        case ITEM_SUBTYPE_NULL1:
        case ITEM_SUBTYPE_NULL2:
          return ITEM_CATEGR_NULL;

        case ITEM_SUBTYPE_GOLDCHEST:
        case ITEM_SUBTYPE_GOLD:
        case ITEM_SUBTYPE_COIN:
        case ITEM_SUBTYPE_GLDHOARD1: 
        case ITEM_SUBTYPE_GLDHOARD2: 
        case ITEM_SUBTYPE_GLDHOARD3: 
        case ITEM_SUBTYPE_GLDHOARD4: 
        case ITEM_SUBTYPE_GLDHOARD5: 
          return ITEM_CATEGR_GOLD;

        case ITEM_SUBTYPE_STATUEWO:
        case ITEM_SUBTYPE_STATUHORN:
        case ITEM_SUBTYPE_STATUE2:
        case ITEM_SUBTYPE_STATUE3:
        case ITEM_SUBTYPE_STATUE4:
        case ITEM_SUBTYPE_STATUE5:
        case ITEM_SUBTYPE_STATUE6:
          return ITEM_CATEGR_STATUES;

        case ITEM_SUBTYPE_DNHEART:
        case ITEM_SUBTYPE_HEROGATE:
        case ITEM_SUBTYPE_BARREL:
        case ITEM_SUBTYPE_TEMPLESTA:
        case ITEM_SUBTYPE_TEMPLESPN:
        case ITEM_SUBTYPE_ANVIL:
        case ITEM_SUBTYPE_PRISONBAR:
        case ITEM_SUBTYPE_SCAVNGEYE:
        case ITEM_SUBTYPE_WRKSHPMCH:
        case ITEM_SUBTYPE_GURDFLAGR:
        case ITEM_SUBTYPE_GURDFLAGB:
        case ITEM_SUBTYPE_GURDFLAGG:
        case ITEM_SUBTYPE_GURDFLAGY:
        case ITEM_SUBTYPE_FLAGPOST:
        case ITEM_SUBTYPE_CANDLSTCK:
        case ITEM_SUBTYPE_GRAVSTONE:
        case ITEM_SUBTYPE_TRAINPOST:
        case ITEM_SUBTYPE_TORTSPIKE:
        case ITEM_SUBTYPE_HEARTFLMR:
        case ITEM_SUBTYPE_HEARTFLMB:
        case ITEM_SUBTYPE_HEARTFLMG:
        case ITEM_SUBTYPE_HEARTFLMY:
        case ITEM_SUBTYPE_TORTURER:
          return ITEM_CATEGR_ROOMEQUIP;

        case ITEM_SUBTYPE_CHICKNGRW:
        case ITEM_SUBTYPE_CHICKN:
        case ITEM_SUBTYPE_CHICKNSTB:
        case ITEM_SUBTYPE_CHICKNWOB:
        case ITEM_SUBTYPE_CHICKNCRK:
        case ITEM_SUBTYPE_CHICKN2:
          return ITEM_CATEGR_FOOD;

        //Spells
        case ITEM_SUBTYPE_SPELLHOE:
        case ITEM_SUBTYPE_SPELLIMP:
        case ITEM_SUBTYPE_SPELLMUST:
        case ITEM_SUBTYPE_SPELLSLAP:
        case ITEM_SUBTYPE_SPELLSOE:
        case ITEM_SUBTYPE_SPELLCTA:
        case ITEM_SUBTYPE_SPELLCAVI:
        case ITEM_SUBTYPE_SPELLHEAL:
        case ITEM_SUBTYPE_SPELLHLDA:
        case ITEM_SUBTYPE_SPELLLIGH:
        case ITEM_SUBTYPE_SPELLSPDC:
        case ITEM_SUBTYPE_SPELLPROT:
        case ITEM_SUBTYPE_SPELLCONC:
        case ITEM_SUBTYPE_SPELLDISE:
        case ITEM_SUBTYPE_SPELLCHKN:
        case ITEM_SUBTYPE_SPELLDWAL:
        case ITEM_SUBTYPE_SPELLTBMB:
        case ITEM_SUBTYPE_SPELLARMG:
          return ITEM_CATEGR_SPELLBOOK;

        case ITEM_SUBTYPE_TORCH:
        case ITEM_SUBTYPE_TORCHUN:
          return ITEM_CATEGR_LIGHTS;

        //Creature lairs
        case ITEM_SUBTYPE_LAIRWIZRD: 
        case ITEM_SUBTYPE_LAIRBARBR: 
        case ITEM_SUBTYPE_LAIRARCHR: 
        case ITEM_SUBTYPE_LAIRMONK: 
        case ITEM_SUBTYPE_LAIRDWRFA: 
        case ITEM_SUBTYPE_LAIRKNGHT: 
        case ITEM_SUBTYPE_LAIRAVATR: 
        case ITEM_SUBTYPE_LAIRTUNLR: 
        case ITEM_SUBTYPE_LAIRWITCH: 
        case ITEM_SUBTYPE_LAIRGIANT: 
        case ITEM_SUBTYPE_LAIRFAIRY:
        case ITEM_SUBTYPE_LAIRTHEFT: 
        case ITEM_SUBTYPE_LAIRSAMUR: 
        case ITEM_SUBTYPE_LAIRHORNY: 
        case ITEM_SUBTYPE_LAIRSKELT: 
        case ITEM_SUBTYPE_LAIRGOBLN: 
        case ITEM_SUBTYPE_LAIRDRAGN: 
        case ITEM_SUBTYPE_LAIRDEMSP: 
        case ITEM_SUBTYPE_LAIRFLY: 
        case ITEM_SUBTYPE_LAIRDKMIS: 
        case ITEM_SUBTYPE_LAIRSORCR: 
        case ITEM_SUBTYPE_LAIRBILDM: 
        case ITEM_SUBTYPE_LAIRIMP: 
        case ITEM_SUBTYPE_LAIRBUG: 
        case ITEM_SUBTYPE_LAIRVAMP: 
        case ITEM_SUBTYPE_LAIRSPIDR: 
        case ITEM_SUBTYPE_LAIRHLHND: 
        case ITEM_SUBTYPE_LAIRGHOST: 
        case ITEM_SUBTYPE_LAIRTENTC: 
        case ITEM_SUBTYPE_LAIRORC:
          return ITEM_CATEGR_CREATLAIR;

        //Dungeon Specials
        case ITEM_SUBTYPE_SPREVMAP: 
        case ITEM_SUBTYPE_SPRESURCT:
        case ITEM_SUBTYPE_SPTRANSFR:
        case ITEM_SUBTYPE_SPSTEALHR:
        case ITEM_SUBTYPE_SPMULTPLY:
        case ITEM_SUBTYPE_SPINCLEV:
        case ITEM_SUBTYPE_SPMKSAFE:
        case ITEM_SUBTYPE_SPHIDNWRL:
          return ITEM_CATEGR_SPECIALBOX;

        //Traps and other workshop items
        case ITEM_SUBTYPE_TBBOULDER:
        case ITEM_SUBTYPE_TBALARM:
        case ITEM_SUBTYPE_TBPOISONG:
        case ITEM_SUBTYPE_TBLIGHTNG:
        case ITEM_SUBTYPE_TBWRDOFPW:
        case ITEM_SUBTYPE_TBLAVA:
        case ITEM_SUBTYPE_TBDUMMY2:
        case ITEM_SUBTYPE_TBDUMMY3:
        case ITEM_SUBTYPE_TBDUMMY4:
        case ITEM_SUBTYPE_TBDUMMY5:
        case ITEM_SUBTYPE_TBDUMMY6:
        case ITEM_SUBTYPE_TBDUMMY7:
        case ITEM_SUBTYPE_DBWOOD:
        case ITEM_SUBTYPE_DBBRACE:
        case ITEM_SUBTYPE_DBSTEEL:
        case ITEM_SUBTYPE_DBMAGIC:
        case ITEM_SUBTYPE_WBITEM:
          return ITEM_CATEGR_WRKSHOPBOX;

        case ITEM_SUBTYPE_PWHAND:
        case ITEM_SUBTYPE_PWHANDGRB:
        case ITEM_SUBTYPE_PWHANDWHP:
        case ITEM_SUBTYPE_PWHANDGLD:
        case ITEM_SUBTYPE_DISEASE:
        case ITEM_SUBTYPE_PWSIGHT:
        case ITEM_SUBTYPE_PWLIGHTNG:
          return ITEM_CATEGR_PWHNDEFFCT;

        case ITEM_SUBTYPE_POTION1:
        case ITEM_SUBTYPE_POTION2:
        case ITEM_SUBTYPE_POTION3:
        case ITEM_SUBTYPE_SPINNKEY:
        case ITEM_SUBTYPE_ARMOUR: 
        case ITEM_SUBTYPE_UNKN32: 
        default:
          return ITEM_CATEGR_VARIOUS;
      }
}

/*
 * Returns creature type name string
 */
char *get_creature_subtype_fullname(unsigned char stype_idx)
{
     int types_count=sizeof(creature_fullnames)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)creature_fullnames[stype_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns creature type name which is max 5 chars long
 */
char *get_creature_subtype_shortname(unsigned char stype_idx)
{
     int types_count=sizeof(creature_names)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)creature_names[stype_idx];
     else
       return "unknw";
}

/*
 * Returns trap type name string
 */
char *get_trap_subtype_fullname(unsigned char stype_idx)
{
     int types_count=sizeof(traps)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)traps[stype_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns door type name string
 */
char *get_door_subtype_fullname(unsigned char stype_idx)
{
     int types_count=sizeof(doors)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)doors[stype_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns square owner name string
 */
char *get_owner_type_fullname(unsigned char type_idx)
{
     int types_count=sizeof(owners)/sizeof(char *);
     if (type_idx<types_count)
       return (char *)owners[type_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns item category text for given subtype
 */
char *get_item_category_fullname(unsigned char stype_idx)
{
     int cat_idx=get_item_category(stype_idx);
     switch (cat_idx)
     {
     case ITEM_CATEGR_NULL:
          return "Null objects";
     case ITEM_CATEGR_VARIOUS:
          return "Various objects";
     case ITEM_CATEGR_ROOMEQUIP:
          return "Room equipment";
     case ITEM_CATEGR_SPELLBOOK:
          return "Spell books";
     case ITEM_CATEGR_STATUES:
          return "Statues";
     case ITEM_CATEGR_GOLD:
          return "Valuable items";
     case ITEM_CATEGR_FOOD:
          return "Creature food";
     case ITEM_CATEGR_CREATLAIR:
          return "Creature lairs";
     case ITEM_CATEGR_SPECIALBOX:
          return "Dungeon Special box";
     case ITEM_CATEGR_WRKSHOPBOX:
          return "Workshop item box";
     case ITEM_CATEGR_PWHNDEFFCT:
          return "Power Hand effect";
     case ITEM_CATEGR_LIGHTS:
          return "Light objects";
     default:
          return "Unknown category";
     }
}

/*
 * Returns room effect description string
 */
char *get_roomeffect_subtype_fullname(unsigned char stype_idx)
{
     int types_count=sizeof(roomeffects)/sizeof(char *);
     if (stype_idx<types_count)
       return (char *)roomeffects[stype_idx];
     else
       return "unknown(?!)";
}

/*
 * Says whether a slab is surrounded by the same things
 */
short slab_is_central(LEVEL *lvl,int x,int y)
{
    if ((x<=0) || (y<=0) || (x>=MAP_MAXINDEX_X) || (y>=MAP_MAXINDEX_Y))
      return false;
    int i, j;
    for (i=-1; i < 2; i++)
      for (j=-1; j < 2; j++)
        if ((lvl->slb[x][y]!=lvl->slb[i+x][j+y])||(lvl->own[x][y]!=lvl->own[i+x][j+y]))
          return false;
    return true;
}

/*
 * Says how many slabs of given type are in 3x3 area surrounding given slab,
 * and including it; returns 0..9;
 */
int slab_siblings_oftype(LEVEL *lvl,int x,int y,unsigned char slab_type)
{
    int amount=0;
    int i, j;
    for (i=x-1; i < x+2; i++)
      for (j=y-1; j < y+2; j++)
      {
      if ((i>=0) && (j>=0) && (i<MAP_SIZE_X) && (j<MAP_SIZE_Y))
        if (lvl->slb[i][j]==slab_type)
          amount++;
      }
    return amount;
}

/*
 * Returns a WBI file entry for given slab
 */
int slab_default_wbi_entry(unsigned char slab_type)
{
  //NOTE: the default levels sometimes have different values here.
  // maybe a subtile starts to be animated after it is revealed?
    switch (slab_type)
    {
    case SLAB_TYPE_LAVA:
    case SLAB_TYPE_WATER:
        return SLAB_WIB_ANIMATE;
    default:
        return SLAB_WIB_SKEW;
    }
}

