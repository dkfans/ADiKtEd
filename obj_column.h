/*
 * obj_column.h
 *
 * Header file for obj_column.c.
 *
 */

#ifndef ADIKT_OBJCOLMN_H
#define ADIKT_OBJCOLMN_H

#define CLM_BOX_EMPTY1         0x0000
  //Standard, digable earth
#define CLM_BOX_EARTHSTD1      0x0001
#define CLM_BOX_EARTHSTD2      0x0002
#define CLM_BOX_EARTHSTD3      0x0003
  //Tamped earth; it looks like digable from top,
  // but different in profile
#define CLM_BOX_EARTHTAMPED1   0x0004
#define CLM_BOX_EARTHTAMPED2   0x0005
#define CLM_BOX_EARTHTAMPED3   0x0006

//TODO: add defines here

#define CLM_BOX_LAVA1          0x0028
#define CLM_BOX_LAVA2          0x0029

//TODO: add defines here

  // Rock with a little of something else on bottom
#define CLM_BOX_ROCKNEARDARK   0x0198
#define CLM_BOX_ROCKNEARLAVA   0x0199
  // Rock floors, color visible on sides
#define CLM_BOX_ROCKFLOORBLUE1 0x019a
#define CLM_BOX_ROCKFLOORBLUE2 0x019b
#define CLM_BOX_ROCKFLOORBLUE3 0x019c
#define CLM_BOX_ROCKFLOORGREY1 0x019d
#define CLM_BOX_ROCKFLOORGREY2 0x019e
#define CLM_BOX_ROCKFLOORGREY3 0x019f
#define CLM_BOX_ROCKFLOORYELL1 0x01a0
#define CLM_BOX_ROCKFLOORYELL2 0x01a1
#define CLM_BOX_ROCKFLOORGREY4 0x01a2
#define CLM_BOX_ROCKFLOORGREY5 0x01a3
#define CLM_BOX_ROCKFLOORWHITE 0x01a4
#define CLM_BOX_ROCFLOORKRED   0x01a5
  // Rocks with glassy "eye" at top
#define CLM_BOX_EYEDROCKBLUE   0x01a6
#define CLM_BOX_EYEDROCKGREEN  0x01a7
#define CLM_BOX_EYEDROCKYELLOW 0x01a8
#define CLM_BOX_EYEDROCKBLINK  0x01a9
#define CLM_BOX_EYEDROCKGREY   0x01aa
  //Various color rocks
#define CLM_BOX_ROCKGREEN      0x01ab
#define CLM_BOX_ROCKBLUE       0x01ac
#define CLM_BOX_ROCKYELLOW     0x01ad
#define CLM_BOX_ROCKBLACK      0x01ae
#define CLM_BOX_ROCKWHITE      0x01af
#define CLM_BOX_ROCKRED        0x01b0
  //Unpenetrable wall
#define CLM_BOX_ROCK           0x01b1
  //Wall of stone
#define CLM_BOX_STONEWALL1     0x01b2
#define CLM_BOX_STONEWALL2     0x01b3
  //Wood, for briges, but maybe other things
#define CLM_BOX_WOOD1          0x01b4
#define CLM_BOX_WOOD2          0x01b5
#define CLM_BOX_WOOD3          0x01b6
#define CLM_BOX_WOOD4          0x01b7
#define CLM_BOX_WOOD5          0x01b8
  //The purple gems
#define CLM_BOX_GEMS1          0x01b9
#define CLM_BOX_GEMS2          0x01ba
#define CLM_BOX_GEMS3          0x01bb
#define CLM_BOX_GEMS4          0x01bc

#define CLM_BOX_MAX_INDEX      511

struct LEVEL;

extern int default_clm;

extern void create_default_clm(void);
extern void set_clm_entry (int num, unsigned int use, int permanent, int lintel,
                     int height, unsigned int solid, int base, int orientation,
                     int c1, int c2, int c3, int c4, int c5, int c6,
                     int c7, int c8);
extern void set_dat_unif (int x, int y, int d);
extern void set_dat (int x, int y, int tl, int tm, int tr, int ml, int mm, int mr,
                 int bl, int bm, int br);
extern void set_dat2 (int cx, int cy, int d);
static void set_clm (int num, unsigned int use,
                 int base, int c1, int c2, 
                 int c3, int c4, int c5, int c6,
                 int c7, int c8);
static void finish_clm (void);
static void set_room_wall (int num, unsigned int use, int base, int c1, int c2,
                     int c3, int c4, int stat);
extern void set_corner_unif (int x, int y, int normal, int border);
extern void set_corner (int x, int y, int normal, int b1, int b2, 
                  int b3, int b4, int b5, int b6,
                  int b7, int b8);

extern void set_pillar_dat_entry(int x, int y, int pillar);
extern void update_dat_entry(int x, int y);


#endif // ADIKT_OBJCOLMN_H
