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

webapp-build:
	cd webapp-src && $(MAKE) build
	find webapp ! -name config.json ! -name webapp -delete
	cp -R webapp-src/dist/* webapp/

webapp-install:
	cd webapp-src && $(MAKE) install
	
webapp-clean:
	cd webapp-src && $(MAKE) clean

angharad-build:
	cd src && make

angharad-install:
	cd src && make install

angharad-clean:
	cd src && make clean

install: angharad-install webapp-install

clean: angharad-clean webapp-clean
