
# 
#  Copyright (C) 2011,2012,2019 Matthew Alton
# 
#  This file is part of Miso.
# 
#  Miso is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  Miso is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with Miso.  If not, see <http://www.gnu.org/licenses/>.
#

CC       = gcc
#CFLAGS   = -g -ansi -Wall
CFLAGS   = -g
#CPPFLAGS = -DDMALLOC
#INCLUDES = -I/opt/include

LIBS     = -lcurses
#LIBS     = -lcurses -lefence
LIBINCS  = -L/opt/lib
#LIBINCS  =
#LDFLAGS  =

OBJS     = args.o error.o field.o find.o form.o image.o key.o lex.o lib.o \
           list.o main.o menu.o parse.o shell.o text.o usage.o value.o    \
           version.o

HEADERS  = lex.h log.h miso.h msg.h parse.h

all: miso

miso: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(LIBINCS) $(LDFLAGS)

$(OBJS): $(HEADERS)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $<

tags:
	ctags *.[ch]

clean:
	rm -f *.o miso tags core core.*

