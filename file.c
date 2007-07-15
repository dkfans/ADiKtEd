#include "globals.h"
#include "dernc.h"

struct memory_file
{
    long len;
    unsigned char *content;
};

struct memory_file read_file (char *iname);

static int load_thing (char *fname);
static int load_apt (char *fname);
static int load_clm (char *fname);
static void write_own (char *fname);
static void write_wib (char *fname);
static void write_dat (char *fname);
static void write_clm (char *fname);
static void write_slb (char *fname);
static void write_tng (char *fname);
static void write_apt (char *fname);
static int verify_map (void);
static unsigned char **load_subtile(char *fname, int length,
				    int x, int y,
				    int linesize, int nlines, int lineoffset,
				    int mbytes, int byteoffset);

void load_map (char *fname);
void save_map (char *mapname);
void start_new_map (void);

unsigned char **load_subtile (char *fname, int length, 
			      int x, int y,
			      int linesize, int nlines, int lineoffset,
			      int mbytes, int byteoffset)
{
    struct memory_file mem;
    int i, j;
    long addr=0;
    unsigned char **dest;

    mem = read_file (fname);
    if (mem.len==-1) /* Couldn't open file */
	return NULL;
    if (mem.len < mbytes*nlines*x*y || mem.len != length)
	return NULL;
    dest = (unsigned char **)malloc (x*sizeof (char *));
    if (!dest)
	die ("Out of memory.\n");
    for (i=0; i < x; i++)
    {
	dest[i]=(unsigned char *)malloc (y*sizeof (char));
	if (!dest[i])
	    die ("Out of memory");
    }
    for (i=0; i < y; i++)
    {
	addr += linesize*lineoffset;
	for (j=0; j < x; j++)
	    dest[j][i]=mem.content[addr+j*mbytes+byteoffset];
	addr += (nlines-lineoffset)*linesize;
    }
    free (mem.content);
    return dest;
}

int load_thing (char *fname)
{
    struct memory_file mem;
    int num, i, j;
    unsigned char *ret;
    
    tng_lookup = (unsigned char ****)malloc (255*sizeof (unsigned short **));
    tng_nums = (unsigned short **)malloc (85*sizeof (unsigned short *));
    tng_subnums = (unsigned short **)malloc (255*sizeof (unsigned short *));
    if (!tng_lookup || !tng_nums || !tng_subnums)
	return 0;
    for (i=0; i < 255; i++)
    {
	tng_lookup[i]=(unsigned char ***)malloc (255*sizeof (unsigned short *));
	tng_subnums[i]=(unsigned short *)malloc (255*sizeof (unsigned short));
	if (!tng_lookup[i] || !tng_subnums[i])
	    return 0;
    }
    for (i=0; i < 85; i++)
    {
	tng_nums[i]=(unsigned short *)malloc (85*sizeof (unsigned short));
	if (!tng_nums[i])
	    return 0;
    }
    for (i=0; i < 255; i++)
    {
	for (j=0; j < 255; j++)
	{
	    tng_lookup[i][j]=NULL;
	    tng_subnums[i][j]=0;
	}
    }
    for (i=0; i < 85; i++)
	for (j=0; j < 85; j++)
	    tng_nums[i][j]=0;
    mem = read_file (fname);
    if (mem.len < 2)
	return 0;
    
    num = mem.content[0]+(mem.content[1]<<8);
    tng_tot=0;
    /* Check everything's cushty */
    if (mem.len-2 != num*21)
	return 0;
    for (i=0; i < num; i++)
    {
	ret=(unsigned char *)malloc (21);
	if (!ret)
	    return 0;
	memcpy (ret, mem.content+21*i+2, 21);
	add_thing (ret);
    }
    free (mem.content);
    return 1;
}

int load_clm (char *fname)
{
    struct memory_file mem;
    int i, j;
    
    clm = (unsigned char **)malloc (2048*sizeof (char *));
    if (!clm)
	return 0;
    for (i=0; i < 2048; i++)
    {
	clm[i]=(unsigned char *)malloc (24);
	if (!clm[i])
	    return 0;
	for (j=0; j < 24; j++)
	    clm[i][j]=0;
    }
    mem = read_file (fname);
    if (mem.len != 49160)
	return 0;
    for (i=0; i < 2048; i++)
	memcpy (clm[i], mem.content+24*i+8, 24);
    free (mem.content);
    return 1;
}

