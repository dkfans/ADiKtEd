/*
 * obj_column.c
 *
 * Functions for maintaining single CLM entries.
 *
 */

#include "obj_column.h"

#include "lev_data.h"
#include "globals.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "graffiti.h"

static void (*custom_columns_gen [])(struct COLUMN_REC *clm_recs[9],
        unsigned char *,unsigned char *, unsigned char **)={
     create_columns_slb_rock,create_columns_slb_gold,              //00
     create_columns_slb_fulldirt,create_columns_slb_earth,
     create_columns_slb_torchdirt,
     create_columns_slb_walldrape,create_columns_slb_walltorch,
     create_columns_slb_wallwtwins,create_columns_slb_wallwwoman,
     create_columns_slb_wallpairshr,
     create_columns_slb_path,create_columns_slb_claimed,           //0a
     create_columns_slb_lava,create_columns_slb_water,
     create_columns_slb_portal,create_columns_slb_treasure,
     create_columns_slb_library,create_columns_slb_prison,
     create_columns_slb_torture,create_columns_slb_training,
     create_columns_slb_dungheart,create_columns_slb_workshop,     //14
     create_columns_slb_scavenger,create_columns_slb_temple,
     create_columns_slb_graveyard,create_columns_slb_hatchery,
     create_columns_slb_lair,create_columns_slb_barracks,          //1a
     create_columns_slb_doorwood,create_columns_slb_doorbrace,
     create_columns_slb_dooriron,create_columns_slb_doormagic,
     create_columns_slb_bridge,create_columns_slb_gems,            //20
     create_columns_slb_guardpost,
     create_columns_slb_thingems_path,create_columns_slb_rock_gndlev,
     create_columns_slb_rockcaped_pathcave,create_columns_slb_skulls_on_lava,
     create_columns_slb_skulls_on_path,create_columns_slb_skulls_on_claimed,
     create_columns_slb_wall_force_relief_splatbody,
     };

const char *custom_columns_fullnames[]={
     "Standard Rock","Standard Gold",              //00
     "Unaffected Earth","Standard Earth",
     SLB_TORCHDIRT_LTEXT,
     SLB_WALLDRAPE_LTEXT,SLB_WALLTORCH_LTEXT,
     SLB_WALLWTWINS_LTEXT,SLB_WALLWWOMAN_LTEXT,
     SLB_WALLPAIRSHR_LTEXT,
     "Standard Path",SLB_CLAIMED_LTEXT,           //0A
     "Standard Lava","Standard Water",
     SLB_PORTAL_LTEXT,SLB_TREASURE_LTEXT,
     SLB_LIBRARY_LTEXT,SLB_PRISON_LTEXT,
     SLB_TORTURE_LTEXT,SLB_TRAINING_LTEXT,
     SLB_DUNGHEART_LTEXT,SLB_WORKSHOP_LTEXT,     //14
     SLB_SCAVENGER_LTEXT,SLB_TEMPLE_LTEXT,
     SLB_GRAVEYARD_LTEXT,SLB_HATCHERY_LTEXT,
     SLB_LAIR_LTEXT,SLB_BARRACKS_LTEXT,          //1A
     SLB_DOORWOOD_LTEXT,SLB_DOORBRACE_LTEXT,
     SLB_DOORIRON_LTEXT,SLB_DOORMAGIC_LTEXT,
     SLB_BRIDGE_LTEXT,"Standard Gems",            //20
     SLB_GUARDPOST_LTEXT,
     "Thin gems on path","Ground level rock",
     "Rock with cave","Skulls on Lava",
     "Skulls on Path","Skulls on Claimed",
     "Wall w/body splat",
     };

const char *cube_fullnames[]={
 "empty cube",          "standard earth 1",   "standard earth 2",     "standard earth 3",//000
 "tamped earth 1",      "tamped earth 2",     "tamped earth 3",       "unknown cube 0x0007",
 "unknown cube 0x0008", "unknown cube 0x0009", "unknown cube 0x000a", "unknown cube 0x000b",//008
 "unknown cube 0x000c", "unknown cube 0x000d", "unknown cube 0x000e", "unknown cube 0x000f",
 "unknown cube 0x0010", "unknown cube 0x0011", "unknown cube 0x0012", "unknown cube 0x0013",//010
 "unknown cube 0x0014", "unknown cube 0x0015", "unknown cube 0x0016", "unknown cube 0x0017",
 "unknown cube 0x0018", "unknown cube 0x0019", "unknown cube 0x001a", "unknown cube 0x001b",//018
 "unknown cube 0x001c", "unknown cube 0x001d", "unknown cube 0x001e", "unknown cube 0x001f",
 "unknown cube 0x0020", "unknown cube 0x0021", "unknown cube 0x0022", "unknown cube 0x0023",//020
 "unknown cube 0x0024", "unknown cube 0x0025", "unknown cube 0x0026", "unknown cube 0x0027",
 "unknown cube 0x0028", "unknown cube 0x0029", "unknown cube 0x002a", "unknown cube 0x002b",//028
 "unknown cube 0x002c", "unknown cube 0x002d", "unknown cube 0x002e", "unknown cube 0x002f",
 "unknown cube 0x0030", "unknown cube 0x0031", "unknown cube 0x0032", "unknown cube 0x0033",//030
 "unknown cube 0x0034", "unknown cube 0x0035", "unknown cube 0x0036", "unknown cube 0x0037",
 "unknown cube 0x0038", "unknown cube 0x0039", "unknown cube 0x003a", "unknown cube 0x003b",//038
 "unknown cube 0x003c", "unknown cube 0x003d", "unknown cube 0x003e", "unknown cube 0x003f",
 "unknown cube 0x0040", "unknown cube 0x0041", "unknown cube 0x0042", "unknown cube 0x0043",//040
 "unknown cube 0x0044", "unknown cube 0x0045", "unknown cube 0x0046", "unknown cube 0x0047",
 "unknown cube 0x0048", "unknown cube 0x0049", "unknown cube 0x004a", "unknown cube 0x004b",//048
 "unknown cube 0x004c", "unknown cube 0x004d", "unknown cube 0x004e", "unknown cube 0x004f",
 "unknown cube 0x0050", "unknown cube 0x0051", "unknown cube 0x0052", "unknown cube 0x0053",//050
 "woman wall rlf TL",   "woman wall rlf TC",   "woman wall rlf TR",   "woman wall rlf UL",
 "woman wall rlf UC",   "woman wall rlf UR",   "woman wall rlf ML",   "woman wall rlf MC",  //058
 "woman wall rlf MR",   "woman wall rlf BL",   "woman wall rlf BC",   "woman wall rlf BR",
 "twins wall rlf TL",   "twins wall rlf TC",   "twins wall rlf TR",   "twins wall rlf ML",  //060
 "twins wall rlf MC",   "twins wall rlf MR",   "twins wall rlf BL",   "twins wall rlf BC",
 "twins wall rlf BR",   "pair shagn rlf TL",   "pair shagn rlf TC",   "pair shagn rlf TR",  //068
 "pair shagn rlf UL",   "pair shagn rlf UC",   "pair shagn rlf UR",   "pair shagn rlf ML",
 "pair shagn rlf MC",   "pair shagn rlf MR",   "pair shagn rlf BL",   "pair shagn rlf BC",  //070
 "pair shagn rlf BR",   "unknown cube 0x0075", "unknown cube 0x0076", "unknown cube 0x0077",
 "unknown cube 0x0078", "unknown cube 0x0079", "unknown cube 0x007a", "unknown cube 0x007b",//078
 "unknown cube 0x007c", "unknown cube 0x007d", "unknown cube 0x007e", "unknown cube 0x007f",
 "unknown cube 0x0080", "unknown cube 0x0081", "unknown cube 0x0082", "unknown cube 0x0083",//080
 "unknown cube 0x0084", "unknown cube 0x0085", "unknown cube 0x0086", "unknown cube 0x0087",
 "unknown cube 0x0088", "unknown cube 0x0089", "unknown cube 0x008a", "unknown cube 0x008b",//088
 "unknown cube 0x008c", "unknown cube 0x008d", "unknown cube 0x008e", "unknown cube 0x008f",
 "unknown cube 0x0090", "unknown cube 0x0091", "unknown cube 0x0092", "unknown cube 0x0093",//090
 "unknown cube 0x0094", "unknown cube 0x0095", "unknown cube 0x0096", "unknown cube 0x0097",
 "unknown cube 0x0098", "unknown cube 0x0099", "unknown cube 0x009a", "unknown cube 0x009b",//098
 "unknown cube 0x009c", "unknown cube 0x009d", "unknown cube 0x009e", "unknown cube 0x009f",
 "unknown cube 0x00a0", "unknown cube 0x00a1", "unknown cube 0x00a2", "unknown cube 0x00a3",//0a0
 "unknown cube 0x00a4", "unknown cube 0x00a5", "unknown cube 0x00a6", "unknown cube 0x00a7",
 "unknown cube 0x00a8", "unknown cube 0x00a9", "unknown cube 0x00aa", "unknown cube 0x00ab",//0a8
 "unknown cube 0x00ac", "unknown cube 0x00ad", "unknown cube 0x00ae", "unknown cube 0x00af",
 "unknown cube 0x00b0", "unknown cube 0x00b1", "unknown cube 0x00b2", "unknown cube 0x00b3",//0b0
 "unknown cube 0x00b4", "unknown cube 0x00b5", "unknown cube 0x00b6", "unknown cube 0x00b7",
 "unknown cube 0x00b8", "unknown cube 0x00b9", "unknown cube 0x00ba", "unknown cube 0x00bb",//0b8
 "unknown cube 0x00bc", "unknown cube 0x00bd", "unknown cube 0x00be", "unknown cube 0x00bf",
 "unknown cube 0x00c0", "unknown cube 0x00c1", "unknown cube 0x00c2", "unknown cube 0x00c3",//0c0
 "unknown cube 0x00c4", "unknown cube 0x00c5", "unknown cube 0x00c6", "unknown cube 0x00c7",
 "unknown cube 0x00c8", "unknown cube 0x00c9", "unknown cube 0x00ca", "unknown cube 0x00cb",//0c8
 "unknown cube 0x00cc", "unknown cube 0x00cd", "unknown cube 0x00ce", "unknown cube 0x00cf",
 "unknown cube 0x00d0", "unknown cube 0x00d1", "unknown cube 0x00d2", "unknown cube 0x00d3",//0d0
 "unknown cube 0x00d4", "unknown cube 0x00d5", "unknown cube 0x00d6", "unknown cube 0x00d7",
 "unknown cube 0x00d8", "unknown cube 0x00d9", "unknown cube 0x00da", "unknown cube 0x00db",//0d8
 "unknown cube 0x00dc", "unknown cube 0x00dd", "unknown cube 0x00de", "unknown cube 0x00df",
 "unknown cube 0x00e0", "head shape wall TL",  "head shape wall TC",  "head shape wall TR", //0e0
 "head shape wall BL",  "head shape wall BC",  "head shape wall BR",  "tortr floor w/circl",
 "skel.splat wall TL",  "skel.splat wall TC",  "skel.splat wall TR",  "skel.splat wall ML", //0e8
 "skel.splat wall MC",  "skel.splat wall MR",  "skel.splat wall BL",  "skel.splat wall BC",
 "skel.splat wall BR",  "unknown cube 0x00f1", "unknown cube 0x00f2", "unknown cube 0x00f3",//0f0
 "unknown cube 0x00f4", "unknown cube 0x00f5", "unknown cube 0x00f6", "unknown cube 0x00f7",
 "unknown cube 0x00f8", "unknown cube 0x00f9", "unknown cube 0x00fa", "unknown cube 0x00fb",//0f8
 "unknown cube 0x00fc", "unknown cube 0x00fd", "unknown cube 0x00fe", "unknown cube 0x00ff",
 "unknown cube 0x0100", "unknown cube 0x0101", "unknown cube 0x0102", "unknown cube 0x0103",//100
 "unknown cube 0x0104", "unknown cube 0x0105", "unknown cube 0x0106", "unknown cube 0x0107",
 "unknown cube 0x0108", "unknown cube 0x0109", "unknown cube 0x010a", "unknown cube 0x010b",//108
 "unknown cube 0x010c", "unknown cube 0x010d", "unknown cube 0x010e", "unknown cube 0x010f",
 "unknown cube 0x0110", "unknown cube 0x0111", "unknown cube 0x0112", "unknown cube 0x0113",//110
 "unknown cube 0x0114", "unknown cube 0x0115", "unknown cube 0x0116", "unknown cube 0x0117",
 "unknown cube 0x0118", "unknown cube 0x0119", "unknown cube 0x011a", "unknown cube 0x011b",//118
 "unknown cube 0x011c", "unknown cube 0x011d", "unknown cube 0x011e", "unknown cube 0x011f",
 "unknown cube 0x0120", "unknown cube 0x0121", "unknown cube 0x0122", "unknown cube 0x0123",//120
 "unknown cube 0x0124", "unknown cube 0x0125", "unknown cube 0x0126", "unknown cube 0x0127",
 "unknown cube 0x0128", "unknown cube 0x0129", "unknown cube 0x012a", "unknown cube 0x012b",//128
 "unknown cube 0x012c", "unknown cube 0x012d", "unknown cube 0x012e", "unknown cube 0x012f",
 "unknown cube 0x0130", "unknown cube 0x0131", "unknown cube 0x0132", "unknown cube 0x0133",//130
 "unknown cube 0x0134", "unknown cube 0x0135", "unknown cube 0x0136", "unknown cube 0x0137",
 "unknown cube 0x0138", "unknown cube 0x0139", "unknown cube 0x013a", "unknown cube 0x013b",//138
 "unknown cube 0x013c", "unknown cube 0x013d", "unknown cube 0x013e", "unknown cube 0x013f",
 "unknown cube 0x0140", "unknown cube 0x0141", "unknown cube 0x0142", "unknown cube 0x0143",//140
 "unknown cube 0x0144", "unknown cube 0x0145", "unknown cube 0x0146", "unknown cube 0x0147",
 "unknown cube 0x0148", "unknown cube 0x0149", "unknown cube 0x014a", "unknown cube 0x014b",//148
 "unknown cube 0x014c", "unknown cube 0x014d", "unknown cube 0x014e", "unknown cube 0x014f",
 "unknown cube 0x0150", "unknown cube 0x0151", "unknown cube 0x0152", "unknown cube 0x0153",//150
 "unknown cube 0x0154", "unknown cube 0x0155", "unknown cube 0x0156", "unknown cube 0x0157",
 "unknown cube 0x0158", "unknown cube 0x0159", "unknown cube 0x015a", "unknown cube 0x015b",//158
 "unknown cube 0x015c", "unknown cube 0x015d", "unknown cube 0x015e", "unknown cube 0x015f",
 "unknown cube 0x0160", "unknown cube 0x0161", "unknown cube 0x0162", "unknown cube 0x0163",//160
 "unknown cube 0x0164", "unknown cube 0x0165", "unknown cube 0x0166", "unknown cube 0x0167",
 "unknown cube 0x0168", "unknown cube 0x0169", "unknown cube 0x016a", "unknown cube 0x016b",//168
 "unknown cube 0x016c", "unknown cube 0x016d", "unknown cube 0x016e", "unknown cube 0x016f",
 "unknown cube 0x0170", "unknown cube 0x0171", "unknown cube 0x0172", "unknown cube 0x0173",//170
 "unknown cube 0x0174", "unknown cube 0x0175", "unknown cube 0x0176", "unknown cube 0x0177",
 "unknown cube 0x0178", "unknown cube 0x0179", "unknown cube 0x017a", "unknown cube 0x017b",//178
 "unknown cube 0x017c", "unknown cube 0x017d", "unknown cube 0x017e", "unknown cube 0x017f",
 "unknown cube 0x0180", "unknown cube 0x0181", "unknown cube 0x0182", "unknown cube 0x0183",//180
 "unknown cube 0x0184", "unknown cube 0x0185", "unknown cube 0x0186", "unknown cube 0x0187",
 "unknown cube 0x0188", "unknown cube 0x0189", "unknown cube 0x018a", "unknown cube 0x018b",//188
 "unknown cube 0x018c", "unknown cube 0x018d", "unknown cube 0x018e", "unknown cube 0x018f",
 "unknown cube 0x0190", "unknown cube 0x0191", "unknown cube 0x0192", "unknown cube 0x0193",//190
 "unknown cube 0x0194", "unknown cube 0x0195", "unknown cube 0x0196", "unknown cube 0x0197",
 "unknown cube 0x0198", "unknown cube 0x0199", "unknown cube 0x019a", "unknown cube 0x019b",//198
 "unknown cube 0x019c", "unknown cube 0x019d", "unknown cube 0x019e", "unknown cube 0x019f",
 "unknown cube 0x01a0", "unknown cube 0x01a1", "unknown cube 0x01a2", "unknown cube 0x01a3",//1a0
 "unknown cube 0x01a4", "unknown cube 0x01a5", "unknown cube 0x01a6", "unknown cube 0x01a7",
 "unknown cube 0x01a8", "unknown cube 0x01a9", "unknown cube 0x01aa", "unknown cube 0x01ab",//1a8
 "unknown cube 0x01ac", "unknown cube 0x01ad", "unknown cube 0x01ae", "unknown cube 0x01af",
 "unknown cube 0x01b0", "unknown cube 0x01b1", "unknown cube 0x01b2", "unknown cube 0x01b3",//1b0
 "unknown cube 0x01b4", "unknown cube 0x01b5", "unknown cube 0x01b6", "unknown cube 0x01b7",
 "unknown cube 0x01b8", "valuable gems 1",     "valuable gems 2",     "valuable gems 3",    //1b8
 "valuable gems 4",     "unknown cube 0x01bd", "unknown cube 0x01be", "unknown cube 0x01bf",
 "unknown cube 0x01c0", "unknown cube 0x01c1", "unknown cube 0x01c2", "unknown cube 0x01c3",//1c0
 "unknown cube 0x01c4", "unknown cube 0x01c5", "unknown cube 0x01c6", "unknown cube 0x01c7",
 "unknown cube 0x01c8", "unknown cube 0x01c9", "unknown cube 0x01ca", "unknown cube 0x01cb",//1c8
 "unknown cube 0x01cc", "unknown cube 0x01cd", "unknown cube 0x01ce", "unknown cube 0x01cf",
 "unknown cube 0x01d0", "unknown cube 0x01d1", "unknown cube 0x01d2", "unknown cube 0x01d3",//1d0
 "unknown cube 0x01d4", "unknown cube 0x01d5", "unknown cube 0x01d6", "unknown cube 0x01d7",
 "unknown cube 0x01d8", "unknown cube 0x01d9", "unknown cube 0x01da", "unknown cube 0x01db",//1d8
 "unknown cube 0x01dc", "unknown cube 0x01dd", "unknown cube 0x01de", "unknown cube 0x01df",
 "unknown cube 0x01e0", "unknown cube 0x01e1", "unknown cube 0x01e2", "unknown cube 0x01e3",//1e0
 "unknown cube 0x01e4", "unknown cube 0x01e5", "unknown cube 0x01e6", "unknown cube 0x01e7",
 "unknown cube 0x01e8", "unknown cube 0x01e9", "unknown cube 0x01ea", "unknown cube 0x01eb",//1e8
 "unknown cube 0x01ec", "unknown cube 0x01ed", "unknown cube 0x01ee", "unknown cube 0x01ef",
 "unknown cube 0x01f0", "unknown cube 0x01f1", "unknown cube 0x01f2", "unknown cube 0x01f3",//1f0
 "unknown cube 0x01f4", "unknown cube 0x01f5", "unknown cube 0x01f6", "unknown cube 0x01f7",
 "unknown cube 0x01f8", "unknown cube 0x01f9", "unknown cube 0x01fa", "unknown cube 0x01fb",//1f8
 "unknown cube 0x01fc", "unknown cube 0x01fd", "unknown cube 0x01fe", "unknown cube 0x01ff",
     };


