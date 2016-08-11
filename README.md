# Angharad

House automation system API server

Central program and orchestrator of the following submodules:

- [Benoic](https://github.com/babelouest/benoic): Command house automation devices as lights, heaters, sensors, switches, currently ZWave devices are supported
- [Carleon](https://github.com/babelouest/carleon): Command house automation side services (music, camera, etc)
- [Gareth](https://github.com/babelouest/gareth): Logs and messenger service

Angharad integrates all those submodules functionnalities and manages scripts, events, user profiles, and a small static file server.

Benoic and Carleon implement a dynamic lbrary loading interface to add/remove/update modules without having to recompile all the system every time an update or an improvement is made.

Each module are compiled independently and have its own dependencies.

# Prerequisites

All Angharad system, its submodules and its custom libraries are written in C language, for better preformance and lightweight resources consumption. GCC and Makefile were used for the development.

## External dependencies

### TL;DR

On Debian-based systems (Debian, Raspbian, Ubuntu), use the following comand to install dependencies:

```shell
sudo apt-get install libmicrohttpd-dev libjansson-dev libcurl4-gnutls-dev uuid-dev libldap2-dev libmysqlclient-dev libsqlite3-dev libconfig-dev

git clone https://github.com/babelouest/orcania.git
cd orcania
make
sudo make install

git clone https://github.com/babelouest/yder.git
cd yder/src
make
sudo make install

git clone https://github.com/babelouest/ulfius.git
cd ulfius/src
make
sudo make install

git clone https://github.com/babelouest/hoel.git
cd hoel/src
make
sudo make install
```

The following external libraries are required to build Angharad:
- [libuuid](https://sourceforge.net/projects/libuuid/), UUID generator
- [libldap](http://www.openldap.org/devel//cvsweb.cgi/libraries/libldap?hideattic=0&sortbydate=0), ldap API
- [libconfig](http://www.hyperrealm.com/libconfig/), Configuration File Library
- [jansson](http://www.digip.org/jansson/), json manipulation library

The following other external libraries are required to build angharad dependencies:
- [libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/), HTTP server library
- [libcurl](https://curl.haxx.se/libcurl/), HTTP client library
- [MySQL client API](https://dev.mysql.com/doc/refman/5.7/en/c-api.html), MySQL/MariaDB API
- [SQLite3](https://www.sqlite.org/), SQLite API

And finally, Angharad depends on the following libraries:
- [Orcania](https://github.com/babelouest/orcania), Potluck with different functions for different purposes that can be shared among angharad programs
- [Yder](https://github.com/babelouest/yder), Logging library
- [Ulfius](https://github.com/babelouest/ulfius), Web Framework for REST Applications
- [Hoel](https://github.com/babelouest/hoel), Database abstraction library

## Benoic modules prerequisites

Benoic implements a ZWave module to control ZWave nodes. It's based on the [OpenZwave library](http://www.openzwave.net/). You must download and install this library before installing benoic zwave device module, version 1.2 or above, preferably a stable version.

## Carleon modules prerequisites

Carleon modules available are the following:
- Motion control, to view pictures and real-time streams of a motion application, depends on [MagickCore](http://www.imagemagick.org/script/magick-core.php) library (`apt-get install libmagickcore-dev`)
- MPD Control, for basic control of a MPD instance: volume, play/pause/stop and playlist loading, depends on [libmpdclient](https://www.musicpd.org/libs/libmpdclient/) (`apt-get install libmpdclient-dev`)

## Other modules

Other modules are in the roadmap, such as [Taulas](https://github.com/babelouest/taulas) devices control for benoic, [Liquidsoap](http://savonet.sourceforge.net/) control, or an open weather api client for Carleon.

# Installation

## Main system compilation

When all prerequisites are installed, clone angharad from github, and git install submodules:

```shell
git clone https://github.com/babelouest/angharad.git
cd angharad
git submodule update --init
make
```

## Benoic modules compilation

Go to Benoic modules directory, then compile the modules you need. If you don't need/have a zwave dongle, you can skip this part.

```shell
cd benoic/device-modules
make libdevzwave.so
```

## Carleon modules compilation

Go to Carleon modules directory, then compile the modules you need.

```shell
cd carleon/service-modules/
make libservicemotion.so # For Motion service
make libservicempd.so # For MPD service
```

## Full system installation

Go back to angharad source folder, then as root, run `make install`:

```shell
cd angharad
sudo make install
```

## Database installation

Depending on your needs, you can use a sqlite3 database (single file, fast and reliable) or a MySQL Database (fast and reliable as well but also more secure).

### MySQL Installation

Run the SQL script angharad.mariadb.sql file under an already existing database.

```shell
mysql < angharad.mariadb.sql
```

### SQLite3 installation

Run the SQL script angharad.sqlite3.sql file in an existing or a new SQLite3 database file:
```shell
sqlite3 angharad.db < angharad.sqlite3.sql
```

# Configuration

The file angharad.conf.sample contains a sample file with all the configuration parameters needed, just fill the parameters with your own environment. Paths can be relatives or absolute.

Copy `angharad.conf.sample` into your configuration folder as `angharad.conf`

```shell
cp angharad.conf.sample /usr/local/etc/angharad.conf
```

## Static file server

`static_files_path` and `static_files_prefix` are used for the static files server, in this case, [Sagremor](https://github.com/babelouest/sagremor) is the dedicated web application and can be hosted there.

## Database

Specify the database parameters in the `database {}` block, depending on your database configuration (SQLite3 or MySQL).

## Authentication

Currently, 2 types of authentication are supported, database users or LDAP authentication. If you use database authentication, you must add users directly in the database, for example:

```sql
insert into a_user (au_login, au_password, au_enabled) values ('user_login', PASSWORD('password'), 1);
```

Please note that users and profile are not linked, any authenticated user can use any profile.

# Run Angharad

To run angharad, simply execute the command:

```shell
/usr/local/bin/angharad --config-file=/usr/local/etc/angharad.conf
```

Check the log messages in the log file, syslog or the console, depending on your configuration, then the server will be up and running when you'll se the following log message:

```log
yyyy-mm-dd hh:mm:ss - Angharad INFO: Start angharad on port 2473
```

Now you can access Angharad API via a third party application like [Sagremor](https://github.com/babelouest/sagremor).
