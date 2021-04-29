# Benoic

House automation controller.

Used to control switches, dimmers, heaters and sensor.

This program is able to run standalone, but it's better worth using with [Angharad](https://github.com/babelouest/angharad) system.

## Benoic modules

Benoic implements an interface to control multiple types of house automation devices. To add a new device type, you have to create a library with the proper functions, then place the library in the folder specified by `modules_path` in your configuration file, then restart the program.

Check [Benoic devices modules How-To](device-modules/README.md) for more information on how to build a device module.

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

## Benoic modules prerequisites

Benoic implements a ZWave module to control ZWave nodes. It's based on the [OpenZwave library](http://www.openzwave.net/). You must download and install this library before installing benoic zwave device module, version 1.2 or above, preferably a stable version.

# Installation

## Standalone compilation

When all prerequisites are installed, clone benoic from github:

```shell
git clone https://github.com/babelouest/benoic.git
cd benoic
make release-standalone
```

## Benoic modules compilation

Go to Benoic modules directory, then compile the modules you need. If you don't need/have a zwave dongle, you can skip this part.

```shell
cd device-modules
make libdevzwave.so
```

## Full system installation

Go back to benoic source folder, then as root, run `make install`:

```shell
cd benoic
sudo make install
```

## Database installation

Depending on your needs, you can use a sqlite3 database (single file, fast and reliable) or a MySQL Database (fast and reliable as well but also more secure).

### MySQL Installation

Run the SQL script benoic.mariadb.sql file under an already existing database.

```shell
mysql < benoic.mariadb.sql
```

### SQLite3 installation

Run the SQL script benoic.sqlite3.sql file in an existing or a new SQLite3 database file:
```shell
sqlite3 benoic.db < benoic.sqlite3.sql
```

# Configuration

The file benoic.conf.sample contains a sample file with all the configuration parameters needed, just fill the parameters with your own environment. Paths can be relatives or absolute.

Copy `benoic.conf.sample` into your configuration folder as `benoic.conf`

```shell
cp benoic.conf.sample /usr/local/etc/benoic.conf
```

# Run Benoic

To run benoic, simply execute the command:

```shell
/usr/local/bin/benoic --config-file=/usr/local/etc/benoic.conf
```

Check the log messages in the log file, syslog or the console, depending on your configuration, then the server will be up and running when you'll se the following log message:

```log
yyyy-mm-dd hh:mm:ss - Benoic INFO: Start benoic on port 2642
```

# API Documentation

The full API Documentation can be found in the file `API.md`:

- [Benoic API](API.md)
