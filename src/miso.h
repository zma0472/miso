
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

#ifndef _MISO_H

#define _MISO_H 1

#include <sys/types.h>
#include <regex.h>

#include "log.h"
#include "msg.h"

#define VERSION_MAJOR  0
#define VERSION_MINOR  9
#define VERSION_PATCH 15

#define DEFAULT_SCHEMA "/opt/etc/miso.conf"

/*  Fix for Tru64 Unix. */
#ifdef __alpha
#define getmaxyx(w,y,x)	((y) = getmaxy(w), (x) = getmaxx(w))
#endif /*  defined __alpha  */

/*  Values assumed by 'type' member of field_t struct.  */
#define NONE_T    0
#define BOOLEAN_T 1
#define TEXT_T    2
#define FLOAT_T   3
#define INTEGER_T 4

#define DEFAULT_FIELD_TYPE       NONE_T
#define DEFAULT_FIELD_WIDTH      64
#define DEFAULT_FIELD_MAX_VALUES 1
#define DEFAULT_FIELD_MIN_VALUES 0
#define DEFAULT_FIELD_REQUIRE    0
#define DEFAULT_FIELD_FLOOR      0
#define DEFAULT_FIELD_CEILING    4294967295
#define DEFAULT_FIELD_PATTERN    ".*"
#define DEFAULT_FIELD_ECHO       1
#define DEFAULT_FIELD_FIND       NULL
#define DEFAULT_FIELD_EDIT       1
#define DEFAULT_FIELD_CONSTANT   0
#define DEFAULT_FIELD_VERIFY     NULL
#define DEFAULT_FIELD_LIST       NULL
#define DEFAULT_FIELD_VALUE      NULL
#define DEFAULT_FIELD_BUF        NULL

#define MAX_FIELD_WIDTH 256

#define MAX_VALUES_LEN 1048576

#define DEFAULT_OFS ':'
#define DEFAULT_OVS ','

#define DEFAULT_VALUE_PATH NULL
#define DEFAULT_VALUES NULL

#define F1_BIT    ( (unsigned long)0x1   )
#define F2_BIT    ( (unsigned long)0x2   )
#define F3_BIT    ( (unsigned long)0x4   )
#define F4_BIT    ( (unsigned long)0x8   )
#define F5_BIT    ( (unsigned long)0x10  )
#define F6_BIT    ( (unsigned long)0x20  )
#define F7_BIT    ( (unsigned long)0x40  )
#define F8_BIT    ( (unsigned long)0x80  )
#define F9_BIT    ( (unsigned long)0x10  )
#define F10_BIT   ( (unsigned long)0x20  )
#define NPAGE_BIT ( (unsigned long)0x40  )
#define PPAGE_BIT ( (unsigned long)0x80  )
#define UP_BIT    ( (unsigned long)0x100 )
#define DOWN_BIT  ( (unsigned long)0x200 )

typedef struct t_field {
    char *label;
    int   type;
    unsigned long   width;
    unsigned long   max_values;
    unsigned long   min_values;
    int   require;
    int   floor;
    int   ceiling;
    regex_t *ptn_cmp;
    char *pattern;
    int   echo;
    char *find;
    int   edit;
    int   constant;
    char *verify;
    char *list;
    char *value;
    char *buf;
    int   valid_field;
    int   valid_value;
} field_t;

typedef struct t_item {
    char *label;
    char *output;
} item_t;

extern char  *program_output;
extern char  *menu_title;
extern char  *form_title;
extern char  *schema;
extern char   field_separator;
extern char   value_separator;
extern char  *value_path;
extern char  *log_path;
extern char  *values;
extern char  *shell_path;

extern FILE *log_file;

extern item_t  **item;

extern field_t **field;

extern int item_count;
extern int field_count;
extern int check_only;

extern int menu(void);
extern int form(void);
extern void print_config(void);
extern int lex(const char *path);
extern void args(int argc, char **argv);
extern void version(void);
extern void usage(void);
extern void output_values(void);
extern int  subshell(void);
extern int  image(void);

extern int io_field(int row, int interact, field_t *f);
extern int new_field(void);
extern int list(field_t *f);
extern int  io_list(field_t *f);
extern int get_key(int y, int x);
extern void find_value(field_t *f);
extern void clean(void);
extern int set_values(void);
extern int form_valid(void);
extern int field_valid(field_t *f);
extern int menu_valid(void);
extern int item_valid(item_t *I);

extern int io_text(int y, int x, char *buf, int width, unsigned long mask);
extern int err_msg(unsigned int error_id);

extern char valid_separator(const char *s);
extern char *str_sep(char **stringp, const char *delim);

#endif /*  !defined _MISO_H  */
