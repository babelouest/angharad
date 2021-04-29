# Sagremor

Angharad 3.0 web client application

![example snapshot](https://github.com/babelouest/sagremor/raw/master/snapshots/dashboard-filled.png)

# Prerequisites

npm, gulp and bower are required to test or build the application

Create a file `src/app/config.js` with your own parameters. You can use `src/app/config.js.sample` if you want an example.

## Test

To test or check your modifications on the application in you local machine, run the following commands:

```bash
$ npm install && bower install
$ gulp serve
```

## Build

Build the application with gulp:

```bash
$ npm install && bower install
$ gulp build
```

Then copy the files in the `dist` folder to your webserver Documentroot.

# Screenshots

**Default dashboard**
![Defaut Dashboard](https://github.com/babelouest/sagremor/raw/master/snapshots/dashboard-default.png)

**Switches and dimmers screen**
![Switches and dimmers](https://github.com/babelouest/sagremor/raw/master/snapshots/switches.png)

**Heaters screen**
![Heaters](https://github.com/babelouest/sagremor/raw/master/snapshots/heaters.png)

**Monitor values screen**
![Monitor values](https://github.com/babelouest/sagremor/raw/master/snapshots/monitor.png)

**List of schedulers and triggers**
![Events list](https://github.com/babelouest/sagremor/raw/master/snapshots/Events.png)

**Edit a scheduler**
![Edit a scheduled event](https://github.com/babelouest/sagremor/raw/master/snapshots/Event-edit.png)

**Edit a dimmer**
![Edit a dimmer](https://github.com/babelouest/sagremor/raw/master/snapshots/dimmer-edit.png)

**Music controller**
![Music controller](https://github.com/babelouest/sagremor/raw/master/snapshots/Music.png)

**Logs**
![Logs screen](https://github.com/babelouest/sagremor/raw/master/snapshots/logs.png)

License: MIT
