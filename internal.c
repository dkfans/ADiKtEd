/*
 * internal.c
 *
 * ???
 *
 */


#include "internal.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_utils.h"
#include "graffiti.h"

void set_dat2 (int cx, int cy, int d);
void update_square (int x, int y);
void update_tngdat (int x, int y);
void check_doors (void);
unsigned char *create_object (int x, int y, int type);
unsigned char *create_thing (int tx, int ty);
unsigned char *create_action_point (int x, int y);
void create_default_clm (void);
void create_clmdattng(void);
void check_doors (void);
int get_dat (int cx, int cy);

static void remove_room_things (int x, int y);
static int get_corner (int x, int y, int i, int j);
static void finish_clm (void);
static int picture_wall (int x, int y, int dx, int dy);
static void set_room_wall4 (int num, unsigned int use, 
                      int base, int c1, int c2,
                      int c3, int c4);
static void set_room_wall3 (int num, unsigned int use, 
                      int base, int c1, int c2,
                      int c3, int c4, int c5, int stat);
static void set_room_wall2 (int num, unsigned int use, 
                      int base, int c1, int c2,
                      int c3, int c4, int c5, int stat);
static void set_room_wall (int num, unsigned int use, int base, int c1, int c2,
                int c3, int c4, int stat);
static void set_pillar_thing (int x, int y, int pillar, int thingnum, int thinghgt);
static void set_pillar (int x, int y, int pillar);
static void set_corner_unif (int x, int y, int normal, int border);
static void set_corner (int x, int y, int normal, int b1, int b2, 
                  int b3, int b4, int b5, int b6,
                  int b7, int b8);
static void set_door_dattng (int x, int y, int datbase, int type, int locked);
static int rnd (int range);
static void set_wall (int x, int y, int d1, int d2, int d3);
static void set_dat_unif (int x, int y, int d);
static void set_dat (int x, int y, int tl, int tm, int tr, int ml, int mm, int mr,
                 int bl, int bm, int br);
static void set_clm_entry (int num, unsigned int use, int permanent, int lintel,
                     int height, unsigned int solid, int base, int orientation,
                     int c1, int c2, int c3, int c4, int c5, int c6,
                     int c7, int c8);
static void set_clm (int num, unsigned int use,
                 int base, int c1, int c2, 
                 int c3, int c4, int c5, int c6,
                 int c7, int c8);


unsigned char *create_action_point(int x, int y)
{
    unsigned char *ret;
    int i;
    ret = create_thing (x,y);
    for (i=4; i < 17; i++)
      ret[i]=0xff;
    ret[17]=0;
    ret[18]=0;
    for (i=0; i < 1023; i++)
    {
      if (!action_used[i])
      {
          ret[19]=(i+1)&255;
          ret[20]=(i+1)>>8;
          action_used[i]=ret;
          return ret;
      }
    }
    die ("No more available action points");
    return NULL;
}

short is_action_point(int x, int y, int z)
{
    unsigned char *thing;
    thing = lvl->tng_lookup[x][y][z];
    return is_action_thing(thing);
}

unsigned char *create_object(int x, int y, int type)
{
    unsigned char *ret;
    
    ret = create_thing (x, y);
    ret[5]=1; // Default height is floor height
    ret[6]=1;
    ret[7]=type;
    ret[8]=lvl->own[x/3][y/3];
    // Put it in the centre of the square
    ret[0]=0x80;
    ret[2]=0x80;
    return ret;
}

unsigned char *create_thing (int tx, int ty)
{
    unsigned char *ret;
    int i;
    char x, y;
    
    x=(unsigned char)tx;
    y=(unsigned char)ty;
    
    ret = (unsigned char *)malloc(SIZEOF_DK_TNG_REC);
    if (!ret)
      die ("Out of memory.");
    for (i=0; i < SIZEOF_DK_TNG_REC; i++)
      ret[i]=0;
    ret[0]=(unsigned char)((tx%3)*0x40+0x40);
    ret[2]=(unsigned char)((ty%3)*0x40+0x40);
    ret[1]=x;
    ret[3]=y;
    return ret;
}

static void remove_room_things (int x, int y)
{
    int cx, cy, i;
    for (cx=x*3; cx < x*3+3; cx++)
      for (cy=y*3; cy < y*3+3; cy++)
      {
          int last_thing=lvl->tng_subnums[cx][cy]-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=lvl->tng_lookup[cx][cy][i];
            if (is_room_thing(thing))
                thing_del(lvl,cx, cy, i);
          }
     }
}

static void remove_noncrucial_room_things(int x, int y)
{
    int cx, cy, i;
    for (cx=x*3; cx < x*3+3; cx++)
      for (cy=y*3; cy < y*3+3; cy++)
      {
          int last_thing=lvl->tng_subnums[cx][cy]-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=lvl->tng_lookup[cx][cy][i];
            if (is_room_thing(thing)&&
               (!is_crucial_thing(thing)))
                thing_del(lvl,cx, cy, i);
          }
     }
}

void create_clmdattng(void)
{
    int x, y;    
    create_default_clm();
    for (x=0; x < MAP_SIZE_X; x++)
      for (y=0; y < MAP_SIZE_Y; y++)
          update_tngdat(x, y);
}

/*
 * Update a slab and all slabs around it
 */
void update_square (int x, int y)
{
    int i, j;
    for (i=-1; i < 2; i++)
      for (j=-1; j < 2; j++)
          update_tngdat(x+i, y+j);
}

/*
 * Update a single slab
 */
