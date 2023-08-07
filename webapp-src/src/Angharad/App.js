/**
 *
 * Angharad front-end application
 *
 * Copyright 2023 Nicolas Mora <mail@babelouest.org>
 *
 * License MIT
 *
 */

import React, { Component } from 'react';
import i18next from 'i18next';

import apiManager from '../lib/APIManager';
import routage from '../lib/Routage';
import oidcConnector from '../lib/OIDCConnector';
import messageDispatcher from '../lib/MessageDispatcher';
import Notification from '../lib/Notification';

import TopMenu from './TopMenu';
import Config from './Config';
import Components from './Components';
import Services from './Services';
import Scripts from './Scripts';
import Schedulers from './Schedulers';
import ModalElement from './ModalElement';
import ModalMonitor from './ModalMonitor';
import ModalDimmer from './ModalDimmer';
import ModalBlind from './ModalBlind';
import ModalService from './ModalService';
import ModalScript from './ModalScript';
import ModalScheduler from './ModalScheduler';
import ModalMap from './ModalMap';
import ModalConfirm from './ModalConfirm';
import ModalOpenService from './ModalOpenService';
import MapContainer from './MapContainer';

class App extends Component {
  constructor(props) {
    super(props);

    this.state = {
      config: props.config,
      oidcStatus: props.config.oidc?.status||false,
      tokenTimeout: false,
      profile: {sub: false, name: false},
      nav: false,
      langChanged: false,
      submodules: [],
      deviceTypes: [],
      deviceList: [],
      serviceList: [],
      script: [],
      scheduler: [],
      deviceOverview: {},
      components: [],
      services: [],
      streamList: [],
      profileList: [],
      adminMode: false,
      showModalElement: false,
      showModalMonitor: false,
      showModalService: false,
      showModalScript: false,
      showModalScheduler: false,
      showModalMap: false,
      showModalConfirm: false,
      showOpenService: false,
      modalDimmerParameters: {device: false, name: false, element: false},
      modalElementParameters: {device: false, type: false, name: false, element: false},
      modalServiceParameters: {type: false, element: false, add: false},
      modalScriptParameters: {add: false, script: {}},
      modalSchedulerParameters: {add: false, scheduler: {}},
      modalMapParameters: {add: false, map: {}},
      modalConfirmParameters: {title: false, message: false, cb: false},
      modalOpenService: {element: false, type: false},
      mapIndex: 0,
      mapPlace: false
    };

    routage.addChangeRouteCallback((route) => {
      this.gotoRoute(route);
    });

    messageDispatcher.subscribe('OIDC', (message) => {
      if (message.status === "disconnected") {
        this.setState({oidcStatus: message.status});
        messageDispatcher.sendMessage('Notification', {type: "warning", message: i18next.t("message.disconnected")});
      } else if (message.status === "connected" || message.status === "refresh") {
        var curDate = new Date();
        apiManager.setToken(message.token, (curDate.getTime()/1000)+message.expires_in);
        if (message.status === "connected") {
          messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("message.connected")});
        }
        var tokenTimeout = false;
        if (oidcConnector.getParameter("responseType").search("code") === -1) {
          clearTimeout(this.state.tokenTimeout);
          tokenTimeout = setTimeout(() => {
            messageDispatcher.sendMessage('Notification', {type: "warning", message: i18next.t("message.sessionTimeout"), autohide: false});
            this.setState({oidcStatus: "timeout"});
          }, message.expires_in*1000);
        }
        this.setState({oidcStatus: "connected", tokenTimeout: tokenTimeout});
      } else if (message.status === "profile") {
        if (message.profile == "error") {
          messageDispatcher.sendMessage('Notification', {type: "warning", message: i18next.t("message.profileError")});
          this.setState({oidcStatus: "disconnected"});
        } else if (message.profile) {
          let sub = message.profile.sub, name = "";
          if (this.state.config.frontend.oidc?.userIdParameter) {
            sub = message.profile[this.state.config.frontend.oidc?.userIdParameter];
          }
          if (this.state.config.frontend.oidc?.nameParameter) {
            name = message.profile[this.state.config.frontend.oidc?.nameParameter];
          }
          this.setState({profile: {sub: sub, name: name}}, () => {
            this.gotoRoute(routage.getCurrentRoute());
            this.initModules();
          });
        }
      }
    });

    messageDispatcher.subscribe('Navigation', (message) => {
      if (message.route && message.route.path) {
        let path = message.route.path.split("/");
        if (path[0] === "properties") {
          this.gotoRoute("properties");
        } else if (path[0] === "components" || path[0] === "services" || path[0] === "scripts" || path[0] === "schedulers" || path[0] === "map") {
          this.gotoRoute(message.route.path);
        }
      } else {
        this.setState({nav: false,mapIndex: 0}, () => {
          this.gotoRoute("");
        });
      }
    });

    messageDispatcher.subscribe('Mode', (message) => {
      if (message.admin) {
        this.setState({adminMode: !this.state.adminMode});
      }
    });

    messageDispatcher.subscribe('Devices', (message) => {
      if (message.status === "update") {
        this.initModules();
      }
    });

    messageDispatcher.subscribe('Component', (message) => {
      if (message.status === "update") {
        let deviceOverview = this.state.deviceOverview;
        if (message.type === "switch") {
          deviceOverview[message.device].switches[message.name].value = message.value;
        } else if (message.type === "dimmer") {
          deviceOverview[message.device].dimmers[message.name].value = message.value;
        }
        this.setState({deviceOverview: deviceOverview});
      } else if (message.status === "edit") {
        this.setState({showModalElement: true, modalElementParameters: {device: message.device, type: message.type, name: message.name, element: message.element}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalElement'), {
            keyboard: false
          });
          modal.show();
        });
      } else if (message.status === "monitor") {
        this.setState({showModalMonitor: true, modalElementParameters: {device: message.device, type: message.type, name: message.name, element: message.element}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalMonitor'), {
            keyboard: false
          });
          modal.show();
        });
      } else if (message.status === "dimmerModal") {
        this.setState({modalDimmerParameters: {device: message.device, name: message.name, element: message.element}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalDimmer'), {
            keyboard: true
          });
          modal.show();
        });
      } else if (message.status === "blindModal") {
        this.setState({modalDimmerParameters: {device: message.device, name: message.name, element: message.element}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalBlind'), {
            keyboard: true
          });
          modal.show();
        });
      } else if (message.status === "refresh") {
        this.refreshData()
        .then((results) => {
          messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("message.refresh")});
        });
      }
    });

    messageDispatcher.subscribe('Service', (message) => {
      if (message.remove) {
        this.setState({showModalConfirm: true,
                       modalConfirmParameters: {title: i18next.t("services.remove-title"),
                                                message: i18next.t("services.remove-message", {name: message.element.name}),
                                                cb: this.cbRemoveService,
                                                type: message.type,
                                                element: message.element}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalConfirm'), {
            keyboard: false
          });
          modal.show();
        });
      } else if (message.status === "open") {
        this.setState({showOpenService: true, modalOpenService: {type: message.type, element: message.element}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalOpenService'), {
            keyboard: false
          });
          modal.show();
        });
      } else {
        this.setState({showModalService: true, modalServiceParameters: {type: message.type, element: message.element, add: message.add}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalService'), {
            keyboard: false
          });
          modal.show();
        });
      }
    });

    messageDispatcher.subscribe('Script', (message) => {
      if (message.remove) {
        this.setState({showModalConfirm: true,
                       modalConfirmParameters: {title: i18next.t("scripts.remove-title"),
                                                message: i18next.t("scripts.remove-message", {name: message.script.name}),
                                                cb: this.cbRemoveScript,
                                                type: "script",
                                                script: message.script}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalConfirm'), {
            keyboard: false
          });
          modal.show();
        });
      } else {
        this.setState({showModalScript: true, modalScriptParameters: {add: message.add, script: message.script}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalScript'), {
            keyboard: false
          });
          modal.show();
        });
      }
    });

    messageDispatcher.subscribe('Scheduler', (message) => {
      if (message.remove) {
        this.setState({showModalConfirm: true,
                       modalConfirmParameters: {title: i18next.t("schedulers.remove-title"),
                                                message: i18next.t("schedulers.remove-message", {name: message.scheduler.name}),
                                                cb: this.cbRemoveScheduler,
                                                type: "scheduler",
                                                scheduler: message.scheduler}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalConfirm'), {
            keyboard: false
          });
          modal.show();
        });
      } else {
        if (message.status === "refresh") {
          apiManager.APIRequestAngharad("scheduler")
         .then((results) => {
           this.setState({scheduler: results});
         })
         .catch((err) => {
           messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
         });
        } else {
          this.setState({showModalScheduler: true, modalSchedulerParameters: {add: message.add, scheduler: message.scheduler}}, () => {
            var modal = new bootstrap.Modal(document.getElementById('modalScheduler'), {
              keyboard: false
            });
            modal.show();
          });
        }
      }
    });

    messageDispatcher.subscribe('Map', (message) => {
      if (message.remove) {
        this.setState({showModalConfirm: true,
                       modalConfirmParameters: {title: i18next.t("maps.remove-title"),
                                                message: i18next.t("maps.remove-message", {name: message.map.name}),
                                                cb: this.cbRemoveMap,
                                                type: "map",
                                                map: message.map}}, () => {
          var modal = new bootstrap.Modal(document.getElementById('modalConfirm'), {
            keyboard: false
          });
          modal.show();
        });
      } else {
        if (message.status === "refresh") {
          apiManager.APIRequestAngharad("profile")
         .then((results) => {
           this.setState({profileList: results});
         })
         .catch((err) => {
           messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
         });
        } else if (message.status === "place") {
          this.setState({mapPlace: !this.state.mapPlace});
        } else if (message.status === "next") {
          if (this.state.profileList.length) {
            let nbMaps = 0;
            this.state.profileList.forEach((profile) => {
              if (!!profile.image && Array.isArray(profile.elements) && (profile.enabled || this.state.adminMode)) {
                nbMaps++;
              }
            });
            if ((this.state.mapIndex+1) < nbMaps) {
              this.setState({mapIndex: this.state.mapIndex+1}, () => {
                routage.addRoute("map/"+this.state.mapIndex);
              });
            } else {
              this.setState({mapIndex: 0}, () => {
                routage.addRoute("map/"+this.state.mapIndex);
              });
            }
          }
        } else if (message.status === "previous") {
          if (this.state.profileList.length) {
            let nbMaps = 0;
            this.state.profileList.forEach((profile) => {
              if (!!profile.image && Array.isArray(profile.elements) && (profile.enabled || this.state.adminMode)) {
                nbMaps++;
              }
            });
            if (!this.state.mapIndex) {
              this.setState({mapIndex: nbMaps-1}, () => {
                routage.addRoute("map/"+this.state.mapIndex);
              });
            } else {
              this.setState({mapIndex: this.state.mapIndex-1}, () => {
                routage.addRoute("map/"+this.state.mapIndex);
              });
            }
          }
        } else {
          this.setState({showModalMap: true, modalMapParameters: {add: message.add, map: message.map}}, () => {
            var modal = new bootstrap.Modal(document.getElementById('modalMap'), {
              keyboard: false
            });
            modal.show();
          });
        }
      }
    });

    messageDispatcher.subscribe('Config', (message) => {
      if (message.lang !== i18next.language) {
        i18next.changeLanguage(message.lang)
        .then(() => {
          this.setState({langChanged: true});
        });
      }
    });

    if (!this.state.config.oidc) {
      this.initModules();
    }

    this.gotoRoute = this.gotoRoute.bind(this);
    this.initModules = this.initModules.bind(this);
    this.refreshData = this.refreshData.bind(this);
    this.cbSaveElement = this.cbSaveElement.bind(this);
    this.cbSaveService = this.cbSaveService.bind(this);
    this.cbSaveScript = this.cbSaveScript.bind(this);
    this.cbCloseMonitor = this.cbCloseMonitor.bind(this);
    this.cbRemoveService = this.cbRemoveService.bind(this);
    this.cbRemoveScript = this.cbRemoveScript.bind(this);
    this.cbRemoveScheduler = this.cbRemoveScheduler.bind(this);
    this.cbRemoveMap = this.cbRemoveMap.bind(this);
    this.refreshLoop = this.refreshLoop.bind(this);
  }

  initModules() {
    return apiManager.APIRequestAngharad("submodule").
    then((submodules) => {
      return this.setState({submodules: submodules}, () => {
        let hasBenoic = false, hasCarleon = false;
        let proms = [];
        submodules.forEach((submodule) => {
          if (submodule.name === "benoic" && submodule.enabled) {
            proms.push(apiManager.APIRequestBenoic("deviceTypes"));
            proms.push(apiManager.APIRequestBenoic("device"));
            hasBenoic = true;
          } else if (submodule.name === "carleon" && submodule.enabled) {
            proms.push(apiManager.APIRequestCarleon("service"));
            hasCarleon = true;
          }
        });
        if (this.state.config.taliesinConfig) {
          apiManager.APIRequestTaliesin("stream")
          .then(result => {
            this.setState({streamList: result});
          })
          .catch(err => {
            messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
          });
        }
        return Promise.all(proms)
        .then((results) => {
          let newState = {};
          if (hasBenoic) {
            newState.deviceTypes = results[0];
            newState.deviceList = results[1];
          }
          if (hasCarleon) {
            if (hasBenoic) {
              newState.serviceList = results[2];
            } else {
              newState.serviceList = results[0];
            }
          }
          if (newState) {
            return this.setState(newState, () => {
              this.refreshData();
              this.refreshLoop();
              this.gotoRoute(routage.getCurrentRoute());
            });
          }
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.modules-error")});
        });
      });
    })
    .catch((err) => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.submodules-error")});
    });
  }
  
  refreshLoop() {
    setTimeout(() => {
      this.refreshData();
      this.refreshLoop();
    }, 300000);
  }

  refreshData() {
    let proms = [];
    proms.push(apiManager.APIRequestAngharad("script")
               .then((results) => {
                 this.setState({script: results});
               })
               .catch((err) => {
                 messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.script-error")});
               }));
    proms.push(apiManager.APIRequestAngharad("scheduler")
               .then((results) => {
                 this.setState({scheduler: results});
               })
               .catch((err) => {
                 messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.scheduler-error")});
               }));
    proms.push(apiManager.APIRequestAngharad("profile")
               .then((results) => {
                 this.setState({profileList: results});
               })
               .catch((err) => {
                 messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.scheduler-error")});
               }));
    this.state.deviceList.forEach(device => {
      proms.push(apiManager.APIRequestBenoic("device/" + encodeURIComponent(device.name) + "/overview")
                 .then((results) => {
                   let deviceOverview = this.state.deviceOverview;
                   deviceOverview[device.name] = results;
                   this.setState({deviceOverview: deviceOverview});
                 })
                 .catch((err) => {
                   messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.device-error", {name: device.name})});
                 }));
    });
    return Promise.all(proms);
  }

  gotoRoute(route) {
    if (route) {
      if (route === "properties" || route.startsWith("scripts") || route.startsWith("schedulers")) {
        this.setState({nav: route});
        routage.addRoute(route);
      } else if (route.startsWith("components")) {
        let path = route.split("/");
        if (path.length === 1) {
          this.setState({nav: "components", components: ["switch", "dimmer", "sensor", "heater", "blind"]});
        } else {
          this.setState({nav: "components", components: path.splice(1)});
        }
        routage.addRoute(route);
      } else if (route.startsWith("services")) {
        let path = route.split("/");
        if (path.length === 1) {
          this.setState({nav: "services", services: ["mock-service", "service-liquidsoap", "service-motion", "service-mpd"]});
        } else {
          this.setState({nav: "services", services: path.splice(1)});
        }
        routage.addRoute(route);
      } else if (route.startsWith("map")) {
        let path = route.split("/");
        if (path.length === 1) {
          this.setState({nav: "map", mapIndex: 0});
        } else {
          this.setState({nav: "map", mapIndex: parseInt(path.splice(1))});
        }
        routage.addRoute(route);
      } else {
        routage.addRoute("");
      }
    } else {
      routage.addRoute("");
    }
  }

  cbSaveElement(result, device, type, name, element) {
    if (result) {
      apiManager.APIRequestBenoic("device/" + encodeURIComponent(device) + "/" + type + "/" + encodeURIComponent(name), "PUT", element)
      .then(() => {
        messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("components.save-ok")});
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalElement'));
    modal.hide();
    this.setState({showModalElement: false, modalElementParameters: {device: false, type: false, name: false, element: false}});
  }
  
  cbCloseMonitor() {
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalMonitor'));
    modal.hide();
    this.setState({showModalMonitor: false, modalElementParameters: {device: false, type: false, name: false, element: false}});
  }

  cbSaveService(result, type, element, add) {
    if (result) {
      if (add) {
        apiManager.APIRequestCarleon(type, "POST", element)
        .then(() => {
          apiManager.APIRequestCarleon("service")
          .then(res => {
            this.setState({serviceList: res}, () => {
              messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("services.save-ok")});
            });
          })
          .catch((err) => {
            messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
          });
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      } else {
        apiManager.APIRequestCarleon(type + "/" + encodeURIComponent(element.name), "PUT", element)
        .then(() => {
          apiManager.APIRequestCarleon("service")
          .then(res => {
            this.setState({serviceList: res}, () => {
              messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("services.save-ok")});
            });
          })
          .catch((err) => {
            messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
          });
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      }
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalService'));
    modal.hide();
    this.setState({showModalService: false, modalServiceParameters: {type: false, element: false, name: false, add: false}});
  }
  
  cbSaveScript(result, script, add) {
    if (result) {
      if (add) {
        apiManager.APIRequestAngharad("script", "POST", script)
        .then(() => {
          apiManager.APIRequestAngharad("script")
          .then(res => {
            this.setState({script: res}, () => {
              messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("scripts.save-ok")});
            });
          });
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      } else {
        apiManager.APIRequestAngharad("script/" + encodeURIComponent(script.name), "PUT", script)
        .then(() => {
          apiManager.APIRequestAngharad("script")
          .then(res => {
            this.setState({script: res}, () => {
              messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("scripts.save-ok")});
            });
          });
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      }
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalScript'));
    modal.hide();
  }
  
  cbSaveScheduler(result, scheduler, add) {
    if (result) {
      if (add) {
        apiManager.APIRequestAngharad("scheduler", "POST", scheduler)
        .then(() => {
          messageDispatcher.sendMessage("Scheduler", {status: "refresh"});
          messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("schedulers.save-ok")});
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      } else {
        apiManager.APIRequestAngharad("scheduler/" + encodeURIComponent(scheduler.name), "PUT", scheduler)
        .then(() => {
          messageDispatcher.sendMessage("Scheduler", {status: "refresh"});
          messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("schedulers.save-ok")});
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      }
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalScheduler'));
    modal.hide();
  }
  
  cbSaveMap(result, map) {
    if (result) {
      apiManager.APIRequestAngharad("profile/" + encodeURIComponent(map.name), "PUT", map)
      .then(() => {
        messageDispatcher.sendMessage("Map", {status: "refresh"});
        messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("maps.save-ok")});
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalMap'));
    modal.hide();
  }

  cbRemoveService(result) {
    if (result) {
      apiManager.APIRequestCarleon(encodeURIComponent(this.state.modalConfirmParameters.type) + "/" + encodeURIComponent(this.state.modalConfirmParameters.element.name), "DELETE")
      .then(() => {
        apiManager.APIRequestCarleon("service")
        .then(res => {
          this.setState({serviceList: res}, () => {
            messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("services.remove-ok")});
          });
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalConfirm'));
    modal.hide();
    this.setState({modalConfirmParameters: {title: false, message: false, cb: false}, showModalConfirm: false});
  }
  
  cbRemoveScript(result) {
    if (result) {
      apiManager.APIRequestAngharad("script/" + encodeURIComponent(this.state.modalConfirmParameters.script.name), "DELETE")
      .then(() => {
        messageDispatcher.sendMessage("Scheduler", {status: "refresh"});
        messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("schedulers.remove-ok")});
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalConfirm'));
    modal.hide();
    this.setState({modalConfirmParameters: {title: false, message: false, cb: false}, showModalConfirm: false});
  }
  
  cbRemoveScheduler(result) {
    if (result) {
      apiManager.APIRequestAngharad("scheduler/" + encodeURIComponent(this.state.modalConfirmParameters.scheduler.name), "DELETE")
      .then(() => {
        apiManager.APIRequestAngharad("scheduler")
        .then(res => {
          this.setState({scheduler: res}, () => {
            messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("schedulers.remove-ok")});
          });
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalConfirm'));
    modal.hide();
    this.setState({modalConfirmParameters: {title: false, message: false, cb: false}, showModalConfirm: false});
  }
  
  cbRemoveMap(result) {
    if (result) {
      apiManager.APIRequestAngharad("profile/" + encodeURIComponent(this.state.modalConfirmParameters.map.name), "DELETE")
      .then(() => {
        apiManager.APIRequestAngharad("profile")
        .then(res => {
          this.setState({profileList: res}, () => {
            messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("maps.remove-ok")});
          });
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalConfirm'));
    modal.hide();
    this.setState({modalConfirmParameters: {title: false, message: false, cb: false}, showModalConfirm: false, mapIndex: 0});
  }

  render() {
    let bodyJsx, modalJsx;
    if (this.state.nav == "properties") {
      bodyJsx = <Config config={this.state.config}
                        submodules={this.state.submodules}
                        deviceTypes={this.state.deviceTypes}
                        deviceList={this.state.deviceList}
                        serviceList={this.state.serviceList}/>
    } else if (this.state.nav == "components") {
      bodyJsx = <Components deviceOverview={this.state.deviceOverview}
                            serviceList={this.state.serviceList}
                            filterComponents={this.state.components}
                            adminMode={this.state.adminMode}/>
    } else if (this.state.nav == "services") {
      bodyJsx = <Services serviceList={this.state.serviceList}
                          filterServices={this.state.services}
                          adminMode={this.state.adminMode}
                          streamList={this.state.streamList}
                          config={this.state.config}/>
    } else if (this.state.nav == "scripts") {
      bodyJsx = <Scripts script={this.state.script}
                         scheduler={this.state.scheduler}
                         adminMode={this.state.adminMode}
                         config={this.state.config}/>
    } else if (this.state.nav == "schedulers") {
      bodyJsx = <Schedulers script={this.state.script}
                            scheduler={this.state.scheduler}
                            adminMode={this.state.adminMode}
                            config={this.state.config}/>
    } else if (this.state.nav == "map" || !this.state.nav) {
      bodyJsx = <MapContainer config={this.state.config}
                              deviceOverview={this.state.deviceOverview}
                              serviceList={this.state.serviceList}
                              script={this.state.script}
                              scheduler={this.state.scheduler}
                              profileList={this.state.profileList}
                              adminMode={this.state.adminMode}
                              mapIndex={this.state.mapIndex}
                              mapPlace={this.state.mapPlace}/>
    }
    if (this.state.showModalElement) {
      modalJsx = <ModalElement device={this.state.modalElementParameters.device}
                               type={this.state.modalElementParameters.type}
                               name={this.state.modalElementParameters.name}
                               element={this.state.modalElementParameters.element}
                               cb={this.cbSaveElement} />
    } else if (this.state.showModalMonitor) {
      modalJsx = <ModalMonitor device={this.state.modalElementParameters.device}
                               type={this.state.modalElementParameters.type}
                               name={this.state.modalElementParameters.name}
                               element={this.state.modalElementParameters.element}
                               cb={this.cbCloseMonitor} />
    } else if (this.state.showModalService) {
      modalJsx = <ModalService type={this.state.modalServiceParameters.type}
                               name={this.state.modalServiceParameters.name}
                               element={this.state.modalServiceParameters.element}
                               add={this.state.modalServiceParameters.add}
                               serviceList={this.state.serviceList}
                               deviceOverview={this.state.deviceOverview}
                               cb={this.cbSaveService} />
    } else if (this.state.showModalScript) {
      modalJsx = <ModalScript script={this.state.modalScriptParameters.script}
                              add={this.state.modalScriptParameters.add}
                              serviceList={this.state.serviceList}
                              deviceOverview={this.state.deviceOverview}
                              scriptList={this.state.script}
                              cb={this.cbSaveScript} />
    } else if (this.state.showModalScheduler) {
      modalJsx = <ModalScheduler scheduler={this.state.modalSchedulerParameters.scheduler}
                                 add={this.state.modalSchedulerParameters.add}
                                 schedulerList={this.state.scheduler}
                                 scriptList={this.state.script}
                                 cb={this.cbSaveScheduler} />
    } else if (this.state.showModalMap) {
      modalJsx = <ModalMap map={this.state.modalMapParameters.map}
                           add={this.state.modalMapParameters.add}
                           profileList={this.state.profileList}
                           cb={this.cbSaveMap} />
    } else if (this.state.showModalConfirm) {
      modalJsx = <ModalConfirm title={this.state.modalConfirmParameters.title}
                               message={this.state.modalConfirmParameters.message}
                               cb={this.state.modalConfirmParameters.cb} />
    } else if (this.state.showOpenService) {
      modalJsx = <ModalOpenService type={this.state.modalOpenService.type}
                                   element={this.state.modalOpenService.element}
                                   deviceOverview={this.state.deviceOverview}
                                   streamList={this.state.streamList}
                                   config={this.state.config} />
    }
    return (
      <div className="container-fluid">
        <TopMenu
          config={this.state.config}
          oidcStatus={!!this.state.oidcStatus}
          deviceOverview={this.state.deviceOverview}
          serviceList={this.state.serviceList}
          adminMode={this.state.adminMode}/>
        {bodyJsx}
        {modalJsx}
        <ModalDimmer device={this.state.modalDimmerParameters.device}
                             name={this.state.modalDimmerParameters.name}
                             element={this.state.modalDimmerParameters.element} />
        <ModalBlind device={this.state.modalDimmerParameters.device}
                             name={this.state.modalDimmerParameters.name}
                             element={this.state.modalDimmerParameters.element} />
        <Notification loggedIn={this.state.oidcStatus===false||this.state.oidcStatus === "connected"}/>
      </div>
      );
  }
}

export default App;
