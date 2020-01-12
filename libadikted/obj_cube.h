/******************************************************************************/
/** @file obj_cube.h
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Header file. Defines exported routines from obj_column.c
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     23 Dec 2007
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef ADIKT_OBJCUBE_H
#define ADIKT_OBJCUBE_H

#define CUBE_EMPTY1         0x0000
  /*Standard, digable earth */
#define CUBE_EARTHSTD1      0x0001
#define CUBE_EARTHSTD2      0x0002
#define CUBE_EARTHSTD3      0x0003
  /*Bright top earth; it looks like digable from top, */
  /* but is different in profile: brighter at top */
  /*dark left version */
#define CUBE_EARTHBRGTOPDKL 0x0004
  /*not dark at side - for center */
#define CUBE_EARTHBRGTOPCTR 0x0005
  /*dark right version */
#define CUBE_EARTHBRGTOPDKR 0x0006
  /*Green earth; standard but little more green */
#define CUBE_EARTHGREENDKL  0x0007
#define CUBE_EARTHGREENDKR  0x0008
  /*Green earth, but standard on top; green only from sides */
#define CUBE_EARTHGRNSDDKL  0x0009
#define CUBE_EARTHGRNSDCTR  0x000a
#define CUBE_EARTHGRNSDDKR  0x000b
  /*White stones, but green earth on top */
  /*Darker at top */
#define CUBE_WHSTNERTHTPDTP 0x000c
  /*Central - no dark region */
#define CUBE_WHSTNERTHTPCTR 0x000d
  /*White stones, from all sides */
#define CUBE_WHITESTONECTR  0x000e
  /*White stones, but green earth on top */
  /*Darker at bottom */
#define CUBE_WHSTNERTHTPDBT 0x000f
  /*Skull stack - used as lair wall */
  /*Wide stack 1 - 3 tiles */
#define CUBE_BRICK_SKLSTW1L 0x0010
#define CUBE_BRICK_SKLSTW1C 0x0011
#define CUBE_BRICK_SKLSTW1R 0x0012
  /*Thinner stacks */
#define CUBE_BRICK_SKULSTT  0x0013
#define CUBE_BRICK_SKULSTM  0x0014
  /*Wide stack 2 - 3 tiles */
#define CUBE_BRICK_SKLSTW2L 0x0015
#define CUBE_BRICK_SKLSTW2C 0x0016
#define CUBE_BRICK_SKLSTW2R 0x0017
  /*Earth with torch plate */
#define CUBE_EARTH_TORCHPLT 0x0018
  /*Path without any stones */
#define CUBE_PATH_SMOOTH1   0x0019
#define CUBE_PATH_SMOOTH2   0x001a
#define CUBE_PATH_SMOOTH3   0x001b
  /*Path with stones */
#define CUBE_PATH_LRGSTONES 0x001c
#define CUBE_PATH_SMLSTONES 0x001d
  /*01e...025 - lair ground */

/*TODO: add defines here */

#define CUBE_LAIRFLR_CRNRRB 0x0025
/* a little green earth, near water version (whole near water, not bottom) */
#define CUBE_GRNEARTH_NRWTR 0x0026
  /* Liquid Water - the one and only */
#define CUBE_WATER          0x0027
  /* Liquid Lava - darker one */
#define CUBE_LAVADK         0x0028
  /* Liquid Lava - a bit brighter one */
#define CUBE_LAVABR         0x0029
  /* Earth with lava at bottom */
#define CUBE_EARTHBTMLAVA   0x002a
  /* Unpenetrable Rock */
#define CUBE_ROCK1          0x002b
#define CUBE_ROCK2          0x002c
#define CUBE_ROCK3          0x002d
#define CUBE_ROCK_ANY       (0x002b +rnd(3))

/*TODO: add defines here */

/*Standard gold */
#define CUBE_GOLD1          0x0031
#define CUBE_GOLD2          0x0032
#define CUBE_GOLD3          0x0033
/*Gold dark at sides - near lava */
#define CUBE_GOLD_DARKSD1   0x0034
#define CUBE_GOLD_DARKSD2   0x0035
#define CUBE_GOLD_DARKSD3   0x0036

/*TODO: add defines here */

  /* The woman wall relief */
#define CUBE_BRICK_RWOMANTL 0x0054
#define CUBE_BRICK_RWOMANTC 0x0055
#define CUBE_BRICK_RWOMANTR 0x0056
#define CUBE_BRICK_RWOMANUL 0x0057
#define CUBE_BRICK_RWOMANUC 0x0058
#define CUBE_BRICK_RWOMANUR 0x0059
#define CUBE_BRICK_RWOMANML 0x005a
#define CUBE_BRICK_RWOMANMC 0x005b
#define CUBE_BRICK_RWOMANMR 0x005c
#define CUBE_BRICK_RWOMANBL 0x005d
#define CUBE_BRICK_RWOMANBC 0x005e
#define CUBE_BRICK_RWOMANBR 0x005f
  /* The twins wall relief */
