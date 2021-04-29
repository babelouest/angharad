# Carleon

House automation side services.

Part of the Angharad system, but you can use it as a standlone program.

Provides the context execution for carleon services.

## Carleon services

Carleon implements an interface to control multiple types of house automation services (camera, music, etc.). To add a new service, you have to create a library with the proper functions, then place the library in the folder specified by `modules_path` in your configuration file, then restart the program.

Check [Carleon services modules How-To](service-modules/README.md) for more information on how to build a service.

# Prerequisites

## External dependencies

### TL;DR

On Debian-based systems (Debian, Raspbian, Ubuntu), use the following comand to install dependencies:

```shell
sudo apt-get install libmicrohttpd-dev libjansson-dev libcurl4-gnutls-dev libmysqlclient-dev libsqlite3-dev libconfig-dev

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

You need gcc and Make to compile the program.

External libraries are mandatory to compile Benoic:
- [libconfig](http://www.hyperrealm.com/libconfig/), Configuration File Library
- [jansson](http://www.digip.org/jansson/), json manipulation library
- [libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/), HTTP server library
- [libcurl](https://curl.haxx.se/libcurl/), HTTP client library
- [MySQL client API](https://dev.mysql.com/doc/refman/5.7/en/c-api.html), MySQL/MariaDB API
- [SQLite3](https://www.sqlite.org/), SQLite API

Also, Benoic is based on Angharad libraries to work:
- [Orcania](https://github.com/babelouest/orcania), Potluck with different functions for different purposes that can be shared among angharad programs
- [Yder](https://github.com/babelouest/yder), Logging library
- [Ulfius](https://github.com/babelouest/ulfius), Web Framework for REST Applications
- [Hoel](https://github.com/babelouest/hoel), Database abstraction library

# Installation

## Standalone compilation

When all prerequisites are installed, clone carleon from github:

```shell
git clone https://github.com/babelouest/carleon.git
cd carleon
make release-standalone
```

## Carleon service compilation

Go to Carleon services directory, then compile the services you need.

```shell
cd service-modules
make libservicemotion.so libservicempd.so
```

## Full system installation

Go back to carleon source folder, then as root, run `make install`:

```shell
cd carleon
sudo make install
```

## Database installation

Depending on your needs, you can use a sqlite3 database (single file, fast and reliable) or a MySQL Database (fast and reliable as well but also more secure).

### MySQL Installation

Run the SQL script carleon.mariadb.sql file under an already existing database.

```shell
mysql < carleon.mariadb.sql
```

### SQLite3 installation

Run the SQL script carleon.sqlite3.sql file in an existing or a new SQLite3 database file:
```shell
sqlite3 carleon.db < carleon.sqlite3.sql
```

# Configuration

The file carleon.conf.sample contains a sample file with all the configuration parameters needed, just fill the parameters with your own environment. Paths can be relatives or absolute.

Copy `carleon.conf.sample` into your configuration folder as `carleon.conf`

```shell
cp carleon.conf.sample /usr/local/etc/carleon.conf
```

# Run Carleon

To run carleon, simply execute the command:

```shell
/usr/local/bin/carleon --config-file=/usr/local/etc/carleon.conf
```

Check the log messages in the log file, syslog or the console, depending on your configuration, then the server will be up and running when you'll se the following log message:

```log
yyyy-mm-dd hh:mm:ss - Carleon INFO: Start carleon on port 2756
```

# API Documentation

The full API Documentation can be found in the file `API.md`:

- [Carleon API](API.md)
