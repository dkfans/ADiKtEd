/*
 * lev_data.c
 *
 * Defines functions for maintaining the level memory structure.
 * This includes creating elements, deleting them and clearing whole structure.
 *
 */

#include "lev_data.h"

#include "globals.h"
#include "internal.h"
#include "obj_slabs.h"
#include "obj_things.h"

struct LEVEL *lvl=NULL;

/*
 * creates object for storing one level; allocates memory and inits
 * the values to zero; drops any previous pointers without deallocating;
 */
short level_init()
{
    lvl=(struct LEVEL *)malloc(sizeof(struct LEVEL));
    if (lvl==NULL)
        die("level_init: Out of memory");
    // map file name
    lvl->fname=(char *)malloc(DISKPATH_SIZE);
    memset(lvl->fname,0,DISKPATH_SIZE*sizeof(char));
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

  { //allocating CLM structures
    lvl->clm = (unsigned char **)malloc(COLUMN_ENTRIES*sizeof(char *));
    if (lvl->clm==NULL)
        die("level_init: Out of memory");
    int i;
    for (i=0; i<COLUMN_ENTRIES; i++)
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
    lvl->dat_high= (unsigned char **)malloc(arr_entries_y*sizeof(char *));
    lvl->dat_low = (unsigned char **)malloc(arr_entries_y*sizeof(char *));
    if ((lvl->dat_high==NULL) || (lvl->dat_low==NULL))
        die("level_init: Out of memory");
    int i;
    for (i=0; i < arr_entries_y; i++)
    {
      lvl->dat_high[i]= (unsigned char *)malloc(arr_entries_x*sizeof(char));
      lvl->dat_low[i] = (unsigned char *)malloc(arr_entries_x*sizeof(char));
      if ((lvl->dat_low[i]==NULL) || (lvl->dat_high[i]==NULL))
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
    lvl->tng_apt_nums =   (unsigned short **)malloc(MAP_SIZE_Y*sizeof(unsigned short *));
    if ((lvl->tng_lookup==NULL) || (lvl->tng_subnums==NULL) || (lvl->tng_apt_nums==NULL))
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
      lvl->tng_apt_nums[i]= (unsigned short *)malloc(MAP_SIZE_X*sizeof(unsigned short));
      if (lvl->tng_apt_nums[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //Allocating "action points" structure
    lvl->apt_lookup = (unsigned char ****)malloc(arr_entries_y*sizeof(unsigned char ***));
    lvl->apt_subnums= (unsigned short **)malloc(arr_entries_y*sizeof(unsigned short *));
    if ((lvl->apt_lookup==NULL) || (lvl->apt_subnums==NULL))
      die("level_init: Out of memory");
    int i;
    for (i=0; i<arr_entries_y; i++)
    {
      lvl->apt_lookup[i]=(unsigned char ***)malloc (arr_entries_x*sizeof(unsigned char **));
      lvl->apt_subnums[i]=(unsigned short *)malloc (arr_entries_x*sizeof(unsigned short));
      if ((lvl->apt_lookup[i]==NULL) || (lvl->apt_subnums[i]==NULL))
        die("level_init: Out of memory");
    }
  }
  { //Allocating WLB structure
    lvl->wlb = (unsigned char **)malloc(MAP_SIZE_Y*sizeof(unsigned char *));
    if (lvl->wlb==NULL)
      die("level_init: Out of memory");
    int i;
    for (i=0; i<MAP_SIZE_Y; i++)
    {
      lvl->wlb[i]=(unsigned char *)malloc(MAP_SIZE_X*sizeof(unsigned char));
      if (lvl->wlb[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  return level_clear(lvl);
}

/*
 * clears the "things" structure for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_tng(struct LEVEL *lvl)
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
          lvl->tng_apt_nums[i][j]=0;

    //Clearing related stats variables
    lvl->stats.hero_gates_count=0;
  return true;
}

/*
 * clears the "actions" structure for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_apt(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Clearing single variables
    lvl->apt_total_count=0;
    //Clearing pointer arrays
    int i,j;
    for (i=0; i<arr_entries_y; i++)
      for (j=0; j<arr_entries_x; j++)
      {
          lvl->apt_lookup[i][j]=NULL;
          lvl->apt_subnums[i][j]=0;
      }
  return true;
}

/*
 * clears the structures for storing level which do not have separate
 * clearing function; drops any old pointers, only file name remains;
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_other(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    int i;
    for (i=0; i<COLUMN_ENTRIES; i++)
      if (lvl->clm[i]!=NULL)
      {
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
      if (lvl->dat_high[i]!=NULL)
        memset(lvl->dat_high[i],0,arr_entries_x*sizeof(char));
      if (lvl->dat_low[i]!=NULL)
        memset(lvl->dat_low[i],0,arr_entries_x*sizeof(char));
    }

    for (i=0; i < arr_entries_y; i++)
    {
      if (lvl->wib[i]!=NULL)
        memset(lvl->wib[i],0,arr_entries_x*sizeof(char));
    }

    for (i=0; i < MAP_SIZE_Y; i++)
    {
      if (lvl->wlb[i]!=NULL)
        memset(lvl->wlb[i],0,MAP_SIZE_X*sizeof(char));
    }
    
    // INF file is easy
    lvl->inf=0x00;

    // TXT script file
    lvl->txt=NULL;
    lvl->txt_lines_count=0;

    // LGT light definition file
    lvl->lgt=NULL;
    lvl->lgt_enties_count=0;
    
    return true;
}

/*
 * clears the whole object for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear(struct LEVEL *lvl)
{
  short result=true;
  result&=level_clear_apt(lvl);
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
      int i;
      for (i=0; i<MAP_SIZE_Y; i++)
          free(lvl->slb[i]);
      free(lvl->slb);
    }

    //Freeing OWN structure
    if (lvl->own!=NULL)
    {
      int i;
      for (i=0; i<MAP_SIZE_Y; i++)
          free(lvl->own[i]);
      free(lvl->own);
    }

    //Freeing DAT structure
    if (lvl->dat_high!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
          free(lvl->dat_high[i]);
      free (lvl->dat_high);
    }
    if (lvl->dat_low!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
          free(lvl->dat_low[i]);
      free (lvl->dat_low);
    }

    //Freeing WIB structure
    if (lvl->wib!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
          free(lvl->wib[i]);
      free (lvl->wib);
    }

    //Freeing "things" structure
    if (lvl->tng_apt_nums!=NULL)
    {
      int i;
      for (i=0; i<MAP_SIZE_Y; i++)
        free(lvl->tng_apt_nums[i]);
      free(lvl->tng_apt_nums);
    }
    if (lvl->tng_lookup!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->tng_lookup[i]);
      free(lvl->tng_lookup);
    }
    if (lvl->tng_subnums!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->tng_subnums[i]);
      free(lvl->tng_subnums);
    }

    //Freeing actions structure
    if (lvl->apt_lookup!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->apt_lookup[i]);
      free(lvl->apt_lookup);
    }
    if (lvl->apt_subnums!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->apt_subnums[i]);
      free(lvl->apt_subnums);
    }

    //Freeing column structure
    if (lvl->clm!=NULL)
    {
      int i;
      for (i=0; i<COLUMN_ENTRIES; i++)
          free (lvl->clm[i]);
      free(lvl->clm);
    }

    //Freeing WLB structure
    if (lvl->wlb!=NULL)
    {
      int i;
      for (i=0; i<MAP_SIZE_Y; i++)
          free(lvl->wlb[i]);
      free(lvl->wlb);
    }
    
    free(lvl->fname);

    //Final freeing - main lvl object
    free(lvl);
    lvl=NULL;
    return true;
}

/*
 * frees "things" structure for storing level; disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared)
 */
short level_free_tng(struct LEVEL *lvl)
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

/*
 * frees "action points" structure for storing level; disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared)
 */
short level_free_apt(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Freeing object arrays
    if ((lvl->apt_subnums!=NULL) && (lvl->apt_lookup!=NULL))
    {
      int cx,cy,k;
      for (cx=0; cx<arr_entries_x; cx++)
      {
          for (cy=0; cy<arr_entries_y; cy++)
          {
            int last_idx=lvl->apt_subnums[cx][cy]-1;
            for (k=last_idx; k >=0; k--)
                actnpt_del(lvl,cx, cy, k);
          }
      }
    }
  return true;
}

/*
 * frees TXT structure for storing level script; disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared)
 */
short level_free_txt(struct LEVEL *lvl)
{
  int idx=lvl->txt_lines_count;
  while (idx>0)
  {
    free(lvl->txt[idx]);
    idx--;
  }
  free(lvl->txt);
  return true;
}

/*
 * frees LGT structure for storing level lights; disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared)
 */
short level_free_lgt(struct LEVEL *lvl)
{
    int i;
    for (i=0;i<lvl->lgt_enties_count;i++)
    {
        free(lvl->lgt[i]);
    }
    free(lvl->lgt);
    lvl->lgt_enties_count=0;
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
  result&=level_free_txt(lvl);
  result&=level_free_lgt(lvl);
  return result;
}

short level_verify(struct LEVEL *lvl, char *actn_name)
{
  char err_msg[LINEMSG_SIZE];
  strcpy(err_msg,"Unknown error");
  short result=VERIF_OK;
  if (result==VERIF_OK)
  {
    result=level_verify_struct(lvl,err_msg);
  }
  if (result==VERIF_OK)
  {
    result=things_verify(lvl,err_msg);
  }
  if (result==VERIF_OK)
  {
    result=slabs_verify(lvl,err_msg);
  }
  if (result==VERIF_OK)
  {
    result=actnpts_verify(lvl,err_msg);
  }
  if (result==VERIF_OK)
  {
    result=columns_verify(lvl,err_msg);
  }
//TODO: verify inf, txt, dat,...
  if (result==VERIF_OK)
  {
    result=level_verify_logic(lvl,err_msg);
  }
  switch (result)
  {
    case VERIF_OK:
      message_info("Level verification passed.");
      return VERIF_OK;
    case VERIF_WARN:
      if (actn_name==NULL)
        message_error("Warning: %s",err_msg);
      else
        message_error("Warning: %s (%s performed)",err_msg,actn_name);
      return VERIF_WARN;
    default:
      if (actn_name==NULL)
      message_error("Error: %s",err_msg);
      else
      message_error("Error: %s (%s cancelled)",err_msg,actn_name);
      return VERIF_ERROR;
  }
}

/*
 * Verifies internal LEVEL structure integrity. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short level_verify_struct(struct LEVEL *lvl, char *err_msg)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Checking base pointers
    if (lvl->tng_subnums==NULL)
    {
          strncpy(err_msg,"Null internal object tng_subnums!",LINEMSG_SIZE);
          return VERIF_ERROR;
    }
    if (lvl->apt_subnums==NULL)
    {
          strncpy(err_msg,"Null internal object apt_subnums!",LINEMSG_SIZE);
          return VERIF_ERROR;
    }
    //Sweeping through structures
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          if (thing==NULL)
          {
              sprintf(err_msg,"Null thing pointer at slab %d,%d.",i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y);
              return VERIF_ERROR;
          }
        }

        int actpt_count=lvl->apt_subnums[i][j];
        for (k=0; k <actpt_count ; k++)
        {
          unsigned char *actnpt = lvl->apt_lookup[i][j][k];
          if (actnpt==NULL)
          {
              sprintf(err_msg,"Null action point pointer at slab %d,%d.",i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y);
              return VERIF_ERROR;
          }
        }
        
      }
    }
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      if (lvl->clm[i]==NULL)
      {
        sprintf(err_msg,"Null CoLuMn entry at index %d.",i);
        return VERIF_ERROR;
      }
    }
  return VERIF_OK;
}

/*
 * Verifies action points parameters. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short actnpts_verify(struct LEVEL *lvl, char *err_msg)
{
  return VERIF_OK;
}

/*
 * Verifies various logic aspects of a map. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short level_verify_logic(struct LEVEL *lvl, char *err_msg)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int i, j, k;

    //Array for storing players heart count
    int hearts[PLAYERS_COUNT];
    for (i=0; i < PLAYERS_COUNT; i++)
      hearts[i]=0;

    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);

          if ((get_thing_type(thing)==THING_TYPE_ITEM) && (get_thing_subtype(thing)==ITEM_SUBTYPE_DNHEART))
          {
            if (get_thing_owner(thing)>=PLAYERS_COUNT-1) // Orphan heart
            {
              sprintf(err_msg,"Unowned dungeon heart on slab %d,%d.",i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y);
              return VERIF_WARN;
            } else
            {
              hearts[get_thing_owner(thing)]++;
            }
          }
        }
        
      }
    }
    for (i=0; i < PLAYERS_COUNT; i++)
    {
      if (hearts[i]>1)
      {
        sprintf(err_msg,"Player %d owns %d dungeon hearts.",i, hearts[i]);
        return VERIF_WARN;
      }
    }
    if (!hearts[0])
    {
        sprintf(err_msg,"Human player doesn't have a dungeon heart.");
        return VERIF_WARN;
    }
    // Future verifications:
    // Maybe check that no things are trapped in rock/earth?
     
  return VERIF_OK;
}

/*
 * creates new level;  requies the memory to be allocated by level_init();
 * calls level_clear(), but not level_free() at start;
 */
void start_new_map(struct LEVEL *lvl)
{
    level_clear(lvl);

    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    // Filling the map with SLAB_TYPE_EARTH
    int i,j;
    for (i=1; i < MAP_MAXINDEX_Y; i++)
      for (j=1; j < MAP_MAXINDEX_X; j++)
      {
          lvl->slb[i][j]=SLAB_TYPE_EARTH; // Dirt
          lvl->own[i][j]=PLAYER_UNSET; //=5, simply ;)
      }
    // Impenetrable rock
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      lvl->slb[i][0]=SLAB_TYPE_ROCK;
      lvl->slb[i][MAP_MAXINDEX_X]=SLAB_TYPE_ROCK;
      lvl->slb[0][i]=SLAB_TYPE_ROCK;
      lvl->slb[MAP_MAXINDEX_X][i]=SLAB_TYPE_ROCK;
      lvl->own[i][0]=PLAYER_UNSET;
      lvl->own[i][MAP_MAXINDEX_X]=PLAYER_UNSET;
      lvl->own[0][i]=PLAYER_UNSET;
      lvl->own[MAP_MAXINDEX_X][i]=PLAYER_UNSET;
    }

    // Clearing things and action points
    lvl->tng_total_count=0;
    lvl->apt_total_count=0;
    for (i=0; i < MAP_SIZE_Y; i++)
      for (j=0; j < MAP_SIZE_X; j++)
          lvl->tng_apt_nums[i][j]=0;
    for (i=0; i < arr_entries_y; i++)
      for (j=0; j < arr_entries_x; j++)
      {
          lvl->tng_lookup[i][j]=NULL;
          lvl->tng_subnums[i][j]=0;
          lvl->apt_lookup[i][j]=NULL;
          lvl->apt_subnums[i][j]=0;
      }

    //Default values for DAT and WIB
    for (i=0; i < arr_entries_y; i++)
      for (j=0; j < arr_entries_x; j++)
      {
          lvl->dat_high[i][j]=0xfc;
          lvl->dat_low[i][j]=0x4c;
          lvl->wib[i][j]=1;
      }

    for (i=0; i < COLUMN_ENTRIES; i++)
    {
      for (j=0; j < SIZEOF_DK_CLM_REC; j++)
          (lvl->clm[i])[j]=0;
    }

    lvl->inf=0x00;

    read_graffiti();
}

void generate_random_map(struct LEVEL *lvl)
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
          lvl->own[j][i]=PLAYER_UNSET;
        }
        if (rand()<rnd_bound)
        {
          lvl->slb[j][MAP_MAXINDEX_X-i]=SLAB_TYPE_ROCK;
          lvl->own[j][MAP_MAXINDEX_X-i]=PLAYER_UNSET;
        }
        if (rand()<rnd_bound)
        {
          lvl->slb[i][j]=SLAB_TYPE_ROCK;
          lvl->own[i][j]=PLAYER_UNSET;
        }
        if (rand()<rnd_bound)
        {
          lvl->slb[MAP_MAXINDEX_X-i][j]=SLAB_TYPE_ROCK;
          lvl->own[MAP_MAXINDEX_X-i][j]=PLAYER_UNSET;
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
        int smr_endx=(val)%MAP_SIZE_X;
        int smr_endy=(val>>8)%MAP_SIZE_Y;
        val=rand();
        int startr=(val)%4+2;
        int endr=(val>>8)%3+1;
        val=rand();
        int distance=ceil(sqrt((smr_startx-smr_endx)*(smr_startx-smr_endx)+(smr_starty-smr_endy)*(smr_starty-smr_endy)));
        int bend=((val)%(distance+1))-(distance>>1);
        slab_draw_smear(lvl,smr_startx,smr_starty,startr,smr_endx,smr_endy,endr,bend,SLAB_TYPE_ROCK);
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
              lvl->own[cx][cy]=PLAYER_UNSET;
            } else
            {
              lvl->slb[cx][cy]=SLAB_TYPE_EARTH;
              lvl->own[cx][cy]=PLAYER_UNSET;
            }
        }
    for (i=1; i < MAP_MAXINDEX_X-1; i++)
      for (j=1; j < MAP_MAXINDEX_Y-1; j++)
      {
          int nsibln=slab_siblings_oftype(lvl,i,j,SLAB_TYPE_ROCK);
          if (nsibln<2)
          {
              lvl->slb[i][j]=SLAB_TYPE_EARTH;
              lvl->own[i][j]=PLAYER_UNSET;
          }
          if (nsibln>7)
          {
              lvl->slb[i][j]=SLAB_TYPE_ROCK;
              lvl->own[i][j]=PLAYER_UNSET;
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

char *get_thing(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    unsigned char *thing=NULL;
    if (num < lvl->tng_subnums[x][y])
      thing = lvl->tng_lookup[x][y][num];
    return thing;
}

void thing_add(struct LEVEL *lvl,unsigned char *thing)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    unsigned int x, y;
    x = get_thing_tilepos_x(thing)%arr_entries_x;
    y = get_thing_tilepos_y(thing)%arr_entries_y;
    lvl->tng_total_count++;
    //setting TNG entries
    lvl->tng_subnums[x][y]++;
    lvl->tng_apt_nums[(x/MAP_SUBNUM_X)][(y/MAP_SUBNUM_Y)]++;
    if (lvl->tng_lookup[x][y]==NULL)
      lvl->tng_lookup[x][y]=(unsigned char **)malloc(sizeof(char *));
    else
      lvl->tng_lookup[x][y]=(unsigned char **)realloc(lvl->tng_lookup[x][y],
                          lvl->tng_subnums[x][y]*sizeof(char *));
    if (lvl->tng_lookup[x][y]==NULL)
      die("thing_add: Out of memory");
    int new_idx=lvl->tng_subnums[x][y]-1;
    lvl->tng_lookup[x][y][new_idx]=thing;
}

/*
 * Removes given thing from the LEVEL structure, updates counter variables.
 * Also frees memory allocated for the thing.
 */
void thing_del(struct LEVEL *lvl,unsigned int x, unsigned int y, unsigned int num)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    unsigned char *thing;
    int i;
    if (num >= lvl->tng_subnums[x][y])
      return;
    lvl->tng_total_count--;
    thing = lvl->tng_lookup[x][y][num];
    free (lvl->tng_lookup[x][y][num]);
    for (i=num; i < lvl->tng_subnums[x][y]-1; i++)
      lvl->tng_lookup[x][y][i]=lvl->tng_lookup[x][y][i+1];
    lvl->tng_subnums[x][y]--;
    lvl->tng_apt_nums[x/3][y/3]--;
    lvl->tng_lookup[x][y]=(unsigned char **)realloc(lvl->tng_lookup[x][y], 
                        lvl->tng_subnums[x][y]*sizeof(char *));
}

unsigned int get_thing_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    return lvl->tng_subnums[x][y];
}

char *get_actnpt(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    unsigned char *actnpt=NULL;
    if (num < lvl->apt_subnums[x][y])
      actnpt = lvl->apt_lookup[x][y][num];
    return actnpt;
}

/*
 * Adds a given action point to the LEVEL structure, updates counter variables
 */
void actnpt_add(struct LEVEL *lvl,unsigned char *actnpt)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    unsigned int x, y;
    x = actnpt[1]%arr_entries_x;
    y = actnpt[3]%arr_entries_y;
    lvl->apt_total_count++;
    //setting APT entries
    unsigned int apt_snum=lvl->apt_subnums[x][y];
    apt_snum++;
    lvl->apt_subnums[x][y]=apt_snum;
    lvl->tng_apt_nums[(x/MAP_SUBNUM_X)][(y/MAP_SUBNUM_Y)]++;
    if (lvl->apt_lookup[x][y]==NULL)
      lvl->apt_lookup[x][y]=(unsigned char **)malloc(sizeof(char *));
    else
      lvl->apt_lookup[x][y]=(unsigned char **)realloc(lvl->apt_lookup[x][y],
                          apt_snum*sizeof(char *));
    if (lvl->apt_lookup[x][y]==NULL)
      die("actnpt_add: Cannot allocate memory.");
    unsigned int new_idx=apt_snum-1;
    lvl->apt_lookup[x][y][new_idx]=actnpt;
}

/*
 * Removes given action point from the LEVEL structure, updates counter variables.
 * Also frees memory allocated for the action point.
 */
void actnpt_del(struct LEVEL *lvl,unsigned int x, unsigned int y, unsigned int num)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    x%=arr_entries_x;
    y%=arr_entries_y;
    unsigned int apt_snum=lvl->apt_subnums[x][y];
    if (num >= apt_snum)
      return;
    lvl->apt_total_count--;
    unsigned char *actnpt;
    actnpt = lvl->apt_lookup[x][y][num];
    free(lvl->apt_lookup[x][y][num]);
    int i;
    apt_snum--;
    for (i=num; i < apt_snum; i++)
      lvl->apt_lookup[x][y][i]=lvl->apt_lookup[x][y][i+1];
    lvl->apt_subnums[x][y]=apt_snum;
    lvl->tng_apt_nums[x/MAP_SUBNUM_X][y/MAP_SUBNUM_Y]--;
    lvl->apt_lookup[x][y]=(unsigned char **)realloc(lvl->apt_lookup[x][y], 
                        apt_snum*sizeof(char *));
}

unsigned int get_actnpt_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    return lvl->apt_subnums[x][y];
}


