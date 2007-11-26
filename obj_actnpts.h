/*
 * obj_actnpts.h
 */

#ifndef ADIKT_OBJACTNPTS_H
#define ADIKT_OBJACTNPTS_H

struct LEVEL;

unsigned char get_actnpt_tilepos_x(unsigned char *actnpt);
short set_actnpt_tilepos_x(unsigned char *actnpt,unsigned char pos_x);
unsigned char get_actnpt_tilepos_y(unsigned char *actnpt);
short set_actnpt_tilepos_y(unsigned char *actnpt,unsigned char pos_y);
char get_actnpt_tilepos_h(unsigned char *actnpt);
short set_actnpt_tilepos_h(unsigned char *actnpt,char pos_h);
short set_actnpt_tilepos(unsigned char *actnpt,unsigned char pos_x,unsigned char pos_y);
char get_actnpt_subtpos_x(unsigned char *actnpt);
short set_actnpt_subtpos_x(unsigned char *actnpt,char pos_x);
char get_actnpt_subtpos_y(unsigned char *actnpt);
short set_actnpt_subtpos_y(unsigned char *actnpt,char pos_y);
char get_actnpt_subtpos_h(unsigned char *actnpt);
short set_actnpt_subtpos_h(unsigned char *actnpt,char pos_h);
short set_actnpt_subtpos(unsigned char *actnpt,char pos_x,char pos_y);

unsigned char *create_actnpt(unsigned int tx, unsigned int ty);

#endif // ADIKT_OBJACTNPTS_H
