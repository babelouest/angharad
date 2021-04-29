/**
 * 
 * action format:
 * 
 * {
 *   "submodule": "submodule_name",   // Required
 *   "element": "element_name"        // Required
 *   "command": "command"             // Required
 *   "parameters": {                  // Required for submodule benoic, depends on the element for submodule carleon
 *     "device": "device_name"        // Required for submodule benoic
 *     "element_type": "element_type" // Required for submodule benoic
 *     "service": "service_name"      // Required for carleon
 *     "param1": "value1",            // for a string value
 *     "param2": 2,                   // for an integer value
 *     "param3", 3.3                  // for a real value
 *   }
 * 
 */
angular.module("sagremorApp")
    .controller("ScriptModalCtrl",
    function($scope, $rootScope, $uibModalInstance, $translate, toaster, sagremorConstant, sagremorService, angharadFactory, sharedData, sagGenericInjectorManager, script) {
        var self = this;
        
        this.script = script;
        this.add = false;
        this.newAction = false;
        this.newActionType = false;
        this.tmpElement = {};
        this.scriptActionElements = angular.copy(sagremorConstant.scriptActionElements);
        this.benoicElements = {
					switches: [],
					dimmers: [],
					heaters: []
				}
				this.error = {
					message: ""
				};
				this.carleonConditionCommandsParameters = {};
				this.carleonConditionElementsList = {};
        
        function init() {
					if (!self.script) {
						self.add = true;
						self.script = {
							actions: [],
							options: {}
						};
					}
					_.forEach(sharedData.all("benoicDevices"), function (device) {
						if (device.enabled && device.connected) {
							_.forEach(device.element.switches, function(element, name) {
								var elt = {
									device: device.name,
									type: "switch",
									name: name,
									display: element.display,
									value: true
								}
								self.benoicElements.switches.push(elt);
							});
							_.forEach(device.element.dimmers, function(element, name) {
								var elt = {
									device: device.name,
									type: "dimmer",
									name: name,
									display: element.display,
									value: 0
								}
								self.benoicElements.dimmers.push(elt);
							});
							_.forEach(device.element.heaters, function(element, name) {
								var elt = {
									device: device.name,
									type: "heater",
									name: name,
									display: element.display,
									value: 20,
									availableModes: element.value.availableModes
								}
								self.benoicElements.heaters.push(elt);
							});
						}
					});
					
					_.forEach(sharedData.all("carleonServices"), function (service, serviceName) {
						var injector = _.find(sagGenericInjectorManager.components, function (inject) {
							return inject.type === serviceName && service.enabled;
						});
						if (!!injector) {
							_.forEach(injector.commands, function (command, commandName) {
								var newAction = {label: command.title, name: serviceName + "$" + commandName, submodule: "carleon"};
								self.carleonConditionElementsList[serviceName + "$" + commandName] = service.element;
								self.scriptActionElements.push(newAction);
								if (!!service.commands && !!service.commands[commandName]) {
									self.carleonConditionCommandsParameters[serviceName + "$" + commandName] = [];
									_.forEach(service.commands[commandName].parameters, function (serviceParameter, serviceParameterName) {
										var commandParameter = {
											name: serviceParameterName,
											title: !!command.parameters[serviceParameterName]?$translate.instant(command.parameters[serviceParameterName]):serviceParameterName,
											type: serviceParameter.type,
											required: serviceParameter.required
										};
										self.carleonConditionCommandsParameters[serviceName + "$" + commandName].push(commandParameter);
									});
								}
							});
						}
					});
			
        }
        
        this.setActionBenoicElement = function () {
					self.newAction.element = self.tmpElement.name;
					self.newAction.parameters.device = self.tmpElement.device;
					self.newAction.parameters.element_type = self.tmpElement.type;
				};
        
        this.setActionCarleonElement = function () {
					self.newAction.element = self.tmpElement.name;
				};
        
        this.trackBenoicElement = function(element, type) {
					if (!!element && !!type) {
						return element.device + "$" + type + "$" + element.name;
					} else {
						return "err$err$err";
					}
				};
		
				this.trackCarleonElement = function (element, action) {
					if (!!element && !!action) {
						return action + "$" + element.name;
					} else {
						return "err$err$err";
					}
				};
        
        this.addAction = function () {
					self.newAction = {};
				};
		
				this.changeActionType = function () {
					self.newAction = {
						submodule: "",
						element: "",
						parameters: {
						}
					};
					switch (self.newActionType.name) {
						case "switch":
							self.newAction.submodule = "benoic";
							self.newAction.command = 1;
							self.newAction.parameters.element_type = "switch";
							break;
						case "dimmer":
							self.newAction.submodule = "benoic";
							self.newAction.command = 0;
							self.newAction.parameters.element_type = "dimmer";
							break;
						case "heater":
							self.newAction.submodule = "benoic";
							self.newAction.command = 20;
							self.newAction.parameters.element_type = "heater";
							self.newAction.parameters.mode = "auto";
							break;
						default:
							var newActionTypeSplitted = self.newActionType.name.split("$");
							self.newAction.parameters.service = newActionTypeSplitted[0];
							self.newAction.command = newActionTypeSplitted[1];
							self.newAction.submodule = "carleon";
							break;
					};
				}
				
				this.removeAction = function ($index) {
					self.script.actions.splice($index, 1);
				};
		
        this.cancel = function () {
            $uibModalInstance.dismiss("cancel");
        };
        
        this.tr = function (id) {
					return $translate.instant(id);
				};

				this.heaterModeDisplay = function (value) {
					return $translate.instant("heater_mode_" + value);
				};
    
        this.save = function () {
					if (self.add) {
						angharadFactory.addScript(self.script).then(function () {
							toaster.pop("success", $translate.instant('script_save'), $translate.instant('script_save_success'));
							sharedData.add("angharadScripts", self.script.name, self.script);
							$rootScope.$broadcast("angharadScriptsChanged");
						}, function (error) {
							toaster.pop("error", $translate.instant('script_save'), $translate.instant('script_save_error'));
						})["finally"](function () {
							$uibModalInstance.dismiss("close");
						});
					} else {
						angharadFactory.setScript(self.script.name, self.script).then(function () {
							toaster.pop("success", $translate.instant('script_save'), $translate.instant('script_save_success'));
							sharedData.add("angharadScripts", self.script.name, self.script);
							$rootScope.$broadcast("angharadScriptsChanged");
						}, function (error) {
							toaster.pop("error", $translate.instant('script_save'), $translate.instant('script_save_error'));
						})["finally"](function () {
							$uibModalInstance.dismiss("close");
						});
					}
        };
        
        this.getBenoicElementDisplay = function (action) {
					var element = sagremorService.getBenoicElement(action.parameters.device, action.parameters.element_type, action.element);
					if (!!element) {
						return element.display;
					} else {
						return $translate.instant("not_found");
					}
				};
		
        this.getCarleonElementDisplay = function (action) {
					var element = sagremorService.getCarleonElement(action.parameters.service, action.element);
					if (!!element) {
						return element.name;
					} else {
						return $translate.instant("not_found");
					}
				};
		
				this.getCarleonElementParameters = function (action) {
					var toReturn = [];
					_.forEach(action.parameters, function (parameter, key) {
						if (key !== "service") {
							toReturn.push({name: key, value: parameter});
						}
					});
					return toReturn;
				};
		
				this.getBenoicElementValue = function (action) {
					switch (action.parameters.element_type) {
						case "switch":
							return action.command===1?$translate.instant("switch_on"):$translate.instant("switch_off");
							break;
						case "dimmer":
							return action.command + " %";
							break;
						case "heater":
							var heater = sagremorService.getBenoicElement(action.parameters.device, "heater", action.element);
							if (!!heater) {
								var unit = heater.options.unit||"";
								return action.command + " " + unit + " (" + action.parameters.mode + ")";
							} else {
								return "error";
							}
							break;
					}
				};
        
        this.cancelAddAction = function () {
					self.newAction = false;
				};
		
				this.saveNewAction = function () {
					self.script.actions.push(self.newAction);
					self.newActionType = false;
				};
				
				this.isScriptValid = function () {
					if (!self.script.name) {
						return false;
					}
					var found = _.find(sharedData.all("angharadScripts"), function (script) {
						return script.name === self.script.name;
					});
					if (!!found && self.add) {
						self.error.message = $translate.instant("script_error_name_exist");
						return false;
					}
					if (self.script.actions.length === 0) {
						return false;
					}
					
					return true;
				};
				
				this.isActionValid = function () {
					if (!self.newActionType) {
						return false;
					}
					
					if (!self.newAction.element) {
						return false;
					}
					
					if (self.newActionType.submodule === "carleon") {
						if (!self.tmpElement) {
							return false;
						}
						
						var ret = true;
						_.forEach(self.carleonConditionCommandsParameters[self.newActionType.name], function (parameter, parameterName) {
							if (parameter.required && !self.newAction.parameters[parameter.name]) {
								ret = false;
							}
						});
						if (!ret) {
							return false;
						}
						
					}
					
					return true;
				};
		
				this.carleonCommand = function(command, service) {
					var curCommand = _.find(self.scriptActionElements, function (action) {
						return action.name === service + "$" + command;
					});
					return curCommand?$translate.instant(curCommand.label):"not found";
				};
		
				this.carleonParameter = function(parameter, service, command) {
					if (!!self.carleonConditionCommandsParameters) {
						var curParameter = _.find(self.carleonConditionCommandsParameters[service + "$" + command], function (param) {
							return param.name === parameter;
						});
						return curParameter?curParameter.title:"not found";
					}
				};
        
        init();
    });
