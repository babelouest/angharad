# Install Angharad

This documents describes the installation of the application Angharad and the following devices modules and services:

```
- Device ZWave for Benoic
- Device Taulas for Benoic
- Service Liquidsoap for Carleon
- Service MPD for Carleon
- Service Motion for Carleon
```

## Prerequisites

All Angharad system, its submodules and its custom libraries are written in C/C++ language, for better performance and lightweight resources consumption. GCC and Makefile were used for the development.

### Benoic modules prerequisites

Benoic implements a ZWave module to control ZWave nodes and a [Taulas](https://github.com/babelouest/taulas) module.

The Benoic module is based on the [OpenZwave library](http://www.openzwave.net/). You must download and install this library before installing benoic zwave device module, version 1.2 or above, preferably a stable version.

### Carleon modules prerequisites

Carleon modules available are the following:
- Motion control, to view pictures and real-time streams of a motion application
- MPD Control, for basic control of a MPD instance: volume, play/pause/stop and playlist loading, depends on [libmpdclient](https://www.musicpd.org/libs/libmpdclient/) (`apt-get install libmpdclient-dev`)
- Liquidsoap Control, for basic control of a [liquidsoap](http://liquidsoap.fm/) based webradio: play/stop/next and view the 10 last played songs

The following libraies are required to build Angharad and its submodules:

```
libmicrohttpd
libjansson
libcurl
libmysqlclient
libsqlite3
libconfig
libssl
libopenzwave
libmpdclient
libjwt
```

On Debian-based systems (Debian, Raspbian, Ubuntu), use the following comand to install the dependencies:

```shell
$ sudo apt-get install -y libmicrohttpd-dev libjansson-dev libcurl4-gnutls-dev libmysqlclient-dev libsqlite3-dev libconfig-dev libssl-dev libopenzwave1.5-dev libmpdclient-dev
# Install libjwt
$ git clone https://github.com/benmcollins/libjwt.git
$ cd libjwt/
$ autoreconf -i
$ ./configure
$ make
$ sudo make install
```

## Pre-compiled packages

You can install Angharad with a pre-compiled package available in the [release pages](https://github.com/babelouest/angharad/releases/latest/). The package files `angharad-full_*` contain the package libraries of `orcania`, `yder`, `ulfius` and `hoel` precompiled for `angharad`, plus `angharad` package.

For example, to install Angharad with the `
For example, to install Hutch with the `hutch-full_1.2.0_Debian_stretch_x86_64.tar.gz` package downloaded on the `releases` page, you must execute the following commands:

```shell
$ sudo apt-get install -y libmicrohttpd-dev libjansson-dev libcurl4-gnutls-dev libmysqlclient-dev libsqlite3-dev libconfig-dev libssl-dev libopenzwave1.5-dev libmpdclient-dev
$ wget https://github.com/benmcollins/libjwt/archive/v1.9.tar.gz
$ tar -zxvf v1.9.tar.gz
$ cd libjwt-1.9
$ autoreconf -i
$ ./configure
$ make && sudo make install
$ wget https://github.com/babelouest/angharad/releases/download/v1.2.0/angharad-full_1.2.0_Debian_stretch_x86_64.tar.gz
$ tar xf angharad-full_1.2.0_Debian_stretch_x86_64.tar.gz
$ sudo dpkg -i liborcania_1.2.0_Debian_stretch_x86_64.deb
$ sudo dpkg -i libyder_1.2.0_Debian_stretch_x86_64.deb
$ sudo dpkg -i libhoel_1.4.0_Debian_stretch_x86_64.deb
$ sudo dpkg -i libulfius_2.3.1_Debian_stretch_x86_64.deb
$ sudo dpkg -i angharad_1.2.0-b.3_Debian_stretch_x86_64.deb
```

## Build from source

Then, download Angharad source code and its submodules.

```shell
git clone https://github.com/babelouest/angharad.git
cd angharad
git submodule update --init
```

### CMake build

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make && sudo make install
```

### Good ol' Makefile

```shell
# Install Orcania
$ git clone https://github.com/babelouest/orcania.git
$ cd orcania/src/
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
$ make -DWITH_PGSQL=off
$ sudo make install

# Install Angharad
$ cd angharad
$ make && sudo make install
```

### Benoic modules

Benoic modules are located in the folder `benoic/device-modules`. If you want to adapt or change the modules for your needs, you can manually compile them.

The module named `device-mock.c` is a mock device that is used to test the behaviour of a module with benoic and angharad systems. A mock device doesn't control any real hardware, but simulates the behaviour of a device. You can use it to help you write your own device module.

```shell
cd benoic/device-modules
make libdevzwave.so # For ZWave device
make libdevtaulas.so # For Taulas device
make libdevmock.so # For mock device
sudo make install # to install compiled devices in $PREFIX/lib/angharad/benoic directory
```

### Carleon services

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

`app_files_path` is used for the static files server, in this case, [Sagremor](https://github.com/babelouest/sagremor) is the dedicated web application and can be hosted there.

Beware that the static web server is not secured with SSL connection, so don't use it outside a trusted network. i.e., don't forward ports directly outside your network.

If you want to access Angharad in a secure connection, you can use apache `mod_proxy` and an https instance for example, and embed Angharad access in it.

## Database

Specify the database parameters in the `database {}` block, depending on your database configuration (SQLite3 or MySQL).

## Authentication

Angharad comes with an OAuth2 authentication method. You can use any OAuth2 authentication provider, as long as it provides [JSON Web Tokens](https://jwt.io/) with Glewlwyd format informations. Or you can install [Glewlwyd](https://github.com/babelouest/glewlwyd) as OAuth2 authentication server.

The scope required for the backend API is `angharad` by default.

Please note that users and profile are not linked, any authenticated user can use any profile.

# Run Angharad

By default, when running, Angharad is accessible at the address [http://localhost:2473/](http://localhost:2473/).

## In the console

To run angharad, simply execute the command:

```shell
/usr/local/bin/angharad --config-file=/usr/local/etc/angharad/angharad.conf
```

Check the log messages in the log file, syslog or the console, depending on your configuration, then the server will be up and running when you'll se the following log message:

```log
yyyy-mm-dd hh:mm:ss - Angharad INFO: Start angharad on port 2473
```

## As a service

Use the script `angharad-init` (SysV Init) or `angharad.service` (SystemD) to run Angharad as a service. Both files are designed for Raspbian distribution but can be easily modify to fit your needs.

### SysV init

Copy the file `angharad-init` in the directory `/etc/init.d` as `angharad`, enable it on startup if needed, then start the service:

```shell
$ sudo cp angharad-init /etc/init.d/angharad
$ sudo update-rc.d angharad defaults
$ sudo service angharad start
```

### SystemD init

Copy the file `angharad.service` in the directory `/etc/systemd/system`, enable it on startup if needed, then start the service:

```shell
$ sudo cp angharad.service /etc/systemd/system
$ sudo systemctl enable angharad
$ sudo service angharad start
```
