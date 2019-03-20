
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

#if !defined(_PARSE_H)

#define _PARSE_H 1

static enum {
    YY_START                   =   0,
    YY_MENU                    =   1,
    YY_MENU_COLON              =   2,
    YY_MENU_TITLE              =   3,
    YY_MENU_TITLE_EQUAL        =   4,
    YY_ITEM                    =   5,
    YY_ITEM_COLON              =   6,
    YY_ITEM_LABEL              =   7,
    YY_ITEM_LABEL_EQUAL        =   8,
    YY_ITEM_OUTPUT             =   9,
    YY_ITEM_OUTPUT_EQUAL       =  10,
    YY_FORM                    =  11,
    YY_FORM_COLON              =  12,
    YY_FORM_TITLE              =  13,
    YY_FORM_TITLE_EQUAL        =  14,
    YY_FIELD                   =  15,
    YY_FIELD_COLON             =  16,
    YY_FIELD_LABEL             =  17,
    YY_FIELD_LABEL_EQUAL       =  18,
    YY_FIELD_TYPE              =  19,
    YY_FIELD_TYPE_EQUAL        =  20,
    YY_FIELD_WIDTH             =  21,
    YY_FIELD_WIDTH_EQUAL       =  22,
    YY_FIELD_MAX_VALUES        =  23,
    YY_FIELD_MAX_VALUES_EQUAL  =  24,
    YY_FIELD_MIN_VALUES        =  25,
    YY_FIELD_MIN_VALUES_EQUAL  =  26,
    YY_FIELD_REQUIRE           =  27,
    YY_FIELD_REQUIRE_EQUAL     =  28,
    YY_FIELD_FLOOR             =  29,
    YY_FIELD_FLOOR_EQUAL       =  30,
    YY_FIELD_CEILING           =  31,
    YY_FIELD_CEILING_EQUAL     =  32,
    YY_FIELD_PATTERN           =  33,
    YY_FIELD_PATTERN_EQUAL     =  34,
    YY_FIELD_ECHO              =  35,
    YY_FIELD_ECHO_EQUAL        =  36,
    YY_FIELD_FIND              =  37,
    YY_FIELD_FIND_EQUAL        =  38,
    YY_FIELD_EDIT              =  39,
    YY_FIELD_EDIT_EQUAL        =  40,
    YY_FIELD_CONSTANT          =  41,
    YY_FIELD_CONSTANT_EQUAL    =  42,
    YY_FIELD_VERIFY            =  43,
    YY_FIELD_VERIFY_EQUAL      =  44,
    YY_FIELD_LIST              =  45,
    YY_FIELD_LIST_EQUAL        =  46,
    YY_FIELD_VALUE             =  47,
    YY_FIELD_VALUE_EQUAL       =  48,
    YY_ERROR                   = 255
} yy_state = YY_START;

#define YY_FAIL(a)                                                            \
do {                                                                          \
    (void)fprintf(stderr, "%s(%d): Syntax error: " a " @ %s(%d.%d)\n",        \
                                      __FILE__, __LINE__, schema, line, col); \
    return (-1);                                                              \
} while (0)

#define CK_EQ(a)                 \
do {                             \
    if ( TOK_EQUALS == tok ) {   \
        yy_state = a ; val = 0;  \
    } else {                     \
        YY_FAIL("Expected '='"); \
    }                            \
} while (0)

#define NEW_FIELD            \
do {                         \
    if ( new_field() < 0 ) { \
        return (-1);         \
    } else {                 \
        val = 0;             \
    }                        \
} while (0)

#define NEW_ITEM                                               \
do {                                                           \
    item_t **tmp_item = NULL;                                  \
    tmp_item = realloc(item, (item_count+1)*sizeof(item_t *)); \
    if ( NULL == tmp_item ) {                                  \
        perror("realloc"); return (-1);                        \
    } else {                                                   \
        item = tmp_item;                                       \
        item[item_count] = calloc(1, sizeof(item_t));          \
        if ( NULL == item[item_count] ) {                      \
            perror("calloc"); return (-1);                     \
        } else {                                               \
            item_count++; yy_state = YY_ITEM; val = 0;         \
        }                                                      \
    }                                                          \
} while (0)

#define CK_TEXT(a, b)                       \
do {                                        \
     if ( TOK_STRING == tok ) {             \
         free(a);                           \
         if ( NULL == (a = strdup(s)) ) {   \
             perror("strdup"); return (-1); \
         } else {                           \
             yy_state = b; val = 0;         \
         }                                  \
     } else {                               \
         YY_FAIL("Expected string value");  \
     }                                      \
} while (0)

#endif  /*  !defined(_PARSE_H)  */

