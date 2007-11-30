/*
 * internal.c
 *
 * ???
 *
 */


#include "internal.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "lev_things.h"
#include "graffiti.h"

static int get_corner (int x, int y, int i, int j);
static void finish_clm (void);
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
static void set_door_dattng (int x, int y, int datbase, int type, int locked);


void create_clmdattng(void)
{
    int x, y;    
    create_default_clm();
    for (x=0; x < MAP_SIZE_X; x++)
      for (y=0; y < MAP_SIZE_Y; y++)
      {
          update_tngdat(x, y);
      }
}

void create_clmdat(void)
{
    int x, y;    
    create_default_clm();
    for (x=0; x < MAP_SIZE_X; x++)
      for (y=0; y < MAP_SIZE_Y; y++)
      {
          update_dat_entry(x, y);
      }
}

/*
 * Update a slab and all slabs around it
 */
void update_square (int x, int y)
{
    int i, j;
    for (i=-1; i < 2; i++)
      for (j=-1; j < 2; j++)
      {
          update_tngdat(x+i, y+j);
      }
}

/*
 * Update a single slab
 * OLD - delete pending
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
//          lvl->wib[bx][by]=slab_default_wbi_entry(s);
      }
    }
    surround=1;
    for (i=0; i<3; i++)
    {
      for (j=0; j<3; j++)
      {
          if ((x+i-1>=0) && (x+i-1<MAP_SIZE_X) && (y+j-1>=0) && (y+j-1 < MAP_SIZE_Y))
          {
            slbrel[i][j]=lvl->slb[x+i-1][y+j-1];
            ownrel[i][j]=lvl->own[x+i-1][y+j-1];
            if ( (lvl->slb[x][y]==slbrel[i][j]) && (lvl->own[x][y]==ownrel[i][j]) )
              slbsame[i][j]=1;
            else
            {
              slbsame[i][j]=0;
              surround=0;
            }
          }
          else
          {
            slbrel[i][j]=-1;
            ownrel[i][j]=-1;
            slbsame[i][j]=0;
            surround=0;
          }
      }
    }
    switch (s)
    {
    case SLAB_TYPE_ROCK: // Rock
      set_dat_unif (lvl, x, y, 1);
      break;
    case SLAB_TYPE_GOLD: // Gold
      set_dat (lvl, x, y, rnd(5)+2, rnd(5)+2, rnd(5)+2, rnd(5)+2,
             rnd(5)+2, rnd(5)+2, rnd(5)+2, rnd(5)+2, rnd(5)+2);
      break;
    case SLAB_TYPE_EARTH: // Earth
      set_dat (lvl, x, y, rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10);
      break;
    case SLAB_TYPE_TORCHDIRT: // Torch dirt
      set_dat (lvl, x, y, rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10);
      for (i=0; i < 3; i+=2)
      {
          if ((slbrel[i][1]>9) && (slbrel[i][1] != 52) &&
            ((slbrel[i][1]<42) || (slbrel[i][1] > 49)))
            set_dat_subtile(lvl, x*3+i, y*3+1, 18);
          if ((slbrel[1][i]>9) && (slbrel[1][i] != 52) &&
            ((slbrel[1][i]<42) || (slbrel[1][i] > 49)))
            set_dat_subtile(lvl, x*3+1, y*3+j, 18);
      }
      break;
    case SLAB_TYPE_WALLDRAPE: // Drape
      set_wall (x, y, 450+lvl->own[x][y]*3, 451+lvl->own[x][y]*3, 
              452+lvl->own[x][y]*3);
      break;
    case SLAB_TYPE_WALLTORCH: // Torch wall
      set_wall (x, y, 219, 220, 221);
      for (i=0; i < 3; i+=2)
      {
          if ((slbrel[i][1]>9) && (slbrel[i][1] != 52) &&
            ((slbrel[i][1]<42) || (slbrel[i][1] > 49)) &&
            !picture_wall (x, y, i, 1))
            set_dat_subtile(lvl, x*3+i, y*3+1, 208);
          if ((slbrel[1][i]>9) && (slbrel[1][i] != 52) &&
            ((slbrel[1][i]<42) || (slbrel[1][i] > 49)) &&
            !picture_wall (x, y, 1, i))
            set_dat_subtile(lvl, x*3+1, y*3+i, 208);
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
      set_dat_unif (lvl, x, y, 110);
      break;
    case SLAB_TYPE_CLAIMED: // Claimed
      set_dat (lvl, x, y, 135,135,135,135,130+lvl->own[x][y],135,135,135,135);
      break;
    case SLAB_TYPE_LAVA: // Lava
      set_dat_unif (lvl, x, y, 105);
      break;
    case SLAB_TYPE_WATER: // Water
      set_dat_unif (lvl, x, y, 100);
      break;
    case SLAB_TYPE_PORTAL: // Portal
      if (surround)
      {
          set_dat (lvl, x, y, 704, 705, 706, 707, 708, 709, 710, 711, 712);
          thing = create_roomeffect(x*3+1, y*3+1, ROOMEFC_SUBTP_ENTRICE); // Dry ice
          thing_add(lvl,thing);
      }
      else
      {
          set_dat_unif (lvl, x, y, 700); // Now we only need to add extras
          for (i=-1; i < 2; i+=2)
          {
            for (j=-1; j < 2; j+=2)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if (slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx,  by,  703);
                  set_dat_subtile(lvl, bx-i,by,  701);
                  set_dat_subtile(lvl, bx,  by-j,701);
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
                  set_dat_subtile(lvl, bx-1,by-j, 701);
                  set_dat_subtile(lvl, bx,  by-j, 701);
                  set_dat_subtile(lvl, bx+1,by-j, 701);
                }
                if (!j && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx-i, by-1,701);
                  set_dat_subtile(lvl, bx-i, by,  701);
                  set_dat_subtile(lvl, bx-i, by+1,701);
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
                  set_dat_subtile(lvl, bx-1, by,702);
                  set_dat_subtile(lvl, bx, by,  702);
                  set_dat_subtile(lvl, bx+1, by,702);
                }
                if (!j && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx, by-1,702);
                  set_dat_subtile(lvl, bx, by,  702);
                  set_dat_subtile(lvl, bx, by+1,702);
                }
            }                        
          }
      }
      break;
    case SLAB_TYPE_TREASURE: // Treasure
      set_dat_unif (lvl, x, y, 500);
      set_pillar_thing (x, y, 501, 0x1c, 0x100);
      break;
    case SLAB_TYPE_LIBRARY: // Library
      if (surround)
          set_dat (lvl, x, y, 540,540,540,541,542,543,540,540,540);
      else
      {
          set_dat_unif (lvl, x, y, 540);
          set_pillar (x, y, 544);
      }
      break;
    case SLAB_TYPE_PRISONCASE: // Prison case is really grotty
      set_corner (x, y, 580, 581, 582, 583, 584, 585, 586, 587, 588);
      // Sides first
      if ((!slbsame [0][1]) && (slbrel[0][1]>9))
      {
          thing_add(lvl,create_item(x*3, y*3, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3, y*3+1, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3, y*3+2, ITEM_SUBTYPE_PRISONBAR));
      }
      if ((!slbsame[2][1]) && (slbrel[2][1]>9))
      {
          thing_add(lvl,create_item(x*3+2, y*3, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3+2, y*3+1, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3+2, y*3+2, ITEM_SUBTYPE_PRISONBAR));
      }
      if (!slbsame [1][0] && slbrel[1][0] > 9)
      {
          if (slbsame[0][1] || slbrel[0][1]<9)
            thing_add(lvl,create_item(x*3, y*3, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3+1, y*3, ITEM_SUBTYPE_PRISONBAR));
          if (slbsame[2][1] || slbrel[2][1]<9)
            thing_add(lvl,create_item(x*3+2, y*3, ITEM_SUBTYPE_PRISONBAR));
      }
      if ((!slbsame[1][2]) && (slbrel[1][2]>9))
      {
          if ((slbsame[0][1]) || (slbrel[0][1]<9))
            thing_add(lvl,create_item(x*3, y*3+2, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3+1, y*3+2, ITEM_SUBTYPE_PRISONBAR));
          if ((slbsame[2][1]) || (slbrel[2][1]<9))
            thing_add(lvl,create_item(x*3+2, y*3+2, ITEM_SUBTYPE_PRISONBAR));
      }
      break;
    case SLAB_TYPE_TORTURE: // Torture
      if (surround)
      {
          set_dat (lvl, x, y, 591, 591, 591, 591, 590,
                 591, 591, 591, 591);
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_TORTURER);
      }
      else
      {
          set_dat (lvl, x, y, 591, 591, 591, 591, 590,
                 591, 591, 591, 591);
          set_corner (x, y, 0, 581, 582, 583, 
                  584, 585, 586, 587, 588);
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_TORTSPIKE);
      }
      thing_add(lvl,thing);
      break;
    case SLAB_TYPE_TRAINING: // Training
      if (surround)
      {
          set_dat_unif (lvl, x, y, 530);
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_TRAINPOST);
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
          set_dat_unif (lvl, x, y, 650);
      else
      {
          set_dat_unif (lvl, x, y, 651); // Now we only need to add extras
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
                  set_dat_subtile(lvl, bx, by, 656);
                  set_dat_subtile(lvl, bx-i, by-j, 655);
                  if (!i)
                  {
                      set_dat_subtile(lvl, bx-1, by, 656);      
                      set_dat_subtile(lvl, bx+1, by, 656);
                      set_dat_subtile(lvl, bx-1, by-j, 655);
                      set_dat_subtile(lvl, bx+1, by-j, 655);
                  }
                  if (!j)
                  {
                      set_dat_subtile(lvl, bx, by-1, 656);      
                      set_dat_subtile(lvl, bx, by+1, 656);
                      set_dat_subtile(lvl, bx-i, by-1, 655);
                      set_dat_subtile(lvl, bx-i, by+1, 655);
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
                      set_dat_subtile(lvl, bx, by, 657+lvl->own[x][y]);
                      set_dat_subtile(lvl, bx-i, by, 654);
                      set_dat_subtile(lvl, bx, by-j, 654);
                  }
                }
            }
            if (flag2 && !flag3)
            {
                set_dat_subtile(lvl, x*3+2, y*3, 652);
                set_dat_subtile(lvl, x*3, y*3+2, 652);
            }
            if (flag3 && !flag2)
            {
                set_dat_subtile(lvl, x*3, y*3, 652);
                set_dat_subtile(lvl, x*3+2, y*3+2, 652);
            }
            if (flag2 || flag3)
            {
                char *obj=create_item(x*3+1, y*3+1,
                  (lvl->own[x][y]>0 && lvl->own[x][y]<4 ? 0x77+lvl->own[x][y] : ITEM_SUBTYPE_HEARTFLMR));
                thing_add(lvl,obj);
                set_dat_subtile(lvl, x*3+1, y*3+1, 653);
            }
          }
      }
      break;
    case SLAB_TYPE_WORKSHOP: // Workshop 
      if (surround)
      {
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_WRKSHPMCH);
          thing_add(lvl,thing);
          set_dat (lvl, x, y, 561, 561, 561, 561, 
                 560, 561, 561, 562, 561);
      }
      else
      {
          set_dat_unif (lvl, x, y, 560);
          set_pillar_thing (x, y, 563, 0x1a, 0x100);
      }
      break;
    case SLAB_TYPE_SCAVENGER: // Scavenger room
      set_dat (lvl, x, y, 641, 641, 641, 641, 640,
             641, 641, 641, 641);
      if (surround)
      {
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_SCAVNGEYE);
          thing_add(lvl,thing);
      }
      else
          set_pillar_thing (x, y, 642, 2, 0x300);
      break;
    case SLAB_TYPE_TEMPLE: // Temple
      if (surround)
          set_dat (lvl, x, y, 612, 613, 614, 615, 616, 617, 618, 619, 620);
      else
      {
          set_dat_unif (lvl, x, y, 610); // Now we only need to add extras
          for (i=-1; i < 2; i+=2)
          {
            for (j=-1; j < 2; j+=2)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if (slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx, by, 622);
                  set_dat_subtile(lvl, bx-i, by-j, 611);
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
                  set_dat_subtile(lvl, bx-i, by-j, 610);
                  set_dat_subtile(lvl, bx-1, by, 621);
                  set_dat_subtile(lvl, bx, by, 621);
                  set_dat_subtile(lvl, bx+1, by, 621);
                }
                if (!j && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx-i, by-j, 610);
                  set_dat_subtile(lvl, bx, by-1, 621);
                  set_dat_subtile(lvl, bx, by, 621);
                  set_dat_subtile(lvl, bx, by+1, 621);
                }
            }                        
          }
          set_pillar_thing (x, y, 0, 4, 0x200);
      }
      break;
    case SLAB_TYPE_GRAVEYARD: // Graveyard
      set_corner (x, y, 630, 631, 632, 633, 634, 635, 636, 637, 638);
      thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_GRAVSTONE);
      thing_add(lvl,thing);
      // Add dry ice instead of pillars
      for (i=0; i < 3; i+=2)
      {
          for(j=0; j < 3; j+=2)
          {
            if (slbsame[i][1] && slbsame[1][j] &&
                !slbsame[2-i][1] && !slbsame[1][2-j])
            {
                thing = create_roomeffect(x*3+i, y*3+j, ROOMEFC_SUBTP_ENTRICE); // Dry ice
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
      set_dat_unif (lvl, x, y, 600);
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
      set_dat (lvl, x, y, 551, 552, 550, 552, 552, 552, 550, 552, 551);
      break;
    case SLAB_TYPE_GEMS:// Gems
      set_dat (lvl, x, y, rnd(9)+20, rnd(9)+20, rnd(9)+20, 
             rnd(9)+20, rnd(9)+20, rnd(9)+20, 
             rnd(9)+20, rnd(9)+20, rnd(9)+20);
      break;
    case SLAB_TYPE_GUARDPOST: // Guardpost
      set_dat (lvl, x, y, 571, 572, 570, 572, 572, 572, 570, 572, 571);
      // Put the flagpost on for keepers
      if (lvl->own[x][y] < 4)
      {
          thing = create_item(x*3+1, y*3+1, 0x73+lvl->own[x][y]);
          set_thing_tilepos_h(thing,2);
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
            thing = create_item(x*3+i, y*3+1, ITEM_SUBTYPE_TORCH);
            set_thing_tilepos_h(thing,2);
            set_thing_subtpos_h(thing,0xe0);
            set_thing_subtpos(thing,i*0x40+0x40,-1);
            thing_add(lvl,thing);
          }
          if ((slbrel[1][i]==3 || slbrel[1][i]==5)
            && !picture_wall (x, y+i-1, 1, 2-i))
          {
            thing = create_item(x*3+1, y*3+i, ITEM_SUBTYPE_TORCH);
            set_thing_tilepos_h(thing,2);
            set_thing_subtpos_h(thing,0xe0);
            set_thing_subtpos(thing,-1,i*0x40+0x40);
            thing_add(lvl,thing);
          }
      }
    }
}

int picture_wall (int x, int y, int dx, int dy)
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
          for (i=get_thing_subnums(lvl,cx,cy)-1; i >=0; i--)
          {
            thing=get_thing(lvl,cx,cy,i);
            if (is_door(thing))
                thing_del(lvl,cx, cy, i);
          }
    set_dat_unif (lvl, x, y, 720);
    // Set orientation of door, fairly arbitrarily :)
    if ((!(x) || (lvl->slb[x-1][y]<10))&&((x==MAP_MAXINDEX_X) || (lvl->slb[x+1][y]<10)))
      o=0;
    else
      o=1;
    thing = create_item(x*3+1, y*3+1, type);
    set_thing_type(thing,THING_TYPE_DOOR); // Make it a door
    thing[13]=o;
    thing[14]=locked;
    thing_add(lvl,thing);
    if (locked)
    {
      thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_SPINNKEY);
      set_thing_tilepos_h(thing,5);
      thing_add(lvl,thing);
    }
    set_dat_subtile(lvl, x*3+o, y*3+1-o, datbase);
    set_dat_subtile(lvl, x*3+1, y*3+1, datbase+1);
    set_dat_subtile(lvl, x*3+2-o, y*3+1+o, datbase+2);
}

void set_wall (int x, int y, int d1, int d2, int d3)
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
    set_dat (lvl, x, y, rnd(8)+10, rnd(8)+10, rnd(8)+10, 
           rnd(8)+10, rnd(8)+10, rnd(8)+10, 
           rnd(8)+10, rnd(8)+10, rnd(8)+10);
    if (surround)
      return;

    set_dat_subtile(lvl, bx+1, by+1, 202+lvl->own[x][y]);
    
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
            set_dat_subtile(lvl, bx+i, by+j, get_corner (x, y, i, j));
            face[i][j]=0;
          }
          else
          {
            face[i][j]=1;
            if (face[i][1]) 
                set_dat_subtile(lvl, bx+i, by+j, 219+i);
            if (face[1][j])
                set_dat_subtile(lvl, bx+i, by+j, 219+j);
          }
      }
    }
    if (face[0][0] && face[1][0] && face[2][0])
    {
      set_dat_subtile(lvl, bx, by, d1);
      set_dat_subtile(lvl, bx+1, by, d2);
      set_dat_subtile(lvl, bx+2, by, d3);
    }
    else if (face [1][0])
      set_dat_subtile(lvl, bx+1, by, 220);
    if (face[0][0] && face[0][1] && face[0][2])
    {
      set_dat_subtile(lvl, bx, by, d1);
      set_dat_subtile(lvl, bx, by+1, d2);
      set_dat_subtile(lvl, bx, by+2, d3);
    }
    else if (face [0][1])
      set_dat_subtile(lvl, bx, by+1, 220);
    if (face[0][2] && face[1][2] && face[2][2])
    {
      set_dat_subtile(lvl, bx, by+2, d1);
      set_dat_subtile(lvl, bx+1, by+2, d2);
      set_dat_subtile(lvl, bx+2, by+2, d3);
    }
    else if (face [1][2])
      set_dat_subtile(lvl, bx+1, by+2, 220);
    if (face[2][0] && face[2][1] && face[2][2])
    {
      set_dat_subtile(lvl, bx+2, by, d1);
      set_dat_subtile(lvl, bx+2, by+1, d2);
      set_dat_subtile(lvl, bx+2, by+2, d3);
    }
    else if (face [2][1])
      set_dat_subtile(lvl, bx+2, by+1, 220);
    for (i=0; i < 3; i+=2)
    {
      j = picture_wall (x, y, i, 1);
      if (j)
      {
          if (face[i][0] && face[i][1] && face[i][2])
          {
            set_dat_subtile(lvl, bx+i, by, j);
            set_dat_subtile(lvl, bx+i, by+1, j+1);
            set_dat_subtile(lvl, bx+i, by+2, j+2);
          }
          else
          {
            set_dat_subtile(lvl, bx+i, by+1, 209);
            if (face[i][0])
                set_dat_subtile(lvl, bx+i, by, 209);
            if (face[i][2])
                set_dat_subtile(lvl, bx+i, by+2, 209);
          }
      }
      j = picture_wall (x, y, 1, i);
      if (j)
      {
          if (face[0][i] && face[1][i] && face[2][i])
          {
            set_dat_subtile(lvl, bx, by+i, j);
            set_dat_subtile(lvl, bx+1, by+i, j+1);
            set_dat_subtile(lvl, bx+2, by+i, j+2);
          }
          else
          {
            set_dat_subtile(lvl, bx+1, by+i, 209);
            if (face[0][i])
                set_dat_subtile(lvl, bx, by+i, 209);
            if (face[2][i])
                set_dat_subtile(lvl, bx+2, by+i, 209);
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
    