/* This *must* be called *after* tng_* are set up */
int load_apt (char *fname)
{
    struct memory_file mem;
    int i, n, nn;
    unsigned char *ret;
    
    mem = read_file (fname);
    if (mem.len < 2)
	return 0;
    
    action_used = (unsigned char **)malloc (1024*sizeof (char *));
    if (!action_used)
	return 0;
    for (i=0; i < 1024; i++)
	action_used[i]=NULL;
    
    apt_tot=0;
    nn = mem.content[0]+(mem.content[1]<<8)+
	(mem.content[2]<<16)+(mem.content[3]<<24);
    /* Check everything's cushty */
    if (mem.len-4 != nn*8)
	return 0;
    for (i=0; i < nn; i++)
    {
	ret=(unsigned char *)malloc (21);
	if (!ret)
	    return 0;
	memcpy (ret, mem.content+8*i+4, 4);
	/* Make sure we know it's an action point */
	memset (ret+4, 0xff, 13);
	memcpy (ret+17, mem.content+8*i+8, 4);
	add_thing (ret);
	n = (ret[20]<<8)+ret[19];
	if (n > 1024)
	    return 0;
	action_used[n-1]=ret;
    }
    if (nn != apt_tot)
    {
	printf ("Internal error in load_apt: nn=%d apt_tot=%d\n", nn, apt_tot);
	exit (1);
    }
    free (mem.content);
    return 1;
}

void write_slb (char *fname)
{
    FILE *fp;
    int i, j;
    fp = fopen (fname, "wb");
    if (!fp)
    {
	sprintf (message, "Can't write to %s\n", fname);
	SLtt_beep();
	return;
    }
    for (i=0; i < 85; i++)
    {
	for (j=0; j < 85; j++)
	{
	    fputc (slb[j][i], fp);
	    fputc (0, fp);
	}
    }
    fclose (fp);
}

void write_own (char *fname)
{
    FILE *fp;
    int i, j, k, l;
    fp = fopen (fname, "wb");
    if (!fp)
    if (!fp)
    {
	sprintf (message, "Can't write to %s\n", fname);
	SLtt_beep();
	return;
    }
    for (i=0; i < 85; i++)
    {
	/* Write each line 3 times */
	for (k=0; k < 3; k++)
	{
	    for (j=0; j < 85; j++)
	    {
		/* Write each character 3 times */
		for (l=0; l < 3; l++)
		    fputc (own[j][i], fp);
	    }
	    fputc (0, fp);
	}
    }
    /* Fill the rest with junk :) */
    for (i=0; i < 256; i++)
	fputc (0, fp);
    fclose (fp);
}

void write_dat (char *fname)
{
    FILE *fp;
    int i, j;
    fp = fopen (fname, "wb");
    if (!fp)
    if (!fp)
    {
	sprintf (message, "Can't write to %s\n", fname);
	SLtt_beep();
	return;
    }
    for (i=0; i < 255; i++)
    {
	for (j=0; j < 255; j++)
	{
	    fputc (dat_low[j][i], fp);
	    fputc (dat_high[j][i], fp);
	}
	fputc (0, fp);
	fputc (0, fp);
    }
    /* Fill the rest with junk :) */
    for (i=0; i < 512; i++)
	fputc (0, fp);
    fclose (fp);
}

void write_clm (char *fname)
{
    FILE *fp;
    int i;
    fp = fopen (fname, "wb");
    if (!fp)
    {
	sprintf (message, "Can't write to %s\n", fname);
	SLtt_beep();
	return;
    }
    fputc (0, fp);
    fputc (8, fp);
    for (i=0; i < 6; i++)
	fputc (0, fp);
    for (i=0; i < 2048; i++)
	fwrite (clm[i], 24, 1, fp);
    fclose (fp);
}

