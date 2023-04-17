#
# Gareth messenger service
#
# Get messages from a REST Webservice
# Send messages or digested data when previously parametered filters are triggered
# Send protocols available: http, smtp, call an external http
#
# Makefile used to build the software
#
# Copyright 2015-2016 Nicolas Mora <mail@babelouest.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU GENERAL PUBLIC LICENSE
# License as published by the Free Software Foundation;
# version 3 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU GENERAL PUBLIC LICENSE for more details.
#
# You should have received a copy of the GNU General Public
# License along with this library.  If not, see <http://www.gnu.org/licenses/>.
#

# Environment variables
PREFIX=/usr/local
CC=gcc
CFLAGS=-c -Wall -Werror -Wextra -Wconversion -D_REENTRANT -I$(PREFIX)/include $(ADDITIONALFLAGS)
LIBS=-L$(PREFIX)/lib -lc -lpthread -lconfig -ljansson -lulfius -lhoel -lyder -lorcania

all: release

gareth-standalone: gareth.o alert.o filter.o message.o gareth.o gareth-standalone.o
	$(CC) -o gareth-standalone gareth-standalone.o gareth.o alert.o filter.o message.o $(LIBS)

gareth-standalone.o: gareth-standalone.c gareth.h
	$(CC) $(CFLAGS) gareth-standalone.c

gareth.o: gareth.c gareth.h
	$(CC) $(CFLAGS) gareth.c

alert.o: alert.c gareth.h
	$(CC) $(CFLAGS) alert.c

filter.o: filter.c gareth.h
	$(CC) $(CFLAGS) filter.c

message.o: message.c gareth.h
	$(CC) $(CFLAGS) message.c

clean:
	rm -f *.o gareth-standalone

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: gareth-standalone

release-standalone: ADDITIONALFLAGS=-O3

release-standalone: gareth-standalone

release: ADDITIONALFLAGS=-O3

release: gareth.o alert.o filter.o message.o

test: debug
	./gareth
