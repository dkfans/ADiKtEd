/******************************************************************************/
/** @file msg_log.h
 * Maintaining of message log.
 * @par Purpose:
 *     Header file. Defines exported routines from msg_log.c.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     25 Apr 2008 - 29 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef BULL_MSGLOG_H
#define BULL_MSGLOG_H

#include "globals.h"
#include <stdarg.h>

struct LEVEL;

DLLIMPORT void init_messages(void);
DLLIMPORT void free_messages(void);

DLLIMPORT void message_error(const char *format, ...);
DLLIMPORT void message_info(const char *format, ...);
DLLIMPORT void message_info_force(const char *format, ...);
DLLIMPORT short message_is_empty(void);
DLLIMPORT void message_release(void);
DLLIMPORT char *message_get(void);
DLLIMPORT char *message_get_prev(void);
DLLIMPORT short message_hold_get(void);
DLLIMPORT unsigned int message_getcount_get(void);

DLLIMPORT void message_log(const char *format, ...);
DLLIMPORT void message_log_simp(const char *str);
DLLIMPORT void message_log_vl(const char *format, va_list val);

DLLIMPORT short set_msglog_fname(char *fname);

#endif // BULL_MSGLOG_H
