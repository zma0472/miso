
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
#include <signal.h>

#include "miso.h"
#include "log.h"

#define BUF_SIZE 1024
#define MAX_X      80
#define MIN_Y  24
#define MIN_X  80

#define FIND_CANCEL (-1)
#define FIND_EXIT   (-2)

static char **S       = NULL;
static int    S_count = 0;

static char *srch_txt = NULL;

static int
find_next(int index)
{
 int i = 0, j = 0;

 if ( NULL != srch_txt ) {
     for (i = index+1; i < S_count; i++) {
         for (j = 0; '\0' != S[i][j]; j++) {
             if ( 0 == strncasecmp(srch_txt, &S[i][j], strlen(srch_txt)) ) {
                 return (i);
             }
         }
     }
     for (i = 0; i < index; i++) {
         for (j = 0; '\0' != S[i][j]; j++) {
             if ( 0 == strncasecmp(srch_txt, &S[i][j], strlen(srch_txt)) ) {
                 return (i);
             }
         }
     }
 }
 return (index);
}

static int
find_text(int index)
{
 int i = 0, value = index, max_y = 0, max_x = 0, c = 0, y = 0, x = 0;
 unsigned long keymask = (F3_BIT|F10_BIT);
 char buf[256];

 (void)memset(buf, '\0', 256);

 getmaxyx(stdscr, max_y, max_x);
 if ( (max_y < MIN_Y) || (max_x < MIN_X) ) {
     return (-1);
 }
 move(max_y-10, 0); clrtobot();
 mvaddch(max_y-10, 0, '+');
 mvaddch(max_y-10, max_x-1, '+');
 mvaddch(max_y-1, 0, '+');
 mvaddch(max_y-1, max_x-1, '+');
 for ( x = 1; x < max_x-1; x++ ) {
     mvaddch(max_y-10, x, '-'); mvaddch(max_y-1, x, '-');
 }
 for ( y = max_y-9; y < max_y-1; y++ ) {
     mvaddch(y, 0, '|'); mvaddch(y, max_x-1, '|');
 }
 mvaddstr(max_y-9, (max_x/2)-2, "Find");
 refresh();

 c = io_text(max_y-7, 4, buf, 256, keymask);

 if ( KEY_F(3) == c ) {
     value = FIND_CANCEL;
 } else if ( KEY_F(10) == c ) {
     value = FIND_EXIT;
 } else if ( KEY_ENTER == c ) {
     if ( '\0' != buf[0] ) {
         if ( NULL != (srch_txt = strdup(buf)) ) {
             value = find_next(index);
         } else {
             value = FIND_CANCEL;
         }
     } else {
         value = FIND_CANCEL;
     }
 } else {
     value = FIND_CANCEL;
 }
 move(max_y-10, 0); clrtobot(); return (value);
}