const unsigned short wib_columns_animate[]={
      0x0221, //water
      0x222, 0x223, //lava
};

const unsigned short dir_rot_000[]={
      IDIR_NW,IDIR_NORTH,IDIR_NE,IDIR_WEST,IDIR_CENTR,IDIR_EAST,
      IDIR_SW,IDIR_SOUTH,IDIR_SE, };

const unsigned short dir_rot_090[]={
      IDIR_NE,IDIR_EAST,IDIR_SE,IDIR_NORTH,IDIR_CENTR,IDIR_SOUTH,
      IDIR_NW,IDIR_WEST,IDIR_SW, };

const unsigned short dir_rot_180[]={
      IDIR_SE,IDIR_SOUTH,IDIR_SW,IDIR_EAST,IDIR_CENTR,IDIR_WEST,
      IDIR_NE,IDIR_NORTH,IDIR_NW, };

const unsigned short dir_rot_270[]={
      IDIR_SW,IDIR_WEST,IDIR_NW,IDIR_SOUTH,IDIR_CENTR,IDIR_NORTH,
      IDIR_SE,IDIR_EAST,IDIR_NE, };

/*
 * Creates empty COLUMN_REC structure, sets default values inside
*/
struct COLUMN_REC *create_column_rec(void)
{
    struct COLUMN_REC *clm_rec=(struct COLUMN_REC *)malloc(sizeof(struct COLUMN_REC));
    if (clm_rec==NULL) die("Cannot allocate memory for clm_rec");
    fill_column_rec(clm_rec, 0, 1, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0);
    return clm_rec;
}

/*
 * Frees memory of given COLUMN_REC entry,.
 */
void free_column_rec(struct COLUMN_REC *clm_rec)
{
    free(clm_rec);
}

/*
 * Fills a given COLUMN_REC entry, using permanent=1, lintel=0,
 * and automatically computing correct height and solid params.
 */
void fill_column_rec_simp(struct COLUMN_REC *clm_rec, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
  fill_column_rec_autosh(clm_rec, use,1, 0, base, 
                 c0, c1, c2, c3, c4, c5, c6, c7);
}

/*
 * Fills a given COLUMN_REC entry, using permanent=0, lintel=0,
 * and automatically computing correct height and solid params.
 */
void fill_column_rec_sim(struct COLUMN_REC *clm_rec, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
  fill_column_rec_autosh(clm_rec, use, 0, 0, base, 
                 c0, c1, c2, c3, c4, c5, c6, c7);
}

/*
 * Fills a given COLUMN_REC entry, using orientation=0,
 * and automatically computing correct height and solid params.
 */
void fill_column_rec_autosh(struct COLUMN_REC *clm_rec, const unsigned int use,
        const int permanent, const int lintel, const int base, 
        const int c0, const int c1, const int c2, const int c3,
        const int c4, const int c5, const int c6, const int c7)
{
    fill_column_rec(clm_rec, use, permanent, lintel, 0, 0, base, 0,
               c0, c1, c2, c3, c4, c5, c6, c7);
    clm_rec->solid=compute_clm_rec_solid(clm_rec);
    clm_rec->height=compute_clm_rec_height(clm_rec);
}

/*
 * Fills a given COLUMN_REC entry, requires all parameters stored in CLM entry.
 */
void fill_column_rec(struct COLUMN_REC *clm_rec, const unsigned int use,
        const int permanent, const int lintel, const int height,
        const unsigned int solid, const int base, const int orientation,
        const int c0, const int c1, const int c2, const int c3,
        const int c4, const int c5, const int c6, const int c7)
{
    clm_rec->use=use;
    clm_rec->permanent=permanent;
    clm_rec->lintel=lintel;
    clm_rec->height=height;
    clm_rec->solid=solid;
    clm_rec->base=base;
    clm_rec->orientation=orientation;
    clm_rec->c[0]=c0;
    clm_rec->c[1]=c1;
    clm_rec->c[2]=c2;
    clm_rec->c[3]=c3;
    clm_rec->c[4]=c4;
    clm_rec->c[5]=c5;
    clm_rec->c[6]=c6;
    clm_rec->c[7]=c7;
}

/*
 * Sets a CLM entry; c1 is the lowest square, c8 - the top one.
 */
void set_clm_entry(unsigned char *clmentry, struct COLUMN_REC *clm_rec)
{
    if ((clmentry==NULL)||(clm_rec==NULL)) return;
    unsigned char *x=clmentry;
    x[0]=clm_rec->use&255;
    x[1]=(clm_rec->use>>8)&255;
    x[2]=(clm_rec->permanent&1)+((clm_rec->lintel&7)<<1)+((clm_rec->height&15)<<4);
    x[3]=clm_rec->solid&255;
    x[4]=(clm_rec->solid>>8)&255;
    x[5]=clm_rec->base&255;
    x[6]=(clm_rec->base>>8)&255;
    x[7]=clm_rec->orientation&255;
    x[8]=clm_rec->c[0]&255;
    x[9]=(clm_rec->c[0]>>8)&255;
    x[10]=clm_rec->c[1]&255;
    x[11]=(clm_rec->c[1]>>8)&255;
    x[12]=clm_rec->c[2]&255;
    x[13]=(clm_rec->c[2]>>8)&255;
    x[14]=clm_rec->c[3]&255;
    x[15]=(clm_rec->c[3]>>8)&255;
    x[16]=clm_rec->c[4]&255;
    x[17]=(clm_rec->c[4]>>8)&255;
    x[18]=clm_rec->c[5]&255;
    x[19]=(clm_rec->c[5]>>8)&255;
    x[20]=clm_rec->c[6]&255;
    x[21]=(clm_rec->c[6]>>8)&255;
    x[22]=clm_rec->c[7]&255;
    x[23]=(clm_rec->c[7]>>8)&255;
}

