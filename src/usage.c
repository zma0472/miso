
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

#include "miso.h"
#include "log.h"

#ifndef LINT
static char rcsid[] =
"$Id: usage.c,v 1.10 2012/05/22 20:31:28 zma0472 Exp $";
#endif  /*  !defined LINT  */

void
usage(void)
{
 printf("\n  Usage:\n"
        "    miso [-h]\n"
        "    miso [-v]\n"
        "    miso [-c] [-d] [-F field_separator] [-V value_separator]\n"
        "                   [-i initial_values] [-f values_path]\n"
        "                   [-l log_path ] [-s shell] [ FILE ]"
        "\n\n");
 return;
}

