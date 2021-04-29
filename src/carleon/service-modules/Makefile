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

PREFIX=/usr/local
MODULES_TARGET=$(PREFIX)/lib/angharad/carleon
CC=gcc
CFLAGS=-c -fPIC -Wall -Werror -Wextra -D_REENTRANT -shared -nostartfiles -I$(PREFIX)/include $(ADDITIONALFLAGS)
LIBS=-L$(PREFIX)/lib -lc -ljansson -lyder -lorcania -lulfius
VERSION=0.1.0

all: release

libservicemock.so: service-mock.o
	$(CC) -shared -Wl,-soname,libservicemock.so -o libservicemock.so service-mock.o $(LIBS)

service-mock.o: service-mock.c
	$(CC) $(CFLAGS) service-mock.c

libservicemotion.so: service-motion.o
	$(CC) -shared -Wl,-soname,libservicemotion.so -o libservicemotion.so service-motion.o $(LIBS) -lpthread

service-motion.o: service-motion.c
	$(CC) $(CFLAGS) service-motion.c

libservicempd.so: service-mpd.o
	$(CC) -shared -Wl,-soname,libservicempd.so -o libservicempd.so service-mpd.o $(LIBS) -lmpdclient -L/usr/local/lib

service-mpd.o: service-mpd.c
	$(CC) $(CFLAGS) service-mpd.c

libserviceliquidsoap.so: service-liquidsoap.o
	$(CC) -shared -Wl,-soname,libserviceliquidsoap.so -o libserviceliquidsoap.so service-liquidsoap.o $(LIBS) -L/usr/local/lib

service-liquidsoap.o: service-liquidsoap.c
	$(CC) $(CFLAGS) service-liquidsoap.c

clean:
	rm -f *.o *.so valgrind.txt unit-tests test-motion test-mpd test-liquidsoap valgrind.txt

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: libservicemock.so libservicemotion.so libservicempd.so libserviceliquidsoap.so

release: ADDITIONALFLAGS=-O3

release: libservicemotion.so libservicempd.so libserviceliquidsoap.so

install:
	mkdir -p $(MODULES_TARGET)
	cp -f *.so $(MODULES_TARGET)

test-motion: ADDITIONALFLAGS=-DDEBUG -g -O0

test-motion: test-service.c service-motion.o
	$(CC) $(CFLAGS) test-service.c
	$(CC) test-service.o service-motion.o -o test-motion -lpthread -ljansson -lyder -lorcania -lulfius -lhoel
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./test-motion 2>valgrind.txt

test-mpd: ADDITIONALFLAGS=-DDEBUG -g -O0

test-mpd: test-service.c service-mpd.o
	$(CC) $(CFLAGS) test-service.c
	$(CC) test-service.o service-mpd.o -o test-mpd -lpthread -ljansson -lyder -lorcania -lulfius -lhoel -lmpdclient
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./test-mpd 2>valgrind.txt

test-liquidsoap: ADDITIONALFLAGS=-DDEBUG -g -O0

test-liquidsoap: test-service.c service-liquidsoap.o
	$(CC) $(CFLAGS) test-service.c
	$(CC) test-service.o service-liquidsoap.o -o test-liquidsoap -lpthread -ljansson -lyder -lorcania -lulfius -lhoel
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./test-liquidsoap 2>valgrind.txt

unit-tests: unit-tests.c
	$(CC) -o unit-tests unit-tests.c -lc -lulfius -lorcania -ljansson -L$(PREFIX)/lib
