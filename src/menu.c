
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

#include <string.h>
#include <curses.h>

#include "miso.h"
#include "log.h"

#define MIN_Y 24
#define MIN_X 80

char *program_output = NULL;

#ifndef LINT
static char rcsid[] =
"$Id: menu.c,v 1.15 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

int
menu(void)
{
 int i = 0, y = 0, max_x = 0, max_y = 0, index = 0, offset = 0;
 int c = 0, k = 0, rows = 0, top_row = 0, bot_row = 0;

 if ( NULL == initscr() ) {
     return (2);
 } else if ( raw() < 0 ) {
     (void)endwin(); return(2);
 } else {
     (void)keypad(stdscr, TRUE); (void)noecho();
 }

 for (;;) {
     getmaxyx(stdscr, max_y, max_x);
     if ( (max_y < MIN_Y) || (max_x < MIN_X) ) {
         clear(); refresh(); (void)endwin(); return (2);
     }
     mvaddnstr(0, (int)((max_x-strlen(menu_title))/2), menu_title, max_x-2);
     mvaddstr(2, 0, "Move cursor to desired item and press Enter.");
     mvaddstr(max_y-2, 0, "F1=Help");
     mvaddstr(max_y-2, (int)(max_x/4), "F2=Refresh");
     mvaddstr(max_y-2, (int)(max_x/2), "F3=Cancel");
     mvaddstr(max_y-2, (int)(3*max_x/4), "F6=Output");
     mvaddstr(max_y-1, 0, "F8=Image");
     mvaddstr(max_y-1, (int)(max_x/4), "F9=Shell");
     mvaddstr(max_y-1, (int)(max_x/2), "F10=Exit");
     mvaddstr(max_y-1, (int)(3*max_x/4), "Enter=Do");
     if ( item_count <= max_y-10 ) {
         rows = max_y-11; bot_row = max_y-4; top_row = 4;
     } else {
         rows = max_y-9; bot_row = max_y-5; top_row = 5;
     }
     for (;;) {
         if ( item_count > rows ) {
             if ( 0 == offset ) {
                 mvaddstr(4, 0, "[TOP]");
             } else {
                 mvprintw(4, 0, "[MORE...%d]", offset);
             }
             clrtoeol();
             if ( offset+rows == item_count ) {
                 mvaddstr(max_y-4, 0, "[BOTTOM]");
             } else {
                 mvprintw(max_y-4, 0, "[MORE...%d]",
                                                  item_count-offset-max_y+9);
             }
             clrtoeol();
         }
         for ( i = offset, y = top_row;
                                (i < item_count) && (y <= bot_row); i++, y++) {
             mvaddnstr(y, 2, item[i]->label, max_x-2); clrtoeol();
         }
         attron(A_STANDOUT);
         mvaddnstr((index-offset+top_row), 2, item[index]->label, max_x-2);
         attroff(A_STANDOUT); clrtoeol(); refresh();
         c = get_key((index-offset+top_row), 1); move(max_y-3, 0); clrtoeol();
         if ( KEY_DOWN == c ) {
             if ( index < item_count-1 ) {
                 mvaddnstr((index-offset+top_row), 2, item[index++]->label,
                                                                      max_x-2);
                 clrtoeol();
                 if ( (index-offset+top_row) > bot_row ) {
                     offset++;
                 }
             }
         } else if ( KEY_UP == c ) {
             if ( index > 0 ) {
                 if ( offset && ((index-offset+top_row) == top_row) ) {
                     offset--;
                 }
                 mvaddnstr((index-offset+top_row), 2, item[index--]->label,
                                                                      max_x-2);
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
             if ( offset < item_count-rows ) {
                 if ( offset < item_count-2*rows ) {
                     offset += rows; index += rows;
                 } else {
                     index  += item_count-offset-rows;
                     offset += item_count-offset-rows;
                 }
             } else {
                 index = item_count-1;
             } 
         } else if ( (KEY_ENTER == c) || ('\r' == c) || ('\n' == c) ) {
             program_output = (item[index]->output);
             clear(); refresh(); (void)endwin(); return (0);
         } else if ( KEY_F(1) == c ) {
             program_output = (item[index]->output);
             clear(); refresh(); (void)endwin(); return (1);
         } else if ( KEY_F(2) == c ) {
             clear(); refresh(); break;
         } else if ( KEY_F(3) == c ) {
             clear(); refresh(); (void)endwin(); return (3);
         } else if ( KEY_F(4) == c ) {
             err_msg(ERR_LIST_UNAVAILABLE);
         } else if ( KEY_F(6) == c ) {
             mvprintw(max_y-3, 0, " ITEM OUTPUT: %s", item[index]->output);
             clrtoeol();
         } else if ( KEY_F(8) == c ) {
             if ( KEY_F(1) == (k = image()) ) {
                 program_output = (item[index]->output);
                 clear(); refresh(); (void)endwin(); return (1);
             } else if ( KEY_F(10) == k ) {
                 clear(); refresh(); (void)endwin(); return (10);
             } else {
                 clear(); refresh(); break;
             }
         } else if ( KEY_F(9) == c ) {
             if ( KEY_F(1) == (k = subshell()) ) {
                 program_output = (item[index]->output);
                 clear(); refresh(); (void)endwin(); return (1);
             } else if ( KEY_F(10) == k ) {
                 clear(); refresh(); (void)endwin(); return (10);
             } else {
                 clear(); refresh(); break;
             }
         } else if ( KEY_F(10) == c ) {
             clear(); refresh(); (void)endwin(); return (10);
         }
     }
 }
}

int
item_valid(item_t *I)
{
 return (0);
}

int
menu_valid(void)
{
 return (0);
}