/*
 * Checks what type the object is. Objects are action points or things.
 * Returns one of OBJECT_TYPE_* value
 */
short get_object_type(struct LEVEL *lvl, unsigned int x, unsigned int y, unsigned int z)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding indices
    x%=arr_entries_x;
    y%=arr_entries_y;
    int tng_num=lvl->tng_subnums[x][y];
    if (z<tng_num) return OBJECT_TYPE_THING;
    int apt_num=lvl->apt_subnums[x][y];
    if (z<(tng_num+apt_num)) return OBJECT_TYPE_ACTNPT;
    return OBJECT_TYPE_NONE;
}

char *get_object(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int z)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding indices
    x%=arr_entries_x;
    y%=arr_entries_y;
    int tng_num=lvl->tng_subnums[x][y];
    if (z<tng_num)
      return get_thing(lvl,x,y,z);
    int apt_num=lvl->apt_subnums[x][y];
    if (z<(tng_num+apt_num))
      return get_actnpt(lvl,x,y,z-tng_num);
    return NULL;
}

void object_del(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int z)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding indices
    x%=arr_entries_x;
    y%=arr_entries_y;
    int tng_num=lvl->tng_subnums[x][y];
    if (z<tng_num)
    {
      thing_del(lvl,x,y,z);
      return;
    }
    int apt_num=lvl->apt_subnums[x][y];
    if (z<(tng_num+apt_num))
    {
      actnpt_del(lvl,x,y,z-tng_num);
      return;
    }
}

unsigned int get_object_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    return lvl->tng_subnums[x][y]+lvl->apt_subnums[x][y];
}

unsigned int get_object_tilnums(struct LEVEL *lvl,unsigned int x,unsigned int y)
{
    if (lvl->tng_apt_nums==NULL) return 0;
    return lvl->tng_apt_nums[x%MAP_SIZE_X][y%MAP_SIZE_Y];
}

short get_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    sx %= arr_entries_x;
    sy %= arr_entries_y;
    return lvl->wib[sx][sy];
}

void set_subtl_wib(struct LEVEL *lvl, short nval, unsigned int sx, unsigned int sy)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    sx %= arr_entries_x;
    sy %= arr_entries_y;
    lvl->wib[sx][sy]=nval;
}

short get_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty)
{
    //Bounding position
    tx %= MAP_SIZE_X;
    ty %= MAP_SUBNUM_Y;
    return lvl->wlb[tx][ty];
}

void set_tile_wlb(struct LEVEL *lvl, short nval, unsigned int tx, unsigned int ty)
{
    //Bounding position
    tx %= MAP_SIZE_X;
    ty %= MAP_SUBNUM_Y;
    lvl->wlb[tx][ty]=nval;
}
