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

The following libraies are required to build Angharad and its submodules:

```
libmicrohttpd
libjansson
libcurl
libmysqlclient
libsqlite3
libconfig
libopenzwave
libmpdclient
```

On Debian-based systems (Debian, Raspbian, Ubuntu), use the following comand to install the dependencies:

```shell
$ sudo apt-get install -y libmicrohttpd-dev libjansson-dev libcurl4-gnutls-dev libmysqlclient-dev libsqlite3-dev libconfig-dev libopenzwave1.5-dev libmpdclient-dev libshout-dev
```

Angharad requires [orcania](https://github.com/babelouest/orcania), [yder](https://github.com/babelouest/yder), [ulfius](https://github.com/babelouest/ulfius), [hoel](https://github.com/babelouest/hoel), [rhonabwy](https://github.com/babelouest/rhonabwy), [iddawc](https://github.com/babelouest/iddawc) and their dependencies.

## Build from source

Then, download Angharad source code and its submodules.

```shell
git clone https://github.com/babelouest/angharad.git
```

### CMake build

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make && sudo make install
```

The available options for cmake are:
- `-DWITH_JOURNALD=[on|off]` (default `on`): Build with journald (SystemD) support for logging
- `-DBUILD_TESTING=[on|off]` (default `off`): Compile unit tests
- `-DWITH_MOCKDEV=[on|off]` (default `off`): Compile Benoic Mock device
- `-DWITH_ZWAVE=[on|off]` (default `off`): Compile Benoic ZWave device
- `-DWITH_TAULAS=[on|off]` (default `off`): Compile Benoic Taulas device
- `-DWITH_MOCKSERVICE=[on|off]` (default `off`): Compile Carleon Mock service
- `-DWITH_MOTION=[on|off]` (default `on`): Compile Carleon Motion controller
- `-DWITH_LIQUIDSOAP=[on|off]` (default `on`): Compile Carleon Liquidsoap controller
- `-DWITH_MPD=[on|off]` (default `on`): Compile Carleon MPD controller
- `-DCMAKE_BUILD_TYPE=[Debug|Release]` (default `Release`): Compile with debugging symbols or not

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

# Install Rhonabwy
$ git clone https://github.com/babelouest/rhonabwy.git
$ cd rhonabwy/src/
$ make
$ sudo make install

# Install Iddawc
$ git clone https://github.com/babelouest/iddawc.git
$ cd iddawc/src/
$ make
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
make libdevesp8266.so # For ESP8266 bridge device
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

It's recommended to disable the static web server for better security, just comment out the property line in the configuration file. It's better to serve the statis files via a webserver such as Apache or NGINX.

## Database

Specify the database parameters in the `database {}` block, depending on your database configuration (SQLite3 or MySQL).

## Authentication

Angharad comes with an OIDC authentication method. You can use any OIDC authentication provider, as long as it provides access tokens using the [JSON Web Token (JWT) Profile for OAuth 2.0 Access Tokens](https://www.rfc-editor.org/rfc/rfc9068.html) method
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
