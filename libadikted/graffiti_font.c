/******************************************************************************/
/** @file graffiti_font.c
 * Wall graffiti fonts for levels.
 * @par Purpose:
 *     Defines fonts for the graffiti module, for painting grafitties on walls.
 * @par Comment:
 *     None.
 * @author   Jon Skeet, Tomasz Lis
 * @date     14 Oct 1997 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include "globals.h"

const unsigned char font_any_chnull[] = {0,0x00};

#define f8(c,a1,a2,a3,a4,a5,a6,a7,a8) const static unsigned char font_adisize8_ch##c[] = {8,0x##a1,0x##a2,0x##a3,0x##a4,0x##a5,0x##a6,0x##a7,0x##a8}

f8(notdef,55,AA,55,AA,55,AA,55,AA);

f8(32,00,00,00,00,00,00,00,00);
f8(33,00,00,00,7a,00,00,00,00);
f8(34,00,00,60,00,00,60,00,00);
f8(35,00,24,7e,24,24,7e,24,00);
f8(36,00,00,3a,2a,7f,2a,2e,00);
f8(37,00,62,64,08,10,26,46,00);
f8(38,00,04,2a,52,2a,04,0a,00);
f8(39,00,00,00,20,40,00,00,00);
f8(40,00,00,00,00,3c,42,00,00);
f8(41,00,00,42,3c,00,00,00,00);
f8(42,00,00,08,2a,1c,2a,08,00);
f8(43,00,00,08,08,3e,08,08,00);
f8(44,00,00,00,01,06,00,00,00);
f8(45,00,00,08,08,08,08,08,00);
f8(46,00,00,00,06,06,00,00,00);
f8(47,00,00,02,04,08,10,20,00);
f8(48,00,3c,46,4a,52,62,3c,00);
f8(49,00,00,22,42,7e,02,02,00);
f8(50,00,26,4a,4a,4a,4a,32,00);
f8(51,00,24,42,42,52,52,2c,00);
f8(52,00,0c,14,24,7e,04,04,00);
f8(53,00,74,52,52,52,52,4c,00);
f8(54,00,3c,52,52,52,52,0c,00);
f8(55,00,40,40,46,48,50,60,00);
f8(56,00,2c,52,52,52,52,2c,00);
f8(57,00,30,4a,4a,4a,4a,3c,00);
f8(58,00,00,00,12,00,00,00,00);
f8(59,00,00,01,26,00,00,00,00);
f8(60,00,00,00,08,14,22,00,00);
f8(61,00,00,14,14,14,14,14,00);
f8(62,00,00,00,22,14,08,00,00);
f8(63,00,20,40,40,4a,50,20,00);
f8(64,00,3c,42,5a,6a,5a,38,00);
f8(65,00,3e,48,48,48,48,3e,00);
f8(66,00,7e,52,52,52,52,2c,00);
f8(67,00,3c,42,42,42,42,24,00);
f8(68,00,7e,42,42,42,24,18,00);
f8(69,00,7e,52,52,52,52,42,00);
f8(70,00,7e,50,50,50,50,40,00);
f8(71,00,3c,42,42,4a,4a,2c,00);
f8(72,00,7e,10,10,10,10,7e,00);
f8(73,00,00,42,42,7e,42,42,00);
f8(74,00,0c,02,02,02,02,7c,00);
f8(75,00,7e,10,10,28,44,02,00);
f8(76,00,7e,02,02,02,02,02,00);
f8(77,00,7e,20,10,10,20,7e,00);
f8(78,00,7e,20,10,08,04,7e,00);
f8(79,00,3c,42,42,42,42,3c,00);
f8(80,00,7e,48,48,48,48,30,00);
f8(81,00,3c,42,4a,46,42,3c,00);
f8(82,00,7e,48,48,48,4e,30,00);
f8(83,00,24,52,52,52,52,0c,00);
f8(84,40,40,40,7e,40,40,40,00);
f8(85,00,7c,02,02,02,02,7c,00);
f8(86,00,78,04,02,02,04,78,00);
f8(87,00,7c,02,04,04,02,7c,00);
f8(88,00,42,24,18,18,24,42,00);
f8(89,40,20,10,0e,10,20,40,00);
f8(90,00,42,46,4a,52,62,42,00);
f8(91,00,00,00,00,7e,42,42,00);
f8(92,00,20,10,08,04,02,00,00);
f8(93,00,42,42,7e,00,00,00,00);
f8(94,00,10,20,7e,20,10,00,00);
f8(95,01,01,01,01,01,01,01,01);
f8(96,00,12,3e,52,52,42,22,00);
f8(97,00,04,2a,2a,2a,1e,00,00);
f8(98,00,00,7e,12,12,12,0c,00);
f8(99,00,00,1c,22,22,22,00,00);
f8(100,00,0c,12,12,12,7e,00,00);
f8(101,00,1c,2a,2a,2a,12,00,00);
f8(102,00,00,00,3e,50,40,00,00);
f8(103,00,18,25,25,25,3e,00,00);
f8(104,00,7e,10,10,10,0e,00,00);
f8(105,00,00,12,5e,02,00,00,00);
f8(106,00,00,02,01,01,5e,00,00);
f8(107,00,00,7e,18,24,02,00,00);
f8(108,00,00,00,7c,02,02,00,00);
f8(109,00,3e,20,1e,20,1e,00,00);
f8(110,00,3e,20,20,20,1e,00,00);
f8(111,00,1c,22,22,22,1c,00,00);
f8(112,00,3f,24,24,24,18,00,00);
f8(113,00,18,24,24,24,3f,01,00);
f8(114,00,00,1e,20,20,20,00,00);
f8(115,00,12,2a,2a,2a,04,00,00);
f8(116,00,00,20,7c,22,02,00,00);
f8(117,00,3c,02,02,02,3c,00,00);
f8(118,00,30,0c,02,0c,30,00,00);
f8(119,00,3c,02,1c,02,3c,00,00);
f8(120,00,22,14,08,14,22,00,00);
f8(121,00,38,05,05,05,3e,00,00);
f8(122,00,22,26,2a,32,22,00,00);
f8(123,00,00,10,10,6e,42,42,00);
f8(124,00,00,00,00,7e,00,00,00);
f8(125,00,42,42,6e,10,10,00,00);
f8(126,00,00,20,40,20,40,00,00);

const int font_adisize8_height=8;
const unsigned char *font_adisize8_chars[] = {
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // 00
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // 08
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // 10
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // 18
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_ch32    ,font_adisize8_ch33    ,font_adisize8_ch34, // 20
    font_adisize8_ch35    ,font_adisize8_ch36    ,font_adisize8_ch37    ,font_adisize8_ch38    ,font_adisize8_ch39    ,font_adisize8_ch40    ,font_adisize8_ch41, // 28
    font_adisize8_ch42    ,font_adisize8_ch43    ,font_adisize8_ch44    ,font_adisize8_ch45    ,font_adisize8_ch46    ,font_adisize8_ch47    ,font_adisize8_ch48, // 30
    font_adisize8_ch49    ,font_adisize8_ch50    ,font_adisize8_ch51    ,font_adisize8_ch52    ,font_adisize8_ch53    ,font_adisize8_ch54    ,font_adisize8_ch55,
    font_adisize8_ch56    ,font_adisize8_ch57    ,font_adisize8_ch58    ,font_adisize8_ch59    ,font_adisize8_ch60    ,font_adisize8_ch61    ,font_adisize8_ch62, // 40
    font_adisize8_ch63    ,font_adisize8_ch64    ,font_adisize8_ch65    ,font_adisize8_ch66    ,font_adisize8_ch67    ,font_adisize8_ch68    ,font_adisize8_ch69,
    font_adisize8_ch70    ,font_adisize8_ch71    ,font_adisize8_ch72    ,font_adisize8_ch73    ,font_adisize8_ch74    ,font_adisize8_ch75    ,font_adisize8_ch76, // 50
    font_adisize8_ch77    ,font_adisize8_ch78    ,font_adisize8_ch79    ,font_adisize8_ch80    ,font_adisize8_ch81    ,font_adisize8_ch82    ,font_adisize8_ch83,
    font_adisize8_ch84    ,font_adisize8_ch85    ,font_adisize8_ch86    ,font_adisize8_ch87    ,font_adisize8_ch88    ,font_adisize8_ch89    ,font_adisize8_ch90,     // 60
    font_adisize8_ch91    ,font_adisize8_ch92    ,font_adisize8_ch93    ,font_adisize8_ch94    ,font_adisize8_ch95    ,font_adisize8_ch96    ,font_adisize8_ch97,
    font_adisize8_ch98    ,font_adisize8_ch99    ,font_adisize8_ch100   ,font_adisize8_ch101   ,font_adisize8_ch102   ,font_adisize8_ch103   ,font_adisize8_ch104,    // 70
    font_adisize8_ch105   ,font_adisize8_ch106   ,font_adisize8_ch107   ,font_adisize8_ch108   ,font_adisize8_ch109   ,font_adisize8_ch110   ,font_adisize8_ch111,
    font_adisize8_ch112   ,font_adisize8_ch113   ,font_adisize8_ch114   ,font_adisize8_ch115   ,font_adisize8_ch116   ,font_adisize8_ch117   ,font_adisize8_ch118,    // 80
    font_adisize8_ch119   ,font_adisize8_ch120   ,font_adisize8_ch121   ,font_adisize8_ch122   ,font_adisize8_ch123   ,font_adisize8_ch124   ,font_adisize8_ch125,
    font_adisize8_ch126   ,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // 90
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // a0
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // b0
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // c0
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // d0
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // e0
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef, // f0
    font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,font_adisize8_chnotdef,
};

#undef f8

//Defining "Adikted classic" font

#define f1(c,a1) const static unsigned char font_adiclssc_ch##c[] = {1,0x##a1}
#define f2(c,a1,a2) const static unsigned char font_adiclssc_ch##c[] = {2,0x##a1,0x##a2}
#define f3(c,a1,a2,a3) const static unsigned char font_adiclssc_ch##c[] = {3,0x##a1,0x##a2,0x##a3}
#define f4(c,a1,a2,a3,a4) const static unsigned char font_adiclssc_ch##c[] = {4,0x##a1,0x##a2,0x##a3,0x##a4}
#define f5(c,a1,a2,a3,a4,a5) const static unsigned char font_adiclssc_ch##c[] = {5,0x##a1,0x##a2,0x##a3,0x##a4,0x##a5}

f4(A,f,14,14,f);
f4(B,1f,15,15,a);
f4(C,E,11,11,a);
f4(D,1f,11,11,e);
f4(E,1f,15,15,11);
f4(F,1f,14,14,10);
f4(G,f,11,15,17);
f4(H,1f,4,4,1f);
f3(I,11,1f,11);
f4(J,11,11,1f,10);
f4(K,1f,4,a,11);
f4(L,1f,1,1,1);
f5(M,1f,8,4,8,1f);
f4(N,1f,8,4,1f);
f4(O,e,11,11,e);
f4(P,1f,14,14,8);
f4(Q,e,11,13,f);
f4(R,1f,14,14,b);
f4(S,9,15,15,12);
f5(T,10,10,1f,10,10);
f4(U,1e,1,1,1e);
f4(V,1e,1,2,1c);
f5(W,1f,2,4,2,1f);
f5(X,11,a,4,a,11);
f5(Y,10,8,7,8,10);
f4(Z,13,15,19,11);

f4(0,f,15,19,f);
f4(1,0,9,1f,1);
f4(2,9,13,15,9);
f4(3,11,15,1d,12);
f4(4,6,a,1f,2);
f4(5,1d,15,15,12);
f4(6,e,15,15,12);
f4(7,10,10,17,18);
f4(8,a,15,15,a);
f4(9,8,15,15,e);

f2(space, 0, 0);
f1(exclam,1d);
f3(dblqt,18,0,18);
f2(oquote,c,14);
f2(cquote,14,18);
f1(period,1);
f2(comma,1,2);
f1(colon,a);
f2(semi,1,a);
f5(query,8,10,15,14,8);
f4(atsign,e,11,1d,d);

f3(notdef,15,a,15);

const int font_adiclssc_height=6;

const unsigned char *font_adiclssc_chars[] = {
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 00
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 08
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 10
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 18
    font_adiclssc_chspace ,font_adiclssc_chexclam,font_adiclssc_chdblqt ,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chcquote,   // 20
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chcomma ,font_adiclssc_chnotdef,font_adiclssc_chperiod,font_adiclssc_chnotdef,   // 28
    font_adiclssc_ch0     ,font_adiclssc_ch1     ,font_adiclssc_ch2     ,font_adiclssc_ch3     ,font_adiclssc_ch4     ,font_adiclssc_ch5     ,font_adiclssc_ch6     ,font_adiclssc_ch7     ,   // 30
    font_adiclssc_ch8     ,font_adiclssc_ch9     ,font_adiclssc_chcolon ,font_adiclssc_chsemi  ,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chquery ,   // 38
    font_adiclssc_chatsign,font_adiclssc_chA     ,font_adiclssc_chB     ,font_adiclssc_chC     ,font_adiclssc_chD     ,font_adiclssc_chE     ,font_adiclssc_chF     ,font_adiclssc_chG     ,   // 40
    font_adiclssc_chH     ,font_adiclssc_chI     ,font_adiclssc_chJ     ,font_adiclssc_chK     ,font_adiclssc_chL     ,font_adiclssc_chM     ,font_adiclssc_chN     ,font_adiclssc_chO     ,   // 48
    font_adiclssc_chP     ,font_adiclssc_chQ     ,font_adiclssc_chR     ,font_adiclssc_chS     ,font_adiclssc_chT     ,font_adiclssc_chU     ,font_adiclssc_chV     ,font_adiclssc_chW     ,   // 50
    font_adiclssc_chX     ,font_adiclssc_chY     ,font_adiclssc_chZ     ,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 58
    font_adiclssc_choquote,font_adiclssc_chA     ,font_adiclssc_chB     ,font_adiclssc_chC     ,font_adiclssc_chD     ,font_adiclssc_chE     ,font_adiclssc_chF     ,font_adiclssc_chG     ,   // 60
    font_adiclssc_chH     ,font_adiclssc_chI     ,font_adiclssc_chJ     ,font_adiclssc_chK     ,font_adiclssc_chL     ,font_adiclssc_chM     ,font_adiclssc_chN     ,font_adiclssc_chO     ,   // 68
    font_adiclssc_chP     ,font_adiclssc_chQ     ,font_adiclssc_chR     ,font_adiclssc_chS     ,font_adiclssc_chT     ,font_adiclssc_chU     ,font_adiclssc_chV     ,font_adiclssc_chW     ,   // 70
    font_adiclssc_chX     ,font_adiclssc_chY     ,font_adiclssc_chZ     ,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 78
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 80
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 88
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 90
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // 98
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // A0
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // A8
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // B0
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // B8
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // C0
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // C8
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // D0
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // D8
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // E0
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // E8
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // F0
    font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,font_adiclssc_chnotdef,   // F8
};

// Cleanup
#undef f1
#undef f2
#undef f3
#undef f4
#undef f5