void write_wib (char *fname)
{
    FILE *fp;
    int i, j;
    fp = fopen (fname, "wb");
    if (!fp)
    {
	sprintf (message, "Can't write to %s\n", fname);
	SLtt_beep();
	return;
    }
    for (i=0; i < 255; i++)
    {
	for (j=0; j < 255; j++)
	    fputc (wib[j][i], fp);
	fputc (0, fp);
    }
    /* Fill the rest with junk :) */
    for (i=0; i < 256; i++)
	fputc (0, fp);
    fclose (fp);
}

void write_tng (char *fname)
{
    FILE *fp;
    int i, j, k;
    fp = fopen (fname, "wb");
    if (!fp)
    {
	sprintf (message, "Can't write to %s\n", fname);
	SLtt_beep();
	return;
    }
    fputc ((tng_tot-apt_tot) & 255, fp);
    fputc ((tng_tot-apt_tot)>>8, fp);
    for (i=0; i < 255; i++)
	for (j=0; j < 255; j++)
	    for (k=0; k < tng_subnums[i][j]; k++)
		if (!is_action_point(i,j,k))
		    fwrite (tng_lookup[i][j][k], 21, 1, fp);
    fclose (fp);
}

void write_apt (char *fname)
{
    FILE *fp;
    int i, j, k;
    fp = fopen (fname, "wb");
    if (!fp)
    {
	sprintf (message, "Can't write to %s\n", fname);
	SLtt_beep();
	return;
    }
    fputc (apt_tot & 255, fp);
    fputc ((apt_tot>>8) & 255, fp);
    fputc ((apt_tot>>16) & 255, fp);
    fputc ((apt_tot>>24) & 255, fp);
    for (i=0; i < 255; i++)
    {
	for (j=0; j < 255; j++)
	{
	    for (k=0; k < tng_subnums[i][j]; k++)
	    {
		if (is_action_point(i,j,k))
		{
		    fwrite (tng_lookup[i][j][k], 4, 1, fp);
		    fwrite (tng_lookup[i][j][k]+17, 4, 1, fp);
		}
	    }
	}
    }
    fclose (fp);
}

void save_map (char *mapname)
{
    char *fname;
    
    strcpy (message, "Map saved.");
    if (!verify_map())
	return;

    create_clmdattng();
    draw_graffiti();
    
    /*
     * Write message now - it will only be displayed if nothing
     * goes wrong
     */
    fname = (char *)malloc (strlen (mapname)+5);
    if (!fname)
	die ("Out of memory.");
    sprintf (fname, "%s.own", mapname);
    write_own (fname);
    sprintf (fname, "%s.slb", mapname);
    write_slb (fname);
    sprintf (fname, "%s.dat", mapname);
    write_dat (fname);
    sprintf (fname, "%s.clm", mapname);
    write_clm (fname);
    sprintf (fname, "%s.tng", mapname);
    write_tng (fname);
    sprintf (fname, "%s.apt", mapname);
    write_apt (fname);
    sprintf (fname, "%s.wib", mapname);
    write_wib (fname);
    
    read_graffiti();
}

int verify_map (void)
{
    int i, j, k;
    unsigned char *thing;
    int hearts[5];
    
    for (i=0; i < 5; i++)
	hearts[i]=0;
    
    for (i=0; i < 255; i++)
    {
	for (j=0; j < 255; j++)
	{
	    for (k=0; k < tng_subnums[i][j]; k++)
	    {
		thing = tng_lookup[i][j][k];
		if (thing[6]==1 && thing[7]==5)
		{
		    if (thing[8]>=5) /* Orphan heart */
		    {
			sprintf (message, "Warning: Unowned dungeon heart on "
				 "slab %d,%d. (Map saved.)",
				 i/3, j/3);
			SLtt_beep();
			return 2;
		    }
		    hearts[thing[8]]++;
		}
	    }
	}
    }
    for (i=0; i < 5; i++)
    {
	if (hearts[i]>1)
	{
	    sprintf (message, "Warning: Player %d owns %d dungeon "
		     "hearts. (Map saved.)", i, hearts[i]);
		     
	    SLtt_beep();
	    return 2;
	}
    }
    if (!hearts[0])
    {
	sprintf (message, "Warning: Human player doesn't have a dungeon"
		 " heart. (Map saved)");
	SLtt_beep();
	return 2;
    }
    /* Future verifications:
     * Maybe check that no things are trapped in rock/earth?
     */
     
    return 1;
}

