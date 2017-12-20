# Angharad

House automation system API server

![example snapshot](https://github.com/babelouest/sagremor/raw/master/snapshots/dashboard-filled.png)
Central program and orchestrator of the following submodules:

- [Benoic](https://github.com/babelouest/benoic): Command house automation devices as lights, heaters, sensors, switches. Currently [ZWave](http://www.z-wave.com/) and [Taulas](https://github.com/babelouest/taulas) devices are supported
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

Currently, 2 types of authentication are supported, database users or LDAP authentication. If you use database authentication, you can manage users with the dedicated API.

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

# Angharad API Documentation

Angharad has several endpoints with their own format. Here is the full documentaition of the REST API.

All the return data are in JSON format.

Except fo static file API, all URL have the prefix called `ANGHARAD_PREFIX`, default is `/angharad`. All the URLs given in this dcumentation (Except for static files) are relative to `ANGHARAD_PREFIX`.

## Authentication

Except when mentionned otherwise, all endpoints require a valid authentication token located in the header or in the cookies. The header or cookies key must be called `"ANGHARAD_SESSION_ID"` and the value must be a valid token value returned b a previous successfull login.

## Submodules management

### Get all submodules available

#### URL

`/submodule/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[
	{
		"name": string, max 64 characters
		"description": string, max 128 characters
		"enabled": boolean
	}
]
```

#### Error Response

Code 500

Internal Error

### Get submodule specified

#### URL

`/submodule/@submodule_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@submodule_name`: Submodule name to look for

#### Success response

Code 200

Content
```javascript
{
	"name": string, max 64 characters
	"description": string, max 128 characters
	"enabled": boolean
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Submodule `@submodule_name`not found

### Enable or disable submodule

#### URL

`/submodule/@submodule_name/enable/@enabled`

#### Method

`GET`

#### URL Parameters

**Required**

`@submodule_name`: Submodule name to look for

`@enabled`: 0 (disable) or 1 (enable)

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Submodule `@submodule_name`not found

## Scripts management

### Get all scripts available

#### URL

`/script`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[
	{

		"name":string, maximum 64 characters, required
		"description":string, maximum 128 characters, optional
		"options":{ optional
			"tags":[
				string, maximum 64 characters
			] Maximum 128 tags
		},
		"actions":[
			{
				"submodule":string, submodule name available (ex, benoic or carleon), required
				"element": string, element name, required
				"parameters":{
					"element_type":string, element type, required if submodule is benoic
					"device":string, device name, required if submodule is benoic
				},
				"command": string|number, command to send to the element, required
			}
		]

	}
]
```

#### Error Response

Code 500

Internal Error

### Get script by its name

#### URL

`/script/@script_name`

#### Method

`GET`

#### URL Parameters

**Required**
`@script_name`: script name

#### Success response

Code 200

Content
```javascript
{

	"name":string, maximum 64 characters, required
	"description":string, maximum 128 characters, optional
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
	"actions":[
		{
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"element_type":string, element type, required if submodule is benoic
				"device":string, device name, required if submodule is benoic
			},
			"command": string|number, command to send to the element, required
		}
	]

}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Script not found

### Add new script

#### URL

`/script/`

#### Method

`POST`

#### Data Parameters

```javascript
{

	"name":string, maximum 64 characters, must be a new name, required
	"description":string, maximum 128 characters, optional
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
	"actions":[
		{
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"element_type":string, element type, required if submodule is benoic
				"device":string, device name, required if submodule is benoic
			},
			"command": string|number, command to send to the element, required
		}
	]

}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Modify a script

#### URL

`/script/@script_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@script_name`: Script name

#### Data Parameters

```javascript
{

	"description":string, maximum 128 characters, optional
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
	"actions":[
		{
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"command": string|number, command to send to the element, required
			"parameters":{
				"element_type":string, element type, required if submodule is benoic
				"device":string, device name, required if submodule is benoic
			}
		}
	]

}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Remove a script

#### URL

`/script/@script_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@script_name`: Script name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Script not found

### Add tag to script

#### URL

`/script/@script_name/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@tag`: tag value, maximum 64 characters, maximum 128 tags per element

`@script_name`: Script name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

script not found

### Delete tag from script

#### URL

`/script/@script_name/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@tag`: tag value, maximum 64 characters, maximum 128 tags per element

`@script_name`: Script name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

script or tag not found

### Run script

#### URL

`/script/@script_name/run`

#### Method

`GET`

#### URL Parameters

**Required**

`@script_name`: Script name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404
Script not found

## Schedulers management

### Get all schedulers

#### URL

`/scheduler/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of scripts

    {
        "name":string, 64 characters max
        "description":string, 128 characters max
        "enabled":boolean
        "repeat":integer, values possible are -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year
        "repeat_value":positive integer
        "next_time":integer, next launch in UNIX EPOCH format
        "remove_after":boolean, if true, this scheduler will be autmatically removed when done
        "conditions":[ Array of conditions objects
			{
				"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains'
				"value":string|integer|boolean
				"submodule":string, submodule name available (ex, benoic or carleon), required
				"element": string, element name, required
				"parameters":{
					"device":string, device name, required if submodule is benoic
					"element_type":string, element type, required if submodule is benoic
				},
			}
        ],
        "scripts":[ Array of scripts to run
            {
                "name":script name
                "enabled":boolean
            }
        ],
		"options":{ optional
			"tags":[
				string, maximum 64 characters
			] Maximum 128 tags
		},
    }

]
```

#### Error Response

Code 500

Internal Error

### Get scheduler by its name

#### URL

`/scheduler/@scheduler_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

#### Success response

