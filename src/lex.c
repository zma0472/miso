
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
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <strings.h>

#include "miso.h"
#include "log.h"
#include "lex.h"

extern int parse(int tok, const char *s, int line, int col);

char  *menu_title  = NULL;
char  *form_title  = NULL;
char **item_label  = NULL;
char **item_output = NULL;

field_t **field = NULL;

int item_count = 0;
int field_count = 0;

#define BUFFER_SIZE 1024
#define TOKEN_SIZE    64

#define LL_FAIL                               \
do {                                          \
    src_line = __LINE__; ll_state = LL_ERROR; \
} while (0)

#define APPEND                           \
do {                                     \
    if ( t_i < TOKEN_SIZE-1 ) {          \
        tok[t_i++] = c; tok[t_i] = '\0'; \
    } else {                             \
        LL_FAIL;                         \
    }                                    \
} while (0)

#ifndef LINT
static char rcsid[] =
"$Id: lex.c,v 1.13 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

static int
str2token(const char *s)
{
 int i = 0;
 struct {
     char *txt;
     int   tkn;
 } token[] = {
     { "MENU",         TOK_MENU },
     { "FORM",         TOK_FORM },
     { "TITLE",        TOK_TITLE },
     { "LABEL",        TOK_LABEL },
     { "OUTPUT",       TOK_OUTPUT },
     { "TYPE",         TOK_TYPE },
     { "WIDTH",        TOK_WIDTH },
     { "MAX_VALUES",   TOK_MAX_VALUES },
     { "MIN_VALUES",   TOK_MIN_VALUES },
     { "REQUIRE",      TOK_REQUIRE },
     { "FLOOR",        TOK_FLOOR },
     { "CEILING",      TOK_CEILING },
     { "PATTERN",      TOK_PATTERN },
     { "ECHO",         TOK_ECHO },
     { "FIND",         TOK_FIND },
     { "EDIT",         TOK_EDIT },
     { "CONSTANT",     TOK_CONSTANT },
     { "VERIFY",       TOK_VERIFY },
     { "LIST",         TOK_LIST },
     { "TEXT",         TOK_TEXT_T },
     { "INTEGER",      TOK_INTEGER_T },
     { "FLOAT",        TOK_FLOAT_T },
     { "BOOLEAN",      TOK_BOOLEAN_T },
     { "TRUE",         TOK_TRUE },
     { "FALSE",        TOK_FALSE },
     { "ON",           TOK_TRUE },
     { "OFF",          TOK_FALSE },
     { "YES",          TOK_TRUE },
     { "NO",           TOK_FALSE },
     { "NULL",         TOK_NULL },
     { "NONE",         TOK_NULL },
     { "VALUE",        TOK_VALUE },
     { "FIELD",        TOK_FIELD },
     { "ITEM",         TOK_ITEM },
     { NULL,           0 }
 };

 for ( i = 0; NULL != token[i].txt; i++ ) {
     if ( 0 == strcasecmp(s, token[i].txt) ) {
         return (token[i].tkn);
     }
 }
 return (0);
}

