/*
 * input_kb.h header file for input_kb.c
 */

#ifndef ADIKT_INPUTKB_H
#define ADIKT_INPUTKB_H

#ifdef MSDOS
#define ABORT_KEY 34		       // scan code for ^G
#else
#define ABORT_KEY 7			       // character code for ^G
#endif

#define KEY_BACKSP     8
#define KEY_TAB        9
#define KEY_ENTER      13
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
#define KEY_ESCAPE     27
#define KEY_CTRL_A      1
#define KEY_CTRL_B      2
#define KEY_CTRL_C      3
#define KEY_CTRL_D      4
#define KEY_CTRL_E      5
#define KEY_CTRL_F      6
#define KEY_CTRL_G      7  //Abort key
#define KEY_CTRL_H      8  //same as Backspace
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

int get_str (char *prompt, char *buf);
unsigned int get_key(void);
void input_init(void);
void input_done(void);
void speaker_beep();

extern short disable_sounds;

#endif // ADIKT_INPUTKB_H
