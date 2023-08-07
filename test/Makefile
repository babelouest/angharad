#
# Angharad House Automation Server
#
# Makefile used to build the tests
#
# Public domain, no copyright. Use at your own risk.
#

CC=gcc
CFLAGS=-Wall -D_REENTRANT -DDEBUG -g -O0
LDFLAGS=-lc -lulfius -lorcania -lrhonabwy -ljansson -lyder -lcheck -lpthread -lm -lrt -lsubunit
TARGET=angharad_submodule angharad_script angharad_scheduler angharad_profile benoic_device benoic_device_element benoic_monitor carleon_service carleon_liquidsoap carleon_motion carleon_mpd
VERBOSE=0
MEMCHECK=0

PARAM_JWKS_PATH=../private-test.jwks

all: build test

clean:
	rm -f *.o *.log valgrind-*.txt $(TARGET)

unit-tests.o: unit-tests.c unit-tests.h
	$(CC) $(CFLAGS) -c unit-tests.c

%: %.c unit-tests.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_%: %
	@if [ "$(VERBOSE)" = "0" ] && [ "$(MEMCHECK)" = "0" ]; then \
		LD_LIBRARY_PATH=. ./run_test.sh ./$^ $(PARAM_JWKS_PATH); \
	elif [ "$(MEMCHECK)" = "0" ]; then \
		LD_LIBRARY_PATH=. ./$^ $(PARAM_JWKS_PATH); \
	else \
		CK_FORK=no LD_LIBRARY_PATH=. $(VALGRIND_COMMAND) ./$^ $(PARAM_JWKS_PATH) 2>valgrind-$@.txt; \
	fi

build: $(TARGET)

test: $(TARGET) test_angharad_submodule test_angharad_script test_angharad_scheduler test_angharad_profile test_benoic_device test_benoic_device_element test_benoic_monitor test_carleon_service test_carleon_liquidsoap test_carleon_motion test_carleon_mpd
