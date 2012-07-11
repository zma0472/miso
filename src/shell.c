
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
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "miso.h"
#include "log.h"

#ifndef LINT
static char rcsid[] =
"$Id: shell.c,v 1.6 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

#define MIN_X 80
#define MIN_Y 24

int
subshell(void)
{
 pid_t pid = -1;
 char *pathname = NULL, *program = NULL;
 int c = -1, max_y = 0, max_x = 0, y = 0, x = 0, k = 0;

 if ( NULL != shell_path ) {
     if ( NULL == (pathname = strdup(shell_path)) ) {
         return (-1);
     } else if ( NULL == (program = basename(program)) ) {
         return (-1);
     } else {
         for (;;) {
             getmaxyx(stdscr, max_y, max_x);
             if ( (max_y < MIN_Y) || (max_x < MIN_X) ) {
                 return (-1);
             }
             mvaddch(max_y-11, 2, '+'); mvaddch(max_y-11, max_x-3, '+');
             mvaddch(max_y-1, 2, '+'); mvaddch(max_y-1, max_x-3, '+');
             for ( x = 3; x < max_x-3; x++) {
                 mvaddch(max_y-11, x, '-'); mvaddch(max_y-1, x, '-');
             }
             for ( y = max_y-10; y < max_y-1; y++ ) {
                 mvaddch(y, 2, '|'); mvaddch(y, max_x-3, '|');
                 for ( x = 3; x < max_x-3; x++ ) {
                     mvaddch(y, x, ' ');
                 }
             }
             mvaddstr(max_y-10, (int)((max_x/2)-6), "SHELL ESCAPE");
             mvaddstr(max_y-8, 4, "Press Enter to proceed to your shell.");
             mvaddstr(max_y-7,8, "Once in the shell, normal exit will return");
             mvaddstr(max_y-6, 8, "you to this application.");
             mvaddstr(max_y-5,4, "Press Cancel to return to the application.");
             mvaddstr(max_y-3,4, "F1=Help"); mvaddstr(max_y-2,4, "F8=Image");
             mvaddstr(max_y-3, (int)(max_x/3), "F2=Refresh");
             mvaddstr(max_y-2, (int)(max_x/3), "F10=Exit");
             mvaddstr(max_y-3, (int)(max_x/3)*2, "F3=Cancel");
             mvaddstr(max_y-2, (int)(max_x/3)*2, "Enter=Do");
             refresh();
             for (;;) {
                 if ( KEY_F(1) == (c = get_key(max_y-4, 4)) ) {
                     free(pathname); return (c);
                 } else if ( KEY_F(2) == c ) {
                     break;
                 } else if ( KEY_F(3) == c ) {
                     free(pathname); return (0);
                 } else if ( KEY_F(8) == c ) {
                     if ( (KEY_F(1) == (k = image())) || (KEY_F(10) == k) ) {
                         free(pathname); return (k);
                     } else {
                         break;
                     }
                 } else if ( KEY_F(10) == c ) {
                     free(pathname); return (c);
                 } else if ( KEY_ENTER == c ) {
                     if ( (pid = fork()) < 0 ) {
                         free(pathname); return (-1);
                     } else if ( pid > 0 ) {
                         waitpid(pid, NULL, 0); reset_prog_mode();
                         clear(); refresh(); free(pathname); return (0);
                     } else {
                         clear(); refresh(); reset_shell_mode();
                         execl(shell_path, program, (char *)NULL);
                     }
                 }
             }
         }
     }
 }
}

