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
#define KEY_CTRL_H      8
#define KEY_CTRL_I      9
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
#define KEY_CTRL_SPACE 0

#define KEY_UP         0x101
#define KEY_DOWN       0x102
#define KEY_LEFT       0x103
#define KEY_RIGHT      0x104
#define KEY_PGUP       0x105
#define KEY_PGDOWN     0x106
#define KEY_HOME       0x107
#define KEY_END        0x108
#define KEY_DEL        0x113

extern int get_str (char *prompt, char *buf);
extern unsigned int get_key(void);
extern void init_slbkey(void);
extern void input_init(void);
extern void input_done(void);
extern void speaker_beep();

extern char *slbkey;
extern unsigned char *slbplacekeys;
extern int slbplacenkeys;

#endif // ADIKT_INPUTKB_H