Code 200
Content
```javascript
{
	"name":string, 64 characters max
	"description":string, 128 characters max
	"enabled":boolean
	"repeat":integer, values possible are -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year
	"repeat_value":positive integer
	"next_time":integer, next launch in UNIX EPOCH format
	"remove_after":boolean, if true, this scheduler will be autmatically removed when done
	"conditions":[ Array of conditions objects
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains'
			"value":string|integer|boolean
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run
		{
			"name":script name
			"enabled":boolean
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Scheduler not found

### Enable or disable a scheduler

#### URL
`/scheduler/@scheduler_name/enable/@enabled`

#### Method

`GET`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

`@enabled`: 1 (true) or 0 (false)

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

Scheduler not found

### Add a new scheduler

#### URL

`/scheduler/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name":string, 64 characters max, required
	"description":string, 128 characters max
	"enabled":boolean, required
	"repeat":integer, values possible are -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year, required
	"repeat_value":positive integer, required
	"remove_after":boolean, if true, this scheduler will be autmatically removed when done
	"conditions":[ Array of conditions objects, conditions are optional
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains', required
			"value":string|integer|boolean, required
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run, at least one scrit is required
		{
			"name":script name, required
			"enabled":boolean, required
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Modify an existing scheduler

#### URL

`/scheduler/@scheduler_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

#### Data Parameters

```javascript
{
	"description":string, 128 characters max
	"enabled":boolean, required
	"repeat":integer, values possible are -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year, required
	"repeat_value":positive integer, required
	"remove_after":boolean, if true, this scheduler will be autmatically removed when done
	"conditions":[ Array of conditions objects, conditions are optional
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains', required
			"value":string|integer|boolean, required
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run, at least one scrit is required
		{
			"name":script name, required
			"enabled":boolean, required
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Scheduler not found

### Remove a scheduler

#### URL

`/scheduler/@scheduler_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Scheduler not found

### Add a new tag to a scheduler

#### URL

`/scheduler/@scheduler_name/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

`@tag`: Tag value, maximum 64 characters, maximum 128 tags per scheduler

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

Scheduler not found

### Remove a tag from a scheduler

#### URL

`/scheduler/@scheduler_name/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

`@tag`: Tag value

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Scheduler or tag not found

## Triggers management

### Send an external message

This endpoint does not require a valid authentication token in the headers or in the cookies.

It's used to send an external message in Angharad. If a trigger matches this message, its scripts will be run.

#### URL

`/alert/@submodule_name/@source/@element/@message/`

#### Method

`GET`

#### URL Parameters

**Required**

`@submodule_name`: string, submodule name of the sender (benoic or carleon)

`@source`: string, source of the sender

`@element`: string, sender element

`@message`: string, message sent

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

### Get all triggers

#### URL

`/trigger/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of trigger objects

    {
        "name":string, max 64 characters
        "description":string, max 128 characters
        "enabled":boolean
        "submodule":string, submodule name available (ex, benoic or carleon)
        "source":string, source of the trigger (ex device for a benoic element), max 64 characters
        "element":string, element of the trigger (ex element name for a benoic element), max 64 characters
        "message":string, message value to match
        "message_match":integer, message match type, values are 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty
		"conditions":[ Array of conditions objects
			{
				"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains'
				"value":string|integer|boolean
				"submodule":string, submodule name available (ex, benoic or carleon), required
				"element": string, element name, required
				"parameters":{
					"device":string, device name, required if submodule is benoic
					"element_type":string, element type, required if submodule is benoic
				},
			}
		],
        "scripts":[ Array of scripts to run
            {
                "name":string, script name
                "enabled":boolean
            }
        ],
		"options":{ optional
			"tags":[
				string, maximum 64 characters
			] Maximum 128 tags
		},
    }

]
```

#### Error Response

Code 500

Internal Error

### Get a trigger by its name

#### URL

`/trigger/@trigger_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@trigger_name`: trigger name

#### Success response

Code 200

Content
```javascript
{
	"name":string, max 64 characters
	"description":string, max 128 characters
	"enabled":boolean
	"submodule":string, submodule name available (ex, benoic or carleon)
	"source":string, source of the trigger (ex device for a benoic element), max 64 characters
	"element":string, element of the trigger (ex element name for a benoic element), max 64 characters
	"message":string, message value to match
	"message_match":integer, message match type, values are 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty
	"conditions":[ Array of conditions objects
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains'
			"value":string|integer|boolean
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run
		{
			"name":string, script name
			"enabled":boolean
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
}
```

#### Error Response

Code 500

Internal Error

Code 404

trigger not found

### Enable or disable a trigger

#### URL

`/trigger/@trigger_name/enable/@enabled`

#### Method

`GET`

#### URL Parameters

**Required**

`@trigger_name`: trigger name

`@enabled`: 0 (disabled) or 1 (enabled)

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

trigger not found

### Add a new trigger

#### URL

`/trigger/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name":string, max 64 characters, required
	"description":string, max 128 characters
	"enabled":boolean, required
	"submodule":string, submodule name available (ex, benoic or carleon), required
	"source":string, source of the trigger (ex device for a benoic element), max 64 characters, required
	"element":string, element of the trigger (ex element name for a benoic element), max 64 characters, required
	"message":string, message value to match, required (can be empty)
	"message_match":integer, message match type, values are 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty, required
	"conditions":[ Array of conditions objects, conditions are optional
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains', required
			"value":string|integer|boolean, required
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run, at least one script is required
		{
			"name":string, script name, required
			"enabled":boolean, required
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Modify an existing trigger

#### URL

`/trigger/@trigger_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@trigger_name`: trigger name

#### Data Parameters

```javascript
{
	"description":string, max 128 characters
	"enabled":boolean, required
	"submodule":string, submodule name available (ex, benoic or carleon), required
	"source":string, source of the trigger (ex device for a benoic element), max 64 characters, required
	"element":string, element of the trigger (ex element name for a benoic element), max 64 characters, required
	"message":string, message value to match, required (can be empty)
	"message_match":integer, message match type, values are 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty, required
	"conditions":[ Array of conditions objects, conditions are optional
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains', required
			"value":string|integer|boolean, required
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run, at least one script is required
		{
			"name":string, script name, required
			"enabled":boolean, required
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

trigger not found

### Remove an existing trigger

#### URL

`/trigger/@trigger_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@trigger_name`: trigger name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Trigger not found

### Add a new tag to a trigger

#### URL

`/trigger/@trigger_name/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@trigger_name`: Trigger name

`@tag`: Tag value, maximum 64 characters, maximum 128 tags per scheduler

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

Trigger not found

### Remove a tag from a trigger

#### URL

`/trigger/@trigger_name/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@trigger_name`: Trigger name

`@tag`: Tag value

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Trigger or tag not found

### Profiles management

### Get all profiles data

#### URL

`/profile`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[
	any json value as profile data
]
```

#### Error Response

Code 500

Internal Error

### Get a profile by its name

#### URL

`/profile/@profile_id`

#### Method

`GET`

#### URL Parameters

**Required**

`@profile_id`: id (name) of the profile

#### Success response

Code 200

Content
```javascript
any json value as profile data
```

#### Error Response

Code 500

Internal Error

OR

Code 404

profile not found

### Update all values of a profile

#### URL

`/profile/@profile_id`

#### Method

`method`

#### URL Parameters

**Required**

`@profile_id`: id (name) of the profile

#### Data Parameters

```javascript
any json value as profile data
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

profile not found

### Delete a profile by its name

#### URL

`/profile/@profile_id`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@profile_id`: id (name) of the profile

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

profile not found

## Authentication management

### Check authentication token

#### URL

`/auth`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
{
	"token":string, the token value
	"validity":integer, date of end of validity for the current token in UNIX EPOCH format
}
```

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

### Check user credentials to receive a valid authentication token

This endpoint does not require a valid authentication token in the headers or the cookies

If the validity sent does not match one in the list below, it will be rounded to the previous value in the list.

#### URL

`/auth`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"user": string, required
	"password": string, required
	"validity": integer, end of validity in seconds for the sent token, possible values are 0: none (end of session), 3600: 1 hour, 43200: 12 hours, 86400: 1 day, 604800: 1 week, above 1 week: 10 years
}
```

#### Success response

Code 200

Content
```javascript
{
	"token":string, the token value
	"validity":integer, date of end of validity for the current token in UNIX EPOCH format
}
```

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

### Revoke current token

#### URL

`/auth`

#### Method

`DELETE`

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

## Users management

These endpoints manage database users only, and only if database users authentication is enabled, otherwise all those endpoints will return an error 404.

### Get all users

#### URL

`/user/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ array of users object
	{
		"login": string
		"enabled", boolean
	}
]
```

#### Error Response

Code 500

Internal Error

### Get a database user by it login

#### URL

`/user/@user_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@user_name`: string, login of the user to look for

#### Success response

Code 200

Content
```javascript
{
	"login": string
	"enabled", boolean
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

User not found

### Add a new database user

#### URL
`/user/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"login": string, must be a new login, max 128 characters, required
	"password": string, password to use, max 128 characters, required
	"enabled", boolean, required
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Modify an existing database user

#### URL

`/user/@user_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@user_name`: string, login of the user

#### Data Parameters

```javascript
{
	"password": string, password to use, max 128 characters, optional
	"enabled", boolean, required
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

database user not found

### Delete an existing database user

#### URL

`/user/@user_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@user_name`: string, login of the user

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

database user not found

## Tokens management

### Get a list of tokens

#### URL

`/token/`

#### Method

`GET`

#### URL Parameters

**Optional**

`login`: string, tokens assigned to the specified login

`enabled`: string ("true"|"false"), look for enabled or revoked tokens

#### Success response

Code 200

Content
```javascript
[
	{

		"token":string, token value
		"login":string, login value assigned to the token
		"enabled":boolean, token valid or revoked
		"last_seen":string, date of last seen token in ISO 8601 format
		"validity":string, date of end of validity of the token in ISO 8601 format

	}
]
```

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

### Revoke a token

#### URL

`/token/`

#### Method

`POST`

#### Data Parameters

```javascript
{

    "token":string, token value

}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Token not found

## Static files

Static files are used to host a static website within Angharad

### Get a static file

#### URL

`/static_file_prefix/@static_file_path`

#### Method

`GET`

#### URL Parameters

**Required**

`@static_file_path`: string, static file path

#### Success response

Code 200

Content

raw file content and its content-type in the headers.

Currently, the supported content-types are:

```
".html": "text/html"
".css": "text/css"
".js": "application/javascript"
".png": "image/png"
".jpeg": "image/jpeg"
".jpg": "image/jpeg"
".ttf": "font/ttf"
".woff": "font/woff"
".woff2": "font/woff2"
".json": "application/json"
".map": "application/octet-stream"
"*": "application/octet-stream"
```

#### Error Response

Code 500

Internal Error

OR

Code 404

file not found

## Options

This endpoint is used to send OPTIONS parameters for all endpoints

### Send OPTIONS parameters

#### URL
`*`

#### Method

`OPTIONS`

#### Success response

Code 200

Content

Headers contain all options
# Benoic API Documentation

Benoic has several endpoints with their own format. Here is the full documentaition of the REST API.

All the return data are in JSON format.

All URL have the prefix called `BENOIC_PREFIX`, default is `/benoic`. All the URLs given in this dcumentation are relative to `BENOIC_PREFIX`.

## Authentication

If used within Angharad application, and except when mentionned otherwise, all endpoints require a valid authentication token located in the header or in the cookies. The header or cookies key must be called `"ANGHARAD_SESSION_ID"` and the value must be a valid token value returned b a previous successfull login.

## Devices management

### Get all device types

Get all the different device types available

#### URL

`/deviceTypes/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of device types
    {

        "uid": string, uid of the type, max 64 chars
        "name":string, name of the type, max 64 chars
        "description":string, descrition of the type, max 512 chars
        "enabled":boolean
        "options":[ // Array of options avaiable to ad a new device of this type
            {
                "name":string, name of the option
                "type":string, values can be "string", "integer", "float" or "boolean"
                "description":string, description of the option
                "optional":boolean
            }
        ],

    }
]
```

#### Error Response

Code 500

Internal Error

### Get all devices

#### URL

`/device/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ array of device objects

    {
        "name":string, name of the device, max 64 chars
        "type_uid":string, uid of the device type
        "description":string, description for the device, max 512 chars
        "enabled":boolean
        "connected":boolean
        "last_seen":string, date of last seen device connected, in ISO 8601 format
        "options":{ object containing options for the current device
        },
    }

]
```

#### Error Response

Code 500

Internal Error

### Get a specific device by its name

#### URL

`/device/@device_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

Content
```javascript
{
    "name":string, name of the device, max 64 chars
    "type_uid":string, uid of the device type
    "description":string, description for the device, max 512 chars
    "enabled":boolean
    "connected":boolean
    "last_seen":string, date of last seen device connected, in ISO 8601 format
    "options":{ object containing options for the current device
    },
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

### Add a new device

#### URL

`/device/`

#### Method

`POST`

#### Data Parameters

```javascript
{
    "name":string, name of the device, max 64 chars, name must not already exist
    "type_uid":string, uid of the device type
    "description":string, description for the device, max 512 chars
    "enabled":boolean
    "connected":boolean
    "options":{ object containing options for the current device
    },
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Modify an existing device

#### URL

`/device/@device_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@device_name`: device name

#### Data Parameters

```javascript
{
    "description":string, description for the device, max 512 chars
    "enabled":boolean
    "connected":boolean
    "options":{ object containing options for the current device
    },
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device not found

### Delete a device

#### URL

`/device/@device_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

### Connect a device

#### URL

`/device/@device_name/connect`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

### Disconnect a device

#### URL

`/device/@device_name/disconnect`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

### Ping a device

#### URL

`/device/@device_name/ping`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

#### Error Response

Code 503

Device not responding to ping command

OR

Code 500

Internal Error

OR

Code 400

Device disconnected or disabled

OR

Code 404

Device not found

## Device elements overview

### Overview Device

#### URL

`/device/@device_name/overview`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

Content
```javascript
{

    "sensors":{ Object containing all sensors
        "sensor_name_1":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":boolean|string|number, sensor value
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
                "unit":string
            },
            "enabled":bolean
        },
        "sensor_name_2":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":boolean
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
                "unit":string
            },
            "enabled":bolean
        }
    },
    "switches":{ Object containing all switches
        "switch_name_1":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":integer, 0 (off) or 1 (on)
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
            },
            "enabled":bolean
        },
        "switch_name_2":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":integer, 0 (off) or 1 (on)
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
            },
            "enabled":bolean
        }
    },
    "dimmers":{ Object containing all dimmers
        "dimmer_name_1":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":integer, between 0 (off) and 100 (max)
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
            },
            "enabled":bolean
        },
        "dimmer_name_2":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":integer, between 0 (off) and 100 (max)
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
            },
            "enabled":bolean
        }
    },
    "heaters":{ Object containing all heaters
        "heater_name_1":{
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "options":{ options object
                "unit":string
            },
            "enabled":bolean
            "monitored":bolean
            "monitored_every":integer, monitor frequency in seconds
            "value":{ value object for a heater
                "mode":string, current mode
                "command":number, temperature command
                "on":boolean, indicates if the heat is on or off
                "availableModes":[ array of strings to get all modes available
                ]
            }
        },
        "heater_name_2":{
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "options":{ options object
                "unit":string
            },
            "enabled":bolean
            "monitored":bolean
            "monitored_every":integer, monitor frequency in seconds
            "value":{ value object for a heater
                "mode":string, current mode
                "command":number, temperature command
                "on":boolean, indicates if the heat is on or off
                "availableModes":[ array of strings to get all modes available
                ]
            }
        }
    }

}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

