#
# Carleon House Automation side services
#
# Command house automation side services via an HTTP REST interface
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
LIBS=-L$(PREFIX)/lib -lc -ldl -ljansson -lulfius -lhoel -lyder -lorcania
MODULES_LOCATION=service-modules

%.o: %.c carleon.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $<

carleon-standalone: carleon.o service.o carleon-standalone.o
	$(CC) -o carleon-standalone carleon-standalone.o carleon.o service.o $(LIBS) -lconfig

modules:
	cd $(MODULES_LOCATION) && $(MAKE) debug

all: release

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: carleon-standalone modules

release-standalone: ADDITIONALFLAGS=-O3

release-standalone: carleon-standalone

release: ADDITIONALFLAGS=-O3

release: carleon.o device.o modules

test: debug
	./carleon-standalone

clean:
	rm -f *.o carleon-standalone
	cd $(MODULES_LOCATION) && $(MAKE) clean

install-modules:
	cd $(MODULES_LOCATION) && $(MAKE) install

install-standalone: carleon-standalone install-modules
	cp carleon-standalone /usr/local/bin