int
list(field_t *f)
{
 int c = 0, fd = -1, p_fd[2] = {-1, -1}, cancel = FALSE, status = 0, i = 0;
 int n = 0, value = -1, x = 0;
 pid_t pid = -1, child_up = FALSE, w_pid = -1;
 char buf[BUF_SIZE], line[MAX_X+1], *s = NULL, **s_tmp = NULL, *path = NULL;

 if ( BOOLEAN_T == f->type ) {
     for ( i = 0; i < S_count; i++ ) {
         free (S[i]); S[i] = NULL;
     }
     free(S); S = NULL; S_count = 0;
     if ( NULL != (S = calloc(3, sizeof(char *))) ) {
         if ( NULL == (S[0] = strdup("\0")) ) {
             free(S); S = NULL;
         } else {
             if ( (0 == strcmp(f->buf,   "yes")) ||
                  (0 == strcmp(f->buf,   "no"))  ||
                  (0 == strcmp(f->value, "yes")) ||
                  (0 == strcmp(f->value, "no")) ) {
                 if ( NULL == (S[1] = strdup("yes")) ) {
                     free(S[0]); S[0] = NULL; free(S); S = NULL;
                 } else if ( NULL == (S[2] = strdup("no")) ) {
                     free(S[0]); S[0] = NULL;
                     free(S[1]); S[1] = NULL; free(S); S = NULL;
                 } else {
                     S_count = 3; value = 0;
                 }
             } else if ( (0 == strcmp(f->buf,   "on"))  ||
                         (0 == strcmp(f->buf,   "off")) ||
                         (0 == strcmp(f->value, "on"))  ||
                         (0 == strcmp(f->value, "off")) ) {
                 if ( NULL == (S[1] = strdup("on")) ) {
                     free(S[0]); S[0] = NULL; free(S); S = NULL;
                 } else if ( NULL == (S[2] = strdup("off")) ) {
                     free(S[0]); S[0] = NULL;
                     free(S[1]); S[1] = NULL; free(S); S = NULL;
                 } else {
                     S_count = 3; value = 0;
                 }
             } else {
                 if ( NULL == (S[1] = strdup("true")) ) {
                     free(S[0]); S[0] = NULL; free(S); S = NULL;
                 } else if ( NULL == (S[2] = strdup("false")) ) {
                     free(S[0]); S[0] = NULL;
                     free(S[1]); S[1] = NULL; free(S); S = NULL;
                 } else {
                     S_count = 3; value = 0;
                 }
             }
         }
     }
 } else if ( NULL == (path = strdup(f->list)) ) {
     perror("strdup");
 } else if ( pipe(p_fd) < 0 ) {
     perror("pipe");
 } else if ( (pid = fork()) < 0 ) {
     (void)close(p_fd[0]); (void)close(p_fd[1]); perror("fork");
 } else if ( 0 == pid ) {
     /*  Child code.  */
     (void)close(p_fd[0]);
     if ( dup2(p_fd[1], STDOUT_FILENO) < 0 ) {
         perror("dup2"); (void)close(p_fd[1]); exit(EXIT_FAILURE);
     } else {
         (void)close(p_fd[1]);
         execl(path, basename(path), (char *)NULL);
     }
 } else {
     /*  Parent code.  */
     (void)close(p_fd[1]); free(path); child_up = TRUE;
     for (;;) {
         nodelay(stdscr, TRUE); c = get_key(-1, -1); nodelay(stdscr, FALSE);
         if ( (KEY_F(3) == c) || (KEY_F(10) == c) ) {
             (void)kill(pid, SIGTERM); (void)close(p_fd[0]); cancel = TRUE;
         } else if ( ERR == c ) {
             if ( (n = read(p_fd[0], buf, BUF_SIZE)) < 0 ) {
                 if ( EINTR == errno ) {
                     continue;
                 } else {
                     perror("read"); break;
                 }
             } else if ( 0 == n ) {
                 (void)close(p_fd[0]);
                 if ( pid == waitpid(pid, &status, 0) ) {
                     value = 0;
                 }
                 break;
             } else {
                 for ( i = 0; i < n; i++ ) {
                     if ( isprint(buf[i]) ) {
                         line[x++] = buf[i]; line[x] = '\0';
                     } else if ( '\n' == buf[i] ) {
                         line[x] = '\0'; x = MAX_X;
                     }
                     if ( MAX_X == x ) {
                         x = 0; s_tmp = realloc(S, (S_count+1)*sizeof(char *));
                         if ( NULL == s_tmp ) {
                             perror("realloc"); return (value);
                         } else {
                             s = strdup(line);
                             if ( NULL == s ) {
                                 perror("strdup"); return (value);
                             } else {
                                 S = s_tmp; S[S_count++] = s;
                             }
                         }
                     }
                 }
             }
         }
     }
 }
 return (value);
}

#define LIST_LEGEND                                   \
do {                                                  \
    mvaddstr(max_y-4, 2, "F1=Help");                  \
    mvaddstr(max_y-4, (int)(max_x/3), "F2=Refresh");  \
    mvaddstr(max_y-4, (int)(max_x/3)*2, "F3=Cancel"); \
    mvaddstr(max_y-3, 2, "F8=Image");                 \
    mvaddstr(max_y-3, (int)(max_x/3), "F10=Exit");    \
    mvaddstr(max_y-3, (int)(max_x/3)*2, "Enter=Do");  \
    mvaddstr(max_y-2, 2, "/=Find");                   \
    mvaddstr(max_y-2, (int)(max_x/3), "n=Find Next"); \
} while (0)

