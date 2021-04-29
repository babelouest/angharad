#
# sagremor Makefile
# build Sagremor dist
#
# Copyright 2018 Nicolas Mora <mail@babelouest.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the MIT License
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU GENERAL PUBLIC LICENSE for more details.
#

build:
	gulp build
	mkdir -p dist/app/login/
	cp src/app/login/login.html dist/app/login/

clean:
	rm -rf node_modules/ bower_components/ dist/

install:
	mkdir -p /usr/local/share/angharad/webapp
	cp -R dist/* /usr/local/share/angharad/webapp/
