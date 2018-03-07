#
# Angharad House Automation Server
#
# Makefile used to build the software
#
# Copyright 2018 Nicolas Mora <mail@babelouest.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU GENERAL PUBLIC LICENSE
# License as published by the Free Software Foundation;
# version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU GENERAL PUBLIC LICENSE for more details.
#
# You should have received a copy of the GNU General Public
# License along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

CC=gcc
CFLAGS=-Wall -D_REENTRANT -DDEBUG -g -O0
LIBS=-lc -lulfius -lorcania -ljansson -lyder -lcheck -lpthread -lm -lrt -lsubunit

PARAM_USER=user1
PARAM_PASSWORD=MyUser1Password!
PARAM_SCOPE=angharad
PARAM_GLEWLWYD_URL=http://localhost:4593/api
PARAM_ANGHARAD_URL=http://localhost:2473/

all: test

clean:
	rm -f *.o *.err *.log angharad_submodule angharad_script angharad_scheduler angharad_trigger angharad_scheduler_trigger_exec angharad_profile benoic_device benoic_device_element benoic_monitor carleon_service carleon_liquidsoap carleon_motion carleon_mpd gareth_alert_smtp gareth_alert_http gareth_filter gareth_message gareth_trigger valgrind.txt

unit-tests.o: unit-tests.c unit-tests.h
	$(CC) $(CFLAGS) -c unit-tests.c

angharad_submodule: angharad_submodule.c unit-tests.o
	$(CC) $(CFLAGS) angharad_submodule.c unit-tests.o -o angharad_submodule $(LIBS)

test_angharad_submodule: angharad_submodule
	./run_test.sh ./angharad_submodule $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

angharad_script: angharad_script.c unit-tests.o
	$(CC) $(CFLAGS) angharad_script.c unit-tests.o -o angharad_script $(LIBS)

test_angharad_script: angharad_script
	./run_test.sh ./angharad_script $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

angharad_scheduler: angharad_scheduler.c unit-tests.o
	$(CC) $(CFLAGS) angharad_scheduler.c unit-tests.o -o angharad_scheduler $(LIBS)

test_angharad_scheduler: angharad_scheduler
	./run_test.sh ./angharad_scheduler $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

angharad_trigger: angharad_trigger.c unit-tests.o
	$(CC) $(CFLAGS) angharad_trigger.c unit-tests.o -o angharad_trigger $(LIBS)

test_angharad_trigger: angharad_trigger
	./run_test.sh ./angharad_trigger $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

angharad_scheduler_trigger_exec: angharad_scheduler_trigger_exec.c unit-tests.o
	$(CC) $(CFLAGS) angharad_scheduler_trigger_exec.c unit-tests.o -o angharad_scheduler_trigger_exec $(LIBS)

test_angharad_scheduler_trigger_exec: angharad_scheduler_trigger_exec
	./run_test.sh ./angharad_scheduler_trigger_exec $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

angharad_profile: angharad_profile.c unit-tests.o
	$(CC) $(CFLAGS) angharad_profile.c unit-tests.o -o angharad_profile $(LIBS)

test_angharad_profile: angharad_profile
	./run_test.sh ./angharad_profile $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

benoic_device: benoic_device.c unit-tests.o
	$(CC) $(CFLAGS) benoic_device.c unit-tests.o -o benoic_device $(LIBS)

test_benoic_device: benoic_device
	./run_test.sh ./benoic_device $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

benoic_device_element: benoic_device_element.c unit-tests.o
	$(CC) $(CFLAGS) benoic_device_element.c unit-tests.o -o benoic_device_element $(LIBS)

test_benoic_device_element: benoic_device_element
	./run_test.sh ./benoic_device_element $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

benoic_monitor: benoic_monitor.c unit-tests.o
	$(CC) $(CFLAGS) benoic_monitor.c unit-tests.o -o benoic_monitor $(LIBS)

test_benoic_monitor: benoic_monitor
	./run_test.sh ./benoic_monitor $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

carleon_service: carleon_service.c unit-tests.o
	$(CC) $(CFLAGS) carleon_service.c unit-tests.o -o carleon_service $(LIBS)

test_carleon_service: carleon_service
	./run_test.sh ./carleon_service $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

carleon_liquidsoap: carleon_liquidsoap.c unit-tests.o
	$(CC) $(CFLAGS) carleon_liquidsoap.c unit-tests.o -o carleon_liquidsoap $(LIBS)

test_carleon_liquidsoap: carleon_liquidsoap
	./run_test.sh ./carleon_liquidsoap $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

carleon_motion: carleon_motion.c unit-tests.o
	$(CC) $(CFLAGS) carleon_motion.c unit-tests.o -o carleon_motion $(LIBS)

test_carleon_motion: carleon_motion
	./run_test.sh ./carleon_motion $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

carleon_mpd: carleon_mpd.c unit-tests.o
	$(CC) $(CFLAGS) carleon_mpd.c unit-tests.o -o carleon_mpd $(LIBS)

test_carleon_mpd: carleon_mpd
	./run_test.sh ./carleon_mpd $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

gareth_alert_smtp: gareth_alert_smtp.c unit-tests.o
	$(CC) $(CFLAGS) gareth_alert_smtp.c unit-tests.o -o gareth_alert_smtp $(LIBS)

test_gareth_alert_smtp: gareth_alert_smtp
	./run_test.sh ./gareth_alert_smtp $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

gareth_alert_http: gareth_alert_http.c unit-tests.o
	$(CC) $(CFLAGS) gareth_alert_http.c unit-tests.o -o gareth_alert_http $(LIBS)

test_gareth_alert_http: gareth_alert_http
	./run_test.sh ./gareth_alert_http $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

gareth_filter: gareth_filter.c unit-tests.o
	$(CC) $(CFLAGS) gareth_filter.c unit-tests.o -o gareth_filter $(LIBS)

test_gareth_filter: gareth_filter
	./run_test.sh ./gareth_filter $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

gareth_message: gareth_message.c unit-tests.o
	$(CC) $(CFLAGS) gareth_message.c unit-tests.o -o gareth_message $(LIBS)

test_gareth_message: gareth_message
	./run_test.sh ./gareth_message $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

gareth_trigger: gareth_trigger.c unit-tests.o
	$(CC) $(CFLAGS) gareth_trigger.c unit-tests.o -o gareth_trigger $(LIBS)

test_gareth_trigger: gareth_trigger
	./run_test.sh ./gareth_trigger $(PARAM_USER) $(PARAM_PASSWORD) $(PARAM_SCOPE) $(PARAM_GLEWLWYD_URL) $(PARAM_ANGHARAD_URL)

test: test_angharad_submodule test_angharad_script test_angharad_scheduler test_angharad_trigger test_angharad_scheduler_trigger_exec test_angharad_profile test_benoic_device test_benoic_device_element test_benoic_monitor test_carleon_service test_carleon_liquidsoap test_carleon_motion test_carleon_mpd test_gareth_alert_smtp test_gareth_alert_http test_gareth_filter test_gareth_message test_gareth_trigger
