#
# Hutch - Password and private data locker
#
# Makefile used to build and install the front-end webapp
#
# Copyright 2017-2021 Nicolas Mora <mail@babelouest.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU GENERAL PUBLIC LICENSE
# License as published by the Free Software Foundation;
# version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU GENERAL PUBLIC LICENSE for more details.
#
# You should have received a copy of the GNU General Public
# License along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

WEBAPP_DEST=../webapp
JSC=npm

all: webapp

install-dev:
	$(JSC) install

dev:
	$(JSC) run dev

webapp: build-webapp install-webapp

build-webapp:
	rm -f dist/bundle.js*
	$(JSC) run build

install-webapp:
	find $(WEBAPP_DEST) ! -name config.json ! -name webapp ! -name .gitignore -delete
	cp -R dist/css/ dist/favicon.ico dist/fonts/ dist/bundle.js dist/bundle.js.LICENSE.txt dist/img/ dist/index.html dist/js/ dist/locales/ dist/config.json.sample $(WEBAPP_DEST)