void update_tngdat(int x, int y)
{
    int zx, zy, bx, by, s;
    
    int i, j;
    int slbsame[3][3];
    int slbrel[3][3];
    int ownrel[3][3];
    int surround;
    int flag1, flag2, flag3;
    unsigned char *thing;
    
    if ((x<0) || (x>MAP_MAXINDEX_X) || (y<0) || (y>MAP_MAXINDEX_Y))
      return;
    //FIXME: previous things shouldn't be just removed, they should be updated!
    remove_noncrucial_room_things(x, y);
    s = lvl->slb[x][y];
    if (s < 4)
      lvl->own[x][y]=5; // Dirt/rock can't be claimed
    for (zx=0; zx < MAP_SUBNUM_X; zx++)
    {
      for (zy=0; zy < MAP_SUBNUM_Y; zy++)
      {
          by = y*MAP_SUBNUM_Y+zy;
          bx = x*MAP_SUBNUM_X+zx;
          lvl->wib[bx][by]=slab_default_wbi_entry(s);
      }
    }
    surround=1;
    for (i=0; i<MAP_SUBNUM_X; i++)
    {
      for (j=0; j<MAP_SUBNUM_Y; j++)
      {
          if ((x+i-1>=0) && (x+i-1<MAP_SIZE_X) && (y+j-1>=0) && (y+j-1 < MAP_SIZE_Y))
          {
            slbrel[i][j]=lvl->slb[x+i-1][y+j-1];
            ownrel[i][j]=lvl->own[x+i-1][y+j-1];
          }
          else
          {
            slbrel[i][j]=-1;
            ownrel[i][j]=-1;
          }
          if ( (lvl->slb[x][y]==slbrel[i][j]) && (lvl->own[x][y]==ownrel[i][j]) )
            slbsame[i][j]=1;
          else
          {
            slbsame[i][j]=0;
            surround=0;
          }
      }
    }
    switch (s)
    {
    case SLAB_TYPE_ROCK: // Rock
      set_dat_unif (x, y, 1);
      break;
    case SLAB_TYPE_GOLD: // Gold
      set_dat (x, y, rnd(5)+2, rnd(5)+2, rnd(5)+2, rnd(5)+2,
             rnd(5)+2, rnd(5)+2, rnd(5)+2, rnd(5)+2, rnd(5)+2);
      break;
    case SLAB_TYPE_EARTH: // Earth
      set_dat (x, y, rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10);
      break;
    case SLAB_TYPE_TORCHDIRT: // Torch dirt
      set_dat (x, y, rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10);
      for (i=0; i < 3; i+=2)
      {
          if (slbrel[i][1]>9 && slbrel[i][1] != 52 &&
            (slbrel[i][1]<42 || slbrel[i][1] > 49))
            set_dat2 (x*3+i, y*3+1, 18);
          if (slbrel[1][i]>9 && slbrel[1][i] != 52 &&
            (slbrel[1][i]<42 || slbrel[1][i] > 49))
            set_dat2 (x*3+1, y*3+j, 18);
      }
      break;
    case SLAB_TYPE_DRAPE: // Drape
      set_wall (x, y, 450+lvl->own[x][y]*3, 451+lvl->own[x][y]*3, 
              452+lvl->own[x][y]*3);
      break;
    case SLAB_TYPE_TORCHWALL: // Torch wall
      set_wall (x, y, 219, 220, 221);
      for (i=0; i < 3; i+=2)
      {
          if (slbrel[i][1]>9 && slbrel[i][1] != 52 &&
            (slbrel[i][1]<42 || slbrel[i][1] > 49) &&
            !picture_wall (x, y, i, 1))
            set_dat2 (x*3+i, y*3+1, 208);
          if (slbrel[1][i]>9 && slbrel[1][i] != 52 &&
            (slbrel[1][i]<42 || slbrel[1][i] > 49) &&
            !picture_wall (x, y, 1, i))
            set_dat2 (x*3+1, y*3+i, 208);
      }
      break;
    case SLAB_TYPE_WALLWTWINS: // Wall with twins
      set_wall (x, y, 213, 214, 215);
      break;
    case SLAB_TYPE_WALLWWOMAN: // Wall with woman on
      set_wall (x, y, 216, 217, 218);
      break;
    case SLAB_TYPE_WALLPAIRSHR: // Wall with pair shagging on
      set_wall (x, y, 222, 223, 224);
      break;
    case SLAB_TYPE_PATH: // Path
      set_dat_unif (x, y, 110);
      break;
    case SLAB_TYPE_CLAIMED: // Claimed
      set_dat (x, y, 135,135,135,135,130+lvl->own[x][y],135,135,135,135);
      break;
    case SLAB_TYPE_LAVA: // Lava
      set_dat_unif (x, y, 105);
      break;
    case SLAB_TYPE_WATER: // Water
      set_dat_unif (x, y, 100);
      break;
    case SLAB_TYPE_PORTAL: // Portal
      if (surround)
      {
          set_dat (x, y, 704, 705, 706, 707, 708, 709, 710, 711, 712);
          thing = create_object (x*3+1, y*3+1, 4); // Dry ice
          thing[6]=THING_TYPE_ROOMEFFECT; // Room effect not object!
          thing_add(lvl,thing);
      }
      else
      {
          set_dat_unif (x, y, 700); // Now we only need to add extras
          for (i=-1; i < 2; i+=2)
          {
            for (j=-1; j < 2; j+=2)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if (slab_is_central(lvl,i+x,j+y))
                {
                  set_dat2(bx,  by,  703);
                  set_dat2(bx-i,by,  701);
                  set_dat2(bx,  by-j,701);
                }
            }
          }
          // Put floor on first, *then* roof
          for (i=-1; i < 2; i++)
          {
            for (j=-1; j < 2; j++)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if ((j==0) && (i==0)) // Ignore ourselves
                  continue; 
                if (!i && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat2 (bx-1,by-j, 701);
                  set_dat2 (bx,  by-j, 701);
                  set_dat2 (bx+1,by-j, 701);
                }
                if (!j && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat2 (bx-i, by-1,701);
                  set_dat2 (bx-i, by,  701);
                  set_dat2 (bx-i, by+1,701);
                }
            }                        
          }
          for (i=-1; i < 2; i++)
          {
            for (j=-1; j < 2; j++)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if (!j && !i) // Ignore ourselves
                  continue; 
                if (!i && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat2 (bx-1, by,702);
                  set_dat2 (bx, by,  702);
                  set_dat2 (bx+1, by,702);
                }
                if (!j && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat2 (bx, by-1,702);
                  set_dat2 (bx, by,  702);
                  set_dat2 (bx, by+1,702);
                }
            }                        
          }
      }
      break;
    case SLAB_TYPE_TREASURE: // Treasure
      set_dat_unif (x, y, 500);
      set_pillar_thing (x, y, 501, 0x1c, 0x100);
      break;
    case SLAB_TYPE_LIBRARY: // Library
      if (surround)
          set_dat (x, y, 540,540,540,541,542,543,540,540,540);
      else
      {
          set_dat_unif (x, y, 540);
          set_pillar (x, y, 544);
      }
      break;
    case SLAB_TYPE_PRISONCASE: // Prison case is really grotty
      set_corner (x, y, 580, 581, 582, 583, 584, 585, 586, 587, 588);
      // Sides first
      if ((!slbsame [0][1]) && (slbrel[0][1]>9))
      {
          thing_add(lvl,create_object (x*3, y*3, 0x1b));
          thing_add(lvl,create_object (x*3, y*3+1, 0x1b));
          thing_add(lvl,create_object (x*3, y*3+2, 0x1b));
      }
      if ((!slbsame[2][1]) && (slbrel[2][1]>9))
      {
          thing_add(lvl,create_object (x*3+2, y*3, 0x1b));
          thing_add(lvl,create_object (x*3+2, y*3+1, 0x1b));
          thing_add(lvl,create_object (x*3+2, y*3+2, 0x1b));
      }
      if (!slbsame [1][0] && slbrel[1][0] > 9)
      {
          if (slbsame[0][1] || slbrel[0][1]<9)
            thing_add(lvl,create_object (x*3, y*3, 0x1b));
          thing_add(lvl,create_object (x*3+1, y*3, 0x1b));
          if (slbsame[2][1] || slbrel[2][1]<9)
            thing_add(lvl,create_object (x*3+2, y*3, 0x1b));
      }
      if ((!slbsame[1][2]) && (slbrel[1][2]>9))
      {
          if ((slbsame[0][1]) || (slbrel[0][1]<9))
            thing_add(lvl,create_object (x*3, y*3+2, 0x1b));
          thing_add(lvl,create_object (x*3+1, y*3+2, 0x1b));
          if ((slbsame[2][1]) || (slbrel[2][1]<9))
            thing_add(lvl,create_object (x*3+2, y*3+2, 0x1b));
      }
      break;
    case SLAB_TYPE_TORTURE: // Torture
      if (surround)
      {
          set_dat (x, y, 591, 591, 591, 591, 590,
                 591, 591, 591, 591);
          thing = create_object (x*3+1, y*3+1, 0x7d);
      }
      else
      {
          set_dat (x, y, 591, 591, 591, 591, 590,
                 591, 591, 591, 591);
          set_corner (x, y, 0, 581, 582, 583, 
                  584, 585, 586, 587, 588);
          thing = create_object (x*3+1, y*3+1, 0x20);
      }
      thing_add(lvl,thing);
      break;
    case SLAB_TYPE_TRAINING: // Training
      if (surround)
      {
          set_dat_unif (x, y, 530);
          thing = create_object (x*3+1, y*3+1, 0x1f);
          thing_add(lvl,thing);
      }
      else
      {
          set_corner_unif (x, y, 530, 531);
          set_pillar_thing (x, y, 532, 2, 0x300);
      }
      break;
    case SLAB_TYPE_DUNGHEART: // Dungeon heart - not perfect, 
               // but should be okay most of the time
      flag1=0; // Are we on an edge?
      flag2=0; // Have we got a top left / bottom right corner?
      flag3=0; // Have we got a top right / bottom left corner?
      if (surround)
          set_dat_unif (x, y, 650);
      else
      {
          set_dat_unif (x, y, 651); // Now we only need to add extras
          for (i=-1; i < 2; i++)
          {
            for (j=-1; j < 2; j++)
            {
                if ((!j && !i) || (i && j)) // Ignore ourselves and corners
                  continue; 
                bx = x*3+i+1;
                by = y*3+j+1;
                if (slab_is_central(lvl,i+x,j+y))
                {
                  flag1=1;
                  set_dat2 (bx, by, 656);
                  set_dat2 (bx-i, by-j, 655);
                  if (!i)
                  {
                      set_dat2 (bx-1, by, 656);      
                      set_dat2 (bx+1, by, 656);
                      set_dat2 (bx-1, by-j, 655);
                      set_dat2 (bx+1, by-j, 655);
                  }
                  if (!j)
                  {
                      set_dat2 (bx, by-1, 656);      
                      set_dat2 (bx, by+1, 656);
                      set_dat2 (bx-i, by-1, 655);
                      set_dat2 (bx-i, by+1, 655);
                  }
                }
            }
          }
          if (!flag1) // Not on any edges
          {
            for (i=-1; i < 2; i+=2)
            {
                for (j=-1; j < 2; j+=2)
                {
                  bx = x*3+i+1;
                  by = y*3+j+1;
                  if (slab_is_central(lvl,x+i,y+j))
                  {
                      if (i==j)
                        flag2=1;
                      else
                        flag3=1;
                      set_dat2 (bx, by, 657+lvl->own[x][y]);
                      set_dat2 (bx-i, by, 654);
                      set_dat2 (bx, by-j, 654);
                  }
                }
            }
            if (flag2 && !flag3)
            {
                set_dat2 (x*3+2, y*3, 652);
                set_dat2 (x*3, y*3+2, 652);
            }
            if (flag3 && !flag2)
            {
                set_dat2 (x*3, y*3, 652);
                set_dat2 (x*3+2, y*3+2, 652);
            }
            if (flag2 || flag3)
            {
                char *obj=create_object 
                        (x*3+1, y*3+1,(lvl->own[x][y]>0 && lvl->own[x][y]<4 ? 
                                   0x77+lvl->own[x][y] : 0x6f));
                thing_add(lvl,obj);
                set_dat2 (x*3+1, y*3+1, 653);
            }
          }
      }
      break;
    case SLAB_TYPE_WORKSHOP: // Workshop 
      if (surround)
      {
          thing = create_object (x*3+1, y*3+1, 0x72);
          thing_add(lvl,thing);
          set_dat (x, y, 561, 561, 561, 561, 
                 560, 561, 561, 562, 561);
      }
      else
      {
          set_dat_unif (x, y, 560);
          set_pillar_thing (x, y, 563, 0x1a, 0x100);
      }
      break;
    case SLAB_TYPE_SCAVENGER: // Scavenger room
      set_dat (x, y, 641, 641, 641, 641, 640,
             641, 641, 641, 641);
      if (surround)
      {
          thing = create_object (x*3+1, y*3+1, 0x71);
          thing_add(lvl,thing);
      }
      else
          set_pillar_thing (x, y, 642, 2, 0x300);
      break;
    case SLAB_TYPE_TEMPLE: // Temple
      if (surround)
          set_dat (x, y, 612, 613, 614, 615, 616, 617, 618, 619, 620);
      else
      {
          set_dat_unif (x, y, 610); // Now we only need to add extras
          for (i=-1; i < 2; i+=2)
          {
            for (j=-1; j < 2; j+=2)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if (slab_is_central(lvl,i+x,j+y))
                {
                  set_dat2 (bx, by, 622);
                  set_dat2 (bx-i, by-j, 611);
                }
            }
          }
          for (i=-1; i < 2; i++)
          {
            for (j=-1; j < 2; j++)
            {
                if (!j && !i) // Ignore ourselves
                  continue; 
                bx = x*3+i+1;
                by = y*3+j+1;
                
                if (!i && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat2 (bx-i, by-j, 610);
                  set_dat2 (bx-1, by, 621);
                  set_dat2 (bx, by, 621);
                  set_dat2 (bx+1, by, 621);
                }
                if (!j && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat2 (bx-i, by-j, 610);
                  set_dat2 (bx, by-1, 621);
                  set_dat2 (bx, by, 621);
                  set_dat2 (bx, by+1, 621);
                }
            }                        
          }
          set_pillar_thing (x, y, 0, 4, 0x200);
      }
      break;
    case SLAB_TYPE_GRAVEYARD: // Graveyard
      set_corner (x, y, 630, 631, 632, 633, 634, 635, 636, 637, 638);
      thing = create_object (x*3+1, y*3+1, 0x1d);
      thing_add(lvl,thing);
      // Add dry ice instead of pillars
      for (i=0; i < 3; i+=2)
      {
          for(j=0; j < 3; j+=2)
          {
            if (slbsame[i][1] && slbsame[1][j] &&
                !slbsame[2-i][1] && !slbsame[1][2-j])
            {
                thing = create_object (x*3+i, y*3+j, 4); // Dry ice
                thing[6]=7; // Room effect not object!
                thing_add(lvl,thing);
            }
          }
      }
      break;
    case SLAB_TYPE_HATCHERY: // Hatchery
      set_corner_unif (x, y, 520, 521);
      set_pillar_thing (x, y, 522, 2, 0x320);
      break;
    case SLAB_TYPE_LAIR: // Lair
      set_corner (x, y, 510, 511, 512, 513, 514,
                515, 516, 517, 518);
      break;
    case SLAB_TYPE_BARRACKS: // Barracks
      set_dat_unif (x, y, 600);
      set_pillar_thing (x, y, 601, 2, 0x320);
      break;
    case SLAB_TYPE_DOORWOOD1:// Wooden door
    case SLAB_TYPE_DOORWOOD2: 
      set_door_dattng (x, y, 721, 1, s&1);
      break;
    case SLAB_TYPE_DOORBRACE1:// Braced door
    case SLAB_TYPE_DOORBRACE2: 
      set_door_dattng (x, y, 724, 2, s&1);
      break;
    case SLAB_TYPE_DOORIRON1:// Iron door
    case SLAB_TYPE_DOORIRON2: 
      set_door_dattng (x, y, 727, 3, s&1);
      break;
    case SLAB_TYPE_DOORMAGIC1:// Magic door
    case SLAB_TYPE_DOORMAGIC2: 
      set_door_dattng (x, y, 730, 4, s&1);
      break;
    case SLAB_TYPE_BRIDGE:// Bridge
      set_dat (x, y, 551, 552, 550, 552, 552, 552, 550, 552, 551);
      break;
    case SLAB_TYPE_GEMS:// Gems
      set_dat (x, y, rnd(9)+20, rnd(9)+20, rnd(9)+20, 
             rnd(9)+20, rnd(9)+20, rnd(9)+20, 
             rnd(9)+20, rnd(9)+20, rnd(9)+20);
      break;
    case SLAB_TYPE_GUARDPOST: // Guardpost
      set_dat (x, y, 571, 572, 570, 572, 572, 572, 570, 572, 571);
      // Put the flagpost on for keepers
      if (lvl->own[x][y] < 4)
      {
          thing = create_object (x*3+1, y*3+1, 0x73+lvl->own[x][y]);
          thing[5]=2;
          thing_add(lvl,thing);
      }
      break;
    }
    // Add torches to the room if it's an appropriate type
    if (s > 9 && !(s>=42 && s <=49) && s != 52)
    {
      for (i=0; i < 3; i+=2)
      {
          if ((slbrel[i][1]==3 || slbrel[i][1]==5)
            && !picture_wall (x+i-1, y, 2-i, 1))
          {
            thing = create_object (x*3+i, y*3+1, 2);
            thing[5]=2;
            thing[4]=0xe0;
            thing[0]=i*0x40+0x40;
            thing[2]=0x80;
            thing_add(lvl,thing);
          }
          if ((slbrel[1][i]==3 || slbrel[1][i]==5)
            && !picture_wall (x, y+i-1, 1, 2-i))
          {
            thing = create_object (x*3+1, y*3+i, 2);
            thing[5]=2;
            thing[4]=0xe0;
            thing[0]=0x80;
            thing[2]=i*0x40+0x40;
            thing_add(lvl,thing);
          }
      }
    }
}

