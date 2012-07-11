
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

#include "miso.h"

#ifndef LINT
static char rcsid[] =
"$Id: main.c,v 1.17 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

char *schema = DEFAULT_SCHEMA;
char  field_separator = DEFAULT_OFS;
char  value_separator = DEFAULT_OVS;
char *value_path = DEFAULT_VALUE_PATH;
char *values = DEFAULT_VALUES;
char *log_label = "MISO";
char *log_path = NULL;
char *shell_path = NULL;
FILE *log_file = NULL;
int   debug = 0;

int
main(int argc, char **argv)
{
 int val = 2;
 char *s = NULL;

 if ( NULL != (s = getenv("MISO_DEBUG")) ) {
     if ( 0 == strcmp(s, "1") ) {
         debug = 1;
     }
 }
 if ( NULL != (s = getenv("MISO_LOG_PATH")) ) {
     if ( NULL == (log_file = fopen((log_path=s), "a")) ) {
         log_file = stderr; ERR("log_path='%s'", log_path);  return (2);
     } else {
         DBG("log_path='%s'", log_path);
     }
     LOG("debug=%d", debug);
 }
 if ( NULL != (s = getenv("MISO_SCHEMA")) ) {
     schema = s; DBG("schema=(MISO_SCHEMA='%s')", s);
 }
 if ( NULL != (s = getenv("MISO_OFS")) ) {
     if ( 0 == (field_separator = valid_separator(s)) ) {
         log_file = stderr; ERR("Invalid output field separator."); exit(2);
     } else {
         DBG("field_separator=(MISO_OFS='%c')", field_separator);
     }
 }
 if ( NULL != (s = getenv("MISO_OVS")) ) {
     if ( 0 == (value_separator = valid_separator(s)) ) {
         log_file = stderr; ERR("Invalid output value separator."); exit(2);
     } else {
         DBG("value_separator=(MISO_OVS='%c')", value_separator);
     }
 }
 if ( NULL != (s = getenv("MISO_VALUES")) ) {
     values = s; DBG("values=(MISO_VALUES='%s')", values);
 }
 if ( NULL != (s = getenv("MISO_VALUE_PATH")) ) {
     value_path = s; DBG("value_path=(MISO_VALUE_PATH='%s')", value_path);
 }
 if ( NULL != (s = getenv("SHELL")) ) {
     shell_path = s; DBG("shell_path=(SHELL='%s')", shell_path);
 }
 if ( NULL != (s = getenv("MISO_SHELL")) ) {
     shell_path = s; DBG("shell_path=(MISO_SHELL='%s')", shell_path);
 }
 args(argc, argv);
 if ( lex(schema) < 0 ) {
     ERR("Lexical error.");
 } else {
     if ( NULL != menu_title ) {
         DBG("menu_title='%s'", menu_title);
         if ( (0 == (val = menu())) || (1 == val) ) {
             DBG("val=%d", val);
             if ( NULL != program_output ) {
                 DBG("program_output='%s'", program_output);
                 fprintf(stderr, "%s\n", program_output);
             }
         }
     } else if ( NULL != form_title ) {
         DBG("form_title='%s'", form_title);
         if ( 0 == set_values() ) {
             if ( (0 == (val = form())) || (1 == val) ) {
                 DBG("val=%d", val);
                 output_values();
             }
         }
     } else {
         ERR("No object title."); exit(2);
     }
 }
 DBG("return(val=%d)", val);
 return (val);
}

