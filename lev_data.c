/*
 * lev_data.c
 *
 * Defines functions for maintaining the level memory structure.
 *
 */

#include "lev_data.h"

#include "globals.h"
#include "internal.h"
#include "obj_utils.h"

LEVEL *lvl=NULL;

void thing_add(LEVEL *lvl,unsigned char *thing)
{
    unsigned int x, y;
    x = thing[1];
    y = thing[3];

    lvl->tng_total_count++;
    if (is_action_thing(thing))
      apt_tot++;
    lvl->tng_subnums[x][y]++;
    lvl->tng_nums[x/3][y/3]++;
    if (lvl->tng_lookup[x][y]==NULL)
      lvl->tng_lookup[x][y]=(unsigned char **)malloc(sizeof (unsigned short *));
    else
      lvl->tng_lookup[x][y]=(unsigned char **)realloc(lvl->tng_lookup[x][y],
                          lvl->tng_subnums[x][y]*sizeof (unsigned short *));
    if (lvl->tng_lookup[x][y]==NULL)
      die("thing_add: Out of memory");
    int new_idx=lvl->tng_subnums[x][y]-1;
    lvl->tng_lookup[x][y][new_idx]=thing;
}

void thing_del(LEVEL *lvl,int x, int y, int num)
{
    unsigned char *thing;
    int i;
    if (num >= lvl->tng_subnums[x][y])
      return;
    lvl->tng_total_count--;
    thing = lvl->tng_lookup[x][y][num];
    if (is_action_thing(thing))
    {
      action_used[thing[19]+(thing[20]<<8)]=NULL;
      apt_tot--;
    }
    free (lvl->tng_lookup[x][y][num]);
    for (i=num; i < lvl->tng_subnums[x][y]-1; i++)
      lvl->tng_lookup[x][y][i]=lvl->tng_lookup[x][y][i+1];
    lvl->tng_subnums[x][y]--;
    lvl->tng_nums[x/3][y/3]--;
    lvl->tng_lookup[x][y]=(unsigned char **)realloc(lvl->tng_lookup[x][y], 
                        lvl->tng_subnums[x][y]*sizeof(char *));
}

/*
 * creates object for storing one level; allocates memory and inits
 * the values to zero; drops any previous pointers without deallocating;
 */
