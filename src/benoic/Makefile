#
# Benoic House Automation service
#
# Command house automation devices via an HTTP REST interface
#
# Makefile used to build the software
#
# Copyright 2016 Nicolas Mora <mail@babelouest.org>
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
CFLAGS=-c -Wall -Werror -Wextra -Wconversion -Wpedantic -D_REENTRANT -I$(PREFIX)/include $(ADDITIONALFLAGS)
LIBS=-L$(PREFIX)/lib -lc -ldl -lpthread -ljansson -lulfius -lhoel -lyder -lorcania
MODULES_LOCATION=device-modules

benoic-standalone: benoic.o device.o device-element.o benoic-standalone.o
	$(CC) -o benoic-standalone benoic-standalone.o benoic.o device.o device-element.o $(LIBS) -lconfig

benoic-standalone.o: benoic-standalone.c benoic.h
	$(CC) $(CFLAGS) benoic-standalone.c

benoic.o: benoic.c benoic.h
	$(CC) $(CFLAGS) benoic.c

device.o: device.c benoic.h
	$(CC) $(CFLAGS) device.c

device-element.o: device-element.c benoic.h
	$(CC) $(CFLAGS) device-element.c

modules:
	cd $(MODULES_LOCATION) && $(MAKE) debug

all: release

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: benoic-standalone modules

release-standalone: ADDITIONALFLAGS=-O3

release-standalone: benoic-standalone modules

release: ADDITIONALFLAGS=-O3

release: benoic.o device.o

test: debug
	./benoic-standalone

clean:
	rm -f *.o benoic-standalone valgrind.txt
	cd $(MODULES_LOCATION) && $(MAKE) clean

install_modules: modules
	cd $(MODULES_LOCATION) && $(MAKE) install
