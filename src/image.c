
/*
 * Copyright (C) 2011,2012 Matthew and Janice Alton
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
#include <ctype.h>

#include "miso.h"
#include "log.h"

#ifndef LINT
static char rcsid[] =
"$Id: image.c,v 1.6 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

#define MIN_Y 24
#define MIN_X 80

int
image(void)
{
 int x = 0, y = 0, c = 0, max_x = 0, max_y = 0, i = 0, j = 0;
 chtype ch = 0;
 WINDOW *img_w = NULL;

 if ( NULL != log_file ) {
     getmaxyx(stdscr, max_y, max_x);
     if ( (max_y < MIN_Y) || (max_x < MIN_X) ) {
         return (-1);
     }
     if ( NULL == (img_w = newwin(0, max_x-24, max_y-11, 13)) ) {
         beep(); return (-1);
     }
     for (;;) {
         getmaxyx(stdscr, max_y, max_x);
         if ( (max_y < MIN_Y) || (max_x < MIN_X) ) {
             delwin(img_w); return (-1);
         }
         (void)wborder(img_w, '|', '|', '-', '-', '+', '+', '+', '+');
         mvwaddstr(img_w, 1, (int)(((max_x-24)/2)-6), "PRINT SCREEN");
         mvwaddstr(img_w, 3, 2, "Press Enter to save the screen image");
         mvwaddstr(img_w, 4, 6, "in the log file.");
         mvwaddstr(img_w, 5, 2,"Press Cancel to return to the application.");
         mvwaddstr(img_w, 8, 2, "F1=Help");
         mvwaddstr(img_w, 8, (int)((max_x-24)/3), "F2=Refresh");
         mvwaddstr(img_w, 9, 2, "F10=Exit");
         mvwaddstr(img_w, 8, (int)(2*(max_x-24)/3), "F3=Cancel");
         mvwaddstr(img_w, 9, (int)((max_x-24)/3), "Enter=Do");
         wrefresh(img_w);
         for (;;) {
             c = get_key(max_y-4, 15);
             if ( (KEY_F(1) == c) || (KEY_F(10) == c) || (KEY_F(3) == c) ) {
                 delwin(img_w); return (c);
             } else if ( KEY_F(2) == c ) {
                 break;
             } else if ( KEY_ENTER == c ) {
                 for ( i = 0; i < max_y; i++ ) {
                     for ( j = 0; j < max_x; j++ ) {
                         if ( isprint(ch = (A_CHARTEXT & mvinch(i, j))) ) {
                             (void)fprintf(log_file, "%c", (int)ch);
                         }
                     }
                 }
                 (void)fprintf(log_file, "\n");
                 delwin(img_w); return (0);
             }
         }
     }
 }
 delwin(img_w); return (0);
}

