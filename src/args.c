
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

#include "miso.h"
#include "log.h"

#ifndef LINT
static char rcsid[] =
"$Id: args.c,v 1.12 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

int check_only = 0;

void
args(int argc, char **argv)
{
 int c = 0;

 while ( (c = getopt(argc, argv, "hvcdF:V:i:f:l:s:")) != -1 ) {
     if ( 'v' == c ) {
         DBG("c='%c'", c);
         version(); exit(0);
     } else if ( 'h' == c ) {
         DBG("c='%c'", c);
         usage(); exit(0);
     } else if ( 'c' == c ) {
         check_only = 1;
         DBG("c='%c' check_only=%d", c, check_only);
     } else if ( 'd' == c ) {
         DBG("c='%c' debug=%d", c, debug);
         debug = 1;
         DBG("c='%c' debug=%d", c, debug);
     } else if ( 'F' == c ) {
         DBG("c='%c'", c);
         if ( 0 == (field_separator = valid_separator(optarg)) ) {
             ERR("Invalid field separator."); exit (2);
         } else {
             DBG("field_separator=(optarg='%s')", optarg);
         }
     } else if ( 'V' == c ) {
         DBG("c='%c'", c);
         if ( 0 == (value_separator = valid_separator(optarg)) ) {
             ERR("Invalid value separator."); exit (2);
         } else {
             DBG("value_separator=(optarg='%s')", optarg);
         }
     } else if ( 'i' == c ) {
         values = optarg;
         DBG("c='%c' values='%s'", c, values);
     } else if ( 'f' == c ) {
         value_path = optarg;
         DBG("c='%c' value_path='%s'", c, value_path);
     } else if ( 'l' == c ) {
         log_path = optarg;
         DBG("c='%c' log_path='%s'", c, log_path);
     } else if ( 's' == c ) {
         shell_path = optarg;
         DBG("c='%c' shell_path='%s'", c, shell_path);
     } else {
         DBG("c='%c'", c);
         usage(); exit(2);
     }
 }
 if ( optind == (argc-1) ) {
     schema = argv[optind];
     DBG("schema='%s'", argv[optind]);
 } else if ( optind < (argc-1) ) {
     DBG("argc=%d optind=%d", argc, optind);
     usage(); exit(2);
 }
 if ( field_separator == value_separator ) {
     if ( NULL == log_file ) {
         log_file = stderr;
     }
     ERR("Field and value separators cannot be the same character."); exit(2);
 }
 return;
}
