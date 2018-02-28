# Angharad

House automation system API server

![example snapshot](https://github.com/babelouest/sagremor/raw/master/snapshots/dashboard-filled.png)
Central program and orchestrator of the following submodules:

- [Benoic](https://github.com/babelouest/benoic): Command house automation devices as lights, heaters, sensors, switches. Currently [ZWave](http://www.z-wave.com/) and [Taulas](https://github.com/babelouest/taulas) devices are supported
- [Carleon](https://github.com/babelouest/carleon): Command house automation side services (music, camera, etc)
- [Gareth](https://github.com/babelouest/gareth): Logs and messenger service

Angharad integrates all those submodules functionalities and manages scripts, events, user profiles, and a small static file server.

Benoic and Carleon implement a dynamic library loading interface to add/remove/update modules without having to recompile all the system every time an update or an improvement is made.

# Installation

Follow the [docs/INSTALL.md](docs/INSTALL.md) document.

# API Documentation

The full API Documentation can be found in the files `API.md` (except for Gareth who is a loner):

- [Angharad API](docs/API.md)
- [Benoic API](https://github.com/babelouest/benoic/blob/master/docs/API.md)
- [Carleon API](https://github.com/babelouest/carleon/blob/master/docs/API.md)
- [Gareth API](https://github.com/babelouest/gareth)