void load_map (char *fname)
{
    char *fnames;
    int i, j;
    
    for (i=0; i < 3; i++)
	for (j=0; j < 3; j++)
	    vistng[i][j]=0;
    
    if (fname==NULL)
    {
	start_new_map();
	return;
    }
    fnames = (char *)malloc (strlen (fname)+5);
    if (!fnames)
	die ("Out of memory.");
    sprintf (fnames, "%s.slb", fname);
    slb = load_subtile (fnames, 14450, 85, 85, 170, 1, 0, 2, 0);
    if (!slb)
    {
	sprintf (message, "Couldn't load %s", fnames);
	SLtt_beep();
	free_map();
	start_new_map();
	return;
    }
    
    sprintf (fnames, "%s.own", fname);
    own = load_subtile (fnames, 65536, 85, 85, 256, 3, 0, 3, 0); 
    
    if (!own)
    {
	sprintf (message, "Couldn't load %s", fnames);
	SLtt_beep();
	free_map();
	start_new_map();
	return;
    }
    sprintf (fnames, "%s.dat", fname);
    dat_low = load_subtile (fnames, 131072, 255, 255, 512, 1, 0, 2, 0);
    dat_high = load_subtile (fnames, 131072, 255, 255, 512, 1, 0, 2, 1);
    if (!dat_low || !dat_high)
    {
	sprintf (message, "Couldn't load %s", fnames);
	SLtt_beep();
	free_map();
	start_new_map();
	return;
    }

    sprintf (fnames, "%s.tng", fname);
    if (!load_thing (fnames))
    {
	sprintf (message, "Couldn't load %s", fnames);
	SLtt_beep();
	free_map();
	start_new_map();
	return;
    }

    sprintf (fnames, "%s.apt", fname);
    if (!load_apt (fnames))
    {
	sprintf (message, "Couldn't load %s", fnames);
	SLtt_beep();
	free_map();
	start_new_map();
	return;
    }

    sprintf (fnames, "%s.clm", fname);
    if (!load_clm (fnames))
    {
	sprintf (message, "Couldn't load %s", fnames);
	SLtt_beep();
	free_map();
	start_new_map();
	return;
    }
    
    sprintf (fnames, "%s.wib", fname);
    wib = load_subtile (fnames, 65536, 255, 255, 256, 1, 0, 1, 0);
    if (!wib)
    {
	sprintf (message, "Couldn't load %s", fnames);
	SLtt_beep();
	free_map();
	start_new_map();
	return;
    }
    read_graffiti();
}

