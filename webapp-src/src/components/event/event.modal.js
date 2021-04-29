angular.module("sagremorApp")
    .controller("EventModalCtrl",
    function($scope, $rootScope, $uibModalInstance, $translate, toaster, sagremorConstant, sagremorService, angharadFactory, sharedData, sagGenericInjectorManager, event, eventType) {
        var self = this;
        
        this.add = true;
        this.error = {
			nameInvalid: true,
			message: ""
		};
        this.globalScriptList = angular.copy(sharedData.all("angharadScripts"));
        this.globalElementList = angular.copy(sagremorConstant.conditionList);
        this.globalTriggerElementList = [];
        this.schedulerRepeatEveryEnum = sagremorConstant.schedulerRepeatEveryEnum;
        this.event = event||{
			name: "",
			description: "",
			enabled: true
		};
        this.scheduler = {
			repeatSwitch: false,
			repeat: -1,
			repeat_value: 0,
			remove_after: false,
			repeatEveryDayOfWeek: {
				sunday: false,
				monday: false,
				tuesday: false,
				wednesday: false,
				thursday: false,
				friday: false,
				saturday: false
			}
		};
        this.trigger = {
			submodule: false,
			source: false,
			element: false
		};
        this.scriptList = [];
        this.conditionList = [];
        this.myDate = new Date();
        this.benoicElements = {
			switches: [],
			dimmers: [],
			heaters: [],
			sensors: []
		}
		this.carleonConditionElementsList = {};
		this.carleonConditionResultParameters = {};
		this.carleonConditionCommandsParameters = {};
		this.scriptActionElements = [];
		this.tmpElement = false;
		this.tmpResult = false;
		this.newCondition = {
			condition: "==",
			value: 0,
			parameters: {}
		};
		this.eventType = eventType;
		
		this.newTrigger = {};
		this.newTriggerTag = {};
        
        this.datePickerOptions = {
			buttonBar: {
				show: true,
				now: {
					show: false,
					text: $translate.instant("date_picker_now")
				},
				today: {
					show: true,
					text: $translate.instant("date_picker_today")
				},
				clear: {
					show: false,
					text: $translate.instant("date_picker_clear")
				},
				date: {
					show: true,
					text: $translate.instant("date_picker_date")
				},
				time: {
					show: true,
					text: $translate.instant("date_picker_time")
				},
				close: {
					show: true,
					text: $translate.instant("date_picker_close")
				}
			}
		};
        
        function init() {
			if (!!event) {
				self.add = false;
				if (self.eventType === "scheduler") {
					self.scheduler = angular.copy(event);
					self.scheduler.repeatSwitch = (event.repeat > -1);
					self.scheduler.repeatEveryDayOfWeek = {
						sunday: false,
						monday: false,
						tuesday: false,
						wednesday: false,
						thursday: false,
						friday: false,
						saturday: false
					};
					if (event.repeat === 3) {
						self.scheduler.repeatEveryDayOfWeek = {
							sunday: ((event.repeat_value & 1) !== 0),
							monday: ((event.repeat_value & 2) !== 0),
							tuesday: ((event.repeat_value & 4) !== 0),
							wednesday: ((event.repeat_value & 8) !== 0),
							thursday: ((event.repeat_value & 16) !== 0),
							friday: ((event.repeat_value & 32) !== 0),
							saturday: ((event.repeat_value & 64) !== 0)
						};
					}
					self.myDate = new Date(event.next_time * 1000);
				} else if (self.eventType === "trigger") {
					self.trigger.submodule = event.submodule;
					self.trigger.source = event.source;
					self.trigger.element = event.element;
				}
				self.scriptList = event.scripts;
				self.conditionList = event.conditions;
			}
			
			_.forEach(sharedData.all("benoicDevices"), function (device) {
				if (device.enabled && device.connected) {
					_.forEach(device.element.switches, function(element, name) {
						if (element.options.trigger) {
							self.globalTriggerElementList.push({submodule: "benoic", source: device.name, element: name, display: element.display});
						}
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
						if (element.options.trigger) {
							self.globalTriggerElementList.push({submodule: "benoic", source: device.name, element: name, display: element.display});
						}
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
						if (element.options.trigger) {
							self.globalTriggerElementList.push({submodule: "benoic", source: device.name, element: name, display: element.display});
						}
						var elt = {
							device: device.name,
							type: "heater",
							name: name,
							display: element.display,
							value: 20,
							options: element.options,
							availableModes: element.value.availableModes,
						}
						self.benoicElements.heaters.push(elt);
					});
					_.forEach(device.element.sensors, function(element, name) {
						if (element.options.trigger) {
							self.globalTriggerElementList.push({submodule: "benoic", source: device.name, element: name, display: element.display});
						}
						var elt = {
							device: device.name,
							type: "sensor",
							name: name,
							display: element.display,
							value: 0,
							options: element.options
						}
						self.benoicElements.sensors.push(elt);
					});
				}
			});
			
			_.forEach(sharedData.all("carleonServices"), function (service, serviceName) {
				var injector = _.find(sagGenericInjectorManager.components, function (inject) {
					return inject.type === serviceName && service.enabled;
				});
				_.forEach(service.element, function (element) {
					if (element.options && element.options.trigger) {
						self.globalTriggerElementList.push({submodule: "carleon", source: service.name, element: element.name, display: element.name});
					}
				});
				if (!!injector) {
					_.forEach(injector.commands, function (command, commandName) {
						var newAction = {label: command.title, name: serviceName + "$" + commandName, submodule: "carleon"};
						self.globalElementList.push(newAction);
						self.carleonConditionElementsList[serviceName + "$" + commandName] = service.element;
						if (!!service.commands[commandName]) {
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
							self.carleonConditionResultParameters[serviceName + "$" + commandName] = [];
							_.forEach(service.commands[commandName].result, function (serviceResult, serviceResultName) {
								if (serviceResultName === "type") {
									var commandResult = {
										title: $translate.instant("condition_result_one_value"),
										type: serviceResult
									}
									self.carleonConditionResultParameters[serviceName + "$" + commandName].push(commandResult);
								} else {
									var commandResult = {
										name: serviceResultName,
										title: $translate.instant(command.result[serviceResultName].title),
										type: serviceResult.type
									}
									self.carleonConditionResultParameters[serviceName + "$" + commandName].push(commandResult);
								}
							});
						}
					});
				}
			});

			_.forEach(sharedData.all("carleonServices"), function (service, serviceName) {
				var injector = _.find(sagGenericInjectorManager.components, function (inject) {
					return inject.type === serviceName && service.enabled;
				});
				if (!!injector) {
					_.forEach(injector.results, function (result, resultName) {
						if (!!result.type && !!result.title) {
							var newCondition = {label: result.title, name: serviceName + "$" + resultName, submodule: "carleon", type: result.type};
							self.globalElementList.push(newCondition);
							self.carleonConditionElementsList[serviceName + "$" + resultName] = service.element;
						} else {
							_.forEach(result, function (curResult, curResultName) {
								var newCondition = {label: curResult.title, name: serviceName + "$" + curResultName, submodule: "carleon", type: curResult.type, field: curResultName};
								self.globalElementList.push(newCondition);
								self.carleonConditionElementsList[serviceName + "$" + curResultName] = service.element;
							});
						}
					});
				}
			});
        }
        
		this.openCalendar = function(e) {
			e.preventDefault();
			e.stopPropagation();

			self.isOpen = true;
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
		
		this.trackTriggerElement = function (element) {
			return element.submodule + "$" + element.source + "$" + element.element;
		};
        
        this.tr = function (id) {
			return $translate.instant(id);
		};
		
		this.isConditionValid = function () {
			if (!!self.newConditionType && self.newConditionType.submodule === "benoic") {
				switch (self.newConditionType.name) {
					case "switch":
					case "dimmer":
					case "sensor":
						return !!self.tmpElement;
						break;
					case "heater":
						if (!self.tmpElement) {
							return false;
						} else {
							return !!self.newCondition.condition && (!!self.newCondition.commandMode || !!self.newCondition.commandHeat);
						}
				}
			} else if (!!self.newConditionType && self.newConditionType.submodule === "carleon") {
				if (!self.newCondition.condition || (!self.newCondition.value && self.newCondition.value !== 0) || !self.tmpElement) {
					return false;
				}
				
				var res = true;
				_.forEach(self.carleonConditionCommandsParameters[self.newConditionType.name], function (parameter) {
					if (parameter.required && (!self.newCondition.parameters[parameter.name] && self.newCondition.parameters[parameter.name] !== 0)) {
						res = false;
					}
				});
				
				return res;
			}
			return true;
		};
		
		this.heaterModeDisplay = function (value) {
			return $translate.instant("heater_mode_" + value);
		};
    
        this.setConditionBenoicElement = function () {
			self.newCondition.submodule = "benoic";
			self.newCondition.element = self.tmpElement.name;
			self.newCondition.parameters.device = self.tmpElement.device;
			self.newCondition.parameters.element_type = self.tmpElement.type;
		};
        
		this.addCondition = function () {
			if (self.newConditionType.submodule === "benoic" && self.newCondition.commandMode) {
				self.newCondition.field = "mode";
				self.newCondition.value = self.newCondition.commandMode;
			}
			if (self.newConditionType.submodule === "benoic" && self.newCondition.commandHeat) {
				self.newCondition.field = "command";
				self.newCondition.value = self.newCondition.commandHeat;
			}
			if (self.newConditionType.submodule === "carleon") {
				var nameSplitted = self.newConditionType.name.split("$");
				self.newCondition.field = self.newConditionType.field;
				self.newCondition.submodule = "carleon";
				self.newCondition.element = self.tmpElement.name;
				self.newCondition.parameters.service = self.tmpElement.type
				self.newCondition.service = nameSplitted[0];
				self.newCondition.command = nameSplitted[1];
				if (!!self.tmpResult.name) {
					self.newCondition.field = self.tmpResult.name;
				}
			}
			
			self.conditionList.push(self.newCondition);
			self.newCondition = {
				condition: "==",
				value: 0,
				parameters: {}
			};
		};
		
		this.removeCondition = function (index) {
			self.conditionList.splice(index, 1);
		};
		
		this.displayCondition = function (condition) {
			if (!!condition && condition.submodule === "benoic") {
				switch (condition.parameters.element_type) {
					case "switch":
						var curSwitch = _.find(self.benoicElements.switches, function (element) {
							return condition.parameters.device === element.device && condition.element === element.name;
						});
						return !!curSwitch?$translate.instant("switch_title")+" "+curSwitch.display+" "+(condition.value?$translate.instant("condition_switch_on"):$translate.instant("condition_switch_off")):$translate.instant("switch_title")+" "+$translate.instant("not_found");
						break;
					case "dimmer":
						var curDimmer = _.find(self.benoicElements.dimmers, function (element) {
							return condition.parameters.device === element.device && condition.element === element.name;
						});
						return !!curDimmer?$translate.instant("dimmer_title")+" "+curDimmer.display+" "+condition.condition+" "+condition.value:$translate.instant("dimmer_title")+" "+$translate.instant("not_found");
						break;
					case "heater":
						var curHeater = _.find(self.benoicElements.heaters, function (element) {
							return condition.parameters.device === element.device && condition.element === element.name;
						});
						if (!!curHeater) {
							if (condition.field === "command") {
								return $translate.instant("heater_title")+" "+curHeater.display+" "+condition.condition+" "+condition.value;
							} else if (condition.field === "mode") {
								return $translate.instant("heater_title")+" "+curHeater.display+" "+condition.condition+" "+$translate.instant("heater_mode_"+condition.value);
							}
						} else {
							return $translate.instant("not_found");
						}
						break;
					case "sensor":
						var curSensor = _.find(self.benoicElements.sensors, function (element) {
							return condition.parameters.device === element.device && condition.element === element.name;
						});
						return !!curSensor?$translate.instant("sensor_title")+" "+curSensor.display+" "+condition.condition+" "+condition.value:$translate.instant("sensor_title")+" "+$translate.instant("not_found");
						break;
				}
			} else if (!!condition && condition.submodule === "carleon") {
				var curCondition = _.find(self.globalElementList, function (cond) {
					return cond.name === condition.service + "$" + condition.command;
				});
				var fieldName = $translate.instant("condition_result_one_value");
				if (!!condition.field) {
					var curResult = _.find(self.carleonConditionResultParameters[curCondition.name], function (res) {
						return res.name === condition.field;
					});
					fieldName = !!curResult?curResult.title:$translate.instant("condition_result_one_value");
				}
				return !!curCondition?$translate.instant(curCondition.label)+" "+fieldName+" "+condition.condition+" "+condition.value:$translate.instant("not_found");
			}
		};
		
		this.addScript = function(script) {
			self.scriptList.push({name: script.name, enabled: true});
			delete self.globalScriptList[script.name];
		};
		
		this.removeScript = function (index) {
			var script = self.scriptList[index];
			self.globalScriptList[script.name] = angular.copy(sharedData.get("angharadScripts", script.name));
			self.scriptList.splice(index, 1);
		};
		
		this.isEventValid = function () {
			if (!self.event.name) {
				self.error.message = $translate.instant("event_name_mandatory");
				return false;
			}
			
			if (self.add) {
				if (self.eventType === "scheduler") {
					if (!!sharedData.get("angharadSchedulers", self.event.name)) {
						self.error.message = $translate.instant("event_name_already_exist");
						return false;
					}
				} else if (self.eventType === "trigger") {
					if (!!sharedData.get("angharadTriggers", self.event.name)) {
						self.error.message = $translate.instant("event_name_already_exist");
						return false;
					}
				}
			}
			
			self.error.nameInvalid = false;
			
			if (!self.eventType) {
				self.error.message = $translate.instant("event_type_mandatory");
				return false;
			}
			
			if (self.eventType === "scheduler") {
				if (!self.myDate) {
					self.error.message = $translate.instant("event_scheduler_date_mandatory");
					return false;
				} else if (self.myDate < new Date() && !self.event.repeat) {
					self.error.message = $translate.instant("event_scheduler_date_future");
					return false;
				}
			} else if (self.eventType === "trigger") {
				if (!self.trigger.submodule || !self.trigger.source || !self.trigger.element) {
					self.error.message = $translate.instant("event_trigger_no_element");
					return false;
				}
			}
			
			if (self.scriptList.length === 0) {
				self.error.message = $translate.instant("event_script_mandatory");
				return false;
			}
			
			self.error.message = "";
			return true;
		};

        this.save = function () {
			if (self.eventType === "scheduler") {
				self.scheduler.name = self.event.name;
				self.scheduler.description = self.event.description;
				self.scheduler.enabled = self.event.enabled;
				self.scheduler.next_time = Math.round(self.myDate.getTime() / 1000);
				if (self.scheduler.repeat === 3) {
					// Day of week
					self.scheduler.repeat_value = 0;
					self.scheduler.repeatEveryDayOfWeek.sunday && (self.scheduler.repeat_value += 1);
					self.scheduler.repeatEveryDayOfWeek.monday && (self.scheduler.repeat_value += 2);
					self.scheduler.repeatEveryDayOfWeek.tuesday && (self.scheduler.repeat_value += 4);
					self.scheduler.repeatEveryDayOfWeek.wednesday && (self.scheduler.repeat_value += 8);
					self.scheduler.repeatEveryDayOfWeek.thursday && (self.scheduler.repeat_value += 16);
					self.scheduler.repeatEveryDayOfWeek.friday && (self.scheduler.repeat_value += 32);
					self.scheduler.repeatEveryDayOfWeek.saturday && (self.scheduler.repeat_value += 64);
				}
				self.scheduler.scripts = self.scriptList;
				self.scheduler.conditions = self.conditionList;
				
				if (self.add) {
					angharadFactory.addScheduler(self.scheduler).then(function () {
						toaster.pop("success", $translate.instant("scheduler_save"), $translate.instant("scheduler_save_success"));
						sharedData.add("angharadSchedulers", self.scheduler.name, self.scheduler);
						$rootScope.$broadcast("angharadSchedulersChanged");
					}, function () {
						toaster.pop("error", $translate.instant("scheduler_save"), $translate.instant("scheduler_save_error"));
					})["finally"](function () {
						$uibModalInstance.dismiss("close");
					});
				} else {
					angharadFactory.setScheduler(self.scheduler.name, self.scheduler).then(function () {
						toaster.pop("success", $translate.instant("scheduler_save"), $translate.instant("scheduler_save_success"));
						sharedData.add("angharadSchedulers", self.scheduler.name, self.scheduler);
						$rootScope.$broadcast("angharadSchedulersChanged");
					}, function () {
						toaster.pop("error", $translate.instant("scheduler_save"), $translate.instant("scheduler_save_error"));
					})["finally"](function () {
						$uibModalInstance.dismiss("close");
					});
				}
			} else if (self.eventType === "trigger") {
				self.trigger.name = self.event.name;
				self.trigger.description = self.event.description;
				self.trigger.enabled = self.event.enabled;
				self.trigger.scripts = self.scriptList;
				self.trigger.conditions = self.conditionList;
				if (self.add) {
					angharadFactory.addTrigger(self.trigger).then(function () {
						toaster.pop("success", $translate.instant("trigger_save"), $translate.instant("trigger_save_success"));
						sharedData.add("angharadTriggers", self.trigger.name, self.trigger);
						$rootScope.$broadcast("angharadTriggersChanged");
					}, function () {
						toaster.pop("error", $translate.instant("trigger_save"), $translate.instant("trigger_save_error"));
					})["finally"](function () {
						$uibModalInstance.dismiss("close");
					});
				} else {
					angharadFactory.setTrigger(self.trigger.name, self.trigger).then(function () {
						toaster.pop("success", $translate.instant("trigger_save"), $translate.instant("trigger_save_success"));
						sharedData.add("angharadTriggers", self.trigger.name, self.trigger);
						$rootScope.$broadcast("angharadTriggersChanged");
					}, function () {
						toaster.pop("error", $translate.instant("trigger_save"), $translate.instant("trigger_save_error"));
					})["finally"](function () {
						$uibModalInstance.dismiss("close");
					});
				}
			}
        };
        
        this.cancel = function () {
            $uibModalInstance.dismiss("cancel");
        };
        
        init();
    });
