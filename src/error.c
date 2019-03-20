
/*
 * Copyright (C) 2011,2012,2019 Matthew Alton
 *
 * This file is part of Miso.
 *
 * Miso is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Miso is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Miso.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <curses.h>

#include "miso.h"
#include "log.h"

static char *messages[] = {
    "General Error.",  /*  ERR_GENERAL  */
    "Memory Error.",   /*  ERR_MEMORY  */
    "No list available.",   /*  ERR_LIST_UNAVAILABLE  */
    NULL
};

int
err_msg(unsigned int error_id)
{
 int blank = TRUE, x = -1, y = -1, i = -1, j = -1, key = -1;
 char buf[1024];

 beep(); getmaxyx(stdscr, y, x);

 if ( y > 0 ) {
     for ( i = y-1; i >= 0; i-- ) {
         move(i, 0); x = instr(buf); blank = TRUE;
         for ( j = 0; j < x; j++ ) {
             if ( ' ' != buf[j] ) {
                 blank = FALSE; break;
             }
         }
         if ( TRUE == blank ) {
             mvprintw(i, 12, "%s", messages[error_id]); refresh();
             key = get_key(-1, -1); move(i, 0); clrtoeol(); refresh(); break;
         }
     }
 }
 return (0);
}