void get_clm_entry(struct COLUMN_REC *clm_rec, const unsigned char *clmentry)
{
    if ((clmentry==NULL)||(clm_rec==NULL)) return;
    clm_rec->use=clmentry[0]+(clmentry[1]<<8);
    clm_rec->permanent=clmentry[2]&1;
    clm_rec->lintel=(clmentry[2]>>1)&7;
    clm_rec->height=(clmentry[2]>>4)&15;
    clm_rec->solid=clmentry[3]+(clmentry[4]<<8);
    clm_rec->base=clmentry[5]+(clmentry[6]<<8);
    clm_rec->orientation=clmentry[7];
    clm_rec->c[0]=clmentry[8]+(clmentry[9]<<8);
    clm_rec->c[1]=clmentry[10]+(clmentry[11]<<8);
    clm_rec->c[2]=clmentry[12]+(clmentry[13]<<8);
    clm_rec->c[3]=clmentry[14]+(clmentry[15]<<8);
    clm_rec->c[4]=clmentry[16]+(clmentry[17]<<8);
    clm_rec->c[5]=clmentry[18]+(clmentry[19]<<8);
    clm_rec->c[6]=clmentry[20]+(clmentry[21]<<8);
    clm_rec->c[7]=clmentry[22]+(clmentry[23]<<8);
}

/*
 * Determines if the two CLM entries looks identically - COLUMN_REC version
 */
short compare_column_recs(struct COLUMN_REC *clm_rec1, struct COLUMN_REC *clm_rec2)
{
  if (clm_rec1->height != clm_rec2->height)
    return false;
  if (clm_rec1->solid != clm_rec2->solid)
    return false;
  if (clm_rec1->lintel != clm_rec2->lintel)
    return false;
  unsigned short height=clm_rec1->height;
  unsigned int solid=clm_rec1->solid;
  if (clm_rec1->base!=clm_rec2->base)
    return false;
  if ((solid&1))
    if (clm_rec1->c[0]!=clm_rec2->c[0])
      return false;
  if ((solid&2))
    if (clm_rec1->c[1]!=clm_rec2->c[1])
      return false;
  if ((solid&4))
    if (clm_rec1->c[2]!=clm_rec2->c[2])
      return false;
  if ((solid&8))
    if (clm_rec1->c[3]!=clm_rec2->c[3])
      return false;
  if ((solid&16))
    if (clm_rec1->c[4]!=clm_rec2->c[4])
      return false;
  if ((solid&32))
    if (clm_rec1->c[5]!=clm_rec2->c[5])
      return false;
  if ((solid&64))
    if (clm_rec1->c[6]!=clm_rec2->c[6])
      return false;
  if ((solid&128))
    if (clm_rec1->c[7]!=clm_rec2->c[7])
      return false;
  return true;
}

/*
 * Determines if the two CLM entries looks identically - basic version
 */
short compare_column_entries(const unsigned char *clmentry1, const unsigned char *clmentry2)
{
   //comparing 'lintel' and 'height', but not 'permanent'
    if ((clmentry1[2]&0x0fe)!=(clmentry2[2]&0x0fe))
        return false;
    // Comparing 'solid' and 'base'
    if (memcmp(clmentry1+3,clmentry2+3,4)!=0)
        return false;
    // Comparing cube values
    if (memcmp(clmentry1+8,clmentry2+8,15)!=0)
        return false;
    return true;
}

unsigned short compute_clm_rec_height(const struct COLUMN_REC *clm_rec)
{
    if (clm_rec==NULL) return 0;
    //First test - 3 free bottom columns makes height at the bottom.
    if ((clm_rec->c[0]==0) && (clm_rec->c[1]==0) && (clm_rec->c[2]==0))
      return 0;
    if ((clm_rec->c[1]==0) && (clm_rec->c[2]==0) && (clm_rec->c[3]==0))
      return 1;
    if ((clm_rec->c[2]==0) && (clm_rec->c[3]==0) && (clm_rec->c[4]==0))
      return 2;
    if ((clm_rec->c[3]==0) && (clm_rec->c[4]==0) && (clm_rec->c[5]==0))
      return 3;
    //Door when they are opening/open
    if ((clm_rec->c[0]==0x094) && (clm_rec->c[1]==0x0))
      return 1;
    //Strange iron door from map 67 (that's probably wrong entry - but the warn message for it annoys me)
    if ((clm_rec->base==0x0b1) && (clm_rec->c[0]==0x0) && (clm_rec->c[1]==0x17c))
      return 0;
    if ((clm_rec->c[7]!=0))
      return 8;
    if ((clm_rec->c[6]!=0))
      return 7;
    if ((clm_rec->c[5]!=0))
      return 6;
    if ((clm_rec->c[4]!=0))
      return 5;
    if ((clm_rec->c[3]!=0))
      return 4;
    if ((clm_rec->c[2]!=0))
      return 3;
    if ((clm_rec->c[1]!=0))
      return 2;
    if ((clm_rec->c[0]!=0))
      return 1;
    return 0;
}

unsigned short compute_clm_rec_solid(const struct COLUMN_REC *clm_rec)
{
    if (clm_rec==NULL) return 0;
    unsigned short solid=0;
    if ((clm_rec->c[7]!=0))
      solid|=128;
    if ((clm_rec->c[6]!=0))
      solid|=64;
    if ((clm_rec->c[5]!=0))
      solid|=32;
    if ((clm_rec->c[4]!=0))
      solid|=16;
    if ((clm_rec->c[3]!=0))
      solid|=8;
    if ((clm_rec->c[2]!=0))
      solid|=4;
    if ((clm_rec->c[1]!=0))
      solid|=2;
    if ((clm_rec->c[0]!=0))
      solid|=1;
    return solid;
}

unsigned int get_clm_entry_use(const unsigned char *clmentry)
{
    return (unsigned int)clmentry[0]+(clmentry[1]<<8);
}

/*
 * Increases USE parameter of the column entry.
 * Returns the new value.
 */
unsigned int clm_entry_use_inc(unsigned char *clmentry)
{
    unsigned int use=clmentry[0]+(clmentry[1]<<8);
    if (use<65535) use++;
    clmentry[0]=use&255;
    clmentry[1]=(use>>8)&255;
    return use;
}

/*
 * Decreases USE parameter of the column entry.
 * Returns the new value.
 */
unsigned int clm_entry_use_dec(unsigned char *clmentry)
{
    unsigned int use=clmentry[0]+(clmentry[1]<<8);
    if (use>0) use--;
    clmentry[0]=use&255;
    clmentry[1]=(use>>8)&255;
    return use;
}

/*
 * Returns the value of "permanent".
 */
short get_clm_entry_permanent(const unsigned char *clmentry)
{
    if (clmentry==NULL) return 0;
    short permanent=clmentry[2]&1;
    return permanent;
}

void set_clm_entry_permanent(unsigned char *clmentry,short nperm)
{
    if (clmentry==NULL) return;
    unsigned char val;
    val=clmentry[2] & 0xfe; //clear permanent bit
    clmentry[2]=(val)|(nperm!=0);
}

unsigned short get_clm_entry_height(const unsigned char *clmentry)
{
    return (unsigned short)(clmentry[2]>>4)&15;
}

unsigned short get_clm_entry_solid(const unsigned char *clmentry)
{
    return (unsigned short)clmentry[3]+(clmentry[4]<<8);
}

/*
 * Returns most top nonzero cube entry.
 */
unsigned short get_clm_entry_topcube(const unsigned char *clmentry)
{
    unsigned short cube=0;
    int i=7;
    while ((cube==0)&&(i>=0))
    {
          cube = clmentry[2*i+8]+(clmentry[2*i+9]<<8);
          i--;
    }
    if (cube==0)
        cube = clmentry[5]+(clmentry[6]<<8);
    return cube;
}

short clm_rec_copy(struct COLUMN_REC *dest_rec,const struct COLUMN_REC *src_rec)
{
    memcpy(dest_rec,src_rec,sizeof(struct COLUMN_REC));
    return true;
}

/*
 * Verifies values in the CLM entry.
 */
short clm_verify_entry(const unsigned char *clmentry, char *err_msg)
{
  struct COLUMN_REC *clm_rec;
  clm_rec=create_column_rec();
  get_clm_entry(clm_rec, clmentry);
  int i;
  for (i=0;i<8;i++)
    if (clm_rec->c[i]>CUBE_MAX_INDEX)
    {
        sprintf(err_msg,"Cube entry %d too large",i);
        return VERIF_WARN;
    }
  unsigned int prop_solid=compute_clm_rec_solid(clm_rec);
  if (clm_rec->solid != prop_solid)
  {
        sprintf(err_msg,"Solid property should be %u",prop_solid);
        return VERIF_WARN;
  }
  unsigned int prop_height=compute_clm_rec_height(clm_rec);
  if (clm_rec->height != prop_height)
  {
        sprintf(err_msg,"Height property should be %u",prop_height);
        return VERIF_WARN;
  }
  free_column_rec(clm_rec);
  return VERIF_OK;
}

/*
 * Returns if the column should be animated (contains water or lava, or similar)
 */
short column_wib_animate(unsigned int clm)
{
    int array_count=sizeof(wib_columns_animate)/sizeof(unsigned short);
    int idx=arr_ushort_pos(wib_columns_animate,clm,array_count);
    if (idx>=0) return true;
    return false;
}

/*
 * Returns a WIB file entry for given column; requires three additional columns:
 *          one from the north, one from the west and from NW corner.
 */
unsigned short column_wib_entry(struct COLUMN_REC *clm_rec,
    struct COLUMN_REC *clm_rec_n,struct COLUMN_REC *clm_rec_w,struct COLUMN_REC *clm_rec_nw)
{
    int i;
    // Preparing info if siblings are animated
    short anim_north=false;
    if (column_wib_animate(clm_rec_n->base))
      anim_north=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec_n->solid&(1<<i))&&(column_wib_animate(clm_rec_n->c[i])))
        anim_north=true;
    }
    short anim_west=false;
    if (column_wib_animate(clm_rec_w->base))
      anim_west=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec_w->solid&(1<<i))&&(column_wib_animate(clm_rec_w->c[i])))
        anim_west=true;
    }
    short anim_nw=false;
    if (column_wib_animate(clm_rec_nw->base))
      anim_nw=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec_nw->solid&(1<<i))&&(column_wib_animate(clm_rec_nw->c[i])))
        anim_nw=true;
    }
    //And if this one alone should animate
    short anim_this=false;
    if (column_wib_animate(clm_rec->base))
      anim_this=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec->solid&(1<<i))&&(column_wib_animate(clm_rec->c[i])))
        anim_this=true;
    }
    //Getting to decision
    if (anim_this&&anim_west&&anim_north&&anim_nw)
      return COLUMN_WIB_ANIMATE;
    return COLUMN_WIB_SKEW;
}

/*
 * Returns custom column type name as text
 */
char *get_custom_column_fullname(unsigned short idx)
{
    int types_count=sizeof(custom_columns_fullnames)/sizeof(char *);
    if (idx<types_count)
      return (char *)custom_columns_fullnames[idx];
    else
      return "unknown(?!)";
}

/*
 * Returns cube type name as text
 */
char *get_cube_fullname(unsigned short idx)
{
    int types_count=sizeof(cube_fullnames)/sizeof(char *);
    if (idx<types_count)
      return (char *)cube_fullnames[idx];
    else
      return "unknown(?!)";
}

/*
 * Executes custom column filling function with given index
 */
short fill_custom_column_data(unsigned short idx,struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    custom_columns_gen[idx](clm_recs,surr_slb,surr_own,surr_tng);
}

/*
 * Fills up 9 CLM entries needed for given slab with specified surroundings.
 */
