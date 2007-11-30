/*
 * graffiti.c
 *
 * Module for handling wall graffities.
 *
 */

#include "graffiti.h"

#include "globals.h"
#include "lev_data.h"
#include "internal.h"
#include "obj_things.h"

typedef struct
{
    char *string;
    int d;
    int x, y;
    int fin;
} adikt_graffiti;

adikt_graffiti **graffiti_data=NULL;
int graffiti_num=0;

void read_graffiti (void);
void draw_graffiti (void);
int add_graffiti (int x, int y, char *string, int d);
void delete_graffiti (int n);
char *get_graffiti (int n);
int is_graffiti (int x, int y);
void free_graffiti (void);

void free_graffiti (void)
{
    int i;
    for (i=0; i < graffiti_num; i++)
    {
      free (graffiti_data[i]->string);
      free (graffiti_data[i]);
    }
    if (graffiti_num)
      free (graffiti_data);
    graffiti_data=NULL;
    graffiti_num=0;
}

int is_graffiti (int x, int y)
{
    int i;
    adikt_graffiti *graf;
    for (i=0; i < graffiti_num; i++)
    {
      graf = graffiti_data[i];
      if (graf->d && x == graf->x && y>=graf->y
          && y <=graf->fin)
          return i;
      if (!graf->d && y == graf->y && x>=graf->x
          && x <=graf->fin)
          return i;
    }
    return -1;
}

char *get_graffiti (int n)
{
    if (n >= graffiti_num || n==-1)
      return "";
    return graffiti_data[n]->string;
}

void delete_graffiti (int n)
{
    int i;
    if (n >= graffiti_num)
      return;
    free (graffiti_data[n]->string);
    free (graffiti_data[n]);
    for (i=n; i < graffiti_num-1; i++)
      graffiti_data[i]=graffiti_data[i+1];
    graffiti_num--;
    if (graffiti_num)
    {
      graffiti_data = (adikt_graffiti **)realloc (graffiti_data, (graffiti_num)*
                         sizeof (adikt_graffiti *));
      if (!graffiti_data)
          die ("Out of memory");
    }
    else
    {
      free (graffiti_data);
      graffiti_data=NULL;
    }
}

int add_graffiti (int x, int y, char *string, int d)
{
    adikt_graffiti *graf;
    int len=0, i, l;
    
    l = strlen (string);
    for (i=0; i < l; i++)
      len+=chars[(unsigned char)string[i]][0];
    if ((d ? y*3:x*3)+len+l > 255)
      return 0;
    if (!graffiti_num)
      graffiti_data = (adikt_graffiti **)malloc (sizeof (adikt_graffiti *));
    else
      graffiti_data = (adikt_graffiti **)realloc (graffiti_data, (graffiti_num+1)*
                         sizeof (adikt_graffiti *));
    graf = (adikt_graffiti *)malloc (sizeof (adikt_graffiti));
    if (!graffiti_data || !graf)
      die ("Out of memory");
    graffiti_data[graffiti_num++]=graf;
    graf->x=x;
    graf->y=y;
    graf->d=d;
    graf->string = strdup (string);
    graf->fin = (d ? y : x)+(len+l)/3;
    return 1;
}

void read_graffiti (void)
{
    unsigned char *clm_check;
    unsigned char *thing;
    int i, j, k;
    int x, y, bx, by, d;
    adikt_graffiti *graf;
    /* Check this is one of our files */
    
    graffiti_data = NULL;
    graffiti_num=0;
    
    clm_check = lvl->clm[2047];
    for (i=0; i < 24; i++)
      if (clm_check[i] != 0xff)
          return;
    
    for (x=0; x < MAP_SIZE_X; x++)
    {
      for (y=0; y < MAP_SIZE_Y; y++)
      {
          bx = x*3;
          by = y*3;
          d = get_dat_subtile(lvl, bx, by)-1500;
          if (!d || d==1) // Got some graffiti
          {
            if (!graffiti_num)
                graffiti_data = (adikt_graffiti **)malloc (sizeof (adikt_graffiti *));
            else
                graffiti_data = (adikt_graffiti **)realloc (graffiti_data, (graffiti_num+1)*
                                 sizeof (adikt_graffiti *));
            graf =(adikt_graffiti *) malloc (sizeof (adikt_graffiti));
            if (!graffiti_data || !graf)
                die ("Out of memory");
            graffiti_data[graffiti_num++]=graf;
            graf->x=x;
            graf->y=y;
            graf->d=d;
            i=0;
            while (get_dat_subtile(lvl, bx+(d ? 1 : i), by+(d ? i : 1))-1503 &&
                   bx+(d ? 1 : i) < 255 && by+(d ? i : 1) < 255)
                i++;
            graf->string = (char *)malloc (i+1);
            if (!graf->string)
                die ("Out of memory");
            for (j=0; j < i; j++)
                graf->string[j]=(char)
                  (get_dat_subtile(lvl, bx+(d ? 1 : j), by+(d ? j : 1))-1503);
            graf->string[i]=0;
            i=0;
            while (get_dat_subtile(lvl, bx+(d ? 1 : i), by+(d ? i : 1))-1502 &&
                   bx+(d ? 1 : i) < 255 && by+(d ? i : 1) < 255)
                i++;
            graf->fin = (d ? y : x)+i/3;
            for (j=0; j <= i/3; j++)
            {
                // Search for "our" torch
                for (k=get_thing_subnums(lvl,bx+(d?1:j*3+1),by+(d?j*3+1:1))-1; k >=0; k--)
                {
                  thing = get_thing(lvl,bx+(d?1:j*3+1),by+(d?j*3+1:1),k);
                  // Check if it's ours
                  if ((get_thing_type(thing)==THING_TYPE_ITEM) && (get_thing_subtype(thing)==ITEM_SUBTYPE_TORCH)
                       && (get_thing_tilepos_h(thing)==127))
                  {
                      // Change the slab and remove the torch
                      lvl->own[x+(d?0:j)][y+(d?j:0)]=thing[10];
                      lvl->slb[x+(d?0:j)][y+(d?j:0)]=thing[11];
                      update_tngdat (x+(d?0:j), y+(d?j:0));
                  }
                  thing_del(lvl,bx+(d?1:j*3+1),by+(d?j*3+1:1),k);
                }
            }
          }
      }
    }
}

