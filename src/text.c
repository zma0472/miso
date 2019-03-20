
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
#include <string.h>
#include <ctype.h>

#include "miso.h"
#include "log.h"

#define MIN_X 24

int
io_text(int y, int x, char *buf, int width, unsigned long mask)
{
 int c = 0, index = 0, offset = 0, cursor = x+1, wall = 0, max_x = 0;
 int max_y = 0;

 getmaxyx(stdscr, max_y, max_x); wall = max_x-3;
 if ( max_x < MIN_X ) {
     return (-1);
 } 
 for (;;) {
     mvaddch(y, x, offset ? '<' : '['); addnstr(&buf[offset], wall-x-1);
     addch(strlen(&buf[offset]) > wall-x-1 ? '>' : ']'); addch(' '); refresh();
     if ( KEY_ENTER == (c = get_key(y, cursor)) ) {
         return (KEY_ENTER);
     } else if ( KEY_LEFT == c ) {
         if ( index ) {
             index--;
             if ( x+1 == cursor ) {
                 if ( offset ) {
                     offset--;
                 }
             } else {
                 cursor--;
             }
         }
     } else if ( KEY_RIGHT == c ) {
         if ( '\0' != buf[index] ) {
             index++;
             if ( cursor == wall ) {
                 offset++;
             } else {
                 cursor++;
             }
         }
     } else if ( KEY_DC == c ) {
         if ( buf[index] ) {
             unsigned int i = 0;
             for (i = index; i < width; i++) {
                 buf[i] = buf[i+1];
             }
             buf[width] = '\0';
         }
     } else if ( KEY_BACKSPACE == c ) {
         if ( index ) {
             unsigned int i = 0;
             for ( i = index-1; i < width; i++ ) {
                 buf[i] = buf[i+1];
             }
             buf[width-1] = '\0'; index--;
             if ( 0 == offset ) {
                 if (x+1 < cursor) {
                     cursor--;
                 }
             } else {
                 offset--;
             }
         }
     } else if ( ((KEY_F(3)  == c) && (mask & F3_BIT))    ||
                 ((KEY_F(4)  == c) && (mask & F4_BIT))    ||
                 ((KEY_F(5)  == c) && (mask & F5_BIT))    ||
                 ((KEY_NPAGE == c) && (mask & NPAGE_BIT)) ||
                 ((KEY_PPAGE == c) && (mask & PPAGE_BIT)) ||
                 ((KEY_DOWN  == c) && (mask & DOWN_BIT))  ||
                 ((KEY_UP    == c) && (mask & UP_BIT))    ||
                 ((KEY_F(10) == c) && (mask & F10_BIT)) ) {
         return (c);
     } else if ( KEY_HOME == c ) {
         index = 0; offset = 0; cursor = x+1;
     } else if ( KEY_END == c ) {
         unsigned int i = 0;
         for ( i = index; i < (width); i++ ) {
             if ( '\0' == buf[i] ) {
                 index = i; break;
             }
         }
         if ( x+1+index < wall ) {
             cursor = x+1+index; offset = 0;
         } else { 
             cursor = wall; offset = x+1+index-wall;
         }
     } else if ( isprint(c) ) {
         if ( '\0' == buf[width-1] ) {
             unsigned int i = 0;
             /*  Right shift all chars to the right of the edit index to make
              *  room for the char insert.
              */
             for ( i = width-1; i > index; i-- ) {
                 buf[i] = buf[i-1];
             }
             /*  Make the insert & increment the edit index.  */
             buf[index++] = c;
             if (cursor == wall) {
                 offset++;
             } else {
                 cursor++;
             }
         }
     }
 }
}

