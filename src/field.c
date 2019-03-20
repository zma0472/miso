
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

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <curses.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>

#include "miso.h"
#include "log.h"

static int
io_boolean(int row, int interact, field_t *f)
{
 int c = 0, k = 0, col = 53, max_row = 0, max_col = 0;

 getmaxyx(stdscr, max_row, max_col);

 move(row, 0);
 if ( f->require ) {
     addch('*');
 }

 clrtoeol();
 attron(A_STANDOUT); mvaddnstr(row, 2, f->label, col-2); attroff(A_STANDOUT);
 for (;;) {
     if ( strcmp(f->value, f->buf) ) {
         attron(A_STANDOUT);
     }
     mvaddstr(row, col, f->buf); attroff(A_STANDOUT); clrtoeol();
     if ( 0 == f->constant ) {
         mvaddch(row, max_col-3, '+');
     }
     if ( 0 == interact ) {
         attroff(A_STANDOUT); mvaddnstr(row, 2, f->label, col-2);
         return (KEY_DOWN);
     }
     refresh(); c = get_key(row, col);
     if ( ('\t' == c) && (0 == f->constant) ) {
         if ( 0 == strcasecmp(f->buf, "true") ) {
             strcpy(f->buf, "false");
         } else if ( 0 == strcasecmp(f->buf, "on") ) {
             strcpy(f->buf, "off");
         } else if ( 0 == strcasecmp(f->buf, "yes") ) {
             strcpy(f->buf, "no");
         } else if ( (0 == strcasecmp(f->buf, "false")) ||
                     (0 == strcasecmp(f->buf, "off"))   ||
                     (0 == strcasecmp(f->buf, "no")) ) {
             f->buf[0] = '\0';
         } else if ( '\0' == f->buf[0] ) {
             if ( (0 == strcasecmp(f->value, "on")) ||
                  (0 == strcasecmp(f->value, "off")) ) {
                 strcpy(f->buf, "on");
             } else if ( (0 == strcasecmp(f->value, "yes")) ||
                         (0 == strcasecmp(f->value, "no")) ) {
                 strcpy(f->buf, "yes");
             } else {
                 strcpy(f->buf, "true");
             }
         }
     } else if ( KEY_F(4) == c ) {
         if ( (KEY_F(1) == (k = io_list(f))) ||
              (KEY_F(3) == k) || (KEY_F(10) == k) ) {
             return (k);
         } else {
             return (KEY_F(2));
         }
     } else if ( KEY_F(8) == c ) {
         if ( (KEY_F(1) == (k = image())) || (KEY_F(10) == k) ) {
             attroff(A_STANDOUT); mvaddnstr(row, 2, f->label, col-2);
             return(k);
         } else {
             return (KEY_F(2));
         }
     } else if ( KEY_F(9) == c ) {
         if ( (KEY_F(1) == (k = subshell())) || (KEY_F(10) == k) ) {
             attroff(A_STANDOUT); mvaddnstr(row, 2, f->label, col-2);
             return(k);
         } else {
             return (KEY_F(2));
         }
     } else if ( (KEY_DOWN   == c) ||
                 (KEY_UP     == c) ||
                 (KEY_PPAGE  == c) ||
                 (KEY_NPAGE  == c) ||
                 (KEY_ENTER  == c) ||
                 ('\n'       == c) ||
                 ('\r'       == c) ||
                 (KEY_F(1)   == c) ||
                 (KEY_F(2)   == c) ||
                 (KEY_F(3)   == c) ||
                 (KEY_F(8)   == c) ||
                 (KEY_F(10)  == c) ) {
         attroff(A_STANDOUT); mvaddnstr(row, 2, f->label, col-2);
         return (c);
     }
 }
}

