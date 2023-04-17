#
# Benoic device modules
#
# Makefile used to build the software
#
# Copyright 2016 Nicolas Mora <mail@babelouest.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation;
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU GENERAL PUBLIC LICENSE for more details.
#
# You should have received a copy of the GNU General Public
# License along with this library.	If not, see <http://www.gnu.org/licenses/>.
#

PREFIX=/usr
MODULES_TARGET=$(PREFIX)/lib/angharad/benoic
LIBZWPATH=/usr/lib
CC=gcc
CPP=g++
CFLAGS=-c -fPIC -Wall -Wconversion -Wconversion -Wno-unknown-pragmas -D_REENTRANT -I$(PREFIX)/include -Wno-pragmas $(ADDITIONALFLAGS)
LIBS=-L$(PREFIX)/lib -lc -ljansson -lyder -lorcania -lulfius -ldl
LDFLAGS += -Wl,-R '-Wl,-R$$ORIGIN' -lopenzwave

#oz_include=$(shell pkg-config --cflags-only-I libopenzwave)
oz_include=-I/usr/include/openzwave/

OZINCLUDES:= $(oz_include) $(oz_include)command_classes $(oz_include)platform $(oz_include)value_classes

all: release

device-mock.o: ../benoic.h device-mock.c
	$(CC) $(CFLAGS) -Werror -Wextra $(FLAGS_YDER) device-mock.c

libdevmock.so: device-mock.o
	$(CC) -shared -Wl,-soname,libdevmock.so -o libdevmock.so device-mock.o $(LIBS)

device-zwave.o: device-zwave.cpp
	$(CPP) $(CFLAGS) -Wno-write-strings $(FLAGS_YDER) $(OZINCLUDES) device-zwave.cpp

libdevzwave.so: device-zwave.o
	$(CC) -shared -Wl,-soname,libdevzwave.so -o libdevzwave.so device-zwave.o $(LIBS) $(LDFLAGS) $(shell pkg-config --libs libopenzwave)

device-taulas.o: device-taulas.c
	$(CC) $(CFLAGS) -Werror -Wextra $(FLAGS_YDER) device-taulas.c

libdevtaulas.so: device-taulas.o
	$(CC) -shared -Wl,-soname,libdevtaulas.so -o libdevtaulas.so device-taulas.o $(LIBS)

device-esp8266.o: device-esp8266.c
	$(CC) $(CFLAGS) -Werror -Wextra $(FLAGS_YDER) device-esp8266.c

libdevesp8266.so: device-esp8266.o
	$(CC) -shared -Wl,-soname,libdevesp8266.so -o libdevesp8266.so device-esp8266.o $(LIBS)

clean:
	rm -f *.o *.so

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: libdevzwave.so libdevtaulas.so libdevesp8266.so libdevmock.so

release: ADDITIONALFLAGS=-O3

release: libdevzwave.so libdevtaulas.so libdevesp8266.so

install:
	mkdir -p $(MODULES_TARGET)
	cp -f *.so $(MODULES_TARGET)
