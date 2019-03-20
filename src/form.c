
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

#include <string.h>
#include <curses.h>

#include "miso.h"
#include "log.h"

#define MIN_Y 10
#define MIN_X 80

#define TOP_ROW 6

int
form_valid(void)
{
 int i = 0;

 if ( NULL == form_title ) {
     return (-1);
 } else if ( 0 == field_count ) {
     return (-1);
 } else {
     for ( i = 0; i < field_count; i++ ) {
         if ( field_valid(field[i]) < 0 ) {
             return (-1);
         }
     }
 }
 return (0);
}

int
form(void)
{
 int i = 0, y = 0, max_x = 0, max_y = 0, index = 0, offset = 0, j = 0;
 int c = 0, rows = 0, bot_row = 0;

 if ( NULL == initscr() ) {
     return (2);
 } else if ( raw() < 0 ) {
     (void)endwin(); return(2);
 } else {
     (void)keypad(stdscr, TRUE); (void)noecho();
 }
 for ( i = 0; i < field_count; i++ ) {
     find_value(field[i]);
 }
 for (;;) {
     getmaxyx(stdscr, max_y, max_x);
     if ( (max_y < MIN_Y) || (max_x < MIN_X) ) {
         clear(); refresh(); (void)endwin(); return (2);
     }
     mvaddnstr(0, (int)((max_x-strlen(form_title))/2), form_title, max_x-2);
     mvaddstr(2, 0, "Type or select values in entry fields.");
     mvaddstr(3, 0, "Press Enter AFTER making all desired changes.");
     mvaddstr(max_y-3, 0, "F1=Help");
     mvaddstr(max_y-3, (int)(max_x/4), "F2=Refresh");
     mvaddstr(max_y-3, (int)(max_x/2), "F3=Cancel");
     mvaddstr(max_y-3, (int)(3*max_x/4), "F4=List");
     mvaddstr(max_y-2, 0, "F5=Reset");
     mvaddstr(max_y-2, (int)(max_x/4), "F6=Command");
     mvaddstr(max_y-2, (int)(max_x/2), "F7=Edit");
     mvaddstr(max_y-2, (int)(3*max_x/4), "F8=Image");
     mvaddstr(max_y-1, 0, "F9=Shell");
     mvaddstr(max_y-1, (int)(max_x/4), "F10=Exit");
     mvaddstr(max_y-1, (int)(max_x/2), "Enter=Do");
     if ( field_count <= max_y-10 ) {
         rows = max_y-10; bot_row = max_y-5;
     } else {
         rows = max_y-11; bot_row = max_y-6;
     }
     for (;;) {
         if ( field_count > rows ) {
             if ( 0 == offset ) {
                 mvaddstr(5, 0, "[TOP]");
             } else {
                 mvprintw(5, 0, "[MORE...%d]", offset);
             }
             clrtoeol();
             if ( offset+max_y-10 > field_count ) {
                 mvaddstr(max_y-5, 0, "[BOTTOM]");
             } else {
                 mvprintw(max_y-5, 0, "[MORE...%d]",
                                                  field_count-offset-max_y+11);
             }
             clrtoeol();
         }
         mvaddstr(5, 55, "[Entry Fields]");
         for ( i = offset, y = TOP_ROW;
                               (i < field_count) && (y <= bot_row); i++, y++) {
             (void)io_field(y, 0, field[i]);
         }
         refresh();
         c = io_field((index-offset+6), 1, field[index]);
         if ( KEY_DOWN == c ) {
             if ( index < field_count-1 ) {
                 mvaddnstr((index-offset+6),
                                            2, field[index++]->label, max_x-2);
                 clrtoeol();
                 if ( (index-offset+6) > bot_row ) {
                     offset++;
                 }
             }
         } else if ( KEY_UP == c ) {
             if ( index > 0 ) {
                 if ( offset && ((index-offset+6) == TOP_ROW) ) {
                     offset--;
                 }
                 mvaddnstr((index-offset+6), 2,
                                               field[index--]->label, max_x-2);
                 clrtoeol();
             }
         } else if ( KEY_PPAGE == c ) {
             if ( offset > 0 ) {
                 if ( offset > rows ) {
                     offset -= rows; index -= rows;
                 } else {
                     index -= offset; offset = 0;
                 }
             } else {
                 index = 0;
             }
         } else if ( KEY_NPAGE == c ) {
             if ( offset < field_count-rows ) {
                 if ( offset < field_count-2*rows ) {
                     offset += rows; index += rows;
                 } else {
                     index  += field_count-offset-rows;
                     offset += field_count-offset-rows;
                 }
             } else {
                 index = field_count-1;
             } 
         } else if ( (KEY_ENTER == c) || ('\r' == c) || ('\n' == c) ) {
             clear(); refresh(); (void)endwin(); return (0);
         } else if ( KEY_F(1) == c ) {
             clear(); refresh(); (void)endwin(); return (1);
         } else if ( KEY_F(2) == c ) {
             clear(); refresh(); break;
         } else if ( KEY_F(3) == c ) {
             clear(); refresh(); (void)endwin(); return (3);
         } else if ( KEY_F(5) == c ) {
             clear(); refresh();
             for ( j = 0; j < field_count; j++ ) {
                 strcpy(field[j]->buf, field[j]->value); find_value(field[j]);
             }
             break;
         } else if ( KEY_F(8) == c ) {
             continue;
         } else if ( KEY_F(10) == c ) {
             clear(); refresh(); (void)endwin(); return (10);
         }
     }
 }
}