int
io_list(field_t *f)
{
 int c = 0, i = -1, y = -1, max_x = 0, max_y = 0, top = 0, bot = 0;
 int index = 0, offset = 0, size = 0, j = 0, first = 1, k = 0;
 int n = 0, rows = 0, find_index = 0, value_count = 0, increase = 0;
 char *s = NULL, *p = NULL, s_v[2] = {'\0', '\0'};
 size_t *on = NULL;

 s_v[0] = value_separator;
 if ( BOOLEAN_T != f->type ) {
     if ( NULL == f->list ) {
         return (0);
     } else if ( access(f->list, X_OK) < 0 ) {
         return (-1);
     }
 }
 if ( ((BOOLEAN_T == f->type ) || (f->list)) && (0 == f->constant) ) { 
     if ( (list(f) < 0) || (0 == S_count) ) {
         return (0);  /*  XXX  */
     } else {
         if ( NULL == (on = calloc(S_count, sizeof(size_t))) ) {
             return (0); /*  XXX  */
         }
         for (;;) {
             getmaxyx(stdscr, max_y, max_x);
             if ( (max_y < MIN_Y) || (max_x < MIN_X) ) {
                 return (0);
             } else {
                 clear(); mvprintw(1, 30, "%s", f->label); bot = max_y-5;
                 if ( 1 == f->max_values ) {
                     top = 5;
                     mvaddstr(3,2,
                               "Move cursor to desired item and press Enter.");
                 } else {
                     top = 7;
                     mvaddstr(top-4, 2,
                                  "Move cursor to desired item and press F7.");
                     mvaddstr(top-3, 6, "ONE OR MORE items can be selected.");
                     mvaddstr(top-2, 2,
                                   "Press Enter AFTER making all selections.");
                 }
                 if ( S_count > max_y-12 ) {
                     top++; bot--;
                 }
             }
             LIST_LEGEND;
             if ( 1 == f->max_values ) {
                 rows = max_y-12;
             } else {
                 rows = max_y-14;
             }
             for (;;) {
                 for ( i = offset, y = top; i < S_count && y < bot; i++, y++) {
                     mvaddch(y, 2, on[i] ? '>' : ' ');
                     mvaddnstr(y, 4, S[i], max_x-6); clrtoeol();
                 }
                 attron(A_STANDOUT);
                 mvaddnstr(top+(index-offset), 4, S[index], max_x-6);
                 attroff(A_STANDOUT);
                 if ( S_count > max_y-12 ) {
                     if ( offset ) {
                         mvprintw(top-1, 2, "[MORE...%d]", offset); clrtoeol();
                     } else {
                         mvaddstr(top-1, 2, "[TOP]"); clrtoeol();
                     }
                     if ( S_count > offset+(bot-top) ) {
                         mvprintw(max_y-6, 2, "[MORE...%d]",
                                                   S_count-(offset+(bot-top)));
                         clrtoeol();
                     } else {
                         mvaddstr(max_y-6, 2, "[BOTTOM]"); clrtoeol();
                     }
                 }
                 (void)border('|', '|', '-', '-', '+', '+', '+', '+');
                 c = get_key(top+(index-offset), 3);
                 if ( KEY_DOWN == c ) {
                     if ( index < S_count-1 ) {
                         index++;
                         if ( (index-offset) > (bot-top-1) ) {
                             offset++;
                         }
                     }
                 } else if ( KEY_UP == c ) {
                     if ( index ) {
                         if ( index == offset ) {
                             offset--;
                         }
                         index--;
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
                     if ( offset < S_count-rows ) {
                         if ( offset < S_count-2*rows ) {
                             offset += rows; index += rows;
                         } else {
                             index  += S_count-offset-rows;
                             offset += S_count-offset-rows;
                         }
                     } else {
                         index = S_count-1;
                     } 
                 } else if ( (KEY_F(1) == c) || (KEY_F(10) == c)  ) {
                     return (c);
                 } else if ( KEY_F(3) == c ) {
                     return (KEY_F(2));
                 } else if ( (KEY_F(7) == c) || (' ' == c) ) {
                     if ( f->max_values > 1 ) {
                         if ( on[index] ) {
                             size -= on[index]; on[index] = 0; value_count--;
                         } else if ( value_count < f->max_values ) {
                             increase = strlen(S[index]);
                             if ( value_count ) {
                                 increase+=1;
                             }
                             if ( size+increase < f->width ) {
                                 on[index] = increase; value_count++;
                                 size+=increase;
                             }
                         } /*  XXX else ERROR  */
                     }
                 } else if ( KEY_F(8) == c ) {
                     if ( (KEY_F(1) == (k = image())) || (KEY_F(10) == k)  ) {
                         return (k);
                     } else if ( KEY_F(2) == k ) {
                         refresh();
                     }
                 } else if ( '/' == c ) {
                     find_index = find_text(index);
                     if ( FIND_CANCEL == find_index ) {
                         find_index = 0;  /*  No-op.  */
                     } else if ( FIND_EXIT == find_index ) {
                         free(srch_txt); srch_txt = NULL;
                         return ( KEY_F(10) );
                     } else if ( find_index > offset+rows-1 ) {
                         /* Forward find off page. */
                         index = find_index; offset = index-rows+1;
                     } else if ( find_index < offset ) {
                         /* Backward find off page. */
                         index = offset = find_index;
                     } else {
                         index = find_index;
                     }
                     LIST_LEGEND;
                 } else if ( 'n' == c ) {
                     find_index = find_next(index);
                     if ( find_index > offset+rows-1 ) {
                         index = find_index; offset = index-rows+1;
                     } else if ( find_index < offset ) {
                         index = offset = find_index;
                     } else {
                         index = find_index;
                     }
                 } else if ( (KEY_ENTER == c) || ('\r' == c) || ('\n' == c) ) {
                     if ( 1 == f->max_values ) {
                         for ( k = 0; k < S_count; k++ ) {
                             on[k] = 0;
                         }
                         on[index] = 1;
                     }
                     f->buf[0] = '\0';
                     for ( j = 0; j < S_count; j++ ) {
                         if ( on[j] ) {
                             if ( !first ) {
                                 strcat(f->buf, s_v);
                             }
                             strcat(f->buf, S[j]); first = 0;
                         }
                     }
                     for ( k = 0; k < S_count; k++ ) {
                         free(S[k]); S[k] = NULL;
                     }
                     free(S); S = NULL; free(on); S_count = 0; return (0);
                 }
             }
         }
     }
 }
 return (0);
}

