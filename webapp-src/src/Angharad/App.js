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
import storage from '../lib/Storage';

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
import ModalSchedulerShow from './ModalSchedulerShow';
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
      mapList: [],
      adminMode: false,
      showDimmer: false,
      showBlind: false,
      showModalElement: false,
      showModalMonitor: false,
      showModalService: false,
      showModalScript: false,
      showModalScheduler: false,
      showModalSchedulerShow: false,
      showModalMap: false,
      showModalConfirm: false,
      showOpenService: false,
      modalDimmerParameters: {device: false, name: false, element: false},
      modalElementParameters: {device: false, type: false, name: false, element: false},
      modalServiceParameters: {type: false, element: false, add: false},
      modalScriptParameters: {add: false, script: {}},
      modalSchedulerParameters: {add: false, scheduler: {}},
      modalSchedulerShowParameters: {scheduler: {}},
      modalMapParameters: {add: false, map: {}},
      modalConfirmParameters: {title: false, message: false, cb: false},
      modalOpenService: {element: false, type: false},
      mapIndex: 0,
      wasConnected: false,
      defaultRoute: "components"
    };

    routage.addChangeRouteCallback((route) => {
      this.gotoRoute(route);
    });

    messageDispatcher.subscribe('OIDC', (message) => {
      if (message.status === "disconnected") {
        if (!this.state.wasConnected && !!storage.getValue("autoConnect")) {
          oidcConnector.connect();
        } else {
          apiManager.setToken(false, 0);
          this.setState({oidcStatus: message.status, submodules: [], deviceTypes: [], deviceList: [], serviceList: [], script: [], scheduler: [], deviceOverview: {}, mapList: []});
          messageDispatcher.sendMessage('Notification', {type: "warning", message: i18next.t("message.disconnected")});
        }
      } else if (message.status === "connected" || message.status === "refresh") {
        var curDate = new Date();
        apiManager.setToken(message.token, (curDate.getTime()/1000)+message.expires_in);
        if (message.status === "connected") {
          messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("message.connected")});
        }
        if (message.status === "refresh") {
          setTimeout(() => {
            this.refreshData();
            messageDispatcher.sendMessage('MpdServiceButton', {status: "refresh"});
            messageDispatcher.sendMessage('WeathermapServiceButton', {status: "refresh"});
          }, 1000);
        }
        var tokenTimeout = false;
        if (oidcConnector.getParameter("responseType").search("code") === -1) {
          clearTimeout(this.state.tokenTimeout);
          tokenTimeout = setTimeout(() => {
            messageDispatcher.sendMessage('Notification', {type: "warning", message: i18next.t("message.sessionTimeout"), autohide: false});
            this.setState({oidcStatus: "timeout"});
          }, (message.expires_in - 120)*1000);
        }
        this.setState({oidcStatus: "connected", tokenTimeout: tokenTimeout});
      } else if (message.status === "profile") {
        if (message.profile == "error") {
          messageDispatcher.sendMessage('Notification', {type: "warning", message: i18next.t("message.profileError")});
          this.setState({oidcStatus: "disconnected", wasConnected: true});
        } else if (message.profile) {
          let sub = message.profile.sub, name = "";
          if (this.state.config.frontend.oidc?.userIdParameter) {
            sub = message.profile[this.state.config.frontend.oidc?.userIdParameter];
          }
          if (this.state.config.frontend.oidc?.nameParameter) {
            name = message.profile[this.state.config.frontend.oidc?.nameParameter];
          }
          this.setState({profile: {sub: sub, name: name}, wasConnected: true}, () => {
            this.gotoRoute(routage.getCurrentRoute());
            this.initModules();
          });
        }
      } else if (message.status === "error") {
        messageDispatcher.sendMessage('Notification', {type: "warning", message: i18next.t("message.connectionError")});
        this.setState({oidcStatus: "disconnected", wasConnected: true});
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
        this.setState({nav: false, mapIndex: 0}, () => {
          this.gotoRoute(this.state.defaultRoute);
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
        this.setState({showModalElement: true,
                       modalElementParameters: {
                         device: message.device,
                         type: message.type,
                         name: message.name,
                         element: message.element
                        }
                      });
      } else if (message.status === "monitor") {
        this.setState({showModalMonitor: true,
                       showOpenService: false,
                       modalElementParameters: {
                         device: message.device,
                         type: message.type,
                         name: message.name,
                         element: message.element
                       }
                    });
      } else if (message.status === "dimmerModal") {
        this.setState({showDimmer: true, showBlind: false, modalDimmerParameters: {device: message.device, name: message.name, element: message.element}});
      } else if (message.status === "blindModal") {
        this.setState({showDimmer: false, showBlind: true, modalDimmerParameters: {device: message.device, name: message.name, element: message.element}});
      } else if (message.status === "refresh") {
        messageDispatcher.sendMessage('Notification', {type: "success", message: <span><i className="fa fa-refresh fa-spin fa-fw elt-left"></i>{i18next.t("message.refreshing")}</span>, autohide: false});
        this.refreshData(true)
        .then((results) => {
          messageDispatcher.sendMessage('Notification', {hideAll: true});
          messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("message.refresh")});
        });
      } else if (message.status === "silentRefresh") {
        if (message.device) {
          this.refreshDevice(message.device);
        } else {
          this.state.deviceList.forEach(device => {
            if (device.enabled && device.connected) {
              this.refreshDevice(device.name);
            }
          });
        }
      }
    });

    messageDispatcher.subscribe('Service', (message) => {
      if (message.remove) {
        this.setState({showModalConfirm: true,
                       modalConfirmParameters: {title: i18next.t("services.remove-title"),
                                                message: i18next.t("services.remove-message", {name: message.element.name}),
                                                cb: this.cbRemoveService,
                                                type: message.type,
                                                element: message.element}});
      } else if (message.status === "open") {
        this.state.serviceList.forEach(service => {
          if (service.name === message.type && service.enabled) {
            service.element.forEach(element => {
              if (element.name === message.element.name) {
                this.setState({showOpenService: true,
                               modalOpenService: {type: message.type, element: element}});
              }
            });
          }
        });
      } else {
        this.setState({showModalService: true,
                       modalServiceParameters: {type: message.type, element: message.element, add: message.add}});
      }
    });

    messageDispatcher.subscribe('Script', (message) => {
      if (message.remove) {
        this.setState({showModalConfirm: true,
                       modalConfirmParameters: {title: i18next.t("scripts.remove-title"),
                                                message: i18next.t("scripts.remove-message", {name: message.script.name}),
                                                cb: this.cbRemoveScript,
                                                type: "script",
                                                script: message.script}});
      } else {
        this.setState({showModalScript: true, modalScriptParameters: {add: message.add, script: message.script}});
      }
    });

    messageDispatcher.subscribe('Scheduler', (message) => {
      if (message.remove) {
        this.setState({showModalConfirm: true,
                       modalConfirmParameters: {title: i18next.t("schedulers.remove-title"),
                                                message: i18next.t("schedulers.remove-message", {name: message.scheduler.name}),
                                                cb: this.cbRemoveScheduler,
                                                type: "scheduler",
                                                scheduler: message.scheduler}});
      } else if (message.show) {
        this.setState({showModalSchedulerShow: true, modalSchedulerShowParameters: {scheduler: message.scheduler}});
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
          this.setState({showModalScheduler: true, modalSchedulerParameters: {add: message.add, scheduler: message.scheduler}});
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
                                                map: message.map}});
      } else {
        if (message.status === "refresh") {
          apiManager.APIRequestAngharad("profile")
         .then((results) => {
            let mapList = [];
            results.forEach((profile, index) => {
              if (!!profile.image && Array.isArray(profile.elements)) {
                mapList.push(profile);
              }
            });
           this.setState({profileList: results, mapList: mapList.sort((a, b) => a.index - b.index)});
         })
         .catch((err) => {
           messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
         });
        } else if (message.status === "next") {
          if (this.state.mapIndex === this.state.mapList.length-1) {
            this.setState({mapIndex: 0}, () => {
              routage.addRoute("map/"+this.state.mapIndex);
            });
          } else {
            this.setState({mapIndex: this.state.mapIndex+1}, () => {
              routage.addRoute("map/"+this.state.mapIndex);
            });
          }
        } else if (message.status === "previous") {
          if (this.state.mapIndex === 0) {
            this.setState({mapIndex: this.state.mapList.length-1}, () => {
              routage.addRoute("map/"+this.state.mapIndex);
            });
          } else {
            this.setState({mapIndex: this.state.mapIndex-1}, () => {
              routage.addRoute("map/"+this.state.mapIndex);
            });
          }
        } else if (message.status === "select") {
          if (message.index >= 0 && message.index < this.state.mapList.length) {
            this.setState({mapIndex: message.index}, () => {
              routage.addRoute("map/"+this.state.mapIndex);
            });
          } else {
            this.setState({mapIndex: 0}, () => {
              routage.addRoute("map/"+this.state.mapIndex);
            });
          }
        } else {
          this.setState({showModalMap: true, modalMapParameters: {add: message.add, map: message.map}});
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
      this.state.oidcStatus = "connected";
      this.initModules();
    }

    this.gotoRoute = this.gotoRoute.bind(this);
    this.initModules = this.initModules.bind(this);
    this.refreshData = this.refreshData.bind(this);
    this.refreshDevice = this.refreshDevice.bind(this);
    this.cbSaveElement = this.cbSaveElement.bind(this);
    this.cbSaveService = this.cbSaveService.bind(this);
    this.cbSaveScript = this.cbSaveScript.bind(this);
    this.cbCloseMonitor = this.cbCloseMonitor.bind(this);
    this.cbRemoveService = this.cbRemoveService.bind(this);
    this.cbRemoveScript = this.cbRemoveScript.bind(this);
    this.cbRemoveScheduler = this.cbRemoveScheduler.bind(this);
    this.cbRemoveMap = this.cbRemoveMap.bind(this);
    this.refreshLoop = this.refreshLoop.bind(this);

    this.hideBlind = this.hideBlind.bind(this);
    this.hideDimmer = this.hideDimmer.bind(this);
    this.hideConfirm = this.hideConfirm.bind(this);
    this.hideElement = this.hideElement.bind(this);
    this.hideMonitor = this.hideMonitor.bind(this);
    this.hideService = this.hideService.bind(this);
    this.hideScript = this.hideScript.bind(this);
    this.hideScheduler = this.hideScheduler.bind(this);
    this.hideSchedulerShow = this.hideSchedulerShow.bind(this);
    this.hideMap = this.hideMap.bind(this);
    this.hideOpenService = this.hideOpenService.bind(this);
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
              this.refreshData()
              .always(() => {
                this.refreshLoop();
                let defaultRoute = "components";
                if (this.state.mapList.length) {
                  defaultRoute = "map";
                }
                this.setState({defaultRoute: defaultRoute}, () => {
                  this.gotoRoute(routage.getCurrentRoute()||storage.getValue("configRoute")||this.state.defaultRoute);
                });
              });
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
      if (this.state.oidcStatus === "connected") {
        this.refreshData();
        this.refreshLoop();
      }
    }, 300000);
  }

  refreshData(fromButton=false) {
    let promsDevice = [];
    return apiManager.APIRequestAngharad("profile")
    .then((results) => {
      let mapList = [];
      results.forEach((profile, index) => {
        if (!!profile.image && Array.isArray(profile.elements)) {
          mapList.push(profile);
        }
      });
      this.setState({profileList: results, mapList: mapList.sort((a, b) => a.index - b.index)}, () => {
        this.state.deviceList.forEach(device => {
          if (device.enabled && device.connected) {
            promsDevice.push(this.refreshDevice(device.name));
          }
        });
        return Promise.all(promsDevice)
        .then(() => {
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
          return Promise.all(proms);
        });
      });
    })
    .catch((err) => {
      if (err.status !== 401 || fromButton) {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.profile-error")});
      }
    });
  }

  refreshDevice(device) {
    return apiManager.APIRequestBenoic("device/" + encodeURIComponent(device) + "/overview")
    .then((results) => {
      let deviceOverview = this.state.deviceOverview;
      deviceOverview[device] = results;
      this.setState({deviceOverview: deviceOverview});
    })
    .catch((err) => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.device-error", {name: device})});
    })
  }

  gotoRoute(route) {
    if (route) {
      if (route === "properties") {
        this.setState({nav: route});
        routage.addRoute(route);
      } else if (route.startsWith("scripts") || route.startsWith("schedulers")) {
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
          this.setState({nav: "services", services: ["mock-service", "service-liquidsoap", "service-motion", "service-mpd", "service-weathermap"]});
        } else {
          this.setState({nav: "services", services: path.splice(1)});
        }
        routage.addRoute(route);
      } else if (route.startsWith("map")) {
        let path = route.split("/");
        if (path.length === 1) {
          this.setState({nav: "map", mapIndex: 0});
        } else {
          let index = parseInt(path.splice(1));
          if (index < 0 || index >= this.state.mapList.length) {
            index = 0;
          }
          this.setState({nav: "map", mapIndex: index});
        }
        routage.addRoute(route);
      }
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
    this.setState({showModalElement: false, modalElementParameters: {device: false, type: false, name: false, element: false}});
  }

  cbCloseMonitor() {
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
    this.setState({modalConfirmParameters: {title: false, message: false, cb: false}, showModalConfirm: false});
  }

  cbRemoveMap(result) {
    if (result) {
      apiManager.APIRequestAngharad("profile/" + encodeURIComponent(this.state.modalConfirmParameters.map.name), "DELETE")
      .then(() => {
        apiManager.APIRequestAngharad("profile")
        .then(res => {
          let mapList = [];
          results.forEach((profile, index) => {
            if (!!profile.image && Array.isArray(profile.elements)) {
              mapList.push(profile);
            }
          });
          this.setState({profileList: res, mapList: mapList.sort((a, b) => a.index - b.index)}, () => {
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
    this.setState({modalConfirmParameters: {title: false, message: false, cb: false}, showModalConfirm: false, mapIndex: 0});
  }

  hideBlind() {
    this.setState({showBlind: false});
  }

  hideDimmer() {
    this.setState({showDimmer: false});
  }

  hideConfirm() {
    this.setState({showModalConfirm: false});
  }

  hideElement() {
    this.setState({showModalElement: false});
  }

  hideMonitor() {
    this.setState({showModalMonitor: false});
  }

  hideService() {
    this.setState({showModalService: false});
  }

  hideScript() {
    this.setState({showModalScript: false});
  }

  hideScheduler() {
    this.setState({showModalScheduler: false});
  }

  hideSchedulerShow() {
    this.setState({showModalSchedulerShow: false});
  }

  hideMap() {
    this.setState({showModalMap: false});
  }

  hideOpenService() {
    this.setState({showOpenService: false});
  }

  render() {
    let bodyJsx;
    if (this.state.nav == "properties") {
      bodyJsx = <Config config={this.state.config}
                        adminMode={this.state.adminMode}
                        submodules={this.state.submodules}
                        deviceTypes={this.state.deviceTypes}
                        deviceList={this.state.deviceList}
                        serviceList={this.state.serviceList}
                        mapList={this.state.mapList}/>
    } else if (this.state.nav == "components") {
      bodyJsx = <Components deviceOverview={this.state.deviceOverview}
                            serviceList={this.state.serviceList}
                            filterComponents={this.state.components}
                            adminMode={this.state.adminMode}/>
    } else if (this.state.nav == "services") {
      bodyJsx = <Services serviceList={this.state.serviceList}
                          filterServices={this.state.services}
                          adminMode={this.state.adminMode}
                          deviceOverview={this.state.deviceOverview}
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
    } else if (this.state.nav == "map") {
      bodyJsx = <MapContainer config={this.state.config}
                              deviceOverview={this.state.deviceOverview}
                              serviceList={this.state.serviceList}
                              script={this.state.script}
                              scheduler={this.state.scheduler}
                              mapList={this.state.mapList}
                              adminMode={this.state.adminMode}
                              mapIndex={this.state.mapIndex} />
    } else {
      bodyJsx = 
      <div className="text-center">
        <img className="img-fluid" src="favicon.ico"/>
      </div>
    }

    return (
      <div className="container-fluid">
        <TopMenu
          config={this.state.config}
          oidcStatus={this.state.oidcStatus}
          oidc={!!this.state.config.oidc}
          deviceOverview={this.state.deviceOverview}
          serviceList={this.state.serviceList}
          adminMode={this.state.adminMode}
          defaultRoute={this.state.defaultRoute} />
        {bodyJsx}
        {this.state.showModalConfirm ? <ModalConfirm handleHideModal={this.hideConfirm}
                                       title={this.state.modalConfirmParameters.title}
                                       message={this.state.modalConfirmParameters.message}
                                       cb={this.state.modalConfirmParameters.cb} /> : null}
        {this.state.showDimmer ? <ModalDimmer handleHideModal={this.hideDimmer}
                                  device={this.state.modalDimmerParameters.device}
                                  name={this.state.modalDimmerParameters.name}
                                  element={this.state.modalDimmerParameters.element} /> : null}
        {this.state.showBlind ? <ModalBlind handleHideModal={this.hideBlind}
                                device={this.state.modalDimmerParameters.device}
                                name={this.state.modalDimmerParameters.name}
                                element={this.state.modalDimmerParameters.element}
                                deviceOverview={this.state.deviceOverview} /> : null}
        {this.state.showModalElement ? <ModalElement handleHideModal={this.hideElement}
                                        device={this.state.modalElementParameters.device}
                                        type={this.state.modalElementParameters.type}
                                        name={this.state.modalElementParameters.name}
                                        element={this.state.modalElementParameters.element}
                                        deviceOverview={this.state.deviceOverview}
                                        cb={this.cbSaveElement} /> : null}
        {this.state.showModalMonitor ? <ModalMonitor handleHideModal={this.hideMonitor}
                                        device={this.state.modalElementParameters.device}
                                        type={this.state.modalElementParameters.type}
                                        name={this.state.modalElementParameters.name}
                                        element={this.state.modalElementParameters.element}
                                        cb={this.cbCloseMonitor} /> : null}
        {this.state.showModalService ? <ModalService handleHideModal={this.hideService}
                                        type={this.state.modalServiceParameters.type}
                                        name={this.state.modalServiceParameters.name}
                                        element={this.state.modalServiceParameters.element}
                                        add={this.state.modalServiceParameters.add}
                                        serviceList={this.state.serviceList}
                                        deviceOverview={this.state.deviceOverview}
                                        cb={this.cbSaveService} /> : null}
        {this.state.showModalScript ? <ModalScript handleHideModal={this.hideScript}
                                      script={this.state.modalScriptParameters.script}
                                      add={this.state.modalScriptParameters.add}
                                      serviceList={this.state.serviceList}
                                      deviceOverview={this.state.deviceOverview}
                                      scriptList={this.state.script}
                                      cb={this.cbSaveScript} /> : null}
        {this.state.showModalScheduler ? <ModalScheduler handleHideModal={this.hideScheduler}
                                          scheduler={this.state.modalSchedulerParameters.scheduler}
                                          add={this.state.modalSchedulerParameters.add}
                                          schedulerList={this.state.scheduler}
                                          scriptList={this.state.script}
                                          cb={this.cbSaveScheduler} /> : null}
        {this.state.showModalSchedulerShow ? <ModalSchedulerShow handleHideModal={this.hideSchedulerShow}
                                              scheduler={this.state.modalSchedulerShowParameters.scheduler} /> : null}
        {this.state.showModalMap ? <ModalMap handleHideModal={this.hideMap}
                                    map={this.state.modalMapParameters.map}
                                    add={this.state.modalMapParameters.add}
                                    mapList={this.state.mapList}
                                    cb={this.cbSaveMap} /> : null}
        {this.state.showOpenService ? <ModalOpenService handleHideModal={this.hideOpenService}
                                      type={this.state.modalOpenService.type}
                                      element={this.state.modalOpenService.element}
                                      deviceOverview={this.state.deviceOverview}
                                      streamList={this.state.streamList}
                                      config={this.state.config} /> : null}
        <Notification loggedIn={this.state.oidcStatus===false||this.state.oidcStatus === "connected"}/>
      </div>
      );
  }
}

export default App;
