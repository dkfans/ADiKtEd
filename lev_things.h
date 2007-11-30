/*
 * lev_things.h
 */

#ifndef ADIKT_LEVTHINGS_H
#define ADIKT_LEVTHINGS_H

struct LEVEL;

void remove_room_things (int x, int y);
void remove_noncrucial_room_things(int x, int y);

short things_verify(struct LEVEL *lvl, char *err_msg);

void set_pillar(int x, int y, int pillar);
void set_pillar_thing(int x, int y, int pillar, int thingnum, int thinghgt);

#endif // ADIKT_LEVTHINGS_H
