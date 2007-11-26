/*
 * obj_actnpts.c
 *
 * Functions for maintaining single action points - APT entries.
 *
 */

#include "obj_actnpts.h"
#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"

unsigned char get_actnpt_tilepos_x(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return (char)actnpt[1];
}

short set_actnpt_tilepos_x(unsigned char *actnpt,unsigned char pos_x)
{
    if (actnpt==NULL) return 0;
    actnpt[1]=pos_x;
}

unsigned char get_actnpt_tilepos_y(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return (char)actnpt[3];
}

short set_actnpt_tilepos_y(unsigned char *actnpt,unsigned char pos_y)
{
    if (actnpt==NULL) return 0;
    actnpt[3]=pos_y;
}

short set_actnpt_tilepos(unsigned char *actnpt,unsigned char pos_x,unsigned char pos_y)
{
    if (actnpt==NULL) return 0;
    actnpt[1]=pos_x;
    actnpt[3]=pos_y;
}

char get_actnpt_tilepos_h(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return (char)actnpt[5];
}

short set_actnpt_tilepos_h(unsigned char *actnpt,char pos_h)
{
    if (actnpt==NULL) return 0;
    actnpt[5]=pos_h;
}

char get_actnpt_subtpos_x(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return (char)actnpt[0];
}

short set_actnpt_subtpos_x(unsigned char *actnpt,char pos_x)
{
    if (actnpt==NULL) return 0;
    actnpt[0]=pos_x;
}

char get_actnpt_subtpos_y(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return (char)actnpt[2];
}

short set_actnpt_subtpos_y(unsigned char *actnpt,char pos_y)
{
    if (actnpt==NULL) return 0;
    actnpt[2]=pos_y;
}

short set_actnpt_subtpos(unsigned char *actnpt,char pos_x,char pos_y)
{
    if (actnpt==NULL) return 0;
    actnpt[0]=pos_x;
    actnpt[2]=pos_y;
}

char get_actnpt_subtpos_h(unsigned char *actnpt)
{
    if (actnpt==NULL) return 0;
    return (char)actnpt[4];
}

short set_actnpt_subtpos_h(unsigned char *actnpt,char pos_h)
{
    if (actnpt==NULL) return 0;
    actnpt[4]=pos_h;
}

/*
 * Creates a new action point, prepared to be placed in given coordinates,
 * but without placing it into LEVEL structure
 */
unsigned char *create_actnpt(unsigned int tx, unsigned int ty)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    tx%=arr_entries_x;
    ty%=arr_entries_y;

    unsigned char *actnpt;
    actnpt = (unsigned char *)malloc(SIZEOF_DK_APT_REC);
    if (actnpt==NULL)
      die ("create_actnpt: Out of memory.");
    int i;
    for (i=0; i < SIZEOF_DK_APT_REC; i++)
      actnpt[i]=0;

    for (i=4; i < 17; i++)
      actnpt[i]=0xff;
    actnpt[17]=0;
    actnpt[18]=0;

    set_actnpt_subtpos(actnpt,((tx%MAP_SUBNUM_X)*0x40+0x40),((ty%MAP_SUBNUM_Y)*0x40+0x40));
    set_actnpt_tilepos(actnpt,(unsigned char)tx,(unsigned char)ty);
    return actnpt;
}
