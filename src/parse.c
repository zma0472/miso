
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
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>

#include "miso.h"
#include "log.h"
#include "lex.h"
#include "parse.h"

#ifndef LINT
static char rcsid[] =
"$Id: parse.c,v 1.22 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

item_t **item = NULL;

int
parse(int tok, const char *s, int line, int col)
{
 int val = -1, i = 0;
 int rgx_flags = (REG_EXTENDED|REG_ICASE|REG_NOSUB|REG_NEWLINE);
 unsigned long w = 0;
 field_t **tmp_field = NULL;
 char     *tmp_str   = NULL;

 if ( YY_START == yy_state ) {
     if ( TOK_MENU == tok ) {
         yy_state = YY_MENU; val = 0;
     } else if ( TOK_FORM == tok ) {
         yy_state = YY_FORM; val = 0;
     } else {
         YY_FAIL("Expected MENU or FORM");
     }
 } else if ( YY_MENU == yy_state ) {
     if ( TOK_COLON == tok ) {
         yy_state = YY_MENU_COLON; val = 0;
     } else {
         YY_FAIL("Expected ':'");
     }
 } else if ( YY_MENU_COLON == yy_state ) {
     if ( TOK_TITLE == tok ) {
         yy_state = YY_MENU_TITLE; val = 0;
     } else if ( TOK_ITEM == tok ) {
         if ( NULL == menu_title ) {
             YY_FAIL("Expected TITLE");
         } else {
             NEW_ITEM;
         }
     } else {
         YY_FAIL("Expected TITLE or ITEM");
     }
 } else if ( YY_MENU_TITLE == yy_state ) {
     CK_EQ(YY_MENU_TITLE_EQUAL);
 } else if ( YY_MENU_TITLE_EQUAL == yy_state ) {
     CK_TEXT(menu_title, YY_MENU_COLON);
 } else if ( YY_ITEM == yy_state ) {
     if ( TOK_COLON == tok ) {
         yy_state = YY_ITEM_COLON; val = 0;
     } else {
         YY_FAIL("Expected ':'");
     }
 } else if ( YY_ITEM_COLON == yy_state ) {
     if ( TOK_LABEL == tok ) {
         yy_state = YY_ITEM_LABEL; val = 0;
     } else if ( TOK_OUTPUT == tok ) {
         yy_state = YY_ITEM_OUTPUT; val = 0;
     } else if ( TOK_ITEM == tok ) {
         if ( NULL == item[item_count-1]->label ) {
             YY_FAIL("Missing item label");
         } else if ( NULL == item[item_count-1]->output ) {
             YY_FAIL("Missing item output");
         } else {
             NEW_ITEM;
         }
     } else if ( TOK_EOF == tok ) {
         if ( NULL == item[item_count-1]->label ) {
             YY_FAIL("Missing item label");
         } else if ( NULL == item[item_count-1]->output ) {
             YY_FAIL("Missing item output");
         } else {
             val = 0;
         }
     } else {
         YY_FAIL("Expected LABEL or OUTPUT or ITEM");
     }
 } else if ( YY_ITEM_LABEL == yy_state ) {
     CK_EQ(YY_ITEM_LABEL_EQUAL);
 } else if ( YY_ITEM_LABEL_EQUAL == yy_state ) {
     CK_TEXT(item[item_count-1]->label, YY_ITEM_COLON);
 } else if ( YY_ITEM_OUTPUT == yy_state ) {
     CK_EQ(YY_ITEM_OUTPUT_EQUAL);
 } else if ( YY_ITEM_OUTPUT_EQUAL == yy_state ) {
     CK_TEXT(item[item_count-1]->output, YY_ITEM_COLON);
 } else if ( YY_FORM == yy_state ) {
     if ( TOK_COLON == tok ) {
         yy_state = YY_FORM_COLON; val = 0;
     } else {
         YY_FAIL("Expected ':'");
     }
 } else if ( YY_FORM_COLON == yy_state ) {
     if ( TOK_TITLE == tok ) {
         yy_state = YY_FORM_TITLE; val = 0;
     } else if ( TOK_FIELD == tok ) {
         if ( NULL == form_title ) {
             YY_FAIL("Expecting TITLE");
         } else {
             NEW_FIELD; yy_state = YY_FIELD;
         }
     } else {
         YY_FAIL("Expected TITLE or FIELD");
     }
 } else if ( YY_FORM_TITLE == yy_state ) {
     CK_EQ(YY_FORM_TITLE_EQUAL);
 } else if ( YY_FORM_TITLE_EQUAL == yy_state ) {
     CK_TEXT(form_title, YY_FORM_COLON);
 } else if ( YY_FIELD == yy_state ) {
     if ( TOK_COLON == tok ) {
         yy_state = YY_FIELD_COLON; val = 0;
     } else {
         YY_FAIL("Expected ':'");
     }
 } else if ( YY_FIELD_COLON == yy_state ) {
     if ( TOK_LABEL == tok ) {
         yy_state = YY_FIELD_LABEL; val = 0;
     } else if ( TOK_TYPE == tok ) {
         yy_state = YY_FIELD_TYPE; val = 0;
     } else if ( TOK_WIDTH == tok ) {
         yy_state = YY_FIELD_WIDTH; val = 0;
     } else if ( TOK_MAX_VALUES == tok ) {
         yy_state = YY_FIELD_MAX_VALUES; val = 0;
     } else if ( TOK_MIN_VALUES == tok ) {
         yy_state = YY_FIELD_MIN_VALUES; val = 0;
     } else if ( TOK_REQUIRE == tok ) {
         yy_state = YY_FIELD_REQUIRE; val = 0;
     } else if ( TOK_FLOOR == tok ) {
         yy_state = YY_FIELD_FLOOR; val = 0;
     } else if ( TOK_CEILING == tok ) {
         yy_state = YY_FIELD_CEILING; val = 0;
     } else if ( TOK_PATTERN == tok ) {
         yy_state = YY_FIELD_PATTERN; val = 0;
     } else if ( TOK_ECHO == tok ) {
         yy_state = YY_FIELD_ECHO; val = 0;
     } else if ( TOK_FIND == tok ) {
         yy_state = YY_FIELD_FIND; val = 0;
     } else if ( TOK_EDIT == tok ) {
         yy_state = YY_FIELD_EDIT; val = 0;
     } else if ( TOK_CONSTANT == tok ) {
         yy_state = YY_FIELD_CONSTANT; val = 0;
     } else if ( TOK_VERIFY == tok ) {
         yy_state = YY_FIELD_VERIFY; val = 0;
     } else if ( TOK_LIST == tok ) {
         yy_state = YY_FIELD_LIST; val = 0;
     } else if ( TOK_VALUE == tok ) {
         yy_state = YY_FIELD_VALUE; val = 0;
     } else if ( TOK_FIELD == tok ) {
         if ( NULL == field[field_count-1]->label ) {
             YY_FAIL("Expecting LABEL");
         } else {
             NEW_FIELD; yy_state = YY_FIELD;
         }
     } else if ( TOK_EOF == tok ) {
         if ( NULL == field[field_count-1]->label ) {
             YY_FAIL("Missing field label");
         } else {
             val = 0;
         }
     } else {
         YY_FAIL("Expected field attribute name");
     }
 } else if ( YY_FIELD_LABEL == yy_state ) {
     CK_EQ(YY_FIELD_LABEL_EQUAL);
 } else if ( YY_FIELD_LABEL_EQUAL == yy_state ) {
     CK_TEXT(field[field_count-1]->label, YY_FIELD_COLON);
 } else if ( YY_FIELD_TYPE == yy_state ) {
     CK_EQ(YY_FIELD_TYPE_EQUAL);
 } else if ( YY_FIELD_TYPE_EQUAL == yy_state ) {
     if ( TOK_TEXT_T == tok ) {
         field[field_count-1]->type = TEXT_T;
         yy_state = YY_FIELD_COLON; val = 0;
     } else if ( TOK_INTEGER_T == tok ) {
         field[field_count-1]->type = INTEGER_T;
         yy_state = YY_FIELD_COLON; val = 0;
     } else if ( TOK_FLOAT_T == tok ) {
         field[field_count-1]->type = FLOAT_T;
         yy_state = YY_FIELD_COLON; val = 0;
     } else if ( TOK_BOOLEAN_T == tok ) {
         field[field_count-1]->type = BOOLEAN_T;
         yy_state = YY_FIELD_COLON; val = 0;
     } else {
         YY_FAIL("Expected field type value");
     }
 } else if ( YY_FIELD_WIDTH == yy_state ) {
     CK_EQ(YY_FIELD_WIDTH_EQUAL);
 } else if ( YY_FIELD_WIDTH_EQUAL == yy_state ) {
     if ( TOK_INTEGER == tok ) {
         field[field_count-1]->width = strtoul(s, NULL, 10);
         w = field[field_count-1]->width;
         if ( (ERANGE == errno) || (0 == w) || (MAX_FIELD_WIDTH < w) ) {
             YY_FAIL("Field width out of range");
         } else {
             tmp_str = realloc(field[field_count-1]->value, w+1);
             if ( NULL == tmp_str ) {
                 perror("realloc"); return (-1);
             } else {
                 field[field_count-1]->value = tmp_str;
                 tmp_str = realloc(field[field_count-1]->buf, w+1);
                 if ( NULL == tmp_str ) {
                     perror("realloc"); return (-1);
                 } else {
                     field[field_count-1]->buf = tmp_str;
                     tmp_str = realloc(field[field_count-1]->value, w+1);
                     if ( NULL == tmp_str ) {
                         perror("realloc"); return (-1);
                     } else {
                         field[field_count-1]->value = tmp_str;
                         field[field_count-1]->value[w] = '\0';
                         field[field_count-1]->buf[w] = '\0';
                         yy_state = YY_FIELD_COLON; val = 0;
                     }
                 }
             }
         }
     } else {
         YY_FAIL("Expected positive integer value");
     }
 } else if ( YY_FIELD_MAX_VALUES == yy_state ) {
     CK_EQ(YY_FIELD_MAX_VALUES_EQUAL);
 } else if ( YY_FIELD_MAX_VALUES_EQUAL == yy_state ) {
     if ( TOK_INTEGER == tok ) {
         field[field_count-1]->max_values = strtoul(s, NULL, 10);
         if ( ERANGE == errno ) {
             YY_FAIL("Field max_values out of range");
         } else {
             yy_state = YY_FIELD_COLON; val = 0;
         }
     } else {
         YY_FAIL("Expected integer value");
     }
 } else if ( YY_FIELD_MIN_VALUES == yy_state ) {
     CK_EQ(YY_FIELD_MIN_VALUES_EQUAL);
 } else if ( YY_FIELD_MIN_VALUES_EQUAL == yy_state ) {
     if ( TOK_INTEGER == tok ) {
         field[field_count-1]->min_values = strtoul(s, NULL, 10);
         if ( ERANGE == errno ) {
             YY_FAIL("Field min_values out of range");
         } else {
             yy_state = YY_FIELD_COLON; val = 0;
         }
     } else {
         YY_FAIL("Expected integer value");
     }
 } else if ( YY_FIELD_REQUIRE == yy_state ) {
     CK_EQ(YY_FIELD_REQUIRE_EQUAL);
 } else if ( YY_FIELD_REQUIRE_EQUAL == yy_state ) {
     if ( TOK_TRUE == tok ) {
         field[field_count-1]->require = 1;
         yy_state = YY_FIELD_COLON; val = 0;
     } else if ( (TOK_FALSE == tok) || (TOK_NULL == tok) ) {
         field[field_count-1]->require = 0;
         yy_state = YY_FIELD_COLON; val = 0;
     } else {
         YY_FAIL("Expected boolean value");
     }
 } else if ( YY_FIELD_FLOOR == yy_state ) {
     CK_EQ(YY_FIELD_FLOOR_EQUAL);
 } else if ( YY_FIELD_FLOOR_EQUAL == yy_state ) {
     if ( TOK_INTEGER == tok ) {
         field[field_count-1]->floor = strtoul(s, NULL, 10);
         if ( ERANGE == errno ) {
             YY_FAIL("Field floor value out of range");
         } else {
             yy_state = YY_FIELD_COLON; val = 0;
         }
     } else {
         YY_FAIL("Expected integer value");
     }
 } else if ( YY_FIELD_CEILING == yy_state ) {
     CK_EQ(YY_FIELD_CEILING_EQUAL);
 } else if ( YY_FIELD_CEILING_EQUAL == yy_state ) {
     if ( TOK_INTEGER == tok ) {
         field[field_count-1]->ceiling = strtoul(s, NULL, 10);
         if ( ERANGE == errno ) {
             YY_FAIL("Field ceiling value out of range");
         } else {
             yy_state = YY_FIELD_COLON; val = 0;
         }
     } else {
         YY_FAIL("Expected integer value");
     }
 } else if ( YY_FIELD_PATTERN == yy_state ) {
     CK_EQ(YY_FIELD_PATTERN_EQUAL);
 } else if ( YY_FIELD_PATTERN_EQUAL == yy_state ) {
     CK_TEXT(field[field_count-1]->pattern, YY_FIELD_COLON);
     regfree(field[field_count-1]->ptn_cmp);
     if ( regcomp(field[field_count-1]->ptn_cmp,
                  field[field_count-1]->pattern, rgx_flags) < 0 ) {
         YY_FAIL("regcomp() FAIL");
     }
 } else if ( YY_FIELD_ECHO == yy_state ) {
     CK_EQ(YY_FIELD_ECHO_EQUAL);
 } else if ( YY_FIELD_ECHO_EQUAL == yy_state ) {
     if ( (TOK_TRUE == tok) || (TOK_NULL == tok) ) {
         field[field_count-1]->echo = 1;
         yy_state = YY_FIELD_COLON; val = 0;
     } else if ( TOK_FALSE == tok ) {
         field[field_count-1]->echo = 0;
         yy_state = YY_FIELD_COLON; val = 0;
     } else {
         YY_FAIL("Expected boolean value");
     }
 } else if ( YY_FIELD_FIND == yy_state ) {
     CK_EQ(YY_FIELD_FIND_EQUAL);
 } else if ( YY_FIELD_FIND_EQUAL == yy_state ) {
     CK_TEXT(field[field_count-1]->find, YY_FIELD_COLON);
 } else if ( YY_FIELD_EDIT == yy_state ) {
     CK_EQ(YY_FIELD_EDIT_EQUAL);
 } else if ( YY_FIELD_EDIT_EQUAL == yy_state ) {
     if ( (TOK_TRUE == tok) || (TOK_NULL == tok) ) {
         field[field_count-1]->edit = 1;
         yy_state = YY_FIELD_COLON; val = 0;
     } else if ( TOK_FALSE == tok ) {
         field[field_count-1]->edit = 0;
         yy_state = YY_FIELD_COLON; val = 0;
     } else {
         YY_FAIL("Expected boolean value");
     }
 } else if ( YY_FIELD_CONSTANT == yy_state ) {
     CK_EQ(YY_FIELD_CONSTANT_EQUAL);
 } else if ( YY_FIELD_CONSTANT_EQUAL == yy_state ) {
     if ( TOK_TRUE == tok ) {
         field[field_count-1]->constant = 1;
         yy_state = YY_FIELD_COLON; val = 0;
     } else if ( (TOK_FALSE == tok) || (TOK_NULL == tok) ) {
         field[field_count-1]->constant = 0;
         yy_state = YY_FIELD_COLON; val = 0;
     } else {
         YY_FAIL("Expected boolean value");
     }
 } else if ( YY_FIELD_VERIFY == yy_state ) {
     CK_EQ(YY_FIELD_VERIFY_EQUAL);
 } else if ( YY_FIELD_VERIFY_EQUAL == yy_state ) {
     CK_TEXT(field[field_count-1]->verify, YY_FIELD_COLON);
 } else if ( YY_FIELD_LIST == yy_state ) {
     CK_EQ(YY_FIELD_LIST_EQUAL);
 } else if ( YY_FIELD_LIST_EQUAL == yy_state ) {
     CK_TEXT(field[field_count-1]->list, YY_FIELD_COLON);
 } else if ( YY_FIELD_VALUE == yy_state ) {
     CK_EQ(YY_FIELD_VALUE_EQUAL);
 } else if ( YY_FIELD_VALUE_EQUAL == yy_state ) {
     if ( TOK_STRING == tok ) {
         if ( (TEXT_T == field[field_count-1]->type) ||
              (NONE_T == field[field_count-1]->type) ) {
             if ( strlen(s) >= field[field_count-1]->width ) {
                 YY_FAIL("Field width exceeded");
             } else {
                 field[field_count-1]->type = TEXT_T;
                 strcpy(field[field_count-1]->value, s);
                 strcpy(field[field_count-1]->buf, s);
                 yy_state = YY_FIELD_COLON; val = 0;
             }
         } else {
             YY_FAIL("Field type/value mismatch");
         }
     } else if ( TOK_INTEGER == tok ) {
         if ( (INTEGER_T == field[field_count-1]->type) ||
              (NONE_T == field[field_count-1]->type) ) {
             if ( strlen(s) >= field[field_count-1]->width ) {
                 YY_FAIL("Field width exceeded");
             } else {
                 field[field_count-1]->type = INTEGER_T;
                 strcpy(field[field_count-1]->value, s);
                 strcpy(field[field_count-1]->buf, s);
                 yy_state = YY_FIELD_COLON; val = 0;
             }
         } else {
             YY_FAIL("Field type/value mismatch");
         }
     } else if ( TOK_FLOAT == tok ) {
         if ( (FLOAT_T == field[field_count-1]->type) ||
              (NONE_T == field[field_count-1]->type) ) {
             if ( strlen(s) >= field[field_count-1]->width ) {
                 YY_FAIL("Field width exceeded");
             } else {
                 field[field_count-1]->type = FLOAT_T;
                 strcpy(field[field_count-1]->value, s);
                 strcpy(field[field_count-1]->buf, s);
                 yy_state = YY_FIELD_COLON; val = 0;
             }
         } else {
             YY_FAIL("Field type/value mismatch");
         }
     } else if ( (TOK_TRUE == tok) ||
                 (TOK_FALSE == tok) ||
                 (TOK_NULL == tok) ) {
         if ( (BOOLEAN_T == field[field_count-1]->type) ||
              (NONE_T == field[field_count-1]->type) ) {
             if ( strlen(s) >= field[field_count-1]->width ) {
                 YY_FAIL("Field width exceeded");
             } else {
                 field[field_count-1]->type = BOOLEAN_T;
                 if ( (TOK_TRUE == tok) || (TOK_FALSE == tok) ) {
                     for ( i = 0; s[i]; i++ ) {
                         field[field_count-1]->value[i] = tolower(s[i]);
                         field[field_count-1]->buf[i] = tolower(s[i]);
                     }
                     field[field_count-1]->value[i] = '\0';
                     field[field_count-1]->buf[i] = '\0';
                 } else {
                     field[field_count-1]->value[0] = '\0';
                     field[field_count-1]->buf[0] = '\0';
                 }
                 yy_state = YY_FIELD_COLON; val = 0;
             }
         } else {
             YY_FAIL("Field type/value mismatch");
         }
     } else {
         YY_FAIL("Expected field value");
     }
 } else {
     yy_state = YY_ERROR; val = -1;
 }
 return (val);
}