## Elements management

"GET", url_prefix, "/device/@device_name/@element_type/@element_name"

### Get a specific element data

#### URL

`/device/@device_name/@element_type/@element_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

#### Success response

Code 200

Depending on your element type, the content will vary

Content
```javascript
sensor value:
{
    "display":string, element display name, max 128 chars
    "description":string, element description, max 128 chars
    "value":boolean|string|number, sensor value
    "monitored":boolean
    "monitored_every":integer, monitor frequency in seconds
    "options":{ options object
        "unit":string
    },
    "enabled":bolean
}

switch value:
{
    "display":string, element display name, max 128 chars
    "description":string, element description, max 128 chars
    "value":integer, 0 (off) or 1 (on)
    "monitored":boolean
    "monitored_every":integer, monitor frequency in seconds
    "options":{ options object
    },
    "enabled":bolean
}

dimmer value:
{
    "display":string, element display name, max 128 chars
    "description":string, element description, max 128 chars
    "value":integer, between 0 (off) and 100 (max)
    "monitored":boolean
    "monitored_every":integer, monitor frequency in seconds
    "options":{ options object
    },
    "enabled":bolean
}

heater value:
{
    "display":string, element display name, max 128 chars
    "description":string, element description, max 128 chars
    "options":{ options object
        "unit":string
    },
    "enabled":bolean
    "monitored":bolean
    "monitored_every":integer, monitor frequency in seconds
    "value":{ value object for a heater
        "mode":string, current mode
        "command":number, temperature command
        "on":boolean, indicates if the heat is on or off
        "availableModes":[ array of strings to get all modes available
        ]
    }
}
```

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Element type incorrect

OR

Code 404

Device or element not found

### Send a command to an element

#### URL

`/device/@device_name/@element_type/@element_name/@command`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

`@command`: number, command value

**Optional**

`@mode`: string, heater mode (heater only)

#### Success response

Code 200

Content

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device or element not found

### Update an element data

#### URL

`/device/@device_name/@element_type/@element_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