#define CUBE_BRICK_RTWINSTL 0x0060
#define CUBE_BRICK_RTWINSTC 0x0061
#define CUBE_BRICK_RTWINSTR 0x0062
#define CUBE_BRICK_RTWINSML 0x0063
#define CUBE_BRICK_RTWINSMC 0x0064
#define CUBE_BRICK_RTWINSMR 0x0065
#define CUBE_BRICK_RTWINSBL 0x0066
#define CUBE_BRICK_RTWINSBC 0x0067
#define CUBE_BRICK_RTWINSBR 0x0068
  /* The pair shaging wall relief */
#define CUBE_BRICK_PAIRSHTL 0x0069
#define CUBE_BRICK_PAIRSHTC 0x006a
#define CUBE_BRICK_PAIRSHTR 0x006b
#define CUBE_BRICK_PAIRSHUL 0x006c
#define CUBE_BRICK_PAIRSHUC 0x006d
#define CUBE_BRICK_PAIRSHUR 0x006e
#define CUBE_BRICK_PAIRSHML 0x006f
#define CUBE_BRICK_PAIRSHMC 0x0070
#define CUBE_BRICK_PAIRSHMR 0x0071
#define CUBE_BRICK_PAIRSHBL 0x0072
#define CUBE_BRICK_PAIRSHBC 0x0073
#define CUBE_BRICK_PAIRSHBR 0x0074

/*TODO: add defines here */

/*Complete darkness - even the light won't make this visible */
#define CUBE_BLACK_ABYSS1   0x00a5
#define CUBE_BLACK_ABYSS2   0x00a6
#define CUBE_BLACK_ABYSS3   0x00a7

/*TODO: add defines here */

  /* The magic mirror wall, for library */
#define CUBE_BRICK_MGMIRRTL 0x00a8
#define CUBE_BRICK_MGMIRRTC 0x00a9
#define CUBE_BRICK_MGMIRRTR 0x00aa
#define CUBE_BRICK_MGMIRRBL 0x00ab
#define CUBE_BRICK_MGMIRRBC 0x00ac
#define CUBE_BRICK_MGMIRRBR 0x00ad

/*TODO: add defines here */

/*Complete darkness - even the light won't make this visible */
#define CUBE_BLACK_ABYSS4   0x00be
#define CUBE_BLACK_ABYSS5   0x00bf

/*TODO: add defines here */

  /* The red eyed head shape wall, for prison */
#define CUBE_BRICK_RDHEADTL 0x00e1
#define CUBE_BRICK_RDHEADTC 0x00e2
#define CUBE_BRICK_RDHEADTR 0x00e3
#define CUBE_BRICK_RDHEADBL 0x00e4
#define CUBE_BRICK_RDHEADBC 0x00e5
#define CUBE_BRICK_RDHEADBR 0x00e6
  /* Floor for torture chamber - the circle for torture spikes */
#define CUBE_FLOOR_TORTCIRC 0x00e7
  /* The skeleton splatted on wall, for torture chamber */
#define CUBE_BRICK_SPSKELTL 0x00e8
#define CUBE_BRICK_SPSKELTC 0x00e9
#define CUBE_BRICK_SPSKELTR 0x00ea
#define CUBE_BRICK_SPSKELML 0x00eb
#define CUBE_BRICK_SPSKELMC 0x00ec
#define CUBE_BRICK_SPSKELMR 0x00ed
#define CUBE_BRICK_SPSKELBL 0x00ee
#define CUBE_BRICK_SPSKELBC 0x00ef
#define CUBE_BRICK_SPSKELBR 0x00f0

/*TODO: add defines here */

  /* The crossed swords wall, for trainig room */
#define CUBE_BRICK_CRSWRDTL 0x00f2
#define CUBE_BRICK_CRSWRDTC 0x00f3
#define CUBE_BRICK_CRSWRDTR 0x00f4
#define CUBE_BRICK_CRSWRDBL 0x00f5
#define CUBE_BRICK_CRSWRDBC 0x00f6
#define CUBE_BRICK_CRSWRDBR 0x00f7

/*TODO: add defines here */

  /* The fire furnace wall, for workshop */
#define CUBE_BRICK_FURNCETL 0x00ff
#define CUBE_BRICK_FURNCETC 0x0100
#define CUBE_BRICK_FURNCETR 0x0101
#define CUBE_BRICK_FURNCEBL 0x0102
#define CUBE_BRICK_FURNCEBC 0x0103
#define CUBE_BRICK_FURNCEBR 0x0104

/*TODO: add defines here */

  /* The god eye in triangle, for scavenger room */
#define CUBE_BRICK_GODEYETL 0x010f
#define CUBE_BRICK_GODEYETC 0x0110
#define CUBE_BRICK_GODEYETR 0x0111
#define CUBE_BRICK_GODEYEBL 0x0112
#define CUBE_BRICK_GODEYEBC 0x0113
#define CUBE_BRICK_GODEYEBR 0x0114

/*TODO: add defines here */

  /* The horned reaper wall relief, for temple */