void create_columns_for_slab(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  unsigned short slab=surr_slb[IDIR_CENTR];
  switch (slab)
  {
    case SLAB_TYPE_ROCK:
      create_columns_slb_rock(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GOLD:
      create_columns_slb_gold(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_EARTH:
      create_columns_slb_earth(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TORCHDIRT:
      create_columns_slb_torchdirt(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLDRAPE:
      create_columns_slb_walldrape(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLTORCH:
      create_columns_slb_walltorch(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLWTWINS:
      create_columns_slb_wallwtwins(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLWWOMAN:
      create_columns_slb_wallwwoman(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLPAIRSHR:
      create_columns_slb_wallpairshr(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PATH:
      create_columns_slb_path(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_CLAIMED:
      create_columns_slb_claimed(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_LAVA:
      create_columns_slb_lava(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WATER:
      create_columns_slb_water(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PORTAL:
      create_columns_slb_portal(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TREASURE:
      create_columns_slb_treasure(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_LIBRARY:
      create_columns_slb_library(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PRISONCASE:
      create_columns_slb_prison(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TORTURE:
      create_columns_slb_torture(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TRAINING:
      create_columns_slb_training(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DUNGHEART:
      create_columns_slb_dungheart(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WORKSHOP:
      create_columns_slb_workshop(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_SCAVENGER:
      create_columns_slb_scavenger(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TEMPLE:
      create_columns_slb_temple(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GRAVEYARD:
      create_columns_slb_graveyard(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_HATCHERY:
      create_columns_slb_hatchery(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_LAIR:
      create_columns_slb_lair(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_BARRACKS:
      create_columns_slb_barracks(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORWOOD1:
    case SLAB_TYPE_DOORWOOD2:
      create_columns_slb_doorwood(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORBRACE1:
    case SLAB_TYPE_DOORBRACE2:
      create_columns_slb_doorbrace(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORIRON1:
    case SLAB_TYPE_DOORIRON2:
      create_columns_slb_dooriron(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORMAGIC1:
    case SLAB_TYPE_DOORMAGIC2:
      create_columns_slb_doormagic(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_BRIDGE:
      create_columns_slb_bridge(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GEMS:
      create_columns_slb_gems(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GUARDPOST:
      create_columns_slb_guardpost(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    default: //on error, make path
      create_columns_slb_path(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    }
}

/*
 * A helper function for using surr_own array
 */
short surrnd_not_enemy(unsigned char *surr_own, short direction)
{
    if ((surr_own[IDIR_CENTR]==surr_own[direction%9])
      ||((surr_own[direction%9])>=PLAYER_UNSET))
        return true;
    return false;
}

/*
 * A helper function for using surr_tng array
 */
unsigned char *surr_tng_find(unsigned char **surr_tng,unsigned char type_idx)
{
  unsigned char *thing=surr_tng[IDIR_CENTR];
  if (thing!=NULL)
  {
    if (get_thing_type(thing)==type_idx)
      return thing;
  }
  int i;
  for (i=0;i<9;i++)
  {
    thing=surr_tng[i];
    if (thing!=NULL)
    {
      if (get_thing_type(thing)==type_idx)
        return thing;
    }
  }
  return NULL;
}

void create_columns_slb_rock(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_rock(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_gold(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  //Central column
  fill_column_gold(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
  //Note: can't use modify_liquid_surrounding() because more than one cube changes near water
  //corner columns
  int i;
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER) || (surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
      fill_column_gold_nearwater(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
    else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA) || (surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
      fill_column_gold_nearlava(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
    else
      fill_column_gold(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
  }
  // Remaining edge columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER))
      fill_column_gold_nearwater(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
    else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA))
      fill_column_gold_nearlava(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
    else
      fill_column_gold(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
  }
  //Switch (remove) corner columns near lava,water,...
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

/*
 * Creates earth which are all filled with dirt, always.
 * May be used for making earth and torchdirt columns.
 */
void create_columns_slb_fulldirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //Center is always earth
  fill_column_earth(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);

  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  int i;
  //Note: can't use modify_liquid_surrounding() - two cubes changes
  //Now corner slabs
  for (i=0;i<4;i++)
  {
      if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
      {
         fill_column_earth_nearwater(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      } else
      if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
      {
         fill_column_earth_nearlava(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      } else
      {
         fill_column_earth(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      }
  }
  // And the middle ones
  for (i=0;i<4;i++)
  {
      if (surr_slb[dir_a[i]]==SLAB_TYPE_WATER)
         fill_column_earth_nearwater(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
      else
      if (surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)
         fill_column_earth_nearlava(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
      else
         fill_column_earth(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_earth(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //Make the standard, 9x9 columns filled with dirt
  create_columns_slb_fulldirt(clm_recs,surr_slb,surr_own,surr_tng);
  //Finally - switch corner columns near lava,water,...
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torchdirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  //This is identical to standard dirt
  create_columns_slb_earth(clm_recs,surr_slb,surr_own,surr_tng);
  //But one of the c[3] entries is replaced with torch-one
  int i;
  short has_torches=false;
  for (i=0;i<4;i++)
  {
// old way - without things (pending delete, but first we must have auto-adding things)
//      if (slab_is_room(surr_slb[dir_a[i]])||slab_is_space(surr_slb[dir_a[i]]))
    unsigned char *thing=surr_tng[dir_a[i]];
    if ((thing!=NULL)&&(is_torch(thing)))
    {
        //Add the torch plate at same height where the torch thing is
        unsigned short height=get_thing_subtile_h(thing);
        if (get_thing_subtpos_h(thing)>127) height++;
        clm_recs[dir_a[i]]->c[height%8]=0x018;
        has_torches=true;
    }
  }
  if (!has_torches)
    modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_skulls_on_lava(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_lava(clm_recs,surr_slb,surr_own,surr_tng);
  place_column_wall_lair_b(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_skulls_on_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_path(clm_recs,surr_slb,surr_own,surr_tng);
  place_column_wall_lair_b(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_skulls_on_claimed(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_claimed(clm_recs,surr_slb,surr_own,surr_tng);
  place_column_wall_lair_b(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

/*
 * Creates wall with red brick inside. Returns where are whole brick walls
 */
void create_columns_slb_wallbrick(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
//TODO: near water and near lava support
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  int i;
  //Filling matrix
  for (i=0;i<9;i++)
    allow_relief[i]=true;
  //We will use the NW, NE, SW, SE values of the matrix to help making brick
  // with shadow
  //Central slab identifies the player
  fill_column_wall_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);

  //Corner slabs - white brick, small red brick or clear earth
  for (i=0;i<4;i++)
  {
    // If we're surrounded by our wall, and there is something short also - red brick
    if (((slab_is_wall(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
       &&((slab_is_short(surr_slb[dir_b[i]]))||(!surrnd_not_enemy(surr_own,dir_b[i]))))
       ||((slab_is_wall(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
       &&((slab_is_short(surr_slb[dir_a[i]]))||(!surrnd_not_enemy(surr_own,dir_a[i])))))
    {
        fill_column_wall_redsmbrick_a(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    } else
    // if we're surrouned with unowned tall thing on _both_ sides, fill with dirt
    if ((slab_is_tall_unclmabl(surr_slb[dir_a[i]])&&slab_is_tall_unclmabl(surr_slb[dir_b[i]]))
      ||(((slab_is_tall(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
      &&(slab_is_tall(surr_slb[dir_b[i]])&& surrnd_not_enemy(surr_own,dir_b[i])))))
    {
      fill_column_earth(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      allow_relief[dir_a[i]]=false;
      allow_relief[dir_b[i]]=false;
    } else
    // Finally - if there's something short, like ground - use the white brick
    {
      fill_column_wall_cobblestones(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
      allow_relief[dir_a[i]]=false;
      allow_relief[dir_b[i]]=false;
      allow_relief[dir_c[i]]=false;
    }
  }

  // And the remaining, middle slabs
  //These cannot be taken in simple 'for' loop, because different directions uses
  // different fill_column_wall_redsmbrick_* functions.
  if ((slab_is_wall(surr_slb[IDIR_NORTH])&&surrnd_not_enemy(surr_own,IDIR_NORTH))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_NORTH]))
  {
    fill_column_earth(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_NORTH]=false;
  } else
  {
    if (allow_relief[IDIR_NE])
      // just standard red brick
      fill_column_wall_redsmbrick_c(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    else
      // the "b" brick has dark edge from this view
      fill_column_wall_redsmbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
  }

  if ((slab_is_wall(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_EAST]))
  {
    fill_column_earth(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_EAST]=false;
  } else
  {
    if (allow_relief[IDIR_SE])
      // just standard red brick
      fill_column_wall_redsmbrick_c(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
    else
      // the "b" brick has dark edge from this view
      fill_column_wall_redsmbrick_b(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
  }

  if ((slab_is_wall(surr_slb[IDIR_SOUTH])&&surrnd_not_enemy(surr_own,IDIR_SOUTH))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_SOUTH]))
  {
    fill_column_earth(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_SOUTH]=false;
  } else
  {
    if (allow_relief[IDIR_SW])
      // just standard red brick
      fill_column_wall_redsmbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    else
      // the "c" brick has dark edge from this view
      fill_column_wall_redsmbrick_c(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
  }

  if ((slab_is_wall(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_WEST]))
  {
    fill_column_earth(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_WEST]=false;
  } else
  {
    if (allow_relief[IDIR_NW])
      // just standard red brick
      fill_column_wall_redsmbrick_b(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
    else
      // the "c" brick has dark edge from this view
      fill_column_wall_redsmbrick_c(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
  }
  //No frail columns switching on reinforced wall, so we're done.
}

/*
 * If some of the walls are next to room, fills the columns with room relief.
 * Modifies allow_relief setting the modified columns to false.
 */
void fill_columns_slb_roomrelief(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_NW,   IDIR_NW,   IDIR_NE,   IDIR_SW};
  const unsigned short dir_b[]={IDIR_WEST, IDIR_NORTH,IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_c[]={IDIR_SW,   IDIR_NE,   IDIR_SE,   IDIR_SE};
  int i;
  for (i=0;i<4;i++)
  {
    unsigned short slab=surr_slb[dir_b[i]];
    if ((allow_relief[dir_b[i]])&&(slab_is_room(slab)))
    {
      //TODO: set edge and corner properly
      short corner=false;
      short edge=false;
      if (fill_side_columns_room_relief(clm_recs[dir_a[i]],clm_recs[dir_b[i]],
          clm_recs[dir_c[i]],slab,surr_own[IDIR_CENTR],corner,edge))
        allow_relief[dir_b[i]]=false;
    }
  }
}

/*
 * Fills columns with relief specific for given room. Returns false if given room
 * has no its own wall, or given slab is not room. 'corner' indicates
 * that a room corner columns should be used. 'edge' means that the wall
 * ends after or before these columns.
 */
short fill_side_columns_room_relief(struct COLUMN_REC *clm_reca,struct COLUMN_REC *clm_recb,
    struct COLUMN_REC *clm_recc,unsigned short room_slab,unsigned char owner, short corner, short edge)
{
  //Note: we're using fill_column_wall_drapebrick to save CLM space.
  //TODO: make corner and edge support
  switch (room_slab)
  {
  case SLAB_TYPE_PORTAL:
    place_column_wall_portal_a(clm_reca,owner);
    place_column_wall_portal_b(clm_recb,owner);
    place_column_wall_portal_a(clm_recc,owner);
    return true;
  case SLAB_TYPE_TREASURE:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    place_column_wall_treasure_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    place_column_wall_treasure_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    place_column_wall_treasure_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_LIBRARY:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    place_column_wall_library_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    place_column_wall_library_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    place_column_wall_library_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_PRISONCASE:
    place_column_wall_prison_a(clm_reca,owner);
    place_column_wall_prison_b(clm_recb,owner);
    place_column_wall_prison_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_TORTURE:
    place_column_wall_torture_a(clm_reca,owner);
    place_column_wall_torture_b(clm_recb,owner);
    place_column_wall_torture_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_TRAINING:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    place_column_wall_training_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    place_column_wall_training_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    place_column_wall_training_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_WORKSHOP:
    place_column_wall_workshop_a(clm_reca,owner);
    place_column_wall_workshop_b(clm_recb,owner);
    place_column_wall_workshop_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_SCAVENGER:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    place_column_wall_scavenger_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    place_column_wall_scavenger_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    place_column_wall_scavenger_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_TEMPLE:
    place_column_wall_temple_a(clm_reca,owner);
    place_column_wall_temple_b(clm_recb,owner);
    place_column_wall_temple_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_GRAVEYARD:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    place_column_wall_graveyard_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    place_column_wall_graveyard_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    place_column_wall_graveyard_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_HATCHERY:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    place_column_wall_hatchery_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    place_column_wall_hatchery_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    place_column_wall_hatchery_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_LAIR:
    place_column_wall_lair_b(clm_recb,owner);
    return true;
  case SLAB_TYPE_BARRACKS:
    place_column_wall_barracks_a(clm_reca,owner);
    place_column_wall_barracks_b(clm_recb,owner);
    place_column_wall_barracks_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_DUNGHEART:
  case SLAB_TYPE_BRIDGE:
  case SLAB_TYPE_GUARDPOST:
  default:
    return false;
  }
}

/*
 * Creates wall with splatted dead body (torture chamber specific)
 */
void create_columns_slb_wall_force_relief_splatbody(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    const unsigned short dir_a[]={IDIR_NW,   IDIR_NE,   IDIR_SE,   IDIR_SW};
    const unsigned short dir_b[]={IDIR_WEST, IDIR_NORTH,IDIR_EAST, IDIR_SOUTH};
    // Filling the wall with brick & cobblestones on edges
    int i;
    for (i=0;i<4;i++)
    {
      fill_column_wall_cobblestones(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
      fill_column_wall_redsmbrick_b(clm_recs[dir_b[i]], surr_own[IDIR_CENTR]);
    }
    fill_column_wall_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
    // Finding best orientation for the relief
    const unsigned short *dir;
    short draw_oposite;
    if (slab_is_short(surr_slb[IDIR_WEST]) && slab_is_short(surr_slb[IDIR_NORTH]))
    {
      dir=dir_rot_000;
      draw_oposite=false;
    } else
    if (slab_is_short(surr_slb[IDIR_EAST]) && slab_is_short(surr_slb[IDIR_SOUTH]))
    {
      dir=dir_rot_180;
      draw_oposite=false;
    } else
    if (slab_is_short(surr_slb[IDIR_EAST]) && slab_is_short(surr_slb[IDIR_WEST]))
    {
      dir=dir_rot_090;
      draw_oposite=true;
    } else
    if (slab_is_short(surr_slb[IDIR_NORTH]) && slab_is_short(surr_slb[IDIR_SOUTH]))
    {
      dir=dir_rot_000;
      draw_oposite=true;
    } else
    if (slab_is_short(surr_slb[IDIR_EAST]) || slab_is_short(surr_slb[IDIR_SOUTH]))
    {
      dir=dir_rot_180;
      draw_oposite=false;
    } else
    //if (slab_is_short(surr_slb[IDIR_WEST]) || slab_is_short(surr_slb[IDIR_NORTH]))
    {
      dir=dir_rot_000;
      draw_oposite=false;
    }
    // Drawing the reliefs
    if (draw_oposite)
    {
        place_column_wall_torture_a(clm_recs[dir[0]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_b(clm_recs[dir[1]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[2]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_a(clm_recs[dir[6]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_b(clm_recs[dir[7]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[8]],surr_own[IDIR_CENTR]);
    } else
    {
      if (dir==dir_rot_180)
      {
        place_column_wall_torture_b(clm_recs[dir[1]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_b(clm_recs[dir[3]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_a(clm_recs[dir[2]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[0]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_a(clm_recs[dir[6]],surr_own[IDIR_CENTR]);
      } else
      {
        place_column_wall_torture_b(clm_recs[dir[1]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_b(clm_recs[dir[3]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[2]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_a(clm_recs[dir[0]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[6]],surr_own[IDIR_CENTR]);
      }
    }
}

/*
 * Creates wall with small drape at top
 */
void create_columns_slb_walldrape(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  //If there's enought place for drape - draw it
  const unsigned short dir_a[]={IDIR_NW,   IDIR_NW,   IDIR_NE,   IDIR_SW};
  const unsigned short dir_b[]={IDIR_WEST, IDIR_NORTH,IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_c[]={IDIR_SW,   IDIR_NE,   IDIR_SE,   IDIR_SE};
  int i;
  for (i=0;i<4;i++)
  {
    if (allow_relief[dir_b[i]])
    {
      fill_column_wall_drapebrick_a(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
      fill_column_wall_drapebrick_b(clm_recs[dir_b[i]], surr_own[IDIR_CENTR]);
      fill_column_wall_drapebrick_c(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    }
  }
}

/*
 * Creates wall with torch plate
 */
void create_columns_slb_walltorch(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  //This variable will help in placing the bas-relief or drape;
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
    // Torch plate
  int i;
  for (i=0;i<4;i++)
  {
    unsigned char *thing=surr_tng[dir_a[i]];
    if ((thing!=NULL)&&(is_torch(thing)))
    {
        //Add the torch plate at same height where the torch thing is
        unsigned short height=get_thing_subtile_h(thing);
        if (get_thing_subtpos_h(thing)>127) height++;
        clm_recs[dir_a[i]]->c[height%8]=0x077;
        allow_relief[dir_a[i]]=false;
    }
  }
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_wallwtwins(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  //If there's enought place for drape - draw it
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}
void create_columns_slb_wallwwoman(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  //If there's enought place for drape - draw it
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_wallpairshr(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  //If there's enought place for drape - draw it
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_path(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x02e, 0x02f);
}

void create_columns_slb_claimed(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};

  //Center
  fill_column_claimedgnd_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);

  //Corners
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_rec_sim(clm_recs[dir_c[i]], 0, 0x0ce,
         0x07f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);

    if (slab_is_claimedgnd(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i])&&
        slab_is_claimedgnd(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
    { //Surrounded by our area
       clm_recs[dir_c[i]]->c[0]=0x07e +rnd(3);
    } else
    if (slab_is_claimedgnd(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
    { //our on side A only
        switch (dir_c[i])
        {
        case IDIR_NW://West only, not north
           clm_recs[dir_c[i]]->c[0]=0x082;
           break;
        case IDIR_NE://North only, not east
           clm_recs[dir_c[i]]->c[0]=0x085;
           break;
        case IDIR_SW://South only, not west
           clm_recs[dir_c[i]]->c[0]=0x084;
           break;
        case IDIR_SE://East only, not south
           clm_recs[dir_c[i]]->c[0]=0x087;
           break;
        }
    } else
    if (slab_is_claimedgnd(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
    { //our on side B only
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x084;
           break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x087;
           break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x082;
           break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x085;
           break;
        }
    } else
    { //Surrounded by not our area
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x081;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x083;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x086;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x088;
          break;
        }
    }
  }

  //And the middle columns
  for (i=0;i<4;i++)
  {
    fill_column_rec_sim(clm_recs[dir_a[i]], 0, 0x0ce,
         0x07f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);

    if (slab_is_claimedgnd(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
    {
       clm_recs[dir_a[i]]->c[0]=0x07e +rnd(3);
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x087;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x084;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x082;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x085;
           break;
        }
    }
  }
  // Water and lava can beat all previous conditions
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x094, 0x093);
}

void create_columns_slb_lava(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_lava(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_water(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_water(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_gems(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //These are, in official editor, same for every surrounding, even near water
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_gem(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  //Switch corner columns near lava,water,...
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_thingems_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_path(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  fill_column_gem(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x02e, 0x02f);
}

void create_columns_slb_portal(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_portal_floor,create_columns_slb_portal_edge,
        create_columns_slb_portal_corner,create_columns_slb_portal_inside,create_columns_slb_portal_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_treasure(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_treasure_floor,create_columns_slb_treasure_edge,
       create_columns_slb_treasure_corner,create_columns_slb_treasure_inside,create_columns_slb_treasure_floor,
       clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_library(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_library_floor,create_columns_slb_library_edge,
        create_columns_slb_library_corner,create_columns_slb_library_inside,create_columns_slb_library_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_prison_floor,create_columns_slb_prison_edge,
        create_columns_slb_prison_corner,create_columns_slb_prison_inside,create_columns_slb_prison_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torture(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_torture_floor,create_columns_slb_torture_edge,
        create_columns_slb_torture_corner,create_columns_slb_torture_inside,create_columns_slb_torture_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_training(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_training_floor,create_columns_slb_training_edge,
        create_columns_slb_training_corner,create_columns_slb_training_inside,create_columns_slb_training_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_dungheart(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    //Note: the 'floor' function is different in DK, and floor can change if user
    // force it to update in the game.
    create_columns_slb_room(create_columns_slb_dungheart_nearinsd,create_columns_slb_dungheart_edge,
        create_columns_slb_dungheart_corner,create_columns_slb_dungheart_inside,create_columns_slb_dungheart_nearinsd,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_workshop(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_workshop_floor,create_columns_slb_workshop_edge,
        create_columns_slb_workshop_corner,create_columns_slb_workshop_inside,create_columns_slb_workshop_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_scavenger(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_scavenger_floor,create_columns_slb_scavenger_edge,
        create_columns_slb_scavenger_corner,create_columns_slb_scavenger_inside,create_columns_slb_scavenger_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_temple(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_temple_floor,create_columns_slb_temple_edge,
        create_columns_slb_temple_corner,create_columns_slb_temple_inside,create_columns_slb_temple_edge,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_graveyard(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_graveyard_floor,create_columns_slb_graveyard_edge,
        create_columns_slb_graveyard_corner,create_columns_slb_graveyard_inside,create_columns_slb_graveyard_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_hatchery(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_hatchery_floor,create_columns_slb_hatchery_edge,
        create_columns_slb_hatchery_corner,create_columns_slb_hatchery_inside,create_columns_slb_hatchery_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_lair(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_lair_floor,create_columns_slb_lair_edge,
        create_columns_slb_lair_corner,create_columns_slb_lair_inside,create_columns_slb_lair_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_barracks(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_barracks_floor,create_columns_slb_barracks_edge,
        create_columns_slb_barracks_corner,create_columns_slb_barracks_inside,create_columns_slb_barracks_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_door_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    fill_column_rec_sim(clm_recs[IDIR_NW],    0, CUBE_PATH_SMOOTH3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_NORTH], 0, 0x0b3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_NE],    0, 0x0b5,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_WEST],  0, 0x0b3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_CENTR], 0, 0x0b1,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_EAST],  0, 0x0ba,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_SW],    0, 0x0b8,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_SOUTH], 0, 0x0ba,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_SE],    0, CUBE_PATH_SMOOTH3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void create_columns_slb_doorwood(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_doorwood_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_doorwood_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorwood_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_doorwood_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_doorwood_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorwood_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void place_column_doorwood_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x169;
  clm_rec->c[pos+1]=0x166;
  clm_rec->c[pos+2]=0x163;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorwood_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x169;
  clm_rec->c[pos+1]=0x166;
  clm_rec->c[pos+2]=0x163;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorwood_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x16b;
  clm_rec->c[pos+1]=0x168;
  clm_rec->c[pos+2]=0x165;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void create_columns_slb_doorbrace(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_doorbrace_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_doorbrace_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorbrace_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_doorbrace_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_doorbrace_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorbrace_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void place_column_doorbrace_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x172;
  clm_rec->c[pos+1]=0x16f;
  clm_rec->c[pos+2]=0x16c;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorbrace_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x173;
  clm_rec->c[pos+1]=0x170;
  clm_rec->c[pos+2]=0x16d;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorbrace_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x174;
  clm_rec->c[pos+1]=0x171;
  clm_rec->c[pos+2]=0x16e;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void create_columns_slb_dooriron(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_dooriron_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_dooriron_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_dooriron_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_dooriron_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_dooriron_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_dooriron_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void place_column_dooriron_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x17b;
  clm_rec->c[pos+1]=0x178;
  clm_rec->c[pos+2]=0x175;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dooriron_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x17c;
  clm_rec->c[pos+1]=0x179;
  clm_rec->c[pos+2]=0x176;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dooriron_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x17d;
  clm_rec->c[pos+1]=0x17a;
  clm_rec->c[pos+2]=0x177;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void create_columns_slb_doormagic(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_doormagic_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_doormagic_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doormagic_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_doormagic_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_doormagic_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doormagic_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void place_column_doormagic_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x12e;
  clm_rec->c[pos+1]=0x12b;
  clm_rec->c[pos+2]=0x128;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doormagic_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x12d;
  clm_rec->c[pos+1]=0x12a;
  clm_rec->c[pos+2]=0x127;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doormagic_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x12c;
  clm_rec->c[pos+1]=0x129;
  clm_rec->c[pos+2]=0x126;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

/*
 * Function modifies a column made of earth,gold or similar material
 * for gaps when near unclaimed, short terrain. The corner
 * column are sometimes changed to the surrounding material.
 */
void modify_frail_columns(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
    const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
    const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
    //This decreases probability of column change if there are no adjacent tall slabs
    int base_prob=33;
    if (slab_is_tall_unclmabl(surr_slb[IDIR_WEST])||slab_is_tall_unclmabl(surr_slb[IDIR_NORTH])||
       slab_is_tall_unclmabl(surr_slb[IDIR_EAST])||slab_is_tall_unclmabl(surr_slb[IDIR_SOUTH]))
       base_prob=66;
    int i;
    //Let's take all corners at once, in a loop
    for (i=0;i<4;i++)
    {
      if (slab_is_short_unclmabl(surr_slb[dir_a[i]]) &&
          slab_is_short_unclmabl(surr_slb[dir_b[i]]))
      {
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_PATH)||(surr_slb[dir_b[i]]==SLAB_TYPE_PATH))
          {
            if (rnd(100)<base_prob)
              fill_column_path(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          } else
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
          {
            if (rnd(100)<base_prob+33)
              fill_column_water(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          } else
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
          {
            if (rnd(100)<base_prob+33)
              fill_column_lava(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          } else
          // My own mod - replacing part of gold/gems with dirt if it is near
          if ((surr_slb[IDIR_CENTR]!=SLAB_TYPE_EARTH)&&(surr_slb[IDIR_CENTR]!=SLAB_TYPE_TORCHDIRT)&&
             ((surr_slb[dir_a[i]]==SLAB_TYPE_EARTH)||(surr_slb[dir_a[i]]==SLAB_TYPE_TORCHDIRT))&&
             ((surr_slb[dir_b[i]]==SLAB_TYPE_EARTH)||(surr_slb[dir_b[i]]==SLAB_TYPE_TORCHDIRT)))
          {
            if (rnd(100)<base_prob+33)
              fill_column_earth(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          }
      }
    }
}

/*
 * Function modifies non-central columns if there is water or lava near.
 * Returns true if something was modified
 */
short modify_liquid_surrounding(struct COLUMN_REC *clm_recs[9],unsigned char *surr_slb,
        short liq_level,unsigned short water_cube,unsigned short lava_cube)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  //initial tests
  if ((liq_level<0)||(liq_level>7)) return false;
  if (surr_slb==NULL) return false;
  short result=false;
  int i;
  //corner columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
    {
       clm_recs[dir_c[i]]->c[liq_level]=water_cube;
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
    {
       clm_recs[dir_c[i]]->c[liq_level]=lava_cube;
       result=true;
    }
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER))
    {
       clm_recs[dir_a[i]]->c[liq_level]=water_cube;
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA))
    {
       clm_recs[dir_a[i]]->c[liq_level]=lava_cube;
       result=true;
    }
  }
  return result;
}

/*
 * Function modifies non-central columns if there is water or lava near.
 * Advanced version - the change is different for every direction.
 * (water_cube and lava_cube should point to 9-element matrix)
 * Returns true if something was modified
 */
short modify_liquid_surrounding_advncd(struct COLUMN_REC *clm_recs[9],
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const short liq_level,const unsigned short *water_cube,const unsigned short *lava_cube)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //initial tests
  if ((liq_level<0)||(liq_level>7)) return false;
  if (surr_slb==NULL) return false;
  short result=false;
  int i;
  //corner columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
    {
        if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=water_cube[dir_b[i]];
        else
        if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=water_cube[dir_a[i]];
        else
         clm_recs[dir_c[i]]->c[liq_level]=water_cube[dir_c[i]];
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
    {
        if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=lava_cube[dir_b[i]];
        else
        if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=lava_cube[dir_a[i]];
        else
         clm_recs[dir_c[i]]->c[liq_level]=lava_cube[dir_c[i]];
       result=true;
    }
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER))
    {
       clm_recs[dir_a[i]]->c[liq_level]=water_cube[dir_a[i]];
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA))
    {
       clm_recs[dir_a[i]]->c[liq_level]=lava_cube[dir_a[i]];
       result=true;
    }
  }
  return result;
}

/*
 * Returns one of the constant arrays for altering direction of room slabs.
 * The direction is made in a way that allows access column at room corner in IDIR_SE.
 */
unsigned short *get_room_corner_direction_indices(unsigned char *surr_slb,unsigned char *surr_own)
{
    unsigned short slab=surr_slb[IDIR_CENTR];
    unsigned char ownr=surr_own[IDIR_CENTR];
    if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
      return (unsigned short *)dir_rot_000;
    if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
      return (unsigned short *)dir_rot_090;
    if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
      return (unsigned short *)dir_rot_180;
    if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
      return (unsigned short *)dir_rot_270;
    if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
      return (unsigned short *)dir_rot_000;
    if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
      return (unsigned short *)dir_rot_090;
    if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
      return (unsigned short *)dir_rot_180;
    if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
      return (unsigned short *)dir_rot_270;
  return (unsigned short *)dir_rot_000;
}

/*
 * Returns one of the constant arrays for altering direction of room slabs.
 * It maps indices so that EAST is the "inside" of room.
 */
unsigned short *get_room_edge_direction_indices(unsigned char *surr_slb,unsigned char *surr_own)
{
    unsigned short slab=surr_slb[IDIR_CENTR];
    unsigned char ownr=surr_own[IDIR_CENTR];

  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
    return (unsigned short *)dir_rot_000;
  if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
    return (unsigned short *)dir_rot_090;
  if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
    return (unsigned short *)dir_rot_180;
  if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
    return (unsigned short *)dir_rot_270;

  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
    return (unsigned short *)dir_rot_000;
  if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
    return (unsigned short *)dir_rot_090;
  if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
    return (unsigned short *)dir_rot_180;
  if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
    return (unsigned short *)dir_rot_270;

  return (unsigned short *)dir_rot_000;
}

void create_columns_slb_room(cr_clm_func cr_floor,cr_clm_func cr_edge,
        cr_clm_func cr_corner,cr_clm_func cr_inside,cr_clm_func cr_nearinsd,
        struct COLUMN_REC *clm_recs[9], unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Checking if completely surrounded
  if ((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr))
  {
      if  ((surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
           (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
           (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
           (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr))
      {
          cr_inside(clm_recs,surr_slb,surr_own,surr_tng);
      } else
      {
          //The 'near inside' columns are usually same that floor,
          //but may differ for rooms with specific corners
          cr_nearinsd(clm_recs,surr_slb,surr_own,surr_tng);
      }
      return;
  }
  //If not completely, maybe we're surrounded from 3 sides (5 with corners)?
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)) ||
      ((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)) ||
      ((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)) ||
      ((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
  {
      cr_edge(clm_recs,surr_slb,surr_own,surr_tng);
      return;
  }
  //If still nothing, maybe we have same surround from two sides and 1 corner,
  // and another two are surely something else
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
//       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
//       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
//       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))) ||

      ((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
//       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))))
  {
      cr_corner(clm_recs,surr_slb,surr_own,surr_tng);
      return;
  }
  //If nothing found - just draw floor of this room
  cr_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_library_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
      fill_column_rec_sim(clm_recs[k*3+i], 0, 0x0f8,
           0x0ae +rnd(2), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0bb, 0x0ba);
}

void create_columns_slb_library_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //Library egde is just its floor
  create_columns_slb_library_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_library_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_library_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_library_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_library_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_library_floor(clm_recs,surr_slb,surr_own,surr_tng);
  place_column_library_bookcase_a(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
  place_column_library_bookcase_b(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  place_column_library_bookcase_c(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
}

void create_columns_slb_dungheart_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
      fill_column_dungheart_floor(clm_recs[k*3+i], surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0a4, 0x0a3);
}

void create_columns_slb_dungheart_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_dungheart_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_edge_direction_indices(surr_slb,surr_own);
    fill_column_dungheart_inside(clm_recs[dir[IDIR_NE]], surr_own[IDIR_CENTR]);
    fill_column_dungheart_inside(clm_recs[dir[IDIR_EAST]], surr_own[IDIR_CENTR]);
    fill_column_dungheart_inside(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
    place_column_univ_stair(clm_recs[dir[IDIR_NORTH]], surr_own[IDIR_CENTR]);
    place_column_univ_stair(clm_recs[dir[IDIR_CENTR]], surr_own[IDIR_CENTR]);
    place_column_univ_stair(clm_recs[dir[IDIR_SOUTH]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_dungheart_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_dungheart_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_dungheart_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_EAST]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_NE]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_SOUTH]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_SW]], surr_own[IDIR_CENTR]);
    fill_column_rec_sim(clm_recs[dir[IDIR_CENTR]], 0, CUBE_PATH_SMOOTH3,
         0x080, 0x180, 0x0, 0x0, 0x0, 0, 0, 0);
}

void create_columns_slb_dungheart_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_dungheart_inside(clm_recs[k*3+i], surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_dungheart_nearinsd(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  create_columns_slb_dungheart_floor(clm_recs,surr_slb,surr_own,surr_tng);
  //Raise the center a little bit
  place_column_univ_stair(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  int i;
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //corner columns
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)) ||
        ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr)))
    {
      short i_op=(i+2)%4;
      if (((surr_slb[dir_a[i_op]]!=slab)||(surr_own[dir_a[i_op]]!=ownr)) ||
          ((surr_slb[dir_b[i_op]]!=slab)||(surr_own[dir_a[i_op]]!=ownr)))
        place_column_dungheart_raise(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
      else
        fill_column_dungheart_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    }
  }
  //and the rest of columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
      short i_op=(i+2)%4;
      if ((surr_slb[dir_a[i_op]]!=slab)||(surr_own[dir_a[i_op]]!=ownr))
        place_column_dungheart_raise(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
      else
        fill_column_dungheart_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    }
  }
}

void create_columns_slb_portal_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_portal_floor(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)) ||
        ((surr_slb[dir_b[i]]!=slab)||(surr_own[dir_b[i]]!=ownr)))
      fill_column_portal_edge(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_portal_floor(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)))
      fill_column_portal_edge(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_portal_floor(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x13b, 0x15f);
}

void create_columns_slb_portal_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_portal_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_edge_direction_indices(surr_slb,surr_own);
    fill_column_portal_step(clm_recs[dir[IDIR_NE]], surr_own[IDIR_CENTR]);
    fill_column_portal_step(clm_recs[dir[IDIR_EAST]], surr_own[IDIR_CENTR]);
    fill_column_portal_step(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_portal_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_portal_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_portal_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_portal_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i;
  for (i=0;i<9;i++)
    fill_column_rec_sim(clm_recs[i], 0, 0x0,
         0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x089+i, 0);
  fill_column_portal_inside_cntr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_temple_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_temple_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x075, 0x076);
}

void create_columns_slb_temple_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  const unsigned short dir_d[]={IDIR_SOUTH, IDIR_WEST, IDIR_NORTH, IDIR_EAST};
  const unsigned short dir_e[]={IDIR_EAST, IDIR_SOUTH, IDIR_WEST, IDIR_NORTH};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Temple edge is like its floor,
  //but it is higher by one cube from inside.
  create_columns_slb_temple_floor(clm_recs,surr_slb,surr_own, surr_tng);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr) &&
         (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr)) &&
        ((surr_slb[dir_d[i]]==slab)&&(surr_own[dir_d[i]]==ownr) ||
         (surr_slb[dir_e[i]]==slab)&&(surr_own[dir_e[i]]==ownr)))
      place_column_temple_edge(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr) &&
         (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr)) &&
        ((surr_slb[dir_d[i]]==slab)&&(surr_own[dir_d[i]]==ownr)))
      place_column_temple_edge(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_temple_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_temple_floor(clm_recs,surr_slb,surr_own, surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    place_column_temple_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
    place_column_temple_corner(clm_recs[dir[IDIR_CENTR]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_temple_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_rec_sim(clm_recs[i], 0, 0x177,
          CUBE_PATH_SMOOTH1 +rnd(5), 0x126 +i, 0x0, 0x0, 0x0, 0, 0, 0);
}

void create_columns_slb_hatchery_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_hatchery_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)) ||
        ((surr_slb[dir_b[i]]!=slab)||(surr_own[dir_b[i]]!=ownr)))
      fill_column_hatchery_edge(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_hatchery_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)))
      fill_column_hatchery_edge(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_hatchery_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
  //Liquid surrounding - lava surround is not as trivial as usually
  unsigned short *water_cube=malloc(9*sizeof(unsigned short));
  unsigned short *lava_cube  =malloc(9*sizeof(unsigned short));
  for (i=0;i<9;i++)
    water_cube[i]=0x159;
  lava_cube[IDIR_NW]=0x15e;
  lava_cube[IDIR_NE]=0x15d;
  lava_cube[IDIR_SW]=0x15c;
  lava_cube[IDIR_SE]=0x15e;
  lava_cube[IDIR_NORTH]=0x15d;
  lava_cube[IDIR_EAST]=0x15d;
  lava_cube[IDIR_SOUTH]=0x15c;
  lava_cube[IDIR_WEST]=rnd(2)?0x15c:0x13c;
  modify_liquid_surrounding_advncd(clm_recs,surr_slb,surr_own, 0,water_cube,lava_cube);
  free(water_cube);
  free(lava_cube);
}

void create_columns_slb_hatchery_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_hatchery_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_hatchery_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_hatchery_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_hatchery_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_hatchery_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_hatchery_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_lair_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_lair_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_lair_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)&&
        (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //Surrounded by our area
       clm_recs[dir_c[i]]->c[0]=CUBE_LAIR_INSIDE;
    } else
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    { //our on side A only
        switch (dir_c[i])
        {
        case IDIR_NW://West only, not north
           clm_recs[dir_c[i]]->c[0]=0x01f;
           break;
        case IDIR_NE://North only, not east
           clm_recs[dir_c[i]]->c[0]=0x022;
           break;
        case IDIR_SW://South only, not west
           clm_recs[dir_c[i]]->c[0]=0x021;
           break;
        case IDIR_SE://East only, not south
           clm_recs[dir_c[i]]->c[0]=0x024;
           break;
        }
    } else
    if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //our on side B only
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x021;
           break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x024;
           break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x01f;
           break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x022;
           break;
        }
    } else
    { //Surrounded by not our area
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x01e;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x020;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x023;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x025;
          break;
        }
    }
  }
  //And the middle columns
  for (i=0;i<4;i++)
  {
     fill_column_lair_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
       clm_recs[dir_a[i]]->c[0]=CUBE_LAIR_INSIDE;
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x024;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x021;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x01f;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x022;
           break;
        }
    }
  }
  // Liquid surrounding is not supported for the lair ground,
  // so we've done
}

void create_columns_slb_lair_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_lair_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_lair_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_lair_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_lair_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_lair_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_graveyard_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_graveyard_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_graveyard_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)&&
        (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //Surrounded by our area
       clm_recs[dir_c[i]]->c[0]=0x142;
    } else
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    { //our on side A only
        switch (dir_c[i])
        {
        case IDIR_NW://West only, not north
           clm_recs[dir_c[i]]->c[0]=0x144;
           break;
        case IDIR_NE://North only, not east
           clm_recs[dir_c[i]]->c[0]=0x147;
           break;
        case IDIR_SW://South only, not west
           clm_recs[dir_c[i]]->c[0]=0x146;
           break;
        case IDIR_SE://East only, not south
           clm_recs[dir_c[i]]->c[0]=0x149;
           break;
        }
    } else
    if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //our on side B only
        switch (dir_c[i])
        {
        case IDIR_NW://not west
           clm_recs[dir_c[i]]->c[0]=0x146;
           break;
        case IDIR_SW://not south
           clm_recs[dir_c[i]]->c[0]=0x149;
           break;
        case IDIR_NE://not north
           clm_recs[dir_c[i]]->c[0]=0x144;
           break;
        case IDIR_SE://not east
           clm_recs[dir_c[i]]->c[0]=0x147;
           break;
        }
    } else
    { //Surrounded by not our area
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x143;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x145;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x148;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x14a;
          break;
        }
    }
  }
  //And the middle columns
  for (i=0;i<4;i++)
  {
     fill_column_graveyard_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
       clm_recs[dir_a[i]]->c[0]=0x142;
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x149;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x146;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x144;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x147;
           break;
        }
    }
  }
  // Liquid surrounding is not supported for the graveyard ground,
  // so we've done
}

void create_columns_slb_graveyard_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_graveyard_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_graveyard_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_graveyard_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_graveyard_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_graveyard_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_barracks_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_barracks_floor(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_barracks_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_barracks_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_barracks_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_barracks_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_barracks_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_barracks_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
    {
      fill_column_barracks_floor(clm_recs[i], surr_own[IDIR_CENTR]);
      place_column_barracks_raise(clm_recs[i], surr_own[IDIR_CENTR]);
    }
}

void create_columns_slb_training_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_training_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0fe, 0x0fd);
}

void create_columns_slb_training_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_training_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_training_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_training_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_training_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_training_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_training_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_treasure_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_treasure_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0a4, 0x0a3);
}

void create_columns_slb_treasure_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_treasure_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_treasure_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_treasure_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_treasure_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_treasure_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_treasure_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_workshop_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_workshop_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x109, 0x108);
}

void create_columns_slb_workshop_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_workshop_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_workshop_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_workshop_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_workshop_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_workshop_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_workshop_floor(clm_recs,surr_slb,surr_own,surr_tng);
  //On central slabs, we can use lava and water cubes too - they have same top.
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     clm_recs[k*3+i]->c[0]=0x107 +rnd(3);
   }
}

void create_columns_slb_scavenger_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_scavenger_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  fill_column_scavenger_inside_cntr(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x11b, 0x11a);
}

void create_columns_slb_scavenger_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_scavenger_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_scavenger_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_scavenger_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_scavenger_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_scavenger_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_scavenger_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_prison_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_prison_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)&&
        (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //Surrounded by our area
       clm_recs[dir_c[i]]->c[0]=0x0c8;
    } else
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    { //our on side A only
        switch (dir_c[i])
        {
        case IDIR_NW://West only, not north
           clm_recs[dir_c[i]]->c[0]=0x0ca;
           break;
        case IDIR_NE://North only, not east
           clm_recs[dir_c[i]]->c[0]=0x0cd;
           break;
        case IDIR_SW://South only, not west
           clm_recs[dir_c[i]]->c[0]=0x0cc;
           break;
        case IDIR_SE://East only, not south
           clm_recs[dir_c[i]]->c[0]=0x0cf;
           break;
        }
    } else
    if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //our on side B only
        switch (dir_c[i])
        {
        case IDIR_NW://not west
           clm_recs[dir_c[i]]->c[0]=0x0cc;
           break;
        case IDIR_SW://not south
           clm_recs[dir_c[i]]->c[0]=0x0cf;
           break;
        case IDIR_NE://not north
           clm_recs[dir_c[i]]->c[0]=0x0ca;
           break;
        case IDIR_SE://not east
           clm_recs[dir_c[i]]->c[0]=0x0cd;
           break;
        }
    } else
    { //Surrounded by not our area
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x0c9;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x0cb;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x0ce;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x0d0;
          break;
        }
    }
  }
  //And the middle columns
  for (i=0;i<4;i++)
  {
     fill_column_prison_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
       clm_recs[dir_a[i]]->c[0]=0x0c8;
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x0cf;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x0cc;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x0ca;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x0cd;
           break;
        }
    }
  }
  // Liquid surrounding - quite hard here
  unsigned short *water_cube=malloc(9*sizeof(unsigned short));
  unsigned short *lava_cube  =malloc(9*sizeof(unsigned short));
  water_cube[IDIR_NW]=0x0d9;
  water_cube[IDIR_NE]=0x0db;
  water_cube[IDIR_SW]=0x0de;
  water_cube[IDIR_SE]=0x0e0;
  water_cube[IDIR_NORTH]=0x0da;
  water_cube[IDIR_EAST]= 0x0dd;
  water_cube[IDIR_SOUTH]=0x0df;
  water_cube[IDIR_WEST]= 0x0dc;
  lava_cube[IDIR_NW]=0x0d1;
  lava_cube[IDIR_NE]=0x0d3;
  lava_cube[IDIR_SW]=0x0d6;
  lava_cube[IDIR_SE]=0x0d8;
  lava_cube[IDIR_NORTH]=0x0d2;
  lava_cube[IDIR_EAST]= 0x0d5;
  lava_cube[IDIR_SOUTH]=0x0d7;
  lava_cube[IDIR_WEST]= 0x0d4;
  modify_liquid_surrounding_advncd(clm_recs,surr_slb,surr_own, 0,water_cube,lava_cube);
  free(water_cube);
  free(lava_cube);
}

void create_columns_slb_prison_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_prison_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_torture_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_floor(clm_recs,surr_slb,surr_own,surr_tng);
    clm_recs[IDIR_CENTR]->c[0]=CUBE_FLOOR_TORTCIRC;
}

void create_columns_slb_torture_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_torture_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torture_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_torture_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torture_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_inside(clm_recs,surr_slb,surr_own,surr_tng);
    clm_recs[IDIR_CENTR]->c[0]=CUBE_FLOOR_TORTCIRC;
}

void create_columns_slb_guardpost(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    fill_column_guardpost_floor_c(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_a(clm_recs[IDIR_NE], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_b(clm_recs[IDIR_SE], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_a(clm_recs[IDIR_SW], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_b(clm_recs[IDIR_NW], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_bridge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_bridge_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_rock_gndlev(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_rock_gndlev(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_rockcaped_pathcave(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_path(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
     clm_recs[k*3+i]->c[4]=CUBE_ROCK1;
     clm_recs[k*3+i]->solid=compute_clm_rec_solid(clm_recs[k*3+i]);
     clm_recs[k*3+i]->height=compute_clm_rec_height(clm_recs[k*3+i]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x02e, 0x02f);
}

void fill_column_rock(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_simp(clm_rec, 0, CUBE_PATH_SMLSTONES,
         CUBE_ROCK3, CUBE_ROCK3, CUBE_ROCK2, CUBE_ROCK2, CUBE_ROCK1, 0x0, 0x0, 0x0);
}

void fill_column_gold(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_EARTHGRNSDCTR, CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), 0, 0, 0);
}

void fill_column_gold_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x026, CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), 0, 0, 0);
}

void fill_column_gold_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_EARTHBTMLAVA, 0x034+rnd(3), 0x034+rnd(3), 0x034+rnd(3), 0x034+rnd(3), 0, 0, 0);
}

void fill_column_earth(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_EARTHGRNSDCTR, 0x001+rnd(3), 0x001+rnd(3), 0x005, 0, 0, 0);
}

void fill_column_earth_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x026, 0x001+rnd(3), 0x001+rnd(3), 0x001+rnd(3), 0x005, 0, 0, 0);
}

void fill_column_earth_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_EARTHBTMLAVA, 0x030, 0x030, 0x030, 0x030, 0, 0, 0);
}

void fill_column_wall_centr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     //fill_column_rec_sim(clm_rec, 0, CUBE_PATH3,
     //    CUBE_PATH1+rnd(5), 0x00a, 0x001+rnd(3), 0x001+rnd(3), 0x004, 0, 0, 0);
     // Simplified - why wasting CLM entires if only top is visible anyway?
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1, CUBE_EARTHGRNSDCTR, CUBE_EARTHSTD2, CUBE_EARTHSTD2, CUBE_EARTHBRGTOPDKL, 0, 0, 0);
     //The "top" is owner-dependent
     if (owner<PLAYER_UNSET)
         clm_rec->c[4]=0x043+owner;
}

/*
 * Column of grey stones, larger than brick.
 */
void fill_column_wall_cobblestones(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x098, 0x097, 0x096, 0x095, 0, 0, 0);
}

/*
 * Column of grey stones with some markings on.
 */
void place_column_wall_cobblestones_mk(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=clm_rec->height;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x03e;
  clm_rec->c[pos+1]=0x039;
  clm_rec->c[pos+2]=0x03a;
  clm_rec->c[pos+3]=0x041;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_wall_redsmbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x053, 0x050, 0x050, 0x04e, 0, 0, 0);
}

void fill_column_wall_redsmbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x052, 0x048+rnd(3), 0x048+rnd(3), 0x04d, 0, 0, 0);
}

void fill_column_wall_redsmbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x051, 0x04f, 0x04f, 0x04c, 0, 0, 0);
}

void fill_column_wall_drapebrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    //we could use fill_column_wall_redsmbrick_a to get this better,
    // but this way we're not losing CLM entries
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1, 0x052, 0x049, 0x049, 0x04d, 0, 0, 0);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[4]=0x0a0; break;
    case PLAYER1:    clm_rec->c[4]=0x19a; break;
    case PLAYER2:    clm_rec->c[4]=0x19d; break;
    case PLAYER3:    clm_rec->c[4]=0x1a0; break;
    case PLAYER_GOOD:clm_rec->c[4]=0x1a3; break;
    default:         clm_rec->c[4]=0x04d; break;
    }
}

void fill_column_wall_drapebrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    //we could use fill_column_wall_redsmbrick_a to get this better,
    // but this way we're not losing CLM entries
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1, 0x052, 0x049, 0x049, 0x04d, 0, 0, 0);
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[4]=0x0a1; break;
  case PLAYER1:    clm_rec->c[4]=0x19b; break;
  case PLAYER2:    clm_rec->c[4]=0x19e; break;
  case PLAYER3:    clm_rec->c[4]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[4]=0x1a4; break;
  default:         clm_rec->c[4]=0x04d; break;
  }
}

void fill_column_wall_drapebrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1, 0x052, 0x049, 0x049, 0x04d, 0, 0, 0);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[4]=0x0a2; break;
    case PLAYER1:    clm_rec->c[4]=0x19c; break;
    case PLAYER2:    clm_rec->c[4]=0x19f; break;
    case PLAYER3:    clm_rec->c[4]=0x1a2; break;
    case PLAYER_GOOD:clm_rec->c[4]=0x1a5; break;
    default:         clm_rec->c[4]=0x04d; break;
    }
}

void fill_column_wall_twinsbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x195, CUBE_BRICK_RTWINSBL, CUBE_BRICK_RTWINSML, CUBE_BRICK_RTWINSTL, 0, 0, 0);
}

void fill_column_wall_twinsbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x196, CUBE_BRICK_RTWINSBC, CUBE_BRICK_RTWINSMC, CUBE_BRICK_RTWINSTC, 0, 0, 0);
}

void fill_column_wall_twinsbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x197, CUBE_BRICK_RTWINSBR, CUBE_BRICK_RTWINSMR, CUBE_BRICK_RTWINSTR, 0, 0, 0);
}

void fill_column_wall_womanbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_RWOMANBL, CUBE_BRICK_RWOMANML,
         CUBE_BRICK_RWOMANUL, CUBE_BRICK_RWOMANTL, 0, 0, 0);
}

void fill_column_wall_womanbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_RWOMANBC, CUBE_BRICK_RWOMANMC,
         CUBE_BRICK_RWOMANUC, CUBE_BRICK_RWOMANTC, 0, 0, 0);
}

void fill_column_wall_womanbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_RWOMANBR, CUBE_BRICK_RWOMANMR,
         CUBE_BRICK_RWOMANUR, CUBE_BRICK_RWOMANTR, 0, 0, 0);
}

void fill_column_wall_pairshrbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_PAIRSHBL, CUBE_BRICK_PAIRSHML,
         CUBE_BRICK_PAIRSHUL, CUBE_BRICK_PAIRSHTL, 0, 0, 0);
}

void fill_column_wall_pairshrbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_PAIRSHBC, CUBE_BRICK_PAIRSHMC,
         CUBE_BRICK_PAIRSHUC, CUBE_BRICK_PAIRSHTC, 0, 0, 0);
}

void fill_column_wall_pairshrbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_PAIRSHBR, CUBE_BRICK_PAIRSHMR,
         CUBE_BRICK_PAIRSHUR, CUBE_BRICK_PAIRSHTR, 0, 0, 0);
}

void fill_column_path(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0cf,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

/*
 * Claimed ground (not claimed wall)
 */
void fill_column_claimedgnd_centr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x0c6, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
     //The "top" is owner-dependent
     if (owner==PLAYER_GOOD)
         clm_rec->c[0]=0x0c7;
     else
     if (owner<PLAYER_UNSET)
         clm_rec->c[0]=0x0c0+owner;
}

void fill_column_claimedgnd_surr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x07e +rnd(3), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_claimedgnd_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_claimedgnd_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x093, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_lava(struct COLUMN_REC *clm_rec, unsigned char owner)
{ //Simplicity of this one scares me..
     fill_column_rec_sim(clm_rec, 0, 0x222 +rnd(2),
         0x0, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_water(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0221,
         0x0, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_gem(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x00a, 0x1b9 +rnd(3), 0x1b9 +rnd(3), 0x1b9 +rnd(3), 0, 0, 0);
}

void fill_column_library_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x0b9, 0x0b8, 0x0b7, 0x0b6, 0, 0, 0);
}

void place_column_library_bookcase_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  clm_rec->base=CUBE_PATH_SMOOTH3;
  clm_rec->c[1]=0x14b;
  clm_rec->c[2]=0x14d;
  clm_rec->c[3]=0x14b;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_library_bookcase_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  clm_rec->base=CUBE_PATH_SMOOTH3;
  clm_rec->c[1]=0x0b1;
  clm_rec->c[2]=0x0b4;
  clm_rec->c[3]=0x0b1;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_library_bookcase_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  clm_rec->base=CUBE_PATH_SMOOTH3;
  clm_rec->c[1]=0x14c;
  clm_rec->c[2]=0x14e;
  clm_rec->c[3]=0x14c;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_temple_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>1) pos=1;
  clm_rec->c[pos+0]=0x122;
  clm_rec->c[pos+1]=0x122;
  clm_rec->c[pos+2]=0x120;
  clm_rec->c[pos+3]=0x11d;
  clm_rec->c[pos+4]=0x120;
  clm_rec->c[pos+5]=0x11e;
  clm_rec->c[pos+6]=0x11d;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_temple_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x177,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x123, 0x0, 0x0, 0x0, 0, 0, 0);
}

void place_column_temple_corner(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
  clm_rec->c[pos]=0x122;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_temple_edge(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
  clm_rec->c[pos]=0x122;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_dungheart_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x09f , 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_dungheart_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
        0x080 , 0x052, 0x048, 0x049, 0x04a, 0x049, 0x17f, 0x1aa);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[7]=CUBE_EYEDROCKRED; break;
    case PLAYER1:    clm_rec->c[7]=CUBE_EYEDROCKBLUE; break;
    case PLAYER2:    clm_rec->c[7]=CUBE_EYEDROCKGREEN; break;
    case PLAYER3:    clm_rec->c[7]=CUBE_EYEDROCKYELLOW; break;
    case PLAYER_GOOD:clm_rec->c[7]=CUBE_EYEDROCKWHITE; break;
    default:         clm_rec->c[7]=CUBE_EYEDROCKBLINK; break;
    }
}

void fill_column_dungheart_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x052, 0x09f, 0x0, 0x0, 0, 0, 0);
}


void place_column_univ_stair(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+1]=clm_rec->c[pos+0];
  clm_rec->c[pos+0]=CUBE_PATH_SMOOTH1 +rnd(5);
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dungheart_raise(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=0x052;
  clm_rec->c[pos+1]=0x04d;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_portal_edge(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x138, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_portal_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x00a, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_portal_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMLSTONES,
        CUBE_PATH_SMOOTH1 +rnd(5), 0x00a, 0x00f, 0x00d, 0x00d, 0x00c, 0x00e, 0x0);
}

void fill_column_portal_step(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_LRGSTONES +rnd(2),
         0x00a, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00e, 0);
}

void fill_column_portal_inside_cntr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x0,
         0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x089+4, 0);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[6]=CUBE_EYEDROCKRED; break;
    case PLAYER1:    clm_rec->c[6]=CUBE_EYEDROCKBLUE; break;
    case PLAYER2:    clm_rec->c[6]=CUBE_EYEDROCKGREEN; break;
    case PLAYER3:    clm_rec->c[6]=CUBE_EYEDROCKYELLOW; break;
    case PLAYER_GOOD:clm_rec->c[6]=CUBE_EYEDROCKWHITE; break;
    default:         clm_rec->c[6]=CUBE_EYEDROCKBLINK; break;
    }
}

void fill_column_hatchery_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3 +rnd(5),
         0x0, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_hatchery_edge(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x15a, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_hatchery_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x15b, 0x158, 0x158, 0x158, 0x158, 0x159, 0, 0);
}

void fill_column_lair_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_LAIR_INSIDE, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_graveyard_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_LRGSTONES,
         0x142, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void place_column_barracks_raise(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height;
  if (pos>7) pos=7;
  if (pos<1) pos=1;
  clm_rec->c[pos-1]=0x183;
  clm_rec->c[pos+0]=0x194;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_barracks_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x181, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_barracks_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
        CUBE_PATH_SMOOTH2, 0x184, 0x184, 0x18f, 0x18e, 0x184, 0x0, 0x0);
}

void fill_column_training_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x13e,
         0x0fc, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_training_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x13e,
        0x0fc, 0x0fb, 0x0fa, 0x0f9, 0x0f9, 0x0f1, 0x0, 0x0);
}

