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
CFLAGS=-c -Wall -Werror -Wextra -Wconversion -D_REENTRANT -I$(PREFIX)/include $(ADDITIONALFLAGS)
LIBS=-L$(PREFIX)/lib -lc -lm -lpthread -ldl -ljansson -lulfius -lhoel -lyder -lorcania -liddawc -lmicrohttpd -lz

# subprojects locations
GARETH_LOCATION=gareth
BENOIC_LOCATION=benoic
CARLEON_LOCATION=carleon

BENOIC_OBJECTS=$(BENOIC_LOCATION)/benoic.o $(BENOIC_LOCATION)/device.o $(BENOIC_LOCATION)/device-element.o
CARLEON_OBJECTS=$(CARLEON_LOCATION)/carleon.o $(CARLEON_LOCATION)/service.o
ANGHARAD_OBJECTS=angharad.o scheduler.o script.o profile.o webservice.o iddawc_resource.o static_compressed_inmemory_website_callback.o http_compression_callback.o

%.o: %.c angharad.h glewlwyd-common.h static_compressed_inmemory_website_callback.h http_compression_callback.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $<

all: release
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE)
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE)

angharad: $(ANGHARAD_OBJECTS) $(BENOIC_OBJECTS) $(CARLEON_OBJECTS)
	$(CC) -o angharad $(ANGHARAD_OBJECTS) $(BENOIC_OBJECTS) $(CARLEON_OBJECTS) $(LIBS) -lconfig

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: DEBUGFLAG=debug

debug: angharad
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE) debug
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE) debug

release: ADDITIONALFLAGS=-O3

release: angharad
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE)
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE)

clean:
	rm -f *.o angharad valgrind.txt
	cd $(GARETH_LOCATION) && $(MAKE) clean
	cd $(BENOIC_LOCATION) && $(MAKE) clean
	cd $(CARLEON_LOCATION) && $(MAKE) clean

test-debug: debug
	./angharad --config-file=../angharad.conf

memcheck: debug
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE) debug
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE) debug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./angharad --config-file=../angharad.conf 2>valgrind.txt

install-angharad:
	cp -f angharad $(PREFIX)/bin

install-modules:
	cd $(BENOIC_LOCATION)/device-modules && $(MAKE) install
	cd $(CARLEON_LOCATION)/service-modules && $(MAKE) install

install: install-angharad install-modules
