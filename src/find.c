
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>

#include "miso.h"
#include "log.h"

#define MAX_X  80
#define MIN_Y  24
#define MIN_X  80

#define BUF_SIZE 1024

#ifndef LINT
static char rcsid[] =
"$Id: find.c,v 1.7 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

void
find_value(field_t *f)
{
 int c = 0, p_fd[2] = {-1, -1}, status = 0, i = 0, n = 0, x = 0;
 pid_t pid = -1;
 char buf[BUF_SIZE], *line = NULL, *path = NULL;

 if ( (NULL != values) && (strlen(f->value) > 0) ) {
     return;
 }
 if ( (NULL == f->find) || (0 == f->width) || (access(f->find, X_OK) < 0) ) {
     return;
 } else {
     if ( NULL == (line = malloc((f->width)+1)) ) {
         return;
     } else {
         memset(line, 0, f->width);
     }
 }
 if ( NULL == (path = strdup(f->find)) ) {  /* basename(3) alters in-place. */
     free(line); return;
 } else if ( pipe(p_fd) < 0 ) {
     free(line); free(path); return;
 } else if ( (pid = fork()) < 0 ) {
     free(line); free(path); (void)close(p_fd[0]); (void)close(p_fd[1]);
     return;
 } else if ( 0 == pid ) {
     /*  Child code.  */
     (void)close(p_fd[0]); free(line);
     if ( dup2(p_fd[1], STDOUT_FILENO) < 0 ) {
         free(path); (void)close(p_fd[1]); exit(EXIT_FAILURE);
     } else {
         (void)close(p_fd[1]); execl(path, basename(path), (char *)NULL);
     }
 } else {
     /*  Parent code.  */
     (void)close(p_fd[1]); free(path);
     for (;;) {
         nodelay(stdscr, TRUE); c = get_key(-1, -1); nodelay(stdscr, FALSE);
         if ( (KEY_F(3) == c) || (KEY_F(10) == c) ) {
             (void)kill(pid, SIGTERM); (void)close(p_fd[0]); free(line);
             (void)waitpid(pid, &status, 0); return;
         } else if ( ERR == c ) {
             if ( (n = read(p_fd[0], buf, BUF_SIZE)) < 0 ) {
                 if ( EINTR == errno ) {
                     continue;
                 } else {
                     (void)kill(pid, SIGTERM); (void)close(p_fd[0]);
                     free(line); (void)waitpid(pid, &status, 0); return;
                 }
             } else {
                 for ( i = 0; i < n; i++ ) {
                     if ( isprint(buf[i]) ) {
                         if ( x < f->width ) {
                             line[x++] = buf[i]; line[x] = '\0';
                         } else {
                             (void)kill(pid, SIGTERM); (void)close(p_fd[0]);
                             free(line); (void)waitpid(pid, &status, 0);
                             return;
                         }
                     } else if ( '\n' == buf[i] ) {
                         line[x] = '\0'; strcpy(f->buf, line); free(line);
                         (void)close(p_fd[0]); return;
                     }
                 }
                 if ( 0 == n ) {
                     (void)close(p_fd[0]); (void)kill(pid, SIGTERM);
                     (void)waitpid(pid, &status, 0);
                     if ( x ) {
                         strcpy(f->buf, line);
                     }
                     free(line); (void)close(p_fd[0]); return;
                 }
             }
         }
     }
 }
}