#define CUBE_BRICK_HORNRPTL 0x012f
#define CUBE_BRICK_HORNRPTC 0x0130
#define CUBE_BRICK_HORNRPTR 0x0131
#define CUBE_BRICK_HORNRPML 0x0132
#define CUBE_BRICK_HORNRPMC 0x0133
#define CUBE_BRICK_HORNRPMR 0x0134
#define CUBE_BRICK_HORNRPBL 0x0135
#define CUBE_BRICK_HORNRPBC 0x0136
#define CUBE_BRICK_HORNRPBR 0x0137

/*TODO: add defines here */

  /* The chicken wall relief, for hatchery */
#define CUBE_BRICK_CHICKNTL 0x014f
#define CUBE_BRICK_CHICKNTC 0x0150
#define CUBE_BRICK_CHICKNTR 0x0151
#define CUBE_BRICK_CHICKNML 0x0152
#define CUBE_BRICK_CHICKNMC 0x0153
#define CUBE_BRICK_CHICKNMR 0x0154
#define CUBE_BRICK_CHICKNBL 0x0155
#define CUBE_BRICK_CHICKNBC 0x0156
#define CUBE_BRICK_CHICKNBR 0x0157

/*TODO: add defines here */

  /* Rocks with glassy "eye" at top */
#define CUBE_EYEDROCKRED    0x017e

/*TODO: add defines here */

  /* The cooperation map wall relief, for barrcaks */
#define CUBE_BRICK_COOPERTL 0x0185
#define CUBE_BRICK_COOPERTC 0x0186
#define CUBE_BRICK_COOPERTR 0x0187
#define CUBE_BRICK_COOPERML 0x0188
#define CUBE_BRICK_COOPRMCR 0x0189
#define CUBE_BRICK_COOPERMR 0x018a
#define CUBE_BRICK_COOPERBL 0x018b
#define CUBE_BRICK_COOPERBC 0x018c
#define CUBE_BRICK_COOPERBR 0x018d

/*TODO: add defines here */

  /* Rock with a little of something else on bottom */
#define CUBE_ROCKBTMDARK    0x0198
#define CUBE_ROCKBTMLAVA    0x0199
  /* Rock floors, color visible on sides */
#define CUBE_ROCKFLOORBLUE1 0x019a
#define CUBE_ROCKFLOORBLUE2 0x019b
#define CUBE_ROCKFLOORBLUE3 0x019c
#define CUBE_ROCKFLOORGREY1 0x019d
#define CUBE_ROCKFLOORGREY2 0x019e
#define CUBE_ROCKFLOORGREY3 0x019f
#define CUBE_ROCKFLOORYELL1 0x01a0
#define CUBE_ROCKFLOORYELL2 0x01a1
#define CUBE_ROCKFLOORGREY4 0x01a2
#define CUBE_ROCKFLOORGREY5 0x01a3
#define CUBE_ROCKFLOORWHITE 0x01a4
#define CUBE_ROCFLOORKRED   0x01a5
  /* Rocks with glassy "eye" at top */
#define CUBE_EYEDROCKBLUE   0x01a6
#define CUBE_EYEDROCKGREEN  0x01a7
#define CUBE_EYEDROCKYELLOW 0x01a8
#define CUBE_EYEDROCKBLINK  0x01a9
#define CUBE_EYEDROCKWHITE  0x01aa
  /*Various color rocks */
#define CUBE_ROCKGREEN      0x01ab
#define CUBE_ROCKBLUE       0x01ac
#define CUBE_ROCKYELLOW     0x01ad
#define CUBE_ROCKBLACK      0x01ae
#define CUBE_ROCKWHITE      0x01af
#define CUBE_ROCKRED        0x01b0
  /*Lair floor */
#define CUBE_LAIR_INSIDE    0x01b1
  /*Wall of stone */
#define CUBE_STONEWALL1     0x01b2
#define CUBE_STONEWALL2     0x01b3
  /*Wood, for briges, but maybe other things */
#define CUBE_WOOD_FLOOR1    0x01b4
#define CUBE_WOOD_FLOOR2    0x01b5
#define CUBE_WOOD_FLOOR3    0x01b6
#define CUBE_WOOD_FLOOR4    0x01b7
#define CUBE_WOOD_FLOOR5    0x01b8
  /*The purple gems */
#define CUBE_GEMS1          0x01b9
#define CUBE_GEMS2          0x01ba
#define CUBE_GEMS3          0x01bb
#define CUBE_GEMS4          0x01bc
#define CUBE_GEMS_ANY       (0x01b9 +rnd(4))

#define CUBE_MAX_INDEX         511

#define CUBE_ANI_WATER      0x0221
  /* Liquid Lava - darker one */
#define CUBE_ANI_LAVADK     0x0222
  /* Liquid Lava - a bit brighter one */
#define CUBE_ANI_LAVABR     0x0223

#define CUBE_ANI_LAVA_ANY   (0x0222 +rnd(2))

#endif /* ADIKT_OBJCUBE_H */