int
lex(const char *path)
{
 int fd = -1, i = 0, n = -1, t_i = 0, line = 1, col = 1, src_line = 0;
 int tok_id = 0, yy_col = 1;
 char c = '\0', buf[BUFFER_SIZE], tok[TOKEN_SIZE];
 void *parser = NULL;

 enum {
     LL_START   =   0,
     LL_COMMENT =   1,
     LL_INTEGER =   2,
     LL_WORD    =   3,
     LL_STRING  =   4,
     LL_QUOTE   =   5,
     LL_ERROR   = 255
 } ll_state = LL_START;

 buf[0] = '\0';
 if ( (fd = open(path, O_RDONLY)) < 0 ) {
     perror("open"); return (-1);
 }
 for (;;) {
     if ( (n = read(fd, buf, BUFFER_SIZE)) < 0 ) {
         if ( EINTR == errno ) {
             continue;
         } else {
             perror("read"); close(fd); return (-1);
         }
     } else if ( 0 == n ) {
         close(fd);
         if ( (LL_START   == ll_state) ||
              (LL_COMMENT == ll_state) ||
              (LL_QUOTE   == ll_state) ) { 
             return (parse(TOK_EOF, "", line, yy_col));
         } else if ( LL_WORD == ll_state ) {
             if ( 0 != (tok_id = str2token(tok)) ) {
                 if ( 0 == parse(tok_id, tok, line, yy_col) ) {
                     return (parse(TOK_EOF, "", line, yy_col));
                 } else {
                     return (-1);
                 }
             } else {
                 LL_FAIL;
             }
         } else if ( LL_INTEGER == ll_state ) {
             return (parse(TOK_INTEGER, tok, line, yy_col));
         } else {
             return (-1);
         }
     } else {
         for ( i = 0; i < n; i++ ) {
             c = buf[i];
             if ( LL_START == ll_state ) {
                 if ( ('\n' == c) || isspace(c) ) {
                     ll_state = LL_START;
                 } else if ( '#' == c ) {
                     ll_state = LL_COMMENT;
                 } else if ( isdigit(c) ) {
                     APPEND; ll_state = LL_INTEGER; yy_col = col;
                 } else if ( ('_' == c) || isalpha(c) ) {
                     APPEND; ll_state = LL_WORD; yy_col = col;
                 } else if ( '\'' == c ) {
                     ll_state = LL_STRING; yy_col = col; tok[0] = '\0';
                 } else if ( '=' == c ) {
                     yy_col = col;
                     if ( parse(TOK_EQUALS, "=", line, yy_col) < 0 ) {
                         return (-1);
                     } else {
                         ll_state = LL_START;
                     }
                 } else {
                     LL_FAIL;
                 }
             } else if ( LL_INTEGER == ll_state ) {
                 if ( isdigit(c) ) {
                     APPEND;
                 } else if ( isspace(c) ) {
                     if ( parse(TOK_INTEGER, tok, line, yy_col) < 0 ) {
                         return (-1);
                     } else {
                         t_i = 0; ll_state = LL_START;
                     }
                 } else {
                     LL_FAIL;
                 }
             } else if ( LL_WORD == ll_state ) {
                 if ( isalnum(c) || ('_' == c) ) {
                     APPEND;
                 } else if ( isspace(c) ) {
                     if ( (tok_id = str2token(tok)) > 0 ) {
                         if ( parse(tok_id, tok, line, yy_col) < 0 ) {
                             return (-1);
                         } else {
                             ll_state = LL_START; t_i = 0;
                         }
                     } else {
                         LL_FAIL;
                     }
                 } else if ( ':' == c ) {
                     if ( (tok_id = str2token(tok)) > 0 ) {
                         if ( parse(tok_id, tok, line, yy_col) < 0 ) {
                             return (-1);
                         } else if ( parse(TOK_COLON, ":",line, yy_col) < 0 ) {
                             return (-1);
                         } else {
                             ll_state = LL_START; t_i = 0;
                         }
                     } else {
                         LL_FAIL;
                     }
                 } else if ( '=' == c ) {
                     if ( (tok_id = str2token(tok)) > 0 ) {
                         if ( parse(tok_id, tok, line, yy_col) < 0 ) {
                             return (-1);
                         } else if ( parse(TOK_EQUALS, "=",line,yy_col) < 0 ) {
                             return (-1);
                         } else {
                             ll_state = LL_START; t_i = 0;
                         }
                     } else {
                         LL_FAIL;
                     }
                 } else {
                     LL_FAIL;
                 }
             } else if ( LL_COMMENT == ll_state ) {
                 if ( '\n' == c ) {
                     ll_state = LL_START;
                 } else if ( !isprint(c) ) {
                     LL_FAIL;
                 }
             } else if ( LL_STRING == ll_state ) {
                 if ( '\'' == c ) {
                     ll_state = LL_QUOTE;
                 } else if ( isprint(c) ) {
                     APPEND;
                 } else {
                     LL_FAIL;
                 }
             } else if ( LL_QUOTE == ll_state ) {
                 if ( '\'' == c ) {
                     APPEND; ll_state = LL_STRING;
                 } else if ( isspace(c) ) {
                     if ( parse(TOK_STRING, tok, line, yy_col) < 0 ) {
                         return (-1);
                     } else {
                         ll_state = LL_START; t_i = 0;
                     }
                 } else {
                     LL_FAIL;
                 }
             } else {
                 LL_FAIL;
             }
             if ( LL_ERROR == ll_state ) {
                 (void)fprintf(stderr,
                               "%s(%d): ERROR: Lexical error @ %s(%d.%d)\n",
                                          __FILE__, src_line, path, line, col);
                 return (-1);
             }
             if ( '\n' == c ) {
                 line++; col = 1;
             } else {
                 col++;
             }
         }
     }
 }
}