int
new_field(void)
{
 field_t **t_p = NULL;
 int rgx_flags = (REG_EXTENDED|REG_ICASE|REG_NOSUB|REG_NEWLINE);

 t_p = realloc(field, (field_count+1)*sizeof(field_t *));
 if ( NULL == t_p ) {
     goto realloc_fail;
 }
 field = t_p;
 if ( NULL == (field[field_count] = calloc(1, sizeof(field_t))) ) {
     goto field_fail;
 }
 field[field_count]->type        = DEFAULT_FIELD_TYPE;
 field[field_count]->width       = DEFAULT_FIELD_WIDTH;
 field[field_count]->max_values  = DEFAULT_FIELD_MAX_VALUES;
 field[field_count]->min_values  = DEFAULT_FIELD_MIN_VALUES;
 field[field_count]->require     = DEFAULT_FIELD_REQUIRE;
 field[field_count]->floor       = DEFAULT_FIELD_FLOOR;
 field[field_count]->ceiling     = DEFAULT_FIELD_CEILING;
 field[field_count]->echo        = DEFAULT_FIELD_ECHO;
 field[field_count]->find        = DEFAULT_FIELD_FIND;
 field[field_count]->edit        = DEFAULT_FIELD_EDIT;
 field[field_count]->constant    = DEFAULT_FIELD_CONSTANT;
 field[field_count]->verify      = DEFAULT_FIELD_VERIFY;
 field[field_count]->list        = DEFAULT_FIELD_LIST;
 field[field_count]->value       = DEFAULT_FIELD_VALUE;
 field[field_count]->buf         = DEFAULT_FIELD_VALUE;
 field[field_count]->valid_field = FALSE;
 field[field_count]->valid_value = FALSE;

 field[field_count]->value = calloc(1, DEFAULT_FIELD_WIDTH+1);
 if ( NULL == field[field_count]->value ) {
     goto value_fail;
 }
 field[field_count]->buf = calloc(1, DEFAULT_FIELD_WIDTH+1);
 if ( NULL == field[field_count]->buf ) {
     goto buf_fail;
 }
 field[field_count]->ptn_cmp = calloc(1, sizeof(regex_t));
 if ( NULL == field[field_count]->ptn_cmp ) {
     goto ptn_cmp_fail;
 }
 field[field_count]->pattern = strdup(DEFAULT_FIELD_PATTERN);
 if ( NULL == field[field_count]->pattern ) {
     goto pattern_fail;
 }
 if ( regcomp(field[field_count]->ptn_cmp,
              field[field_count]->pattern, rgx_flags) < 0 ) {
     goto rgx_fail;
 }
 field_count++; return (0);

pattern_fail:

rgx_fail:
 free(field[field_count]->ptn_cmp); field[field_count]->ptn_cmp = NULL;

ptn_cmp_fail:
 free(field[field_count]->buf); field[field_count]->buf = NULL;

buf_fail:
 free(field[field_count]->value); field[field_count]->value = NULL;

value_fail:
 free(field[field_count]); field[field_count] = 0;

field_fail:
 t_p = realloc(field, field_count*sizeof(field_t *));
 if ( NULL != t_p ) {
     field = t_p;
 }

realloc_fail:
 return (-1);
}