short level_init()
{
    lvl=(LEVEL *)malloc(sizeof(LEVEL));
    if (lvl==NULL)
        die("level_init: Out of memory");

    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

  { //allocating CLM structures
    lvl->clm = (unsigned char **)malloc (2048*sizeof (char *));
    if (lvl->clm==NULL)
        die("level_init: Out of memory");
    int i;
    for (i=0; i < 2048; i++)
    {
      lvl->clm[i]=(unsigned char *)malloc(SIZEOF_DK_CLM_REC);
      if (lvl->clm[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //allocating SLB structures
    int i;
    lvl->slb = (unsigned char **)malloc(MAP_SIZE_Y*sizeof(char *));
    if (lvl->slb==NULL)
        die("level_init: Out of memory");
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      lvl->slb[i] = (unsigned char *)malloc(MAP_SIZE_X*sizeof(char));
      if (lvl->slb[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //allocating OWN structures
    int i;
    lvl->own = (unsigned char **)malloc(MAP_SIZE_Y*sizeof(char *));
    if (lvl->own==NULL)
        die("level_init: Out of memory");
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      lvl->own[i] = (unsigned char *)malloc(MAP_SIZE_X*sizeof(char));
      if (lvl->own[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //allocating DAT structures
    dat_high= (unsigned char **)malloc(arr_entries_y*sizeof(char *));
    dat_low = (unsigned char **)malloc(arr_entries_y*sizeof(char *));
    if ((dat_high==NULL) || (dat_low==NULL))
        die("level_init: Out of memory");
    int i;
    for (i=0; i < arr_entries_y; i++)
    {
      dat_high[i]= (unsigned char *)malloc(arr_entries_x*sizeof(char));
      dat_low[i] = (unsigned char *)malloc(arr_entries_x*sizeof(char));
      if ((dat_low[i]==NULL) || (dat_high[i]==NULL))
        die("level_init: Out of memory");
    }
  }
  { //allocating WIB structures
    lvl->wib= (unsigned char **)malloc(arr_entries_y*sizeof(char *));
    if (lvl->wib==NULL)
        die("level_init: Out of memory");
    int i;
    for (i=0; i < arr_entries_y; i++)
    {
      lvl->wib[i]= (unsigned char *)malloc(arr_entries_x*sizeof(char));
      if (lvl->wib[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //Allocating "things" structure
    lvl->tng_lookup = (unsigned char ****)malloc(arr_entries_y*sizeof(unsigned char ***));
    lvl->tng_subnums= (unsigned short **)malloc(arr_entries_y*sizeof(unsigned short *));
    lvl->tng_nums =   (unsigned short **)malloc(MAP_SIZE_Y*sizeof(unsigned short *));
    if ((lvl->tng_lookup==NULL) || (lvl->tng_subnums==NULL) || (lvl->tng_nums==NULL))
      die("level_init: Out of memory");
    int i;
    for (i=0; i<arr_entries_y; i++)
    {
      lvl->tng_lookup[i]=(unsigned char ***)malloc (arr_entries_x*sizeof(unsigned char **));
      lvl->tng_subnums[i]=(unsigned short *)malloc (arr_entries_x*sizeof(unsigned short));
      if ((lvl->tng_lookup[i]==NULL) || (lvl->tng_subnums[i]==NULL))
        die("level_init: Out of memory");
    }
    for (i=0; i<MAP_SIZE_Y; i++)
    {
      lvl->tng_nums[i]= (unsigned short *)malloc(MAP_SIZE_X*sizeof(unsigned short));
      if (lvl->tng_nums[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //Allocating action structures
    action_used = (unsigned char **)malloc(1024*sizeof (unsigned char *));
    if (action_used==NULL)
        die("level_init: Out of memory");
  }
  return level_clear(lvl);
}

/*
 * clears the "actions" structure for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_act(LEVEL *lvl)
{
    if (action_used==NULL)
      return false;
    //Clearing single variables
    int i;
    for (i=0; i < 1024; i++)
      action_used[i]=NULL;
    return true;
}
/*
 * clears the "things" structure for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_tng(LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Clearing single variables
    lvl->tng_total_count=0;
    //Clearing pointer arrays
    int i,j;
    for (i=0; i<arr_entries_y; i++)
      for (j=0; j<arr_entries_x; j++)
      {
          lvl->tng_lookup[i][j]=NULL;
          lvl->tng_subnums[i][j]=0;
      }
    for (i=0; i<MAP_SIZE_Y; i++)
      for (j=0; j<MAP_SIZE_X; j++)
          lvl->tng_nums[i][j]=0;

    //Clearing related stats variables
    lvl->stats.hero_gates_count=0;
  return true;
}

/*
 * clears the structures for storing level which do not have separate
 ( clearing function; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_other(LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    int i;
    for (i=0; i < 2048; i++)
    {
      if (lvl->clm[i]!=NULL)
        memset(lvl->clm[i],0,SIZEOF_DK_CLM_REC);
    }

    for (i=0; i < MAP_SIZE_Y; i++)
    {
      if (lvl->slb[i]!=NULL)
        memset(lvl->slb[i],0,MAP_SIZE_X*sizeof(char));
    }
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      if (lvl->own[i]!=NULL)
        memset(lvl->own[i],0,MAP_SIZE_X*sizeof(char));
    }

    for (i=0; i < arr_entries_y; i++)
    {
      if (dat_high[i]!=NULL)
        memset(dat_high[i],0,arr_entries_x*sizeof(char));
      if (dat_low[i]!=NULL)
        memset(dat_low[i],0,arr_entries_x*sizeof(char));
    }

    for (i=0; i < arr_entries_y; i++)
    {
      if (lvl->wib[i]!=NULL)
        memset(lvl->wib[i],0,arr_entries_x*sizeof(char));
    }
    return true;
}

/*
 * clears the whole object for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear(LEVEL *lvl)
{
  short result=true;
  result&=level_clear_act(lvl);
  result&=level_clear_tng(lvl);
  result&=level_clear_other(lvl);
  return result;
}

/*
 * frees structures for storing level; frees only the memory
 * allocated by level_init(); to free the content of loaded level,
 * you must call level_free() first;
 */
short level_deinit()
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    //Freeing SLB structure
    if (lvl->slb!=NULL)
    {
      int x;
      for (x=0; x < MAP_SIZE_X; x++)
          free(lvl->slb[x]);
      free(lvl->slb);
    }

    //Freeing OWN structure
    if (lvl->own!=NULL)
    {
      int x;
      for (x=0; x < MAP_SIZE_X; x++)
          free(lvl->own[x]);
      free(lvl->own);
    }

    //Freeing DAT structure
    if (dat_high!=NULL)
    {
      int x;
      for (x=0; x < arr_entries_x; x++)
          free(dat_high[x]);
      free (dat_high);
    }
    if (dat_low!=NULL)
    {
      int x;
      for (x=0; x < arr_entries_x; x++)
          free(dat_low[x]);
      free (dat_low);
    }

    //Freeing WIB structure
    if (lvl->wib!=NULL)
    {
      int x;
      for (x=0; x < arr_entries_x; x++)
          free(lvl->wib[x]);
      free (lvl->wib);
    }

    //Freeing "things" structure
    int i;
    for (i=0; i<MAP_SIZE_Y; i++)
      free(lvl->tng_nums[i]);
    for (i=0; i<arr_entries_y; i++)
    {
      free(lvl->tng_lookup[i]);
      free(lvl->tng_subnums[i]);
    }
    //Freeing base "things" structure pointers
    free(lvl->tng_lookup);
    free(lvl->tng_subnums);
    free(lvl->tng_nums);

    //Freeing actions structure
    if (action_used!=NULL)
    {
      free(action_used);
    }
    if (lvl->clm!=NULL)
    {
      int x;
      for (x=0; x < 2048; x++)
          free (lvl->clm[x]);
      free (lvl->clm);
    }

    //Final freeing - main lvl object
    free(lvl);
    lvl=NULL;
    return true;
}

/*
 * frees structures for storing level; frees only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared - use level_clear() to set nulls to pointers)
 */
short level_free()
{
  short result=true;
  result&=level_free_tng(lvl);
  return result;
}

/*
 * frees "things" structure for storing level; drops only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared)
 */
short level_free_tng(LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Freeing object arrays
    if ((lvl->tng_subnums!=NULL) && (lvl->tng_lookup!=NULL))
    {
      int cx,cy,k;
      for (cx=0; cx<arr_entries_x; cx++)
      {
          for (cy=0; cy<arr_entries_y; cy++)
          {
            int last_idx=lvl->tng_subnums[cx][cy]-1;
            for (k=last_idx; k >=0; k--)
                thing_del(lvl,cx, cy, k);
          }
      }
    }
  return true;
}

int verify_map(LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int i, j, k;

    //Array for storing players heart count
    int hearts[OWNERS_COUNT];
    for (i=0; i < OWNERS_COUNT; i++)
      hearts[i]=0;

    if (lvl->tng_subnums==NULL)
    {
          message_error("Error: Null internal object tng_subnums!");
          return 0;
    }
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=lvl->tng_subnums[i][j];
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = lvl->tng_lookup[i][j][k];
          if (thing==NULL)
            {
              message_error("Error: Null thing pointer "
                         "at slab %d,%d. (action cancelled))",i/3, j/3);
                return 0;
            };
          if ((thing[6]==THING_TYPE_ITEM) && (thing[7]==ITEM_SUBTYPE_DNHEART))
          {
            if (thing[8]>=OWNERS_COUNT-1) // Orphan heart
            {
              message_error("Warning: Unowned dungeon heart on "
                         "slab %d,%d. (Map saved.)",i/3, j/3);
                return 2;
            } else
            {
              hearts[thing[8]]++;
            }
          }
        }
      }
    }
    for (i=0; i < 5; i++)
    {
      if (hearts[i]>1)
      {
        message_error("Warning: Player %d owns %d dungeon "
             "hearts. (Map saved.)", i, hearts[i]);
          return 2;
      }
    }
    if (!hearts[0])
    {
        message_error("Warning: Human player doesn't have a dungeon"
             " heart. (Map saved)");
      return 2;
    }
    /* Future verifications:
     * Maybe check that no things are trapped in rock/earth?
     */
     
    return 1;
}

/*
 * creates new level;  requies the memory to be allocated by level_init();
 * calls level_clear(), but not level_free() at start;
 */
void start_new_map(LEVEL *lvl)
{
    level_clear(lvl);

    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    int i,j;
    for (i=1; i < MAP_MAXINDEX_Y; i++)
      for (j=1; j < MAP_MAXINDEX_X; j++)
      {
          lvl->slb[i][j]=SLAB_TYPE_EARTH; // Dirt
          lvl->own[i][j]=5;
      }
    // Impenetrable rock
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      lvl->slb[i][0]=SLAB_TYPE_ROCK;
      lvl->slb[i][MAP_MAXINDEX_X]=SLAB_TYPE_ROCK;
      lvl->slb[0][i]=SLAB_TYPE_ROCK;
      lvl->slb[MAP_MAXINDEX_X][i]=SLAB_TYPE_ROCK;
      lvl->own[i][0]=5;
      lvl->own[i][MAP_MAXINDEX_X]=5;
      lvl->own[0][i]=5;
      lvl->own[MAP_MAXINDEX_X][i]=5;
    }
    lvl->tng_total_count=0;
    apt_tot=0;

    for (i=0; i < MAP_SIZE_Y; i++)
      for (j=0; j < MAP_SIZE_X; j++)
          lvl->tng_nums[i][j]=0;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
          lvl->tng_lookup[i][j]=NULL;
          lvl->tng_subnums[i][j]=0;
          dat_high[i][j]=0xfc;
          dat_low[i][j]=0x4c;
          lvl->wib[i][j]=1;
      }
    }

    for (i=0; i < 1024; i++)
      action_used[i]=NULL;
    for (i=0; i < 2048; i++)
    {
      for (j=0; j < SIZEOF_DK_CLM_REC; j++)
          lvl->clm[i][j]=0;
    }
    read_graffiti();
}

void generate_random_map(LEVEL *lvl)
{
     start_new_map(lvl);
    // Impenetrable rock at borders
    int i,j,k,l;
    for (i=1; i < (MAP_MAXINDEX_Y>>1); i++)
      for (j=0; j < MAP_MAXINDEX_X-1; j++)
      {
        int ir=MAP_MAXINDEX_Y-i;
        int rnd_bound=RAND_MAX/((i/2)+1);
        if (rand()<rnd_bound)
        {
          lvl->slb[j][i]=SLAB_TYPE_ROCK;
          lvl->own[j][i]=5;
        }
        if (rand()<rnd_bound)
        {
          lvl->slb[j][MAP_MAXINDEX_X-i]=SLAB_TYPE_ROCK;
          lvl->own[j][MAP_MAXINDEX_X-i]=5;
        }
        if (rand()<rnd_bound)
        {
          lvl->slb[i][j]=SLAB_TYPE_ROCK;
          lvl->own[i][j]=5;
        }
        if (rand()<rnd_bound)
        {
          lvl->slb[MAP_MAXINDEX_X-i][j]=SLAB_TYPE_ROCK;
          lvl->own[MAP_MAXINDEX_X-i][j]=5;
        }
      }
    //Deleting small rocks and enlarging big rocks
    const int slabgrow_bound_val=4;
    int cx,cy;
    for (i=1; i < (MAP_SIZE_X>>1); i++)
      for (j=1; j < (MAP_SIZE_Y>>1); j++)
        for (k=0; k < 4; k++)
        {
            if ((k%2)==0) cx=i; else cx=MAP_MAXINDEX_X-i;
            if ((k>>1)==0) cy=j; else cy=MAP_MAXINDEX_Y-j;
            int nsibln=slab_siblings_oftype(lvl,cx,cy,SLAB_TYPE_ROCK);
            if (nsibln>slabgrow_bound_val)
            {
              lvl->slb[cx][cy]=SLAB_TYPE_ROCK;
              lvl->own[cx][cy]=5;
            } else
            {
              lvl->slb[cx][cy]=SLAB_TYPE_EARTH;
              lvl->own[cx][cy]=5;
            }
        }
    for (i=1; i < MAP_MAXINDEX_X-1; i++)
      for (j=1; j < MAP_MAXINDEX_Y-1; j++)
      {
          int nsibln=slab_siblings_oftype(lvl,i,j,SLAB_TYPE_ROCK);
          if (nsibln<2)
          {
              lvl->slb[i][j]=SLAB_TYPE_EARTH;
              lvl->own[i][j]=5;
          }
          if (nsibln>7)
          {
              lvl->slb[i][j]=SLAB_TYPE_ROCK;
              lvl->own[i][j]=5;
          }
      }
    int num_smears=(rand()%20);
    if (num_smears<10)
    {
      while (num_smears>4) num_smears=num_smears>>1;
      for (l=0;l<num_smears;l++)
      {
        int val=rand();
        int smr_startx=val%MAP_SIZE_X;
        int smr_starty=(val>>8)%MAP_SIZE_Y;
        val=rand();
        int smr_endx=val%MAP_SIZE_X;
        int smr_endy=(val>>8)%MAP_SIZE_Y;
        //FIXME: draw the smear from point to point
      }
    }
    create_clmdattng();
}

void free_map(void)
{
    level_free();
    level_clear(lvl);
    free_graffiti();
}
