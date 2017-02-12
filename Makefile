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
CFLAGS=-c -Wall -D_REENTRANT -DLDAP_DEPRECATED -I$(PREFIX)/include $(ADDITIONALFLAGS)
LIBS=-L$(PREFIX)/lib -lc -lm -lpthread -ldl -ljansson -lulfius -lhoel -lyder -lorcania -ljwt

# subprojects locations
GARETH_LOCATION=gareth
BENOIC_LOCATION=benoic
CARLEON_LOCATION=carleon

GARETH_LIBS=$(GARETH_LOCATION)/gareth.o $(GARETH_LOCATION)/alert.o $(GARETH_LOCATION)/filter.o $(GARETH_LOCATION)/message.o
BENOIC_LIBS=$(BENOIC_LOCATION)/benoic.o $(BENOIC_LOCATION)/device.o $(BENOIC_LOCATION)/device-element.o
CARLEON_LIBS=$(CARLEON_LOCATION)/carleon.o $(CARLEON_LOCATION)/service.o
ANGHARAD_LIBS=angharad.o condition.o scheduler.o trigger.o script.o profile.o webservice.o glewlwyd_resource.o

all: release
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE)
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE)

angharad: $(ANGHARAD_LIBS) $(GARETH_LIBS) $(BENOIC_LIBS) $(CARLEON_LIBS)
	$(CC) -o angharad $(ANGHARAD_LIBS) $(GARETH_LIBS) $(BENOIC_LIBS) $(CARLEON_LIBS) $(LIBS) -lconfig

angharad.o: angharad.c angharad.h
	$(CC) $(CFLAGS) angharad.c

condition.o: condition.c angharad.h
	$(CC) $(CFLAGS) condition.c

scheduler.o: scheduler.c angharad.h
	$(CC) $(CFLAGS) scheduler.c

trigger.o: trigger.c angharad.h
	$(CC) $(CFLAGS) trigger.c

script.o: script.c angharad.h
	$(CC) $(CFLAGS) script.c

profile.o: profile.c angharad.h
	$(CC) $(CFLAGS) profile.c

webservice.o: webservice.c angharad.h
	$(CC) $(CFLAGS) webservice.c

glewlwyd_resource.o: glewlwyd_resource.c angharad.h
	$(CC) $(CFLAGS) glewlwyd_resource.c

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: DEBUGFLAG=debug

debug: angharad unit-tests
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE) debug
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE) debug

release: ADDITIONALFLAGS=-O3

release: angharad
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE)
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE)

clean:
	rm -f *.o angharad unit-tests valgrind.txt
	cd $(GARETH_LOCATION) && $(MAKE) clean
	cd $(BENOIC_LOCATION) && $(MAKE) clean
	cd $(CARLEON_LOCATION) && $(MAKE) clean

unit-tests: unit-tests.c
	$(CC) -o unit-tests unit-tests.c -lc -lulfius -lorcania -ljansson -L$(PREFIX)/lib

memcheck: debug
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE) debug
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE) debug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./angharad --config-file=angharad.conf 2>valgrind.txt

install-angharad:
	cp -f angharad $(PREFIX)/bin

install-modules:
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE) install
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE) install

install: install-angharad install-modules