int
io_field(int row, int interact, field_t *f)
{
 int c = 0, max_col = 0, max_row = 0, col = 51, k = 0;
 int wall = 0, index = 0, cursor = 0, offset = 0;

 if ( ! f->width ) {
     return (0);
 }
 getmaxyx(stdscr, max_row, max_col); wall = max_col-6; cursor = col+2;

 if ( BOOLEAN_T == f->type ) {
     return (io_boolean(row, interact, f));
 }
 move(row, 0);
 if ( f->require ) {
     addch('*');
 }
 clrtoeol();
 if ( interact ) {
     attron(A_STANDOUT);
 }
 mvaddnstr(row, 2, f->label, col-2); attroff(A_STANDOUT);
 for (;;) {
     int i = 0;
     if ( 0 == f->constant ) {
         if ( offset ) {
             mvaddch(row, col+1, '<');
         } else {
             mvaddch(row, col+1, '[');
         }
     }
     if ( strcmp(f->value, f->buf) ) {
         attron(A_STANDOUT);
     }
     if ( f->echo ) {
         mvaddnstr(row, col+2, &f->buf[offset], wall-col-2);
     } else {
         move(row, col+2);
         for ( i = offset; (i < wall-col-2) && (f->buf[i]); i++) {
             addch('*');
         }
     }
     attroff(A_STANDOUT);
     if ( 0 == f->constant ) {
         if ( strlen(f->buf)-offset > wall-col-2 ) {
             addch('>');
         } else {
             addch(']');
         }
     }
     clrtoeol();
     if ( (NULL != f->list) && (0 == f->constant) ) {
         mvaddch(row, max_col-3, '+');
     }
     if ( (FLOAT_T == f->type) || (INTEGER_T == f->type) ) {
         mvaddch(row, max_col-2, '#');
     }
     move(row, cursor); refresh();

     if ( 0 == interact ) {
         return (0);
     }

     c = get_key(row, cursor);

     if ( (KEY_ENTER == c) || ('\n' == c) || ('\r' == c) ) {
         attroff(A_STANDOUT); mvaddnstr(row, 2, f->label, col-2);
         return (KEY_ENTER);
     } else if ( KEY_LEFT == c ) {
         if ( index ) {
             index--;
             if ( col+2 == cursor ) {
                 if ( offset ) {
                     offset--;
                 }
             } else {
                 cursor--;
             }
         }
     } else if ( KEY_RIGHT == c ) {
         if ( '\0' != f->buf[index] ) {
             index++;
             if ( cursor == wall ) {
                 offset++;
             } else {
                 cursor++;
             }
         }
     } else if ( KEY_DC == c ) {
         if ( 0 == f->constant ) {
             char *p = NULL;
             for ( p = &(f->buf[index]); p && *p; p++ ) {
                 *p = *(p+1);
             }
         }
     } else if ( ((KEY_BACKSPACE == c) || (127 == c)) && (0 == f->constant) ) {
         if ( index ) {
             char *p = NULL;
             for ( p = &(f->buf[--index]); p && *p; p++ ) {
                 *p = *(p+1);
             }
             if ( 0 == offset ) {
                 if (col+2 < cursor) {
                     cursor--;
                 }
             } else {
                 offset--;
             }
         }
     } else if ( (KEY_F(1)  == c) ||
                 (KEY_F(2)  == c) ||
                 (KEY_F(3)  == c) ||
                 (KEY_F(5)  == c) ||
                 (KEY_NPAGE == c) ||
                 (KEY_PPAGE == c) ||
                 (KEY_DOWN  == c) ||
                 (KEY_UP    == c) ||
                 (KEY_F(10) == c) ) {
         attroff(A_STANDOUT); mvaddnstr(row, 2, f->label, col-2);
         return (c);
     } else if ( KEY_HOME == c ) {
         index = 0; offset = 0; cursor = col+2;
     } else if ( KEY_END == c ) {
         unsigned int i = 0;
         index = strlen(f->buf);
         if ( col+2+index < wall ) {
             cursor = col+2+index; offset = 0;
         } else { 
             cursor = wall; offset = col+2+index-wall;
         }
     } else if ( KEY_F(4) == c ) {
         if ( (KEY_F(1) == (k = io_list(f))) ||
              (KEY_F(3) == k) || (KEY_F(10) == k) ) {
             return (k);
         } else {
             return (KEY_F(2));
         }
     } else if ( KEY_F(8) == c ) {
         if ( (KEY_F(1) == (k = image())) || (KEY_F(10) == k) ) {
             return (k);
         } else {
             return (KEY_F(2));
         }
     } else if ( KEY_F(9) == c ) {
         if ( (KEY_F(1) == (k = subshell())) || (KEY_F(10) == k) ) {
             return (k);
         } else {
             return (KEY_F(2));
         }
     } else if ( isprint(c) && (c != field_separator) ) {
         if ( (strlen(f->buf) < f->width) && (0 == f->constant) ) {
             unsigned int i = 0;
             if ( (INTEGER_T == f->type) && !isdigit(c) ) {
                 continue;
             }
             /*  Right shift all chars to the right of the edit index to make
              *  room for the char insert.
              */
             for ( i = strlen(f->buf); i > index; i-- ) {
                 f->buf[i] = f->buf[i-1];
             }
             /*  Make the insert & increment the edit index.  */
             f->buf[index++] = c;
             if (cursor == wall) {
                 offset++;
             } else {
                 cursor++;
             }
         }
     }
 }
}

int
field_valid(field_t *f)
{
 return (0);
}