void fill_column_treasure_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x06a,
         0x09f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_treasure_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x06a,
        0x09f, 0x03e, 0x039, 0x03a, 0x039, 0x041, 0x0, 0x0);
}

void fill_column_workshop_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x15a,
         0x107, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_workshop_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
        CUBE_PATH_LRGSTONES, 0x10e, 0x10d, 0x10c, 0x10b, 0x0, 0x0, 0x0);
}


void fill_column_scavenger_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0f8,
         0x119, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_scavenger_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x0f8,
        CUBE_PATH_SMOOTH1, 0x118, 0x117, 0x116, 0x115, 0x0, 0x0, 0x0);
}

void fill_column_scavenger_inside_cntr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0f8,
         0x11c, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_prison_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x118,
         0x0c8, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_guardpost_floor_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0b0 +rnd(3),
         0x07e +rnd(3), CUBE_WOOD_FLOOR1, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_guardpost_floor_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0b0 +rnd(3),
         0x07e +rnd(3), CUBE_WOOD_FLOOR2, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_guardpost_floor_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0b0 +rnd(3),
         0x07e +rnd(3), CUBE_WOOD_FLOOR3, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_bridge_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x1f3,
         0x1b8, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_rock_gndlev(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_simp(clm_rec, 0, CUBE_PATH_SMLSTONES,
         CUBE_ROCK1 +rnd(3), 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
}

void place_column_wall_drape_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[pos+0]=0x0a0; break;
    case PLAYER1:    clm_rec->c[pos+0]=0x19a; break;
    case PLAYER2:    clm_rec->c[pos+0]=0x19d; break;
    case PLAYER3:    clm_rec->c[pos+0]=0x1a0; break;
    case PLAYER_GOOD:clm_rec->c[pos+0]=0x1a3; break;
    default:         clm_rec->c[pos+0]=0x04d; break;
    }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_drape_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[4]=0x0a1; break;
  case PLAYER1:    clm_rec->c[4]=0x19b; break;
  case PLAYER2:    clm_rec->c[4]=0x19e; break;
  case PLAYER3:    clm_rec->c[4]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[4]=0x1a4; break;
  default:         clm_rec->c[4]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_drape_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[4]=0x0a2; break;
  case PLAYER1:    clm_rec->c[4]=0x19c; break;
  case PLAYER2:    clm_rec->c[4]=0x19f; break;
  case PLAYER3:    clm_rec->c[4]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[4]=0x1a5; break;
  default:         clm_rec->c[4]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_portal_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x098;
  clm_rec->c[pos+1]=0x097;
  clm_rec->c[pos+2]=0x096;
  clm_rec->c[pos+3]=0x095;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_portal_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x04b;
  clm_rec->c[pos+1]=0x04b;
  clm_rec->c[pos+2]=0x04b;
  clm_rec->c[pos+3]=0x04b;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_treasure_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=0x09c;
  clm_rec->c[pos+1]=0x099;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_treasure_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=0x09d;
  clm_rec->c[pos+1]=0x09a;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_treasure_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=0x09e;
  clm_rec->c[pos+1]=0x09b;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_training_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=0x053;
  clm_rec->c[pos+1]=CUBE_BRICK_CRSWRDBL;
  clm_rec->c[pos+2]=CUBE_BRICK_CRSWRDTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_training_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=0x0fb;
  clm_rec->c[pos+1]=CUBE_BRICK_CRSWRDBC;
  clm_rec->c[pos+2]=CUBE_BRICK_CRSWRDTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_training_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=0x051;
  clm_rec->c[pos+1]=CUBE_BRICK_CRSWRDBR;
  clm_rec->c[pos+2]=CUBE_BRICK_CRSWRDTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_library_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_MGMIRRBL;
  clm_rec->c[pos+1]=CUBE_BRICK_MGMIRRTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_library_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_MGMIRRBC;
  clm_rec->c[pos+1]=CUBE_BRICK_MGMIRRTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_library_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_MGMIRRBR;
  clm_rec->c[pos+1]=CUBE_BRICK_MGMIRRTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_scavenger_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_GODEYEBL;
  clm_rec->c[pos+1]=CUBE_BRICK_GODEYETL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_scavenger_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_GODEYEBC;
  clm_rec->c[pos+1]=CUBE_BRICK_GODEYETC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_scavenger_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_GODEYEBR;
  clm_rec->c[pos+1]=CUBE_BRICK_GODEYETR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_graveyard_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_STONEWALL2;
  clm_rec->c[pos+1]=CUBE_STONEWALL1;
  clm_rec->c[pos+2]=0x050;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_graveyard_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=0x098;
  clm_rec->c[pos+1]=0x097;
  clm_rec->c[pos+2]=0x096;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_graveyard_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_STONEWALL2;
  clm_rec->c[pos+1]=CUBE_STONEWALL1;
  clm_rec->c[pos+2]=0x04f;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_lair_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=CUBE_BRICK_SKLSTW2C;
  clm_rec->c[pos+1]=CUBE_BRICK_SKULSTM;
  clm_rec->c[pos+2]=CUBE_BRICK_SKULSTT;
  clm_rec->c[pos+3]=CUBE_BRICK_SKLSTW1C;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_workshop_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_FURNCEBL;
  clm_rec->c[pos+1]=CUBE_BRICK_FURNCETL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_workshop_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_FURNCEBC;
  clm_rec->c[pos+1]=CUBE_BRICK_FURNCETC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_workshop_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_FURNCEBR;
  clm_rec->c[pos+1]=CUBE_BRICK_FURNCETR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_prison_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_RDHEADBL;
  clm_rec->c[pos+1]=CUBE_BRICK_RDHEADTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_prison_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_RDHEADBC;
  clm_rec->c[pos+1]=CUBE_BRICK_RDHEADTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_prison_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_RDHEADBR;
  clm_rec->c[pos+1]=CUBE_BRICK_RDHEADTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_torture_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_SPSKELBL;
  clm_rec->c[pos+1]=CUBE_BRICK_SPSKELML;
  clm_rec->c[pos+2]=CUBE_BRICK_SPSKELTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_torture_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_SPSKELBC;
  clm_rec->c[pos+1]=CUBE_BRICK_SPSKELMC;
  clm_rec->c[pos+2]=CUBE_BRICK_SPSKELTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_torture_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_SPSKELBR;
  clm_rec->c[pos+1]=CUBE_BRICK_SPSKELMR;
  clm_rec->c[pos+2]=CUBE_BRICK_SPSKELTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_barracks_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_COOPERBL;
  clm_rec->c[pos+1]=CUBE_BRICK_COOPERML;
  clm_rec->c[pos+2]=CUBE_BRICK_COOPERTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_barracks_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_COOPERBC;
  clm_rec->c[pos+1]=CUBE_BRICK_COOPERMC;
  clm_rec->c[pos+2]=CUBE_BRICK_COOPERTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_barracks_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_COOPERBR;
  clm_rec->c[pos+1]=CUBE_BRICK_COOPERMR;
  clm_rec->c[pos+2]=CUBE_BRICK_COOPERTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_temple_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x122;
  clm_rec->c[pos+1]=CUBE_BRICK_HORNRPBL;
  clm_rec->c[pos+2]=CUBE_BRICK_HORNRPML;
  clm_rec->c[pos+3]=CUBE_BRICK_HORNRPTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_temple_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x122;
  clm_rec->c[pos+1]=CUBE_BRICK_HORNRPBC;
  clm_rec->c[pos+2]=CUBE_BRICK_HORNRPMC;
  clm_rec->c[pos+3]=CUBE_BRICK_HORNRPTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_temple_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x122;
  clm_rec->c[pos+1]=CUBE_BRICK_HORNRPBR;
  clm_rec->c[pos+2]=CUBE_BRICK_HORNRPMR;
  clm_rec->c[pos+3]=CUBE_BRICK_HORNRPTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_hatchery_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_CHICKNBL;
  clm_rec->c[pos+1]=CUBE_BRICK_CHICKNML;
  clm_rec->c[pos+2]=CUBE_BRICK_CHICKNTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_hatchery_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_CHICKNBC;
  clm_rec->c[pos+1]=CUBE_BRICK_CHICKNMC;
  clm_rec->c[pos+2]=CUBE_BRICK_CHICKNTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_hatchery_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_CHICKNBR;
  clm_rec->c[pos+1]=CUBE_BRICK_CHICKNMR;
  clm_rec->c[pos+2]=CUBE_BRICK_CHICKNTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}
