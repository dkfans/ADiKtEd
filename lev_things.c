/*
 * lev_things.c
 *
 * Defines routines for maintaining list of TNG entries,
 * provides higher level TNG control than obj_things
 *
 */

#include "lev_things.h"

#include "globals.h"
#include "obj_things.h"
#include "lev_data.h"

/*
 * Removes all things that usually exist only in specific rooms
 */
void remove_room_things (int x, int y)
{
    int cx, cy, i;
    for (cx=x*3; cx < x*3+3; cx++)
      for (cy=y*3; cy < y*3+3; cy++)
      {
          int last_thing=get_thing_subnums(lvl,cx,cy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,cx,cy,i);
            if (is_room_thing(thing))
                thing_del(lvl,cx, cy, i);
          }
     }
}

/*
 * Removes all things that usually exist only in specific rooms,
 * and aren't crucial for the game (like gates and dungeon hearts)
 */
void remove_noncrucial_room_things(int x, int y)
{
    int cx, cy, i;
    for (cx=x*3; cx < x*3+3; cx++)
      for (cy=y*3; cy < y*3+3; cy++)
      {
          int last_thing=get_thing_subnums(lvl,cx,cy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,cx,cy,i);
            if (is_room_thing(thing) && (!is_crucial_thing(thing)))
                thing_del(lvl,cx, cy, i);
          }
     }
}

/*
 * Verifies thing types and parameters. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short things_verify(struct LEVEL *lvl, char *err_msg)
{
    char child_err_msg[LINEMSG_SIZE];
    strcpy(child_err_msg,"Unknown error");
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Sweeping through things
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          short result=thing_verify(thing,child_err_msg);
          if (result!=VERIF_OK)
          {
            sprintf(err_msg,"%s at slab %d,%d.",child_err_msg,i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y);
            return result;
          }
        }
      }
    }
  return VERIF_OK;
}

void set_pillar(int x, int y, int pillar)
{
    set_pillar_thing(x, y, pillar, 0, 0);
}

void set_pillar_thing(int x, int y, int pillar, int thingnum, int thinghgt)
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
                set_dat_subtile(lvl, bx+i, by+j, pillar);
            if (thingnum)
            {
                thing = create_item(bx+1, by+1, thingnum);
                set_thing_tilepos_h(thing,(thinghgt>>8)&255);
                set_thing_subtpos_h(thing,thinghgt&255);
                if (thingnum == 2) // Put torches right by the pillars
                {
                  set_thing_subtpos(thing,i*0x40+0x40,j*0x40+0x40);
                }
                thing [12]=(MAP_SIZE_X*y+x) >> 8;
                thing [11]=((MAP_SIZE_X*y+x) & 255);
                thing_add(lvl,thing);
            }
          }
      }
    }
}
