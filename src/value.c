
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
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "miso.h"
#include "log.h"

#ifndef LINT
static char rcsid[] =
"$Id: value.c,v 1.9 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

char
valid_separator(const char *s)
{
 char c = 0;
 size_t len = 0;

 if ( (1 == (len = strlen(s))) && (isprint(s[0]) || isspace(s[0])) ) {
     c = s[0]; 
 } else if ( (2 == len) && ('\\' == s[0]) ) {
     if ( 't' == s[1] ) {
         c = '\t';
     } else if ( 'n' == s[1] ) {
         c = '\n';
     } else if ( 'v' == s[1] ) {
         c = '\v';
     } else if ( 'f' == s[1] ) {
         c = '\f';
     } else if ( 'r' == s[1] ) {
         c = '\r';
     }
 } else {
     ERR("Invalid separator.");
 }
 return (c);
}

void
output_values(void)
{
 int i = 0;
 
 for ( i = 0; i < field_count; i++ ) {
     (void)fprintf(stderr, "%s", field[i]->buf);
     if ( i < field_count-1 ) {
         (void)fprintf(stderr, "%c", field_separator);
     } else {
         (void)fprintf(stderr, "\n");
     }
 }
 return;
}

static int
assign_value(field_t *f, char *v)
{
 unsigned long x = 0, v_c = 1;
 char *p = NULL, *endptr = NULL, *q = NULL;

 if ( strlen(v) > f->width ) {
     ERR("Field width=%lu exceeded '%s'", f->width, v); return (-1);
 }
 for ( q = v; q && *q; q++ ) {
     if ( value_separator == *q ) {
         if ( BOOLEAN_T == f->type ) {
             ERR("Value separator character in boolean value"); return (-1);
         } else if ( ++v_c > f->max_values ) {
             ERR("max_values=%lu exceeded by v_c=%lu", f->max_values, v_c);
             return (-1);
         }
     } else if ( field_separator == *q ) {
         ERR("Field separator character in value assignment."); return (-1);
     }
 }
 if ( BOOLEAN_T == f->type ) {
     if ( 0 == strcasecmp(v, "true") ) {
         strcpy(f->buf, "true");
     } else if ( 0 == strcasecmp(v, "false") ) {
         strcpy(f->buf, "false");
     } else if ( 0 == strcasecmp(v, "yes") ) {
         strcpy(f->buf, "yes");
     } else if ( 0 == strcasecmp(v, "no") ) {
         strcpy(f->buf, "no");
     } else if ( 0 == strcasecmp(v, "on") ) {
         strcpy(f->buf, "on");
     } else if ( 0 == strcasecmp(v, "off") ) {
         strcpy(f->buf, "off");
     } else {
         ERR("Invalid boolean value v='%s'", v); return (-1);
     }
 } else if ( TEXT_T == f->type ) {
     for ( p = v; p && *p; p++ ) {
         if ( !isprint(*p) ) {
             return (-1);
         }
     }
     strcpy(f->buf, v);
 } else if ( FLOAT_T == f->type ) {
     return (-1);  /*  XXX  */
 } else if ( INTEGER_T == f->type ) {
     if ( ULONG_MAX == (x = strtoul(v, &endptr, 10)) ) {
         if ( ERANGE == errno ) {
             ERR("ULONG_MAX overflow v='%s'", v); return (-1);
         }
     }
     if ( '\0' != *v ) {
         if ( '\0' == *endptr ) {
             if ( (x <= (unsigned long)f->ceiling) &&
                  (x >= (unsigned long)f->floor) ) {
                 strcpy(f->buf, v);
             } else {
                 ERR("Invalid integer value x=%lu v='%s'", x, v);
             }
         } else {
             ERR("Invalid character in integer value v='%s'", v); return (-1);
         }
     }
 } else {
     return (-1);
 }
 return (0);
}

int
set_values(void)
{
 int fd = -1, n = 0, val = -1, f_c = 0;
 char buf[MAX_VALUES_LEN], *bp = NULL, *fp = NULL, *vp = NULL;

 if ( NULL == field ) {
     return (0);
 } else {
     if ( NULL == values ) {
         if ( NULL == value_path ) {
             return (0);
         } else {
             if ( (fd = open(value_path, O_RDONLY)) < 0 ) {
                 ERR("Open initial values file FAIL"); return (-1);
             } else {
                 memset(buf, 0, MAX_VALUES_LEN);
                 if ( (n = read(fd, buf, MAX_VALUES_LEN-1)) < 0 ) {
                     ERR("Read initial values file FAIL."); return (-1);
                 } else if ( 0 <= n ) {
                     buf[n] = '\0'; values = buf;
                 }
                 (void)close(fd);
             }
         }
     }
     bp = &(values[0]);
     while ( NULL != (fp = str_sep(&bp, &field_separator)) ) {
         if ( ++f_c > field_count ) {
             ERR("field_count=%d exceeded by f_c=%d", field_count, f_c);
             return (-1);
         } else if ( assign_value(field[f_c-1], fp) < 0 ) {
             return (-1);
         }
     }
 }
 return (0);
}