static int picture_wall (int x, int y, int dx, int dy)
{
    static int offset[54]={0,0,0,0,0,0,0,0,0,0, // 0-9
      0,0,0,0,0,0,225,0,285,0, // 10-19
      330,0,345,0,270,0,0,0,300,0, // 20-29
      405,0,375,0,390,0,255,0,240,0, // 30-39
      360,0,0,0,0,0,0,0,0,0, // 40-49
      0,0,0,0}; // 50-53
    static int offset2[54]={0,0,0,0,0,0,0,0,0,0, // 0-9
      0,0,0,0,0,0,800,0,860,0, // 10-19
      905,0,920,0,845,0,0,0,875,0, // 20-29
      980,0,950,0,965,0,830,0,815,0, // 30-39
      935,0,0,0,0,0,0,0,0,0, // 40-49
      0,0,0,0}; // 50-53
    int i, j;
    int slbrel[3][3];
    int ownrel[3][3];

    int s, o;
    
    for (i=0; i < 3; i++)
    {
      for (j=0; j < 3; j++)
      {
          if (x+i-1>=0 && x+i-1 < MAP_SIZE_X && y+j-1>=0 && y+j-1 < MAP_SIZE_Y)
          {
            slbrel[i][j]=lvl->slb[x+i-1][y+j-1];
            ownrel[i][j]=lvl->own[x+i-1][y+j-1];
          }
          else
          {
            slbrel[i][j]=-1;
            ownrel[i][j]=-1;
          }
      }
    }
    s = slbrel[dx][dy];
    if (s < 9 || s > 53)
      return 0;
    if (!offset[s] && !offset2[s])
      return 0;
    o = ownrel[dx][dy];
    
    // Check we're a wall
    if (slbrel[1][1]<4 || slbrel[1][1]>8)
      return 0;
    // Check we have walls on both sides of us
    if (slbrel[2-dy][2-dx]>8 ||
        slbrel[dy][dx]>8)
      return offset2[s]+o*3;
    // Check we have the same room near us
    if (ownrel[dx-dy+1][dy+dx-1] != o ||
      ownrel[dx+dy-1][dy-dx+1] != o ||
      slbrel[dx-dy+1][dy+dx-1] != s ||
      slbrel[dx+dy-1][dy-dx+1] != s)
      return offset2[s]+o*3;
    return offset[s]+o*3;
}
      
static void set_door_dattng (int x, int y, int datbase, int type, int locked)
{
    int o;
    int cx, cy, i;
    unsigned char *thing;
    
    // Remove door things
    for (cx=x*3; cx < x*3+3; cx++)
      for (cy=y*3; cy < y*3+3; cy++)
          for (i=lvl->tng_subnums[cx][cy]-1; i >=0; i--)
            if (lvl->tng_lookup[cx][cy][i][6]==9)
                thing_del(lvl,cx, cy, i);
    set_dat_unif (x, y, 720);
    // Set orientation of door, fairly arbitrarily :)
    if ((!x || lvl->slb[x-1][y]<10)&&(x==MAP_MAXINDEX_X || lvl->slb[x+1][y]<10))
      o=0;
    else
      o=1;
    thing = create_object (x*3+1, y*3+1, type);
    thing[6]=9; // Make it a door
    thing[13]=o;
    thing[14]=locked;
    thing_add(lvl,thing);
    if (locked)
    {
      thing = create_object (x*3+1, y*3+1, 0x2c);
      thing[5]=5;
      thing_add(lvl,thing);
    }
    set_dat2 (x*3+o, y*3+1-o, datbase);
    set_dat2 (x*3+1, y*3+1, datbase+1);
    set_dat2 (x*3+2-o, y*3+1+o, datbase+2);
}

/*
 * Create the standard clm file. Entries structured as follows:
 */
