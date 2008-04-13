/******************************************************************************/
// input_kb.h - Platform independence assurance module.
/******************************************************************************/
// Author:  Tomasz Lis
// Created: 12 Dec 2007

// Purpose:
//   Header file. Defines exported routines from input_kb.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_INPUTKB_H
#define ADIKT_INPUTKB_H

#define ERR_KB_NONE      0x000
#define ERR_KB_ALREADY   0x001
#define ERR_KB_INTERNAL  0x004
#define ERR_KB_LIBINIT   0x005

#if defined(MSDOS)
#define ABORT_KEY 34		       // scan code for ^G
#else
#define ABORT_KEY 7			       // character code for ^G
#endif

#define KEY_BACKSP     127
#define KEY_TAB        9
#define KEY_ENTER      13
#define KEY_ESCAPE     27

#define KEY_F1         0x201
#define KEY_F2         0x202
#define KEY_F3         0x203
#define KEY_F4         0x204
#define KEY_F5         0x205
#define KEY_F6         0x206
#define KEY_F7         0x207
#define KEY_F8         0x208
#define KEY_F9         0x209
#define KEY_F10        0x20a
#define KEY_F11        0x20b
#define KEY_F12        0x20c

#define KEY_CTRL_A      1
#define KEY_CTRL_B      2
#define KEY_CTRL_C      3
#define KEY_CTRL_D      4
#define KEY_CTRL_E      5
#define KEY_CTRL_F      6
#define KEY_CTRL_G      7  //Abort key
#define KEY_CTRL_H      8 
#define KEY_CTRL_I      9  //same as Tab
#define KEY_CTRL_J     10
#define KEY_CTRL_K     11
#define KEY_CTRL_L     12
#define KEY_CTRL_M     13  //same as Enter - better use the Enter
#define KEY_CTRL_N     14
#define KEY_CTRL_O     15
#define KEY_CTRL_P     16
#define KEY_CTRL_Q     17
#define KEY_CTRL_R     18
#define KEY_CTRL_S     19
#define KEY_CTRL_T     20
#define KEY_CTRL_U     21
#define KEY_CTRL_V     22
#define KEY_CTRL_W     23
#define KEY_CTRL_X     24
#define KEY_CTRL_Y     25
#define KEY_CTRL_Z     26
#define KEY_CTRL_SPACE 65535

#define KEY_UP         0x101
#define KEY_DOWN       0x102
#define KEY_LEFT       0x103
#define KEY_RIGHT      0x104
#define KEY_PGUP       0x105
#define KEY_PGDOWN     0x106
#define KEY_HOME       0x107
#define KEY_END        0x108
#define KEY_DEL        0x113

//TODO: set proper values here when Slang will support this
#define KEY_CTRL_UP     0x5101
#define KEY_CTRL_DOWN   0x5102
#define KEY_CTRL_LEFT   0x5103
#define KEY_CTRL_RIGHT  0x5104
#define KEY_CTRL_PGUP   0x5105
#define KEY_CTRL_PGDOWN 0x5106
#define KEY_CTRL_HOME   0x5107
#define KEY_CTRL_END    0x6108
#define KEY_ALT_UP      0x6101
#define KEY_ALT_DOWN    0x6102
#define KEY_ALT_LEFT    0x6103
#define KEY_ALT_RIGHT   0x6104

#define KEY_A           'a'
#define KEY_B           'b'
#define KEY_C           'c'
#define KEY_D           'd'
#define KEY_E           'e'
#define KEY_F           'f'
#define KEY_G           'g'
#define KEY_H           'h'
#define KEY_I           'i'
#define KEY_J           'g'
#define KEY_K           'k'
#define KEY_L           'l'
#define KEY_M           'm'
#define KEY_N           'n'
#define KEY_O           'o'
#define KEY_P           'p'
#define KEY_Q           'q'
#define KEY_R           'r'
#define KEY_S           's'
#define KEY_T           't'
#define KEY_U           'u'
#define KEY_V           'v'
#define KEY_W           'w'
#define KEY_X           'x'
#define KEY_Y           'y'
#define KEY_Z           'z'

#define KEY_SHIFT_A     'A'
#define KEY_SHIFT_B     'B'
#define KEY_SHIFT_C     'C'
#define KEY_SHIFT_D     'D'
#define KEY_SHIFT_E     'E'
#define KEY_SHIFT_F     'F'
#define KEY_SHIFT_G     'G'
#define KEY_SHIFT_H     'H'
#define KEY_SHIFT_I     'I'
#define KEY_SHIFT_J     'J'
#define KEY_SHIFT_K     'K'
#define KEY_SHIFT_L     'L'
#define KEY_SHIFT_M     'M'
#define KEY_SHIFT_N     'N'
#define KEY_SHIFT_O     'O'
#define KEY_SHIFT_P     'P'
#define KEY_SHIFT_Q     'Q'
#define KEY_SHIFT_R     'R'
#define KEY_SHIFT_S     'S'
#define KEY_SHIFT_T     'T'
#define KEY_SHIFT_U     'U'
#define KEY_SHIFT_V     'V'
#define KEY_SHIFT_W     'W'
#define KEY_SHIFT_X     'X'
#define KEY_SHIFT_Y     'Y'
#define KEY_SHIFT_Z     'Z'

#define KEY_NUM0        '0'
#define KEY_NUM1        '1'
#define KEY_NUM2        '2'
#define KEY_NUM3        '3'
#define KEY_NUM4        '4'
#define KEY_NUM5        '5'
#define KEY_NUM6        '6'
#define KEY_NUM7        '7'
#define KEY_NUM8        '8'
#define KEY_NUM9        '9'
#define KEY_NPLUS       '+'
#define KEY_NMINUS      '-'
#define KEY_EQUAL       '='
#define KEY_GREATER     '>'
#define KEY_SMALLER     '<'

#define KEY_SPACE       ' '
#define KEY_EXCLAM      '!'
#define KEY_AT          '@'
#define KEY_HASH        '#'
#define KEY_DOLLAR      '$'
#define KEY_PERCENT     '%'
#define KEY_DASH        '^'
#define KEY_AMPRSNT     '&'
#define KEY_SLASH       '/'
#define KEY_BSLASH      '\\'
#define KEY_UNDERLN     '_'
#define KEY_DOT         '.'
#define KEY_ASTERIX     '*'
#define KEY_TILDE       '~'
#define KEY_APOSTRT     '`'
#define KEY_APOSTR      '\''

#define KEY_SQRBRCKTL   '['
#define KEY_SQRBRCKTR   ']'
#define KEY_RNDBRCKTL   '('
#define KEY_RNDBRCKTR   ')'

int get_str(char *prompt, char *buf);
unsigned int get_key(void);
short input_init(void);
short input_done(void);
void speaker_beep(void);
unsigned char key_to_ascii(int key);

extern short disable_sounds;

#endif // ADIKT_INPUTKB_H
