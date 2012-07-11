
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

#include "miso.h"
#include "log.h"

#ifndef LINT
static char rcsid[] =
"$Id: key.c,v 1.5 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

#define KEY_ESCAPE 27

int
get_key(int y, int x)
{
 int c;

 if ( ( y >= 0 ) && ( x >= 0 ) ) {
     move(y, x);
 }
 if ( KEY_ESCAPE == (c = getch()) ) {
     nodelay(stdscr, TRUE); c = getch(); nodelay(stdscr, FALSE);
     if ( ERR == c ) {
         c = KEY_ESCAPE;
     } else if ( '1' == c ) {
         c = KEY_F(1);
     } else if ( '2' == c ) {
         c = KEY_F(2);
     } else if ( '3' == c ) {
         c = KEY_F(3);
     } else if ( '4' == c ) {
         c = KEY_F(4);
     } else if ( '5' == c ) {
         c = KEY_F(5);
     } else if ( '6' == c ) {
         c = KEY_F(6);
     } else if ( '7' == c ) {
         c = KEY_F(7);
     } else if ( '8' == c ) {
         c = KEY_F(8);
     } else if ( '9' == c ) {
         c = KEY_F(9);
     } else if ( '0' == c ) {
         c = KEY_F(10);
     }
 } else if ( ('\n' == c) || ('\r' == c) ) {
     c = KEY_ENTER;
 }
 return (c);
}

