#
# Angharad system
#
# Manage all subsystems and handles the house automation server:
# - Benoic
# - Gareth
# - Carleon
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
CFLAGS=-c -Wall -D_REENTRANT -I$(PREFIX)/include $(ADDITIONALFLAGS)
LIBS=-L$(PREFIX)/lib -lc -lpthread -ldl -ljansson -lulfius -lhoel -lyder -lorcania

# subprojects locations
GARETH_LOCATION=../gareth
BENOIC_LOCATION=../benoic
CARLEON_LOCATION=../carleon

GARETH_LIBS=$(GARETH_LOCATION)/gareth.o $(GARETH_LOCATION)/alert.o $(GARETH_LOCATION)/filter.o $(GARETH_LOCATION)/message.o
BENOIC_LIBS=$(BENOIC_LOCATION)/benoic.o $(BENOIC_LOCATION)/device.o $(BENOIC_LOCATION)/device-element.o
CARLEON_LIBS=$(CARLEON_LOCATION)/carleon.o $(CARLEON_LOCATION)/service.o $(CARLEON_LOCATION)/profile.o

angharad: gareth.o benoic.o carleon.o angharad.h angharad.o event.o script.o webservice.o
	$(CC) -o angharad angharad.o event.o script.o webservice.o $(GARETH_LIBS) $(BENOIC_LIBS) $(CARLEON_LIBS) $(LIBS) -lconfig

angharad.o: angharad.c angharad.h
	$(CC) $(CFLAGS) angharad.c

event.o: event.c angharad.h
	$(CC) $(CFLAGS) event.c

script.o: script.c angharad.h
	$(CC) $(CFLAGS) script.c

webservice.o: webservice.c angharad.h
	$(CC) $(CFLAGS) webservice.c

gareth.o:
	cd $(GARETH_LOCATION) && $(MAKE) $(DEBUGFLAG)

benoic.o:
	cd $(BENOIC_LOCATION) && $(MAKE) $(DEBUGFLAG)

carleon.o:
	cd $(CARLEON_LOCATION) && $(MAKE) $(DEBUGFLAG)

all: release

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: DEBUGFLAG=debug

debug: angharad unit-tests

release: ADDITIONALFLAGS=-O3

release: angharad unit-tests

clean:
	rm -f *.o angharad unit-tests
	cd $(GARETH_LOCATION) && $(MAKE) clean
	cd $(BENOIC_LOCATION) && $(MAKE) clean
	cd $(CARLEON_LOCATION) && $(MAKE) clean

unit-tests: unit-tests.c
	$(CC) -o unit-tests unit-tests.c -lc -lulfius -lorcania -ljansson -L$(PREFIX)/lib
