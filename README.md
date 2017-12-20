# Angharad

House automation system API server

![example snapshot](https://github.com/babelouest/sagremor/raw/master/snapshots/dashboard-filled.png)
Central program and orchestrator of the following submodules:

- [Benoic](https://github.com/babelouest/benoic): Command house automation devices as lights, heaters, sensors, switches. Currently [ZWave](http://www.z-wave.com/) and [Taulas](https://github.com/babelouest/taulas) devices are supported
- [Carleon](https://github.com/babelouest/carleon): Command house automation side services (music, camera, etc)
- [Gareth](https://github.com/babelouest/gareth): Logs and messenger service

Angharad integrates all those submodules functionalities and manages scripts, events, user profiles, and a small static file server.

Benoic and Carleon implement a dynamic library loading interface to add/remove/update modules without having to recompile all the system every time an update or an improvement is made.

Each module are compiled independently and have its own dependencies.

# Prerequisites

All Angharad system, its submodules and its custom libraries are written in C language, for better performance and lightweight resources consumption. GCC and Makefile were used for the development.

## External dependencies

### TL;DR

On Debian-based systems (Debian, Raspbian, Ubuntu), use the following comand to install dependencies:

```shell
$ sudo apt-get install libmicrohttpd-dev libjansson-dev libcurl4-gnutls-dev libmysqlclient-dev libsqlite3-dev libconfig-dev libssl-dev
# Install libjwt
$ git clone https://github.com/benmcollins/libjwt.git
$ cd libjwt/
$ autoreconf -i
$ ./configure
$ make
$ sudo make install

# Install Orcania
$ git clone https://github.com/babelouest/orcania.git
$ cd orcania/
$ make
$ sudo make install

# Install Yder
$ git clone https://github.com/babelouest/yder.git
$ cd yder/src/
$ make
$ sudo make install

# Install Ulfius
$ git clone https://github.com/babelouest/ulfius.git
$ cd ulfius/src/
$ make
$ sudo make install

# Install Hoel
$ git clone https://github.com/babelouest/hoel.git
$ cd hoel/src/
$ make
$ sudo make install
```

The following external libraries are required to build Angharad:
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
- [Libjwt](https://github.com/benmcollins/libjwt), JWT parser

## Benoic modules prerequisites

Benoic implements a ZWave module to control ZWave nodes and a [Taulas](https://github.com/babelouest/taulas) module.

The Benoic module is based on the [OpenZwave library](http://www.openzwave.net/). You must download and install this library before installing benoic zwave device module, version 1.2 or above, preferably a stable version.

## Carleon modules prerequisites

Carleon modules available are the following:
- Motion control, to view pictures and real-time streams of a motion application
- MPD Control, for basic control of a MPD instance: volume, play/pause/stop and playlist loading, depends on [libmpdclient](https://www.musicpd.org/libs/libmpdclient/) (`apt-get install libmpdclient-dev`)
- Liquidsoap Control, for basic control of a [liquidsoap](http://liquidsoap.fm/) based webradio: play/stop/next and view the 10 last played songs

# Installation

## Main system compilation

When all prerequisites are installed, clone angharad from github, and git install submodules:

```shell
git clone https://github.com/babelouest/angharad.git
cd angharad
git submodule update --init
make
```

This will compile angharad program, and all device-modules for Benoic and services for Carleon.

## Full system installation

Go back to angharad source folder, then as root, run `make install`:

```shell
cd angharad
sudo make install
```

## Benoic modules

Benoic modules are located in the folder `benoic/device-modules`. If you want to adapt or change the modules for your needs, you can manually compile them.

The module named `device-mock.c` is a mock device that is used to test the behaviour of a module with benoic and angharad systems. A mock device doesn't control any real hardware, but simulates the behaviour of a device. You can use it to help you write your own device module.

```shell
cd benoic/device-modules
make libdevzwave.so # For ZWave device
make libdevtaulas.so # For Taulas device
make libdevmock.so # For mock device
sudo make install # to install compiled devices in $PREFIX/lib/angharad/benoic directory
```

## Carleon modules

Carleon service are located in the folder `carleon/service-modules/`.

The service named `service-mock.c` is a mock service that is used to test the behaviour of a service with carleon and angharad systems. A mock service doesn't control any real service, but simulates one. You can use it to help you write your own service.

```shell
cd carleon/service-modules/
make libservicemotion.so # For Motion service
make libservicempd.so # For MPD service
make libserviceliquidsoap.so # For MPD service
make libservicemock.so # For mock service
sudo make install # to install compiled devices in $PREFIX/lib/angharad/carleon directory
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

Beware that the static web server is not secured with SSL connection, so don't use it outside a trusted network. i.e., don't forward ports directly outside your network.

If you want to access angharad in a secure connection, you can use apache `mod_proxy` and an https instance for example, and embed angharad access in it.

## Database

Specify the database parameters in the `database {}` block, depending on your database configuration (SQLite3 or MySQL).

## Authentication

Angharad comes with an OAuth2 authentication method. You can use any OAuth2 authentication provider, as long as it provides [JSON Web Tokens](https://jwt.io/) with Glewlwyd format informations. Or you can install [Glewlwyd](https://github.com/babelouest/glewlwyd) as OAuth2 authentication server.

The scope required for the backend API is `angharad` by default.

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

# API Documentation

The full API Documentation can be found in the files `API.md` (except for Gareth who is a loner):

- [Angharad API](API.md)
- [Benoic API](https://github.com/babelouest/benoic/blob/master/API.md)
- [Carleon API](https://github.com/babelouest/carleon/blob/master/API.md)
- [Gareth API](https://github.com/babelouest/gareth)