void create_default_clm(void)
{
    if (!default_clm)
      default_clm=1;
    else
      return;
    // 0-99: Rocks etc
    default_clm=1;
    set_clm (1, 0x8c5d, 0x1d, 0x2d, 0x2d, 0x2c, 0x2c, 0x2b, 0, 0, 0); // Rock
    set_clm (2, 0xb, 0x1b, 0x19, 0xa, 0x31, 0x33, 0x32, 0, 0, 0); // Gold
    set_clm (3, 0xb, 0x1b, 0x1a, 0xa, 0x32, 0x33, 0x31, 0, 0, 0);
    set_clm (4, 0xb, 0x1b, 0x1b, 0xa, 0x33, 0x31, 0x32, 0, 0, 0);
    set_clm (5, 0xb, 0x1b, 0x1c, 0xa, 0x31, 0x31, 0x33, 0, 0, 0);
    set_clm (6, 0xb, 0x1b, 0x1d, 0xa, 0x33, 0x32, 0x31, 0, 0, 0);
    set_clm (10, 0x177e, 0x1b, 0x1d, 0xa, 2, 2, 5, 0, 0, 0); // Earth
    set_clm (11, 0x1819, 0x1b, 0x1c, 0xa, 1, 2, 5, 0, 0, 0); 
    set_clm (12, 0x2f56, 0x1b, 0x1a, 0xa, 3, 1, 5, 0, 0, 0); 
    set_clm (13, 0x2f02, 0x1b, 0x1b, 0xa, 3, 1, 5, 0, 0, 0); 
    set_clm (14, 0x3b35, 0x1b, 0x19, 0xa, 2, 2, 5, 0, 0, 0); 
    set_clm (15, 0x2fa5, 0x1b, 0x1b, 0xa, 3, 2, 5, 0, 0, 0); 
    set_clm (16, 0x2f91, 0x1b, 0x1b, 0xa, 1, 2, 5, 0, 0, 0); 
    set_clm (17, 0x2f30, 0x1b, 0x1b, 0xa, 2, 2, 5, 0, 0, 0);
    set_clm (18, 0xd, 0x1b, 0x19, 0xa, 1, 0x18, 5, 0, 0, 0); // Torchplate
    set_clm (20, 1, 0x1b, 0x19, 0xa, 0x1b9, 0x1ba, 0x1bb, 0, 0, 0); // Gems
    set_clm (21, 1, 0x1b, 0x1a, 0xa, 0x1ba, 0x1bb, 0x1b9, 0, 0, 0);
    set_clm (22, 1, 0x1b, 0x1b, 0xa, 0x1b9, 0x1b9, 0x1bc, 0, 0, 0);
    set_clm (23, 1, 0x1b, 0x1c, 0xa, 0x1bb, 0x1bc, 0x1bb, 0, 0, 0);
    set_clm (24, 1, 0x1b, 0x19, 0xa, 0x1bc, 0x1ba, 0x1bc, 0, 0, 0);
    set_clm (25, 1, 0x1b, 0x1a, 0xa, 0x1b9, 0x1bb, 0x1ba, 0, 0, 0);
    set_clm (26, 1, 0x1b, 0x1b, 0xa, 0x1ba, 0x1ba, 0x1bb, 0, 0, 0);
    set_clm (27, 1, 0x1b, 0x1c, 0xa, 0x1bb, 0x1bc, 0x1ba, 0, 0, 0);
    set_clm (28, 1, 0x1b, 0x1d, 0xa, 0x1b9, 0x1b9, 0x1b9, 0, 0, 0);

    // 100-199: Water, lava, paths, claimed areas
    set_clm (100, 0xcea8, 0x0221, 0, 0, 0, 0, 0, 0, 0, 0); // Water 1
    set_clm (105, 0x18ba, 0x0222, 0, 0, 0, 0, 0, 0, 0, 0); // Lava 1
    set_clm (106, 0x18ba, 0x0222, 0, 0, 0, 0, 0, 0, 0, 0); // Lava 2
    set_clm (110, 0x0594, 0x00cf, 0x1a, 0, 0, 0, 0, 0, 0, 0); // Path
    // Expand this later :) FIXME
    // Claimed areas
    set_clm (130, 0x0045, 0x00ce, 0x00c0, 0, 0, 0, 0, 0, 0, 0); // Player 0 centre
    set_clm (131, 0x0012, 0x00ce, 0x00c1, 0, 0, 0, 0, 0, 0, 0); // Player 1 centre
    set_clm (132, 0x0010, 0x00ce, 0x00c2, 0, 0, 0, 0, 0, 0, 0); // Player 2 centre
    set_clm (133, 0x0010, 0x00ce, 0x00c3, 0, 0, 0, 0, 0, 0, 0); // Player 3 centre
    set_clm (134, 0x0184, 0x00ce, 0x00c7, 0, 0, 0, 0, 0, 0, 0); // Player 4 centre
    // Fix this properly later FIXME
    set_clm (135, 1, 0xce, 0x80, 0, 0, 0, 0, 0, 0, 0); // Non-centres

    // 200-499: Walls
    set_clm (200, 0x23, 0x1b, 0x1c, 0x98, 0x97, 0x96, 0x95, 0, 0, 0); // Corner
    set_clm (202, 0x934, 0x1b, 0x19, 0xa, 2, 2, 0x43, 0, 0, 0); // Centres
    set_clm (203, 0x18, 0x1b, 0x19, 0xa, 2, 2, 0x44, 0, 0, 0);
    set_clm (204, 0x18, 0x1b, 0x19, 0xa, 2, 2, 0x45, 0, 0, 0);
    set_clm (205, 0x18, 0x1b, 0x19, 0xa, 2, 2, 0x46, 0, 0, 0);
    set_clm (206, 0x21c, 0x1b, 0x19, 0xa, 2, 2, 0x47, 0, 0, 0);
    set_clm (207, 0x23, 0x1b, 0x19, 0xa, 2, 2, 4, 0, 0, 0);

    // Torch plate
    set_clm (208, 3, 0x1b, 0x1a, 0x52, 0x48, 0x77, 0x4d, 0, 0, 0);
    
    // For non-corner bits of corner rooms :)
    set_clm (209, 4, 0x1b, 0x1a, 0x52, 0x49, 0x49, 0x4e, 0, 0, 0);

    // Twins
    set_clm (213, 1, 0x1b, 0x1c, 0x195, 0x66, 0x63, 0x60, 0, 0, 0);
    set_clm (214, 4, 0x1b, 0x1a, 0x196, 0x67, 0x64, 0x61, 0, 0, 0);
    set_clm (215, 1, 0x1b, 0x1b, 0x197, 0x68, 0x65, 0x62, 0, 0, 0);

    // Woman
    set_clm (216, 0x1c, 0x1b, 0x19, 0x5d, 0x5a, 0x57, 0x54, 0, 0, 0);
    set_clm (217, 0x1c, 0x1b, 0x1b, 0x5e, 0x5b, 0x58, 0x55, 0, 0, 0);
    set_clm (218, 0x1c, 0x1b, 0x1c, 0x5f, 0x5c, 0x59, 0x56, 0, 0, 0);
    
    // Blank wall
    set_clm (219, 7, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x4e, 0, 0, 0);
    set_clm (220, 7, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x4e, 0, 0, 0);
    set_clm (221, 7, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x4e, 0, 0, 0);
    
    // Shagging wall
    set_clm (222, 0xf, 0x1d, 0x1d, 0x72, 0x6f, 0x6c, 0x69, 0, 0, 0);
    set_clm (223, 0xb, 0x1a, 0x1d, 0x73, 0x70, 0x6d, 0x6a, 0, 0, 0);
    set_clm (224, 0x1f, 0x1b, 0x1d, 0x74, 0x71, 0x6e, 0x6b, 0, 0, 0);
    
    // Treasure wall
    set_room_wall (225, 0xa, 0x1b, 0x19, 0x52, 0x9c, 0x99, 2);
    set_room_wall (800, 0x8f, 0x1b, 0x19, 0x52, 0x49, 0x49, 4);
    
    // Lair wall
    set_clm (240, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm (241, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm (242, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    set_clm (243, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm (244, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm (245, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    set_clm (246, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm (247, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm (248, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    set_clm (249, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm (250, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm (251, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    set_clm (252, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm (253, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm (254, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    
    set_clm (815, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0xa0, 0, 0, 0);
    set_clm (816, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0xa1, 0, 0, 0);
    set_clm (817, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0xa2, 0, 0, 0);
    set_clm (818, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x19a, 0, 0, 0);
    set_clm (819, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x19b, 0, 0, 0);
    set_clm (820, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0x19c, 0, 0, 0);
    set_clm (821, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x19d, 0, 0, 0);
    set_clm (822, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x19e, 0, 0, 0);
    set_clm (823, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0x19f, 0, 0, 0);
    set_clm (824, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x1a0, 0, 0, 0);
    set_clm (825, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x1a1, 0, 0, 0);
    set_clm (826, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0x1a2, 0, 0, 0);
    set_clm (827, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x1a3, 0, 0, 0);
    set_clm (828, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x1a2, 0, 0, 0);
    set_clm (829, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0x1a5, 0, 0, 0);

    // Hatchery wall
    set_room_wall (255, 8, 0x1b, 0x19, 0x155, 0x152, 0x14f, 1);
    set_room_wall4 (830, 6, 0x1b, 0x19, 0x15b, 0x158, 0x158);

    // Training room wall
    set_clm (270, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0xa0, 0, 0, 0);
    set_clm (271, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0xa1, 0, 0, 0);
    set_clm (272, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0xa2, 0, 0, 0);
    set_clm (273, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0x19a, 0, 0, 0);
    set_clm (274, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0x19b, 0, 0, 0);
    set_clm (275, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0x19c, 0, 0, 0);
    set_clm (276, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0x19d, 0, 0, 0);
    set_clm (277, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0x19e, 0, 0, 0);
    set_clm (278, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0x19f, 0, 0, 0);
    set_clm (279, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0x1a0, 0, 0, 0);
    set_clm (280, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0x1a1, 0, 0, 0);
    set_clm (281, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0x1a2, 0, 0, 0);
    set_clm (282, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0x1a3, 0, 0, 0);
    set_clm (283, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0x1a4, 0, 0, 0);
    set_clm (284, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0x1a5, 0, 0, 0);

    set_room_wall4 (845, 7, 0x1b, 0x19, 0xfb, 0xfa, 0xf9);

    // Library wall
    set_room_wall (285, 7, 0x1b, 0x19, 0x52, 0xab, 0xa8, 2);
    set_room_wall (860, 6, 0x1b, 0x15, 0x52, 0xb3, 0xb0, 2);
    // Workshop wall

    set_room_wall3 (300, 1, 0x1b, 0x1b, 0x52, 0x102, 0xff, 0x4d, 2);
    set_room_wall4 (875, 7, 0x1b, 0x19, 0x10e, 0x10d, 0x10c);
    
    // Leave room for guardpost wall

    // Prison wall

    set_room_wall3 (330, 3, 0x1b, 0x1c, 0x52, 0xe4, 0xe1, 0x4d, 2);
    set_room_wall4 (905, 0xc, 0x1b, 0x19, 0xbc, 0xbd, 0xbc);
    
    // Torture wall

    set_room_wall2 (345, 7, 0x1b, 0x1b, 0x52, 0xee, 0xeb, 0xe8, 2);
    set_room_wall2 (920, 7, 0x1b, 0x1b, 0x52, 0xee, 0xeb, 0xe8, 2);

    // Barracks wall
    set_room_wall2 (360, 7, 0x1b, 0x1b, 0x52, 0x18b, 0x188, 0x185, 2);
    set_room_wall4 (935, 0xc, 0x1b, 0x184, 0x184, 0x190, 0x184);
    
    // Temple wall

    set_room_wall2 (375, 0xa, 0x1b, 0x19, 0x138, 0x135, 0x132, 0x12f, 1);

    set_clm (950, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0xa0, 0, 0, 0);
    set_clm (951, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0xa1, 0, 0, 0);
    set_clm (952, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0xa2, 0, 0, 0);
    set_clm (953, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0x19a, 0, 0, 0);
    set_clm (954, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0x19b, 0, 0, 0);
    set_clm (955, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0x19c, 0, 0, 0);
    set_clm (956, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0x19d, 0, 0, 0);
    set_clm (957, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0x19e, 0, 0, 0);
    set_clm (958, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0x19f, 0, 0, 0);
    set_clm (959, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0x1a0, 0, 0, 0);
    set_clm (960, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0x1a1, 0, 0, 0);
    set_clm (961, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0x1a2, 0, 0, 0);
    set_clm (962, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0x1a3, 0, 0, 0);
    set_clm (963, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0x1a4, 0, 0, 0);
    set_clm (964, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0x1a5, 0, 0, 0);

    // Graveyard wall

    set_clm (390, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0xa0, 0, 0, 0);
    set_clm (391, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0xa1, 0, 0, 0);
    set_clm (392, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0xa2, 0, 0, 0);
    set_clm (393, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x19a, 0, 0, 0);
    set_clm (394, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x19b, 0, 0, 0);
    set_clm (395, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x19c, 0, 0, 0);
    set_clm (396, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x19d, 0, 0, 0);
    set_clm (397, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x19e, 0, 0, 0);
    set_clm (398, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x19f, 0, 0, 0);
    set_clm (399, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x1a0, 0, 0, 0);
    set_clm (400, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x1a1, 0, 0, 0);
    set_clm (401, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x1a2, 0, 0, 0);
    set_clm (402, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x1a3, 0, 0, 0);
    set_clm (403, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x1a4, 0, 0, 0);
    set_clm (404, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x1a5, 0, 0, 0);

    set_clm (965, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0xa0, 0, 0, 0);
    set_clm (966, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0xa1, 0, 0, 0);
    set_clm (967, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0xa2, 0, 0, 0);
    set_clm (968, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x19a, 0, 0, 0);
    set_clm (969, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x19b, 0, 0, 0);
    set_clm (970, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x19c, 0, 0, 0);
    set_clm (971, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x19d, 0, 0, 0);
    set_clm (972, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x19e, 0, 0, 0);
    set_clm (973, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x19f, 0, 0, 0);
    set_clm (974, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x1a0, 0, 0, 0);
    set_clm (975, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x1a1, 0, 0, 0);
    set_clm (976, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x1a2, 0, 0, 0);
    set_clm (977, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x1a3, 0, 0, 0);
    set_clm (978, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x1a4, 0, 0, 0);
    set_clm (979, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x1a5, 0, 0, 0);
    
    // Scavenger wall

    set_room_wall (405, 4, 0x1b, 0x19, 0x52, 0x112, 0x10f, 2);
    set_room_wall4 (980, 6, 0x1b, 0x19, 0x118, 0x117, 0x116);
    
    
    // Drapes
    set_clm (450, 0xf, 0x1b, 0x19, 0x52, 0x49, 0x49, 0xa0, 0, 0, 0);
    set_clm (451, 0xf, 0x1b, 0x19, 0x52, 0x49, 0x49, 0xa1, 0, 0, 0);
    set_clm (452, 0x10, 0x1b, 0x19, 0x52, 0x49, 0x49, 0xa2, 0, 0, 0);
    
    set_clm (453, 0x12, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19a, 0, 0, 0);
    set_clm (454, 0xd, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19b, 0, 0, 0);
    set_clm (455, 0x12, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19c, 0, 0, 0);
    
    set_clm (456, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19d, 0, 0, 0);
    set_clm (457, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19e, 0, 0, 0);
    set_clm (458, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19f, 0, 0, 0);
    
    set_clm (459, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a0, 0, 0, 0);
    set_clm (460, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a1, 0, 0, 0);
    set_clm (461, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a2, 0, 0, 0);

    set_clm (462, 0x56, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a3, 0, 0, 0);
    set_clm (463, 0x55, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a4, 0, 0, 0);
    set_clm (464, 0x56, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a5, 0, 0, 0);

    finish_clm(); // Put this in to avoid a stack fault
}

static void finish_clm (void)
{
    // 500-799: Rooms
    set_clm (500, 0xa0, 0x6a, 0x9f, 0, 0, 0, 0, 0, 0, 0); // Treasure
    set_clm (501, 0xc, 0x6a, 0x9f, 0x3e, 
           0x39, 0x3a, 0x39, 0x41, 0, 0); // Pillar
    set_clm (502, 0xc, 0x6a, 0x9f, 0x3e, 
           0x39, 0x3a, 0x41, 0, 0, 0); // Corner
    set_clm (510, 0xfffc, 0x1b, 0x1b1, 0, 0, 0, 0, 0, 0, 0); // Lair
    set_clm (511, 0xfffd, 0x1b, 0x1e, 0, 0, 0, 0, 0, 0, 0); // Border
    set_clm (512, 0xfff5, 0x1b, 0x1f, 0, 0, 0, 0, 0, 0, 0);
    set_clm (513, 0xfff9, 0x1b, 0x20, 0, 0, 0, 0, 0, 0, 0);
    set_clm (514, 0xfffa, 0x1b, 0x21, 0, 0, 0, 0, 0, 0, 0);
    set_clm (515, 0xfff4, 0x1b, 0x22, 0, 0, 0, 0, 0, 0, 0);
    set_clm (516, 0xfffa, 0x1b, 0x23, 0, 0, 0, 0, 0, 0, 0);
    set_clm (517, 0xffef, 0x1b, 0x24, 0, 0, 0, 0, 0, 0, 0);
    set_clm (518, 0xfffb, 0x1b, 0x25, 0, 0, 0, 0, 0, 0, 0);
 
    
    set_clm (520, 0x24a, 0x1b, 0, 0, 0, 0, 0, 0, 0, 0); // Hatchery
    set_clm (521, 0xfd42, 0x1b, 0x15a, 0, 0, 0, 0, 0, 0, 0); // Border
    set_clm (522, 0xffbc, 0x1b, 0x15b, 0x158, 0x158, 0x158,
           0x158, 0x159, 0, 0); // Pillar
    set_clm (523, 0xffbc, 0x1b, 0x15b, 0x158, 0x158, 0x158,
           0x159, 0, 0, 0); // Pillar
    set_clm (530, 0xff05, 0x13e, 0xfc, 0, 0, 0, 0, 0, 0, 0); // Training
    set_clm (531, 0x23, 0x13e, 0xfe, 0, 0, 0,0, 0, 0, 0); // Border
    set_clm (532, 5, 0x13e, 0xfc, 0xfb, 0xfa, 0xf9,
           0xf9, 0xf1, 0, 0); // Pillar
    set_clm (533, 5, 0x13e, 0xfc, 0xfb, 0xfa, 0xf9,
           0xf1, 0, 0, 0); // Pillar
    set_clm (540, 0xa2, 0xf8, 0xae, 0, 0, 0, 0, 0, 0, 0); // Library
    set_clm (541, 4, 0x1b, 0xae, 0x14b, 0x14d, 0x14b, 0, 0, 0, 0); // left
    set_clm (542, 5, 0x1b, 0xae, 0xb1, 0xb4, 0xb1, 0, 0, 0, 0); // middle
    set_clm (543, 2, 0x1b, 0xaf, 0x14c, 0x14e, 0x14c, 0, 0, 0, 0); // right
    set_clm (544, 1, 0x1b, 0x1a, 0xb9, 0xb8, 0xb7, 0xb6, 0, 0, 0); // Pillar/corner
    set_clm (550, 3, 0x221, 0x1b4, 0, 0, 0, 0, 0, 0, 0); // Bridge
    set_clm (551, 2, 0x221, 0x1b5, 0, 0, 0, 0, 0, 0, 0);
    set_clm (552, 3, 0x221, 0x1b6, 0, 0, 0, 0, 0, 0, 0);
    set_clm (560, 0xffcb, 0x15a, 0x107, 0, 0, 0, 0, 0, 0, 0); // Workshop centre/edge
    set_clm (561, 0xfe8c, 0x15a, 0x109, 0, 0, 0, 0, 0, 0, 0); // Non-centre most
    set_clm (562, 0x1b, 0x15a, 0x108, 0, 0, 0, 0, 0, 0, 0); // Non-centre bottom
    set_clm (563, 0x22, 0x1b, 0x1b, 0x10e, 0x10d, 0x10c,
           0x10b, 0, 0, 0); // Pillar
    set_clm (570, 3, 0xb2, 0x80, 0x1b4, 0, 0, 0, 0, 0, 0); // Guardpost FIXME?
    set_clm (571, 2, 0xb0, 0x80, 0x1b5, 0, 0, 0, 0, 0, 0); 
    set_clm (572, 3, 0xb0, 0x80, 0x1b6, 0, 0, 0, 0, 0, 0); 
    set_clm (580, 0xfff5, 0x118, 0xc8, 0, 0, 0, 0, 0, 0, 0); // Prison
    set_clm (581, 0xfff8, 0x118, 0xc9, 0, 0, 0, 0, 0, 0, 0); // Borders
    set_clm (582, 0xfff1, 0x118, 0xca, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (583, 0xfff3, 0x118, 0xcb, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (584, 0xfff0, 0x118, 0xcc, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (585, 0xffed, 0x118, 0xcd, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (586, 0xfff3, 0x118, 0xce, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (587, 0xffec, 0x118, 0xcf, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (588, 0xfff1, 0x118, 0xd0, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (589, 0xc, 0x118, 0x19, 0xbc, 0xbd, 0xbc, 0xbc, 0, 0, 0);
    set_clm (590, 0x19, 0x118, 0xe7, 0, 0, 0, 0, 0, 0, 0); // Torture
    set_clm (591, 0xfff5, 0x118, 0xc8, 0, 0, 0, 0, 0, 0, 0); // Non-centre
    set_clm (592, 0xfff8, 0x118, 0xc9, 0, 0, 0, 0, 0, 0 ,0); // Border
    // Rest of borders are as prison
    set_clm (600, 1, 0x1b, 0x183, 0, 0, 0, 0, 0, 0, 0); // Barracks
    set_clm (601, 0x22, 0x1b, 0x1b, 0x184, 0x184, 0x18f,
           0x18e, 0x184, 0, 0); // Pillar
    set_clm (602, 0x22, 0x1b, 0x1b, 0x184, 0x18f,
           0x18e, 0x184, 0, 0, 0); // Pillar
    // Temple
    set_clm (610, 0x56, 0x177, 0x1b, 0x123, 0, 0, 0, 0, 0, 0); // Edge
    set_clm (611, 8, 0x177, 0x1b, 0x122, 0, 0, 0, 0, 0, 0); // Edge corner centre
    set_clm (612, 1, 0x177, 0x1b, 0x126, 0, 0, 0, 0, 0, 0); // Pool top left
    set_clm (613, 2, 0x177, 0x1b, 0x127, 0, 0, 0, 0, 0, 0); // Pool top mid
    set_clm (614, 1, 0x177, 0x1b, 0x128, 0, 0, 0, 0, 0, 0); // Pool top right
    set_clm (615, 1, 0x177, 0x1b, 0x129, 0, 0, 0, 0, 0, 0); // Pool mid left
    set_clm (616, 1, 0x177, 0x1b, 0x12a, 0, 0, 0, 0, 0, 0); // Pool mid mid
    set_clm (617, 1, 0x177, 0x1b, 0x12b, 0, 0, 0, 0, 0, 0); // Pool mid right
    set_clm (618, 1, 0x177, 0x1b, 0x12c, 0, 0, 0, 0, 0, 0); // Pool bot left
    set_clm (619, 1, 0x177, 0x1b, 0x12d, 0, 0, 0, 0, 0, 0); // Pool bot mid
    set_clm (620, 2, 0x177, 0x1b, 0x12e, 0, 0, 0, 0, 0, 0); // Pool bot right
    set_clm (621, 6, 0x177, 0x1b, 0x123, 0x122, 0, 0, 0, 0, 0); // Pool wall
    set_clm (622, 6, 0x177, 0x1b, 0x122, // Column
           0x122, 0x120, 0x11d, 0x120, 0x11e, 0x11d);
    set_clm (623, 6, 0x177, 0x1b, // Corner
           0x122, 0x120, 0x11d, 0x120, 0x11e, 0x11d, 0);
    
    set_clm (630, 0x4d, 0x1c, 0x142, 0, 0, 0, 0, 0, 0, 0); // Graveyard
    set_clm (631, 2, 0x1c, 0x143, 0, 0, 0, 0, 0, 0, 0); // Border
    set_clm (632, 9, 0x1c, 0x144, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (633, 0, 0x1c, 0x145, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (634, 8, 0x1c, 0x146, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (635, 1, 0x1c, 0x147, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (636, 0xffff, 0x1c, 0x148, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (637, 0, 0x1c, 0x149, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (638, 0, 0x1c, 0x14a, 0, 0, 0, 0, 0, 0, 0); 
    set_clm (639, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x95, 0, 0, 0);
    set_clm (640, 0xf, 0xf8, 0x11c, 0, 0, 0, 0, 0, 0, 0); // Scavenger
    set_clm (641, 0x62, 0xf8, 0x119, 0, 0, 0, 0, 0, 0, 0); // off-centre
    set_clm (642, 0xc, 0xf8, 0x119, 0x118, 0x117, 0x116, // Pillar
           0x115, 0, 0, 0);
    
    // Dungeon heart
    set_clm (650, 0xdc, 0x1b, 0x1b, 0x52, 0x9f, 0, 0, 0, 0, 0); // Centre tiles
    set_clm (651, 0xff7e, 0x201, 0xa4, 0, 0, 0, 0, 0, 0, 0); // Very edge
    set_clm (652, 0xffb8, 0x1b, 0xa4, 0x52, 0x4d, 0,0,0,0,0); // Corner strange bits
    set_clm (653, 0x17, 0x1b, 0x80, 0x180, 0, 0, 0, 0, 0, 0); // Outside corners
    set_clm (654, 0xfd4f, 0x1b, 0x9f, 0x52, 0x4d, 0, 0, 0, 0, 0); // By above
    set_clm (655, 0x22, 0x1b, 0x1c, 0x9f, 0, 0, 0, 0, 0, 0); // Outside edge
    set_clm (656, 0x121, 0x1b, 0x19, 0x52, 0x9f, 0,0,0,0,0); // Inside edge
    set_clm (657, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x17e); // Pillar - human player
    set_clm (658, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a6); // Pillar - player 1
    set_clm (659, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a7); // Pillar - player 2
    set_clm (660, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a8); // Pillar - player 3
    set_clm (661, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a8); // Dummy top in case of a "hero heart"
    set_clm (662, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a8); // Dummy top in case of an unclaimed heart
    // Entrance
    set_clm (700, 0xffd6, 0x1b, 0x13b, 0, 0, 0, 0, 0, 0, 0); // Very edge
    set_clm (701, 0xb7, 0x1b, 0x1a, 0xa, 0, 0, 0, 0, 0, 0); // Edge of building
    set_clm (702, 0xffd3, 0x1c, 0xa, 0, 0, 0, 0, 0, 0xe, 0); // Edge of roof
    set_clm (703, 0x1f, 0x1d, 0x1a, 0xa, 
           0xf, 0xd, 0xd, 0xd, 0xe, 0); // Support pillar
    set_clm (704, 0xfff6, 0, 0, 0, 0, 0, 0, 0, 0x89, 0); // Round the centre...
    set_clm (705, 0xffee, 0, 0, 0, 0, 0, 0, 0, 0x8a, 0);
    set_clm (706, 0xfff7, 0, 0, 0, 0, 0, 0, 0, 0x8b, 0);
    set_clm (707, 0xffe4, 0, 0, 0, 0, 0, 0, 0, 0x8c, 0);
    set_clm (708, 4, 0, 0, 0, 0, 0, 0, 0, 0x1a9, 0); // Centre
    set_clm (709, 0xffe4, 0, 0, 0, 0, 0, 0, 0, 0x8e, 0);// Round again
    set_clm (710, 0xfff8, 0, 0, 0, 0, 0, 0, 0, 0x8f, 0);
    set_clm (711, 0xffe5, 0, 0, 0, 0, 0, 0, 0, 0x90, 0);
    set_clm (712, 0xfff7, 0, 0, 0, 0, 0, 0, 0, 0x91, 0);

    // Doors
    set_clm (720, 0xffda, 0xb5, 0x94, 0, 0, 0, 0, 0, 0, 0); // Door floor
    // top/left, centre, bottom/right
    // wooden
    set_clm (721, 0x2e, 0xb3, 0x94, 0x169, 0x166, 0x163, 0x4d, 0, 0, 0);
    set_clm (722, 0x1e, 0xb1, 0xc6, 0x16a, 0x167, 0x164, 0x4d, 0, 0, 0);
    set_clm (723, 0x30, 0xba, 0x94, 0x16b, 0x168, 0x165, 0x4d, 0, 0, 0);
    // braced
    set_clm (724, 0xe, 0xb3, 0x94, 0x172, 0x16f, 0x16c, 0x4d, 0, 0, 0);
    set_clm (725, 0x8, 0xb1, 0xc6, 0x173, 0x170, 0x16d, 0x4d, 0, 0, 0);
    set_clm (726, 0xe, 0xba, 0x94, 0x174, 0x171, 0x16e, 0x4d, 0, 0, 0);
    // iron
    set_clm (727, 0x14, 0xb3, 0x94, 0x17b, 0x178, 0x175, 0x1a3, 0, 0, 0);
    set_clm (728, 0x10, 0xb1, 0xc7, 0x17c, 0x179, 0x176, 0x1a4, 0, 0, 0);
    set_clm (729, 0x15, 0xba, 0x94, 0x17d, 0x17a, 0x177, 0x1a5, 0, 0, 0);
    // magic
    set_clm (730, 0x3, 0xb3, 0x94, 0x12e, 0x12b, 0x128, 0x19a, 0, 0, 0);
    set_clm (731, 0x4, 0xb1, 0xc1, 0x12d, 0x12a, 0x127, 0x19b, 0, 0, 0);
    set_clm (732, 0x3, 0xba, 0x94, 0x12c, 0x129, 0x126, 0x19c, 0, 0, 0);

    // Funky stuff for text
    {
      int i;
      if (fontheight == 8)
          for (i=0; i < 256; i++)
            set_clm_entry (1700+i, 0xffff, 1, 7, 8, 0x3f, 0x1b, 0, 
                         ((i>>0)&1) ? 0x184 : 2, ((i>>1)&1) ? 0x184 : 2, 
                         ((i>>2)&1) ? 0x184 : 2, ((i>>3)&1) ? 0x184 : 2, 
                         ((i>>4)&1) ? 0x184 : 2, ((i>>5)&1) ? 0x184 : 2, 
                         ((i>>6)&1) ? 0x184 : 2, ((i>>7)&1) ? 0x184 : 2);
      else
          for (i=0; i < 256; i++)
            set_clm_entry (1700+i, 0xffff, 1, 7, 8, 0x3f, 0x1b, 0, 0x1b,
                         fontheight>0 ? (((i>>0)&1) ? 0x184 : 2) : 0,
                         fontheight>1 ? (((i>>1)&1) ? 0x184 : 2) : 0,
                         fontheight>2 ? (((i>>2)&1) ? 0x184 : 2) : 0,
                         fontheight>3 ? (((i>>3)&1) ? 0x184 : 2) : 0,
                         fontheight>4 ? (((i>>4)&1) ? 0x184 : 2) : 0,
                         fontheight>5 ? (((i>>5)&1) ? 0x184 : 2) : 0,
                         fontheight>6 ? (((i>>6)&1) ? 0x184 : 2) : 0);
      for (i=0; i < 130; i++)
          set_clm_entry (1500+i, 0xffff, 1, 7, 8, 0x3f, 0x1b, 0, 0x1b,
                     fontheight>0 ? 2 : 0,
                     fontheight>1 ? 2 : 0,
                     fontheight>2 ? 2 : 0,
                     fontheight>3 ? 2 : 0,
                     fontheight>4 ? 2 : 0,
                     fontheight>5 ? 2 : 0,
                     fontheight>6 ? 2 : 0); 
    }
    // Set our signature on the clm
    memset (lvl->clm[2047], 0xff, 24);
}

static void set_room_wall (int num, unsigned int use, int base, int c1, int c2,
                     int c3, int c4, int stat)
{
    int drape[5]={0xa0, 0x19a, 0x19d, 0x1a0, 0x1a3};
    int i;
    
    for (i=0; i < 5; i++)
    {
      set_clm (num+i*3, use, base, c1, c2, c3, c4, drape[i], 0, 0, 0);
      set_clm (num+i*3+1, use, base, c1+(stat<1), c2+(stat<2),
             c3+(stat<3), c4+(stat<4), drape[i]+1, 0, 0, 0); 
      set_clm (num+i*3+2, use, base, c1+2*(stat<1), c2+2*(stat<2),
             c3+2*(stat<3), c4+2*(stat<4), drape[i]+2, 0, 0, 0);
    }
}

static void set_room_wall2 (int num, unsigned int use, int base, int c1, int c2,
                      int c3, int c4, int c5, int stat)
{
    int i;
    // Wastes clm space, but that's not a problem
    for (i=0; i < 5; i++)
    {
      set_clm (num+i*3, use, base, c1, c2, c3, c4, c5, 0, 0, 0);
      set_clm (num+i*3+1, use, base, c1+(stat<0), c2+(stat<2),
             c3+(stat<3), c4+(stat<4), c5+(stat<5), 0, 0, 0); 
      set_clm (num+i*3+2, use, base, c1+2*(stat<1), c2+2*(stat<2), 
             c3+2*(stat<3), c4+2*(stat<4), c5+2*(stat<5), 0, 0, 0);
    }
}

static void set_room_wall3 (int num, unsigned int use, int base, int c1, int c2,
                      int c3, int c4, int c5, int stat)
{
    int i;
    // Wastes clm space, but that's not a problem
    for (i=0; i < 5; i++)
    {
      set_clm (num+i*3, use, base, c1, c2, c3, c4, c5, 0, 0, 0); 
      set_clm (num+i*3+1, use, base, c1+(stat<1), c2+(stat<2),
             c3+(stat<3), c4+(stat<4), c5, 0, 0, 0); 
      set_clm (num+i*3+2, use, base, c1+2*(stat<1), c2+2*(stat<2),
             c3+2*(stat<3), c4+2*(stat<4), c5, 0, 0, 0);
    }
}

static void set_room_wall4 (int num, unsigned int use, int base, int c1, int c2,
                      int c3, int c4)
{
    int drape[5]={0xa0, 0x19a, 0x19d, 0x1a0, 0x1a3};
    int i;
    
    for (i=0; i < 5; i++)
    {
      set_clm (num+i*3, 0x1a, 0x1b, 0x19, 0x53, 0x50, 0x50, 
             drape[i], 0, 0, 0);
      set_clm (num+i*3+1, use, base, c1, c2, c3, c4, 
             drape[i]+1, 0, 0, 0); 
      set_clm (num+i*3+2, 0x1a, 0x1b, 0x19, 0x51, 0x4f, 0x4f,
             drape[i]+2, 0, 0, 0);
    }
}

static void set_clm (int num, unsigned int use,
                 int base, int c1, int c2, 
                 int c3, int c4, int c5, int c6,
                 int c7, int c8)
{
    int solid=0;
    int height=0;
    if (c1)
    {
      height=1;
      solid++;
    }
    if (c2)
    {
      height=2;
      solid+=2;
    }
    if (c3)
    {
      height=3;
      solid+=4;
    }
    if (c4)
    {
      height=4;
      solid+=8;
    }
    if (c5)
    {
      height=5;
      solid+=16;
    }
    if (c6)
    {
      height=6;
      solid+=32;
    }
    if (c7)
    {
      height=7;
      solid+=64;
    }
    if (c8)
    {
      height=8;
      solid+=128;
    }
    set_clm_entry (num, use, 1, 0, height, solid, base, 0, c1, c2, c3,
               c4, c5, c6, c7, c8);
}

    
static void set_clm_entry (int num, unsigned int use, int permanent, int lintel,
                     int height, unsigned int solid, int base, int orientation,
                     int c1, int c2, int c3, int c4, int c5, int c6,
                     int c7, int c8)
{
    char *x;
    x = (char *)lvl->clm[num];
    x[0]=use&255;
    x[1]=(use>>8)&255;
    x[2]=(permanent&1)+((lintel&7)<<1)+((height&15)<<4);
    x[3]=solid&255;
    x[4]=(solid>>8)&255;
    x[5]=base&255;
    x[6]=(base>>8)&255;
    x[7]=orientation&255;
    x[8]=c1&255;
    x[9]=(c1>>8)&255;
    x[10]=c2&255;
    x[11]=(c2>>8)&255;
    x[12]=c3&255;
    x[13]=(c3>>8)&255;
    x[14]=c4&255;
    x[15]=(c4>>8)&255;
    x[16]=c5&255;
    x[17]=(c5>>8)&255;
    x[18]=c6&255;
    x[19]=(c6>>8)&255;
    x[20]=c7&255;
    x[21]=(c7>>8)&255;
    x[22]=c8&255;
    x[23]=(c8>>8)&255;
}

static int rnd (int range)
{
    return (int) (((float)range)*rand()/(RAND_MAX+1.0));
}

static void set_dat (int x, int y, int tl, int tm, int tr, int ml, int mm, int mr,
            int bl, int bm, int br)
{
    set_dat2 (x*3, y*3, tl);
    set_dat2 (x*3+1, y*3, tm);
    set_dat2 (x*3+2, y*3, tr);
    set_dat2 (x*3, y*3+1, ml);
    set_dat2 (x*3+1, y*3+1, mm);
    set_dat2 (x*3+2, y*3+1, mr);
    set_dat2 (x*3, y*3+2, bl);
    set_dat2 (x*3+1, y*3+2, bm);
    set_dat2 (x*3+2, y*3+2, br);
}

static void set_dat_unif (int x, int y, int d)
{
    set_dat (x, y, d, d, d, d, d, d, d, d, d);
}

void set_dat2 (int cx, int cy, int d)
{
    unsigned int x;
    x = 0x10000-d;
    dat_high [cx][cy]=(x>>8)&255;
    dat_low [cx][cy]=x&255;
}

int get_dat (int cx, int cy)
{          
    return 0x10000-((dat_high[cx][cy]<<8)+dat_low[cx][cy]);
}
      
static void set_wall (int x, int y, int d1, int d2, int d3)
{
    int i, j;
    int slbsame[3][3];
    int surround=1;
    int bx, by;
    int face[3][3];
    
    bx=x*3;
    by=y*3;
    for (i=0; i < 3; i++)
    {
      for (j=0; j < 3; j++)
      {
          if (x+i-1 < 0 || x+i-1 > MAP_MAXINDEX_X || y+j-1 < 0 || y+j-1 > MAP_MAXINDEX_Y)
          {
            slbsame[i][j]=0;
            surround=0;
          }
          else
          {
            if (lvl->slb[x+i-1][y+j-1] < 10 || lvl->slb[x+i-1][y+j-1]==52)
                slbsame[i][j]=1;
            else
            {
                slbsame[i][j]=0;
                surround=0;
            }
          }
      }
    }
    set_dat (x, y, rnd(8)+10, rnd(8)+10, rnd(8)+10, 
           rnd(8)+10, rnd(8)+10, rnd(8)+10, 
           rnd(8)+10, rnd(8)+10, rnd(8)+10);
    if (surround)
      return;

    set_dat2 (bx+1, by+1, 202+lvl->own[x][y]);
    
    for (i=0; i < 3; i+=2)
    {
      face[i][1]=1-slbsame[i][1];
      face[1][i]=1-slbsame[1][i];
    }
    for (i=0; i < 3; i+=2)
    {
      for (j=0; j < 3; j+=2)
      {
          if (!slbsame[i][j] &&
            (slbsame[i][1]==slbsame[1][j]))
          {
            set_dat2 (bx+i, by+j, get_corner (x, y, i, j));
            face[i][j]=0;
          }
          else
          {
            face[i][j]=1;
            if (face[i][1]) 
                set_dat2 (bx+i, by+j, 219+i);
            if (face[1][j])
                set_dat2 (bx+i, by+j, 219+j);
          }
      }
    }
    if (face[0][0] && face[1][0] && face[2][0])
    {
      set_dat2 (bx, by, d1);
      set_dat2 (bx+1, by, d2);
      set_dat2 (bx+2, by, d3);
    }
    else if (face [1][0])
      set_dat2 (bx+1, by, 220);
    if (face[0][0] && face[0][1] && face[0][2])
    {
      set_dat2 (bx, by, d1);
      set_dat2 (bx, by+1, d2);
      set_dat2 (bx, by+2, d3);
    }
    else if (face [0][1])
      set_dat2 (bx, by+1, 220);
    if (face[0][2] && face[1][2] && face[2][2])
    {
      set_dat2 (bx, by+2, d1);
      set_dat2 (bx+1, by+2, d2);
      set_dat2 (bx+2, by+2, d3);
    }
    else if (face [1][2])
      set_dat2 (bx+1, by+2, 220);
    if (face[2][0] && face[2][1] && face[2][2])
    {
      set_dat2 (bx+2, by, d1);
      set_dat2 (bx+2, by+1, d2);
      set_dat2 (bx+2, by+2, d3);
    }
    else if (face [2][1])
      set_dat2 (bx+2, by+1, 220);
    for (i=0; i < 3; i+=2)
    {
      j = picture_wall (x, y, i, 1);
      if (j)
      {
          if (face[i][0] && face[i][1] && face[i][2])
          {
            set_dat2 (bx+i, by, j);
            set_dat2 (bx+i, by+1, j+1);
            set_dat2 (bx+i, by+2, j+2);
          }
          else
          {
            set_dat2 (bx+i, by+1, 209);
            if (face[i][0])
                set_dat2 (bx+i, by, 209);
            if (face[i][2])
                set_dat2 (bx+i, by+2, 209);
          }
      }
      j = picture_wall (x, y, 1, i);
      if (j)
      {
          if (face[0][i] && face[1][i] && face[2][i])
          {
            set_dat2 (bx, by+i, j);
            set_dat2 (bx+1, by+i, j+1);
            set_dat2 (bx+2, by+i, j+2);
          }
          else
          {
            set_dat2 (bx+1, by+i, 209);
            if (face[0][i])
                set_dat2 (bx, by+i, 209);
            if (face[2][i])
                set_dat2 (bx+2, by+i, 209);
          }
      }
    }
}

static int get_corner (int x, int y, int i, int j)
{
    static int cor[54]={0,0,0,0,0,0,0,0,0,0, // 0-9
      0,0,0,0,0,0,639,0,544,0, // 10-19
      589,0,921,0,533,0,0,0,563,0, // 20-29
      642,0,623,0,639,0,523,0,0,0, // 30-39
      602,0,0,0,0,0,0,0,0,0, // 40-49
      0,0,0,0}; // 50-53
    
    int s;
    
    if (x+i-1 < 0 || x+i-1>MAP_MAXINDEX_X || y+j-1 < 0 || y+j-1 > MAP_MAXINDEX_Y)
      return 200;
    s = lvl->slb[x+i-1][y+j-1];
    if (s > 53)
      return 200;
    if (!cor[s])
      return 200;
    return cor[s];
}
    
static void set_pillar (int x, int y, int pillar)
{
    set_pillar_thing (x, y, pillar, 0, 0);
}

static void set_pillar_thing (int x, int y, int pillar, int thingnum, int thinghgt)
{
    int slbsame[3][3];
    int i, j;
    int bx, by;
    unsigned char *thing;
    
    bx = x*3;
    by = y*3;
    for (i=0; i < 3; i++)
    {
      for (j=0; j < 3; j++)
      {
          slbsame[i][j]=0;
          if (x+i-1>=0 && x+i-1 < MAP_SIZE_X && y+j-1>=0 && y+j-1 < MAP_SIZE_Y)
          {
            if (lvl->slb[x][y]==lvl->slb[x+i-1][y+j-1] && 
                lvl->own[x][y]==lvl->own[x+i-1][y+j-1])
                slbsame[i][j]=1;
          }
      }
    }
    for (i=0; i < 3; i+=2)
    {
      for (j=0; j < 3; j+=2)
      {
          if (slbsame[i][1] && slbsame[1][j] &&
            !slbsame[2-i][1] && !slbsame[1][2-j])
          {
            if (pillar)
                set_dat2 (bx+i, by+j, pillar);
            if (thingnum)
            {
                thing = create_object (bx+1, by+1, thingnum);
                thing[5]=(thinghgt>>8)&255;
                thing[4]=thinghgt&255;
                if (thingnum == 2) // Put torches right by the pillars
                {
                  thing[0]=i*0x40+0x40;
                  thing[2]=j*0x40+0x40;
                }
                thing [12]=(MAP_SIZE_X*y+x) >> 8;
                thing [11]=((MAP_SIZE_X*y+x) & 255);
                thing_add(lvl,thing);
            }
          }
      }
    }
}

static void set_corner_unif (int x, int y, int normal, int border)
{
    set_corner (x, y, normal, border, border, border, border,
            border, border, border, border);
}

static void set_corner (int x, int y, int normal, int b1, int b2, int b3, int b4, int b5, int b6,
                  int b7, int b8)
{
    int slbsame[3][3];
    int border[3][3];
    int i, j;
    int bx, by;
    
    border[0][0]=b1;
    border[1][0]=b2;
    border[2][0]=b3;
    border[0][1]=b4;
    border[1][1]=normal;
    border[2][1]=b5;
    border[0][2]=b6;
    border[1][2]=b7;
    border[2][2]=b8;

    if (normal)
      set_dat_unif (x, y, normal);
    bx = x*3;
    by = y*3;
    for (i=0; i < 3; i++)
    {
      for (j=0; j < 3; j++)
      {
          slbsame[i][j]=0;
          if (x+i-1>=0 && x+i-1 < MAP_SIZE_X && y+j-1>=0 && y+j-1 < MAP_SIZE_Y)
          {
            if (lvl->slb[x][y]==lvl->slb[x+i-1][y+j-1] && 
                lvl->own[x][y]==lvl->own[x+i-1][y+j-1])
                slbsame[i][j]=1;
          }
      }
    }
    for (i=0; i < 3; i+=2)
      for (j=0; j < 3; j+=2)
          if (!slbsame[i][j])
            set_dat2 (bx+i, by+j, border[i][j]);
    for (i=0; i < 3; i++)
    {
      for (j=0; j < 3; j++)
      {
          if (!slbsame[i][j])
          {
            if (j==1)
            {
                set_dat2 (bx+i, by, border[i][1]);
                set_dat2 (bx+i, by+1, border[i][1]);
                set_dat2 (bx+i, by+2, border[i][1]);
            }
            if (i==1)
            {
                set_dat2 (bx, by+j, border[1][j]);
                set_dat2 (bx+1, by+j, border[1][j]);
                set_dat2 (bx+2, by+j, border[1][j]);
            }                
          }
      }
    }
    for (i=0; i < 3; i+=2)
      for (j=0; j < 3; j+=2)
          if (!slbsame[i][1] && !slbsame[1][j])
            set_dat2 (bx+i, by+j, border[i][j]);
}

// Use our own strange slb thing to get locked/unlocked doors right
void check_doors(void)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    unsigned char *thing;
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
          int things_count=lvl->tng_subnums[i][j];
          for (k=0; k<things_count; k++)
          {
            thing = lvl->tng_lookup[i][j][k];
            if (thing[6]==THING_TYPE_DOOR)
            {
                if ((lvl->slb[i/3][j/3]&0xfe) == thing[7]*2+40) // Update the slab
                  lvl->slb[i/3][j/3] = thing[7]*2+40+thing[14];
            }
          }
      }
    }
}
