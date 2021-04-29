# Gareth

Log application and messenging using a Json/REST interface.

Part of the Angharad System, this program is intended to log messages from Angharad elements, and can send an http request or an smtp message if a trigger has been set for the message.

A message has the following attributes:
- A priority (none, low, medium, high or critical)
- A source (Who sent the message ?)
- A text message (What is is about ?)
- A set of tags

You can see the logs, and filter your search with attributes, you can also display the messages corresponding to a named filter that has its criteria. Finally, you can attach an alert to a named filter, so if a message enters and correspond to a filter, an alert will be sent.

An alert is technically a smtp message or a http request. A http request can be used to send sms or call other webservices for examples.

# Prerequisites

Gareth depend on the libraries [Ulfius](https://github.com/babelouest/ulfius/), [Yder](https://github.com/babelouest/yder/), [Hoel](https://github.com/babelouest/hoel/), [Orcania](https://github.com/babelouest/orcania/) for the Angharad libraries, also [Jansson](http://www.digip.org/jansson/) and [Libconfig](http://www.hyperrealm.com/libconfig/) for the non Angharad libraries.

You must install the dependencies (and their dependencies too) before installing Gareth.

You must use a MariaDB/Mysql database or a SQLite database.

# Standalone Installation

Compile gareth as a standalone application:

```shell
$ make release-standalone
```

# Configuration

You must specify a configuration file when you start gareth-standalone. You can copy the file `gareth.conf.sample` to `gareth.conf` and modify it to fit your needs.

# Start and stop Gareth

This is the online help for gareth:

```shell
$ gareth-standalone -h
Gareth Messaging REST Webservice

Messaging system using a JSON/REST interface

-c --config-file=PATH
	Path to configuration file
-p --port=PORT
	Port to listen to
-u --url-prefix=PREFIX
	URL prefix
-m --log-mode=MODE
	Log Mode
	console, syslog or file
	If you want multiple modes, separate them with a comma ","
	default: console
-l --log-level=LEVEL
	Log level
	NONE, ERROR, WARNING, INFO, DEBUG
	default: ERROR
-f --log-file=PATH
	Path for log file if log mode file is specified
-h --help
	Print this message
```

All those parameters can be specified in the configuration file.

To start gareth-standalone via the command-line, enter the command:

```shell
$ ./gareth-standalone --config-file=gareth.conf
```

Right after start, your log should see in the logs:

```
2016-01-14 18:54:20 - Gareth INFO: Starting Gareth alert and messenger service 
2016-01-14 18:54:20 - Gareth INFO: Start gareth on port 4784, prefix: gareth 
```

The service is now started and can be called using the HTTP REST API interface.