void draw_graffiti (void)
{
    int x, y, d;
    char *string;
    int n, i, j, l, bx, by, bx2, by2, len=0;
    unsigned char *thing;
    static int dy[2][3]={{0,1,2}, {0,0,0}};
    static int dx[2][3]={{0,0,0}, {0,1,2}};
    
    for (n=0; n < graffiti_num; n++)
    {
      string = graffiti_data[n]->string;
      x = graffiti_data[n]->x;
      y = graffiti_data[n]->y;
      d = graffiti_data[n]->d;
      
      bx=x*3;
      by=y*3;
      l = strlen (string);
      len=0;
      for (i=0; i < l; i++)
          len+=chars[(unsigned char)string[i]][0];
      for (i=0; i < len+1+l; i++)
      {
          set_dat_subtile(lvl, bx+i*(1-d)+dx[d][0], by+i*d+dy[d][0], 1700);
          if (i<l)
            set_dat_subtile(lvl, bx+i*(1-d)+dx[d][1], by+i*d+dy[d][1], 1503+string[i]);
          else if (i==l) // Terminate string
            set_dat_subtile(lvl, bx+i*(1-d)+dx[d][1], by+i*d+dy[d][1], 1503); 
          else
            set_dat_subtile(lvl, bx+i*(1-d)+dx[d][1], by+i*d+dy[d][1], 1700);
          set_dat_subtile(lvl, bx+i*(1-d)+dx[d][2], by+i*d+dy[d][2], 1700);
          if (i%3==1)
          {
            thing = create_item(bx+(d ? 1 : i), by+(d ? i : 1), ITEM_SUBTYPE_TORCH);
            set_thing_tilepos_h(thing,127);
            thing[10]=lvl->own[x+(i/3)*(1-d)][y+(i/3)*d]; /* Save the data */
            thing[11]=lvl->slb[x+(i/3)*(1-d)][y+(i/3)*d];
            thing_add(lvl,thing);
            lvl->own[x+(i/3)*(1-d)][y+(i/3)*d]=5;
            lvl->slb[x+(i/3)*(1-d)][y+(i/3)*d]=0;
          }
      }
      set_dat_subtile(lvl, bx, by, 1500+d);
      if (i%3==1)
      {
          thing = create_item (bx+(d ? 1 : i), by+(d ? i : 1), ITEM_SUBTYPE_TORCH);
          set_thing_tilepos_h(thing,127);
          thing[10]=lvl->own[x+(i/3)*(1-d)][y+(i/3)*d]; // Save the data
          thing[11]=lvl->slb[x+(i/3)*(1-d)][y+(i/3)*d];
          thing_add(lvl,thing);
          lvl->own[x+(i/3)*(1-d)][y+(i/3)*d]=5;
          lvl->slb[x+(i/3)*(1-d)][y+(i/3)*d]=0;
      }
      /* Put in the graffiti terminating jobsworth */
      i--;
      set_dat_subtile(lvl, bx+i*(1-d)+dx[d][1], by+i*d+dy[d][1], 1502);
      if (!d)
      {
          bx2 = bx+len+l-1;
          bx++;
          for (i=0; i < l; i++)
          {
            for (j=0; j < chars[(unsigned char)string[i]][0]; j++)
            {
                set_dat_subtile(lvl, bx++, by+2, 1700+chars [(unsigned char)string[i]][j+1]);
                set_dat_subtile(lvl, bx2--, by, 1700+chars [(unsigned char)string[i]][j+1]);
            }
            if (i!=l-1)
            {
                set_dat_subtile(lvl, bx++, by+2, 1700);
                set_dat_subtile(lvl, bx2--, by, 1700);
            }
          }
      }
      else
      {
          by2 = by+len+l-1;
          by++;
          for (i=0; i < l; i++)
          {
            for (j=0; j < chars[(unsigned char)string[i]][0]; j++)
            {
                set_dat_subtile(lvl, bx, by++, 1700+chars [(unsigned char)string[i]][j+1]);
                set_dat_subtile(lvl, bx+2, by2--, 1700+chars [(unsigned char)string[i]][j+1]);
            }
            if (i!=l-1)
            {
                set_dat_subtile(lvl, bx, by++, 1700);
                set_dat_subtile(lvl, bx+2, by2--, 1700);
            }
          }
      }
      free (string);
      free (graffiti_data[n]);
    }
    free (graffiti_data);
    graffiti_data=NULL;
    graffiti_num=0;
}
