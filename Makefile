#
# angharad root Makefile
# build Angharad program and Sagremor dist files
#
# Copyright 2018 Nicolas Mora <mail@babelouest.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU GPL v3 license.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU GENERAL PUBLIC LICENSE for more details.
#

sagremor-build:
	cd webapp-src && $(MAKE) build
	rm -rf webapp/*
	cp -R webapp-src/dist webapp
	rm -f webapp/config.json

angharad-build:
	cd src && make

angharad-install:
	cd src && make install