void start_new_map (void)
{
    int i, j;
    slb = (unsigned char **)malloc (85*sizeof (char *));
    own = (unsigned char **)malloc (85*sizeof (char *));
    if (!slb || !own)
	die ("Out of memory.");
    for (i=0; i < 85; i++)
    {
	slb[i] = (unsigned char *)malloc (85*sizeof(char));
	own[i] = (unsigned char *)malloc (85*sizeof (char));
	if (!own[i] || !slb[i])
	    die ("Out of memory.");
    }
    dat_high=(unsigned char **)malloc (255*sizeof (char *));
    dat_low=(unsigned char **)malloc (255*sizeof (char *));
    wib = (unsigned char **)malloc (255*sizeof (char *));
    if (!dat_high || !dat_low || !wib)
	die ("Out of memory");
    for (i=0; i < 255; i++)
    {
	dat_high[i] = (unsigned char *)malloc (255*sizeof(char));
	dat_low[i] = (unsigned char *)malloc (255*sizeof (char));
	wib[i]=(unsigned char *)malloc (255*sizeof (char));
	if (!dat_low[i] || !dat_high[i] || !wib[i])
	    die ("Out of memory.");
    }
    for (i=1; i < 84; i++)
    {
	for (j=1; j < 84; j++)
	{
	    slb[i][j]=2; /* Dirt */
	    own[i][j]=5;
	}
    }
    /* Impenetrable rock */
    for (i=0; i < 85; i++)
    {
	slb[i][0]=0;
	slb[i][84]=0;
	slb[0][i]=0;
	slb[84][i]=0;
	own[i][0]=5;
	own[i][84]=5;
	own[0][i]=5;
	own[84][i]=5;
    }
    tng_tot=0;
    apt_tot=0;
    action_used = (unsigned char **)malloc (1024*sizeof (unsigned char *));
    for (i=0; i < 1024; i++)
	action_used[i]=NULL;
    tng_lookup = (unsigned char ****)malloc (255*sizeof (unsigned char ***));
    tng_subnums= (unsigned short **)malloc (255*sizeof (unsigned short *));
    tng_nums = (unsigned short **)malloc (85*sizeof (unsigned short *));
    if (!tng_lookup || !tng_nums)
	die ("Out of memory.");
    for (i=0; i < 255; i++)
    {
	tng_lookup[i]=(unsigned char ***)malloc (255*sizeof (unsigned char **));
	tng_subnums[i]=(unsigned short *)malloc (255*sizeof (unsigned short));
	if (!tng_lookup[i] || !tng_subnums[i])
	    die ("Out of memory.");
    }
    for (i=0; i < 85; i++)
    {
	tng_nums[i]=(unsigned short *)malloc (85*sizeof (unsigned short));
	if (!tng_nums[i])
	    die ("Out of memory.");
    }
    for (i=0; i < 85; i++)
	for (j=0; j < 85; j++)
	    tng_nums[i][j]=0;
    for (i=0; i < 255; i++)
    {
	for (j=0; j < 255; j++)
	{
	    tng_lookup[i][j]=NULL;
	    tng_subnums[i][j]=0;
	    dat_high[i][j]=0xfc;
	    dat_low[i][j]=0x4c;
	    wib[i][j]=1;
	}
    }
    clm = (unsigned char **)malloc (2048*sizeof (char *));
    if (!clm)
	die ("Out of memory.");
    for (i=0; i < 2048; i++)
    {
	clm[i]=(unsigned char *)malloc (24);
	if (!clm[i])
	    die ("Out of memory.");
	for (j=0; j < 24; j++)
	    clm[i][j]=0;
    }
    read_graffiti();
}

/* Read a file, possibly compressed, and decompress it if necessary */
struct memory_file read_file (char *iname)
{
    FILE *ifp;
    long plen, ulen;
    void *packed, *unpacked;
    char buffer[80];
    struct memory_file ret;

    ifp = fopen(iname, "rb");
    if (!ifp) 
    {
	ret.content=NULL;
	ret.len=-1;
	return ret;
    }
    fseek (ifp, 0L, SEEK_END);
    plen = ftell (ifp);
    rewind (ifp);
    packed = malloc(plen);
    if (!packed) 
	die ("Out of memory.");
    fread (packed, 1, plen, ifp);
    fclose (ifp);
    ulen = rnc_ulen (packed);
    if (ulen < 0) 
    {
	if (ulen == -1) /* File wasn't RNC to start with */
	{
	    ret.content = (unsigned char *)packed;
	    ret.len = plen;
	    return ret;
	}
	if (strlen (iname) > 40)
	    iname[40]=0;
	sprintf (buffer, "read_file: %s", rnc_error (ulen));
	die (buffer);
    }
    unpacked = malloc(ulen);
    if (!unpacked) 
	die ("Out of memory.");
    ulen = rnc_unpack (packed, unpacked, 0);
    if (ulen < 0)
    {
	if (strlen (iname) > 40)
	    iname[40]=0;
	sprintf (buffer, "read_file: %s", rnc_error (ulen));
	die (buffer);
    }

    free (packed);
    ret.content=(unsigned char *)unpacked;
    ret.len=ulen;
    return ret;
}