#### Data Parameters

```javascript
{
    "display": string, max 128 chars
    "description": string, max 128 chars
    "enabled": boolean
    "options": object containing options values, example unit value for sensors
    "monitored":bolean
    "monitored_every":integer, monitor frequency in seconds
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device or element not found

### Add a tag to an element

#### URL

`/device/@device_name/@element_type/@element_name/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

`@tag`: string, tag value

#### Success response

Code 200

Content

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device or element not found

### Delete a tag from an element

#### URL

`/device/@device_name/@element_type/@element_name/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

`@tag`: string, tag value

#### Success response

Code 200

Content

#### Error Response

Code 500

Internal Error

OR

Code 404

Device or element not found

### Get an element's monitored values

#### URL

`/monitor/@device_name/@element_type/@element_name/`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

**Optional**

`from`: Start date for the monitoring, in UNX EPOCH format

`to`: End date for the monitoring, in UNX EPOCH format

#### Success response

Code 200

Content
```javascript
[ Array of monitor objects

    {
        "timestamp":integer, date for the monitoring, in UNX EPOCH format
        "value":string, value for the monitoring
    }

]
```

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device or element not found
# Carleon API Documentation

Carleon has several endpoints with their own format. Here is the full documentaition of the REST API.

All the return data are in JSON format.

All URL have the prefix called `CARLEON_PREFIX`, default is `/carleon`. All the URLs given in this dcumentation are relative to `CARLEON_PREFIX`.

This is only carleon API, each carleon services can add their own endpoints, their documentation can be found in [carleon service modules](service-modules/README.md)

## Authentication

If used within Angharad application, and except when mentionned otherwise, all endpoints require a valid authentication token located in the header or in the cookies. The header or cookies key must be called `"ANGHARAD_SESSION_ID"` and the value must be a valid token value returned b a previous successfull login.

### Get all services available, their description, their elements and their commands

#### URL

`/service/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[

    {
        "name":string, name of the service, max 64 chars
        "description":string, description of the service, max 64 chars
        "enabled":boolean
        "commands":{ object containing all commands
            "exec1":{ command name
                "parameters":{ input parameters
                    "param1":{ parameter name
                        "type":string, values can be "string", "integer", "real" or "boolean"
                        "required":boolean
                    },
                    "param2":{ another parameter name
                        "type":string, values can be "string", "integer", "real" or "boolean"
                        "required":boolean
                    },
                    "param3":{
                        "type":"real"
                    }
                },
                "result":{ Result object, this command has 2 values in its result
                    "value1":{ result name
                        "type":string, values can be "string", "integer", "real" or "boolean"
                    },
                    "value2":{ another result name
                        "type":string, values can be "string", "integer", "real" or "boolean"
                    }
                }
            },
            "exec2":{ another command name
                "result":{ result object, this command has 1 value in its result
                    "type":string, values can be "string", "integer", "real" or "boolean"
                }
            },
        },
        "element":[ Elements list of all the service elements
            {
                "name":string, element name, max 64 chars
                "description":string, element description, max 64 chars
                "tags":[ Array of tags, each tag is astring, max 64 chars, max 128 tags per element
                ]
            }
        ]
    },

]
```

#### Error Response

Code 500

Internal Error

### Enable or disable a service

#### URL

`/service/@service_name/enable/@enable_value`

#### Method

`PUT`

#### URL Parameters

**Required**

`@service_name`: service name

`@enable_value`: 0 (disable) or 1 (enable)

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Service not found

### Add a tag to a service element

#### URL

`/service/@service_name/@element_id/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@service_name`: service name

`@element_id`: element name

`@tag`: string, tag value

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Service or element not found

### Remove a tag from a service element

#### URL

`/service/@service_name/@element_id/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@service_name`: service name

`@element_id`: element name

`@tag`: string, tag value

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Service or element or tag not found

### Clean all data of a service element

#### URL

`/service/@service_name/@element_id/cleanup`

#### Method

`PUT`

#### URL Parameters

**Required**

`@service_name`: service name

`@element_id`: element name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Service or element not found
# Mock service API

This service does nothing for real, but behaves as a real carleon service. It's designed to help create a client program and create other services.

This service use carleon prefix as prefix. All endpoints described here assume to have carleon prefix before the url.

### Get all mock service elements

#### URL

`/mock-service/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ array of moc-service elements

    {
        "name": string, name of the element
        "description": string, description of the element
    }

]
```

#### Error Response

Code 500

Internal Error

### Get a moc-service element by its name

#### URL

`/mock-service/@element_id`

#### Method

`GET`

#### URL Parameters

**Required**

`@element_id`: element name

#### Success response

Code 200

Content
```javascript
{
	"name": string, name of the element
	"description": string, description of the element
}

```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Add a new mock-service element

#### URL

`/mock-service/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name": string, name of the element, max 64 chars
	"description": string, description of the element, max 128 chars
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Modify an existing mock-service element

#### URL

`/mock-service/@element_id`

#### Method

`PUT`

#### URL Parameters

**Required**

`@element_id`: element name

#### Data Parameters

```javascript
{
	"description": string, description of the element, max 128 chars
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Element not found

### Delete an existing mock-service element

#### URL

`/mock-service/@element_id`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@element_id`: element name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Execute a mock-service command exec1 on an element

#### URL

`/mock-service/@element_id/command/exec1/@param1/@param2/@param3`

#### Method

`GET`

#### URL Parameters

**Required**

`@element_id`: element name

`@param1`: string

`@param2`: integer

`@param3`: real

#### Success response

Code 200

Content
```javascript
{
	"value1": string
	"value2": integer
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Execute a mock-service command exec2 on an element

#### URL

`/mock-service/@element_id/command/exec2/@param1`

#### Method

`GET`

#### URL Parameters

**Required**

`@element_id`: element name

`@param1`: string

#### Success response

Code 200

Content
```javascript
{
	"value": string
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Execute a mock-service command exec3 on an element

#### URL

`/mock-service/@element_id/command/exec3`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
{
	"value": string
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Execute a mock-service command exec4 on an element

#### URL

`/mock-service/@element_id/command/exec4`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
{
	"value1": integer
	"value2": boolean
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found
# MPD service API

MPD service is used to get status and send commands to a MPD instance

This service use carleon prefix as prefix. All endpoints described here assume to have carleon prefix before the url.

## Service MPD elements management

### Get all service-mpd elements

#### URL

`/service-mpd/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of service-mpd elements

    {
        "name": string, name of the element, max 64 chars
        "description": string, description of the element, max 128 chars
        "host": string, host where the mpd instance is hosted, max 128 chars
        "port": integer, port where mpd instance is hosted, if 0 then default
    }

]
```

#### Error Response

Code 500

Internal Error

### Get a service-mpd element by its name

#### URL

`/service-mpd/@name`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

#### Success response

Code 200

Content
```javascript
{
	"name": string, name of the element, max 64 chars
	"description": string, description of the element, max 128 chars
	"host": string, host where the mpd instance is hosted, max 128 chars
	"port": integer, port where mpd instance is hosted, if 0 then default
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

service-mpd not found

### Add a new service-mpd element

#### URL

`/service-mpd/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name": string, name of the element, max 64 chars, required
	"description": string, description of the element, max 128 chars, optional
	"host": string, host where the mpd instance is hosted, max 128 chars, required
	"port": integer, port where mpd instance is hosted, if 0 then default mpd port, optional
	"password": string, password to connect to the mpd instance, max 128 chars, optional
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Modify an existing service-mpd element

#### URL

`/service-mpd/@name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@name`: string, name of the element

#### Data Parameters

```javascript
{
	"name": string, name of the element, max 64 chars, required
	"description": string, description of the element, max 128 chars, optional
	"host": string, host where the mpd instance is hosted, max 128 chars, required
	"port": integer, port where mpd instance is hosted, if 0 then default mpd port, optional
	"password": string, password to connect to the mpd instance, max 128 chars, optional
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Element not found

### Delete an existing service-mpd element

#### URL

`/service-mpd/@name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@name`: string, name of the element

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Get status and current song details of a service-mpd element

#### URL

`/service-mpd/@name/status`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`, name of the element

#### Success response

Code 200

Content
```javascript
{

    "state":string, state of the player, values can be "stop", "play", "pause", "unknown"
    "volume":integer, volume of the player, between 0 and 100
    "elapsed_time":integer, elapsed time of the current song in seconds
    "total_time":integer, total time of the current song in seconds
    "name":string, Name of the playlist, optional
    "title":string, title of current song
    "artist":string, artist of current song, optional
    "album":string, album of current song, optional
    "date":string, year of current song, optional
    "track":string, track of current song, optional

}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Send a playback command

#### URL

`/service-mpd/@name/action/@action_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

`@action_name`, string, name of the action to launch, values can be "stop", "play", "pause"

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Element not found

### Get playlists available

#### URL

`/service-mpd/@name/playlists`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

#### Success response

Code 200

Content
```javascript
[ Array of strings

    "playlist_1",
    "playlist_2",
    "playlist_3",
    ...

]
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Load and play a playlist

#### URL

`/service-mpd/@name/playlists/@playlist_name/load`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

`@playlist_name`: string, name of the playlist

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Element or playlist not found

### Change the volume

#### URL

`/service-mpd/@name/volume/@value`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

`@value`: integer, volume to set, value must be between 0 (off) and 100 (max)

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Element not found
# Motion service API

Motion service is used to display motion pictures taken or real-time streams.

This service use carleon prefix as prefix. All endpoints described here assume to have carleon prefix before the url.

## Motion-service elements management

### Get all motion-service elements

#### URL

`/service-motion/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of service-motion elements
    {
        "name": string, name of the element, max 64 chars
        "description": string, description of the element, max 128 chars
        "config_uri": string, uri of the motion instance configuration to check if it's still responding, max 512 chars
        "stream_list": [ Array of streams
            {
                "uri": string, uri to access the stream directly from the client, max 512 chars
                "name": string, name of the stream, max 64 chars
                "snapshot_uri": string, uri to run a snapshot command on the stream, max 512 chars
            }
        ],
        "file_list": [ Array of files lists of the pricures taken by this motion instance
			{
				"name": string, name of the file list, max 64 chars
				"path": string, full path of the pictures, max 512 chars
				"thumbnail_path": string, full path of the pictures thumbnails, max 512 chars, optional
			}
		]
    }
]
```

#### Error Response

Code 500

Internal Error

### Get a service-motion element by its name

#### URL

`/service-motion/@name`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: name of the service-motion element

#### Success response

Code 200

Content
```javascript
{
	"name": string, name of the element, max 64 chars
	"description": string, description of the element, max 128 chars
	"config_uri": string, uri of the motion instance configuration to check if it's still responding, max 512 chars
	"stream_list": [ Array of streams
		{
			"uri": string, uri to access the stream directly from the client, max 512 chars
			"name": string, name of the stream, max 64 chars
			"snapshot_uri": string, uri to run a snapshot command on the stream, max 512 chars
		}
	],
	"file_list": [ Array of files lists of the pricures taken by this motion instance
		{
			"name": string, name of the file list, max 64 chars
			"path": string, full path of the pictures, max 512 chars
			"thumbnail_path": string, full path of the pictures thumbnails, max 512 chars, optional
		}
	]
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

service-motion element not found

### Add a new service-motion element

#### URL

`/service-motion/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name": string, name of the element, max 64 chars, required
	"description": string, description of the element, max 128 chars, optional
	"config_uri": string, uri of the motion instance configuration to check if it's still responding, max 512 chars, required
	"stream_list": [ Array of streams, optional
		{
			"uri": string, uri to access the stream directly from the client, max 512 chars, required
			"name": string, name of the stream, max 64 chars, required
			"snapshot_uri": string, uri to run a snapshot command on the stream, max 512 chars, required
		}
	],
	"file_list": [ Array of files lists of the pricures taken by this motion instance, optional
		{
			"name": string, name of the file list, max 64 chars, required
			"path": string, full path of the pictures, max 512 chars, required
			"thumbnail_path": string, full path of the pictures thumbnails, max 512 chars, optional
		}
	]
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Modify an existing service-motion element

#### URL

`/service-motion/@name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@name`: name of the element

#### Data Parameters

```javascript
{
	"description": string, description of the element, max 128 chars, optional
	"config_uri": string, uri of the motion instance configuration to check if it's still responding, max 512 chars, required
	"stream_list": [ Array of streams, optional
		{
			"uri": string, uri to access the stream directly from the client, max 512 chars, required
			"name": string, name of the stream, max 64 chars, required
			"snapshot_uri": string, uri to run a snapshot command on the stream, max 512 chars, required
		}
	],
	"file_list": [ Array of files lists of the pricures taken by this motion instance, optional
		{
			"name": string, name of the file list, max 64 chars, required
			"path": string, full path of the pictures, max 512 chars, required
			"thumbnail_path": string, full path of the pictures thumbnails, max 512 chars, optional
		}
	]
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Service-motion element not found

### Delete an existing service-motion element

#### URL

`/service-motion/@name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@name`: name of the element

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Service-motion element not found

## Service-motion element commads

### Get current status of a service-motion element

#### URL

`/service-motion/@name/status`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: name of the element

**Optional**

`offset`: offset to start file-list

`limit`: limit of a file-list result number of files

#### Success response

Code 200

Content
```javascript
{

    "online":boolean, is the motion instance responding?
    "file_list":{ Object containing lists of files
        "file_list_name_1":[ Array of string, each element is a picture file available
        ],
        "file_list_name_2":[ Array of string, each element is a picture file available
        ],
    },
    "stream_list":[ Array of stream objects
        {
            "name":string, name of the stream object
            "snapshot_uri":string, uri to run a snapshot command on the stream, max 512 chars, required
            "uri":uri to access the stream directly from the client, max 512 chars, required
        }
    ]

}
```

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Element not found

### Get a file from a file list

#### URL

`/service-motion/@name/image/@file_list/@file_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: name of the element

`@file_list`: name of the file list

`@file_name`: name of the file

#### Success response

Code 200

Content

Raw content of the file

#### Error Response

Code 500

Internal Error

OR

Code 404

Element or file list or file not found

### Send a snapshot command to a motion instance

#### URL

`/service-motion/@name/stream/@stream_name/snapshot`

#### Method

`PUT`

#### URL Parameters

**Required**

`@name`: name of the element

`@stream_name`: name of the stream

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Element or steam not found

# Gareth API Documentation

In the documentation, we assume you use the prefix `/gareth`.

For all endpoints, return status can be 200 for success, 400 for invalid parameters, 404 for resource or page not found, or 500 for internal server error. On an error 400, the json body result explains the error.

## Message management APIs

### POST /gareth/message

Post a new message.

#### Parameters

##### URL

None

##### Body

```javascrit
{
  "priority": string, values available: NONE, LOW, MEDIUM, HIGH, CRITICAL
  "source": string, maximum 64 characters
  "text": string, maximum 256 characters
  "tags": array of strings, maximum 16 tags, and maximum 32 characters per tag
}
```

##### Result body

None

### GET /gareth/message/@filter_name

Get a list of messages, filter result using a filter name, or message attributes criteria

#### Parameters

##### URL

- @filter_name: string, name of an existing filter. If not set, all messages are displayed
- date: integer, filter messages younger than date in seconds
- source: string filter messages from source parameter
- priority: string, filter messages that have the corresponding priority
- text: string, filter messages that contain the filter value
- tags: string, filter messages that contain the specified tag
- limit: integer, specify the maximal number of message to display, limit can't be hogher than 100. If no specified, limit is set to 20 messages
- offset: integer, specify the offset message to start the search

##### Body

None

##### Result Body

An array of messages. A message has the following format:

```javascrit
{
  "date": string
  "priority": string, values available: NONE, LOW, MEDIUM, HIGH, CRITICAL
  "source": string
  "text": string
  "tags": array of strings
}
```

### Message trigger

If a message correspond to a filter that has alerts (see below), this will trigger a message to be send.

For example, let's say you have this following smtp alert:

```javascrit
{
  "name": "smtp1",
  "host": "localhost",
  "port": null,
  "tls": null,
  "check_ca": null,
  "user": null,
  "password": null,
  "from": "gareth@mail.tld",
  "to": "user@mail.tld",
  "cc": "archive@mail.tld",
  "bcc": null,
  "subject": "Alert message from {source}",
  "body": "On {date}, {source} has sent the following alert: {message}\n\nPriority: {priority}\nAdditional tags: {tags}"
}
```

Then, Gareth has received the following message:

```javascrit
{
  "priority": "HIGH",
  "source": "Door Sensor",
  "text": "The front door has been opened",
  "tags": ["front", "door"]
}
```

The triggered message will have the following form:

```
From: gareth@mail.tld
To: user@mail.tld
Cc: archive@mail.tld
Subject: Alert message from Door Sensor

On 2016/01/14 18:21:12, Door Sensor has sent the following alert: The front door has been opened

Priority: High
Additional tags: front, door
```

## Alerts management APIs

An alert is used joined with one or several filters, when a message is posted and matches a filter (e.g. a message with priority `CRITICAL` from the source `door sensor`), a message alert is triggered.

An alert can be either a smtp message (email) or http request (to call an sms api from your provider for example).

An smtp alert has the following format:

```javascrit
{
  "name": name of the smtp alert, must be unique, string, max 64 characters, mandatory
  "host": smtp hostname, string, max 128 characters, mandatory
  "port": smtp port number, integer, between 0 and 65535, optional, if set to 0 or not set, default port number will be used (25 or 587, depending on tls set to true or not)
  "tls": use TLS to connect, boolean, optional, default false
  "check_ca": check the certificate authority of the smtp host if tls is activated, boolean, optional, default false
  "user": username to connect to the smtp host, string, max 128 characters, optional
  "password": password to connect to the smtp host, string, max 128 characters, optional
  "from": email address of the sender, string, max 128 characters, mandatory
  "to": email address of the recipient, string, max 128 characters, mandatory
  "cc": email address of the recipient in cc, string, max 128 characters, optional
  "bcc": email address of the recipient in bcc, string, max 128 characters, optional
  "subject": subject of the email, string, max 128 characters, mandatory
  "body": body of the email, string, max 512 characters, optional
}
```

`subject` and `body` can contain some specific patterns:
- {priority}: will be replaced by the priority of the message
- {source}: will be replaced by the source of the message
- {message}: will be replaced by the text of the message
- {tags}: will be replaced by the list of tags of the message, separated by a comma
- {date}: will be replaced by the date of the message, using the format format `YYYY/MM/DD hh:mm:ss`

An http alert has the following format:

```javascrit
{
  "name": name of the http alert, must be unique, string, max 64 characters, mandatory
  "method": http method (GET, POST, PUT, etc.), string, max 16 characters, optional (default GET)
  "url": url to call, string, max 128 characters, mandatory
  "body": body of the http call, max 512 characters, optional
  "http_headers": [ Array of http headers to add to the alert, optional
    {
      "key": key of the http header, max 64 characters, mandatory
      "value": value of the http header, max 128 characters, mandatory
    }
  ]
}
```

The attributes `url`, `body`, `key` or `value` can contain patterns as described above:
- {priority}: will be replaced by the priority of the message
- {source}: will be replaced by the source of the message
- {message}: will be replaced by the text of the message
- {tags}: will be replaced by the list of tags of the message, separated by a coma
- {date}: will be replaced by the date of the message, using the format format `YYYY/MM/DD hh:mm:ss`

If specified in the url and the body, these patterns will be url encoded according to the [RFC 3986](https://tools.ietf.org/html/rfc3986).

### GET /gareth/alert

Get all alerts for all types (http and smtp)

#### Parameters

##### URL

None

##### Body

None

##### Result Body

An object with 2 attributes: an array of smtp alerts and an array of http alerts:

```javascrit
{
  "smtp": [
    {
      "name": name of the smtp alert, string, max 64 characters
      "host": smtp hostname, string, max 128 characters
      "port": smtp port number, integer, between 0 and 65535, if set to 0, default port number will be used (25 or 587, depending on tls set to true or not)
      "tls": use TLS to connect, boolean
      "check_ca": check the certificate authority of the smtp host if tls is activated, boolean
      "user": username to connect to the smtp host, string, max 128 characters or null
      "password": password to connect to the smtp host, string, max 128 characters or null
      "from": email address of the sender, string, max 128 characters
      "to": email address of the recipient, string, max 128 characters
      "cc": email address of the recipient in cc, string, max 128 characters
      "bcc": email address of the recipient in bcc, string, max 128 characters
      "subject": subject of the email, string, max 128 characters
      "body": body of the email, string, max 512 characters
    }
  ],
  "http": [
    {
      "name": name of the http alert, string, max 64 characters
      "method": http method (GET, POST, PUT, etc.), string, max 16 characters
      "url": url to call, string, max 128 characters
      "body": body of the http call, max 512 characters
      "http_headers": [
        {
          "key": key of the http header, max 64 characters
          "value": value of the http header, max 128 characters
        }
      ]
    }
  ]
}
```

### GET /gareth/alert/@type/@alert_name

Get a apecific alert of the specified type and name

#### Parameters

##### URL

- @type: smtp or http
- @alert_name: name of the alert

##### Body

None

##### Result Body

smtp alert:

```javascrit
{
  "name": name of the smtp alert, string, max 64 characters
  "host": smtp hostname, string, max 128 characters
  "port": smtp port number, integer, between 0 and 65535, if set to 0, default port number will be used (25 or 587, depending on tls set to true or not)
  "tls": use TLS to connect, boolean
  "check_ca": check the certificate authority of the smtp host if tls is activated, boolean
  "user": username to connect to the smtp host, string, max 128 characters or null
  "password": password to connect to the smtp host, string, max 128 characters or null
  "from": email address of the sender, string, max 128 characters
  "to": email address of the recipient, string, max 128 characters
  "cc": email address of the recipient in cc, string, max 128 characters
  "bcc": email address of the recipient in bcc, string, max 128 characters
  "subject": subject of the email, string, max 128 characters
  "body": body of the email, string, max 512 characters
}
```

http alert:

```javascrit
{
  "name": name of the http alert, string, max 64 characters
  "method": http method (GET, POST, PUT, etc.), string, max 16 characters
  "url": url to call, string, max 128 characters
  "body": body of the http call, max 512 characters
  "http_headers": [
    {
      "key": key of the http header, max 64 characters
      "value": value of the http header, max 128 characters
    }
  ]
}
```

### POST /gareth/alert/@type/

Add a new alert of the specified type

#### Parameters

##### URL

- @type: smtp or http

##### Body

smtp alert:
```javascrit
{
  "name": name of the smtp alert, must be unique, string, max 64 characters, mandatory
  "host": smtp hostname, string, max 128 characters, mandatory
  "port": smtp port number, integer, between 0 and 65535, optional, if set to 0 or not set, default port number will be used (25 or 587, depending on tls set to true or not)
  "tls": use TLS to connect, boolean, optional, default false
  "check_ca": check the certificate authority of the smtp host if tls is activated, boolean, optional, default false
  "user": username to connect to the smtp host, string, max 128 characters, optional
  "password": password to connect to the smtp host, string, max 128 characters, optional
  "from": email address of the sender, string, max 128 characters, mandatory
  "to": email address of the recipient, string, max 128 characters, mandatory
  "cc": email address of the recipient in cc, string, max 128 characters, optional
  "bcc": email address of the recipient in bcc, string, max 128 characters, optional
  "subject": subject of the email, string, max 128 characters, mandatory
  "body": body of the email, string, max 512 characters, optional
}
```

http alert:
```javascrit
{
  "name": name of the http alert, must be unique, string, max 64 characters, mandatory
  "method": http method (GET, POST, PUT, etc.), string, max 16 characters, optional (default GET)
  "url": url to call, string, max 128 characters, mandatory
  "body": body of the http call, max 512 characters, optional
  "http_headers": [ Array of http headers to add to the alert, optional
    {
      "key": key of the http header, max 64 characters, mandatory
      "value": value of the http header, max 128 characters, mandatory
    }
  ]
}
```

##### Result Body

None

### PUT /gareth/alert/@type/@alert_name

Modify an existing alert of the specified type

#### Parameters

##### URL

- @type: smtp or http
- @alert_name: name of the alert

##### Body

smtp alert:
```javascrit
{
  "host": smtp hostname, string, max 128 characters, mandatory
  "port": smtp port number, integer, between 0 and 65535, optional, if set to 0 or not set, default port number will be used (25 or 587, depending on tls set to true or not)
  "tls": use TLS to connect, boolean, optional, default false
  "check_ca": check the certificate authority of the smtp host if tls is activated, boolean, optional, default false
  "user": username to connect to the smtp host, string, max 128 characters, optional
  "password": password to connect to the smtp host, string, max 128 characters, optional
  "from": email address of the sender, string, max 128 characters, mandatory
  "to": email address of the recipient, string, max 128 characters, mandatory
  "cc": email address of the recipient in cc, string, max 128 characters, optional
  "bcc": email address of the recipient in bcc, string, max 128 characters, optional
  "subject": subject of the email, string, max 128 characters, mandatory
  "body": body of the email, string, max 512 characters, optional
}
```

http alert:
```javascrit
{
  "method": http method (GET, POST, PUT, etc.), string, max 16 characters, optional (default GET)
  "url": url to call, string, max 128 characters, mandatory
  "body": body of the http call, max 512 characters, optional
  "http_headers": [ Array of http headers to add to the alert, optional
    {
      "key": key of the http header, max 64 characters, mandatory
      "value": value of the http header, max 128 characters, mandatory
    }
  ]
}
```

##### Result Body

None

### DELETE /gareth/alert/@type/@alert_name

Delete an existing alert of the specified type

#### Parameters

##### URL

- @type: smtp or http
- @alert_name: name of the alert

##### Body

None

##### Result Body

None

## Filters management APIs

Filters can be used for both filter a message list or trigger an smtp or http message when a message matches the filter.

A filter has the following format:

```javascrit
  "name": name of the filter, string, max 
  "description": "valid test",
  "filter_clauses": [
    {"element": "priority", "operator": ">=", "value": 2}
  ],
  "filter_alerts": {"smtp": ["smtp1"], "http": ["http1"]}
```
# Benoic modules

Benoic implements an interface to control multiple types of house automation devices. To add a new device type, you have to create a library with the proper functions, then place the library in the folder specified by `modules_path` in your configuration file, then restart the program.

The mock module in the file `device-mock.c` can be used to create your own module.

## Device module interace

A device module is a library loaded with `dlopen/dlsync`. All mandatory functions will return a json object of type `json_t *`, except for `b_device_has_element` which returns a int.

The returned json object must have the following format:

```json
{
  result: number,
  other values
}
```

The `result` value is used to return a status code. The different status codes available are:

```C
#define WEBSERVICE_RESULT_ERROR     0
#define WEBSERVICE_RESULT_OK        1
#define WEBSERVICE_RESULT_NOT_FOUND 2
#define WEBSERVICE_RESULT_TIMEOUT   3
#define WEBSERVICE_RESULT_PARAM     4
```

These functions are mandatory for a device module:

```C
/**
 *
 * Device type initialization
 * 
 * Initialize the device type and return a json object of the following format:
 * {
 *   result: number, result value
 *   uid: string, a unique identifier for your device type
 *   name: string, the name of your device type
 *   description: string
 *   options: json array, contains the name and type of parameters used to instanciate a device
 * }
 *
 * The options array has the following format:
 * {
 *   name: string, // Name of the option
 *   type: "string"|"numeric"|"boolean", // Type of th option, must be one of those values
 *   description: string, // Description for the option
 *   optional: boolean // is the option optional or not?
 * }
 * 
 */
json_t * b_device_type_init ();

/**
 * 
 * Connects the device
 * 
 * Connects a device and initialize its parameters and context
 * device parameter is a json object containing the following values:
 * {
 *   name: string
 *   options: array // options values according to the options format returned in the b_device_type_init function
 * }
 * device_ptr is a pointer you can use to store any other parameter, structure or value you want without having to rebuild it every time the device is called
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_connect (json_t * device, void ** device_ptr);

/**
 * 
 * Disconnects the device
 * 
 * Disconnect the device and clean device_ptr
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_disconnect (json_t * device, void * device_ptr);

/**
 * 
 * Pings the device
 * 
 * Check if the device is still availble
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_ping (json_t * device, void * device_ptr);

/**
 * 
 * Get all the elements and their values for the device
 * 
 * Return value
 * {
 *   result: number, result value
 *   sensors: {
 *     sensor_name_1: {
 *       unit: string,
 *       value: string|number
 *     },
 *     sensor_name_2: {
 *       unit: string,
 *       value: string|number
 *     }, etc
 *   }
 *   switches: {
 *     switch_name_1: number (0 or 1),
 *     switch_name_2: number (0 or 1),
 *     etc.
 *   }
 *   dimmers: {
 *     dimmer_name_1: number (between 0 and 100),
 *     dimmer_name_2: number (between 0 and 100),
 *     etc.
 *   }
 *   heaters: {
 *     heater_name_1: {
 *       mode: string,
 *       command: float,
 *       on: boolean,
 *       availableModes: Array of strings listing all the different modes available
 *     },
 *     heater_name_2: {
 *       mode: string,
 *       command: float,
 *       on: boolean,
 *       availableModes: Array of strings listing all the different modes available
 *     }
 *     etc.
 *   }
 * }
 * 
 */
json_t * b_device_overview (json_t * device, void * device_ptr);

/**
 * 
 * Get the value of the specified sensor on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 *   value: string|number
 * }
 * 
 */
json_t * b_device_get_sensor (json_t * device, const char * sensor_name, void * device_ptr);

/**
 * 
 * Get the value of the specified switch on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 *   value: number (0 or 1),
 * }
 * 
 */
json_t * b_device_get_switch (json_t * device, const char * switch_name, void * device_ptr);

/**
 * 
 * Send the command on the specified switch on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_set_switch (json_t * device, const char * switch_name, const int command, void * device_ptr);

/**
 * 
 * Get the value of the specified dimmer on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 *   value: number (between 0 and 100),
 * }
 * 
 */
json_t * b_device_get_dimmer (json_t * device, const char * dimmer_name, void * device_ptr);

/**
 * 
 * Send the command on the specified dimmer on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_set_dimmer (json_t * device, const char * dimmer_name, const int command, void * device_ptr);

/**
 * 
 * Get the value of the specified heater on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 *   mode: string,
 *   command: float,
 *   on: boolean,
 *   availableModes: Array of strings listing all the different modes available
 * }
 * 
 */
json_t * b_device_get_heater (json_t * device, const char * heater_name, void * device_ptr);

/**
 * 
 * Send the command on the specified heater on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_set_heater (json_t * device, const char * heater_name, const char * mode, const float command, void * device_ptr);

/**
 * 
 * Return true if the specified element exist on the specified device, false otherwise
 * 
 * element_type can have the following values:
 * 
 * ELEMENT_TYPE_SENSOR 1
 * ELEMENT_TYPE_SWITCH 2
 * ELEMENT_TYPE_DIMMER 3
 * ELEMENT_TYPE_HEATER 4
 * 
 */
int b_device_has_element (json_t * device, int element_type, const char * element_name, void * device_ptr);
```
# Carleon services

Carleon implements an interface to control multiple types of house automation services. To add a new service, you have to create a library with the proper functions, then place the library in the folder specified by `services_path` in your configuration file, then restart the program.

In the service module, you have access to carleon configuration and the ulfius instance, which means you can access the database without instanciating your own database connector:

```C
/**
 *
 * Carleon configuration structure
 *
 */
struct _carleon_config {
  char                    * services_path;
  struct _h_connection    * conn;
  struct _carleon_service * service_list;
  char                    * alert_url;
};
```

The intended behaviour is to create your own webservices that will be available within Carleon. Within your service, the user can create elements and execute commands to those elements. Commands can be automatized via Angharad scripts and events.

The mock service in the file `service-mock.c` can be used to create your own module.

## Service module interace

A service module is a library loaded with `dlopen/dlsync`. All mandatory functions will return a json object of type `json_t *`.

The returned json object must have the following format:

```json
{
  result: number,
  other values
}
```

The `result` value is used to return a status code. The different status codes available are:

```C
#define WEBSERVICE_RESULT_ERROR     0
#define WEBSERVICE_RESULT_OK        1
#define WEBSERVICE_RESULT_NOT_FOUND 2
#define WEBSERVICE_RESULT_TIMEOUT   3
#define WEBSERVICE_RESULT_PARAM     4
```

These functions are mandatory for a device module:

```C
/**
 * 
 * Initializes the service
 * You can add here your own ulfius endpoints
 * 
 * Return value
 * 
 * {
 *   result: number, result value
 *   name: string, name of the service
 *   description: string, description for the service
 * }
 * 
 */
json_t * c_service_init(struct _u_instance * instance, const char * url_prefix, struct _carleon_config * config);

/**
 * Closes the service
 * You can remove your endpoints here
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * c_service_close(struct _u_instance * instance, const char * url_prefix);


/**
 * Get the available commands executable within angharad scripts or events
 * 
 * Return value
 * {
 *   result: number, result value
 *   commands: object containing different commands
 * }
 * 
 * A command must have the following format:
 * command_name: {
 *   parameters: {
 *     param_name_1: {
 *       type: "string"|"integer"|"boolean",
 *       required: boolean (default is false)
 *     },
 *     param_name_2: {
 *       type: "string"|"integer"|"boolean",
 *       required: boolean (default is false)
 *     },
 *     etc
 *   },
 *   result: {
 *     type: "string"|"integer"|"boolean"
 *     OR
 *     {
 *       result_value_1: { type: "string"|"integer"|"boolean" },
 *       result_value_2: { type: "string"|"integer"|"boolean" },
 *       etc
 *     }
 *   }
 * }
 */
json_t * c_service_command_get_list(struct _carleon_config * config);

/**
 * Get the available elements of the service
 * 
 * Return value
 * {
 *   result: number, result value
 *   element: array of elements
 * }
 * 
 * An element returned must have at least the following format:
 * {
 *   name: string, name of the element
 * }
 * 
 */
json_t * c_service_element_get_list(struct _carleon_config * config);

/**
 * Execute the specified command to the specified element with the specified parameters
 * Return the value of the command if needed
 * 
 * Return value
 * {
 *   result: number, result value
 *   value: "string"|"integer"|"boolean" OR object containing the result values names and their values
 * }
 * 
 */
json_t * c_service_exec(struct _carleon_config * config, const char * command, const char * element, json_t * parameters);
```
