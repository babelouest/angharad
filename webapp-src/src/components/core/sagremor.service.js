angular.module("sagremorApp")
	.factory("sagremorService",
		function($uibModal, $translate, toaster, sharedData, sagremorParams, sagremorConfirm, angharadFactory, benoicFactory, carleonFactory) {
			var sagremorFactory = {};

			sagremorFactory.monitor = function(element) {
				var modalInstance = $uibModal.open({
					animation: true,
					templateUrl: "components/modals/monitor.modal.html",
					controller: "MonitorModalCtrl",
					controllerAs: "MonitorModalCtrl",
					size: "md",
					resolve: {
						element: function() {
							return element;
						}
					}
				});
			};

			sagremorFactory.imagePopup = function(title, src) {
				var modalInstance = $uibModal.open({
					animation: true,
					templateUrl: "components/modals/image.modal.html",
					controller: "ImageModalCtrl",
					controllerAs: "ImageModalCtrl",
					size: "lg",
					resolve: {
						title: function() {
							return title;
						},
						src: function() {
							return src;
						}
					}
				});
			};

			sagremorFactory.addToDashboard = function(element) {
				var tag = "SGMR$D$0$0";
				var profile = sagremorParams.currentProfile;
				if (!profile.addTo) {
					profile.addTo = {};
				}
				if (!profile.addTo.D) {
					profile.addTo.D = [];
				}
				if (!!element.device) {
					var newElement = {
						device: element.device,
						type: element.type,
						name: element.name,
						tag: tag
					};
					profile.addTo.D.push(newElement);
				} else {
					var newElement = {
						type: element.type,
						name: element.name,
						tag: tag
					};
					profile.addTo.D.push(newElement);
				}
				return angharadFactory.setProfile(profile.name, profile).then(function() {
					toaster.pop({
						type: "success",
						title: $translate.instant("angharad_add_to_dashboard"),
						body: $translate.instant("angharad_add_to_dashboard_success")
					});
				}, function() {
					toaster.pop({
						type: "error",
						title: $translate.instant("angharad_add_to_dashboard"),
						body: $translate.instant("angharad_add_to_dashboard_error")
					});
				});
			};

			sagremorFactory.bindToElement = function(script) {
				return modalInstance = $uibModal.open({
					animation: true,
					templateUrl: "components/modals/scriptBindElement.modal.html",
					controller: "ScriptBindElementModalCtrl",
					controllerAs: "ScriptBindElementModalCtrl",
					size: "md",
					resolve: {
						script: function() {
							return script;
						}
					}
				});
			};

			sagremorFactory.removeFromDashboard = function(element) {
				var dashboardWidgets = sagremorParams.dashboardWidgets;
				if (dashboardWidgets !== undefined) {
					_.remove(dashboardWidgets, function(widget) {
						return (widget.type === element.type && widget.element.device === element.device && widget.element.name === element.name);
					});
				}
				sagremorParams.dashboardWidgets = dashboardWidgets;
				return true;
			};

			sagremorFactory.editSwitcher = function(switcher) {
				var modalInstance = $uibModal.open({
					animation: true,
					templateUrl: "components/switch/switch.modal.html",
					controller: "SwitchesModalCtrl",
					controllerAs: "SwitchesModalCtrl",
					size: "sm",
					resolve: {
						switcher: function() {
							return switcher;
						}
					}
				});
			};

			sagremorFactory.editSensor = function(sensor) {
				var modalInstance = $uibModal.open({
					animation: true,
					templateUrl: "components/sensor/sensor.modal.html",
					controller: "SensorsModalCtrl",
					controllerAs: "SensorsModalCtrl",
					size: "sm",
					resolve: {
						sensor: function() {
							return sensor;
						}
					}
				});
			};

			sagremorFactory.editHeater = function(heater) {
				var modalInstance = $uibModal.open({
					animation: true,
					templateUrl: "components/heater/heater.modal.html",
					controller: "HeatersModalCtrl",
					controllerAs: "HeatersModalCtrl",
					size: "sm",
					resolve: {
						heater: function() {
							return heater;
						}
					}
				});
			};

			sagremorFactory.editDimmer = function(dimmer) {
				var modalInstance = $uibModal.open({
					animation: true,
					templateUrl: "components/dimmer/dimmer.modal.html",
					controller: "DimmersModalCtrl",
					controllerAs: "DimmersModalCtrl",
					size: "sm",
					resolve: {
						dimmer: function() {
							return dimmer;
						}
					}
				});
			};

			sagremorFactory.editScript = function(script) {
				var modalInstance = $uibModal.open({
					animation: true,
					templateUrl: "components/script/script.modal.html",
					controller: "ScriptModalCtrl",
					controllerAs: "ScriptModalCtrl",
					size: "md",
					resolve: {
						script: function() {
							return script;
						}
					}
				});
			};

			sagremorFactory.removeScript = function(script) {
				return sagremorConfirm.open($translate.instant("script_remove"), $translate.instant("script_remove_confirm")).then(function(result) {
					return angharadFactory.removeScript(script.name).then(function() {
						sharedData.remove("angharadScripts", script.name);
						toaster.pop("success", $translate.instant("script_remove"), $translate.instant("script_remove_success"));
					}, function(error) {
						toaster.pop("error", $translate.instant("script_remove"), $translate.instant("script_remove_error"));
					});
				});
			};

			sagremorFactory.editEvent = function(event, eventType) {
				var modalInstance = $uibModal.open({
					animation: true,
					templateUrl: "components/event/event.modal.html",
					controller: "EventModalCtrl",
					controllerAs: "EventModalCtrl",
					size: "md",
					resolve: {
						event: function() {
							return event;
						},
						eventType: function() {
							return eventType;
						}
					}
				});
			};

			sagremorFactory.removeScheduler = function(event) {
				return sagremorConfirm.open($translate.instant("scheduler_remove"), $translate.instant("scheduler_remove_confirm")).then(function(result) {
					return angharadFactory.removeScheduler(event.name).then(function() {
						sharedData.remove("angharadSchedulers", event.name);
						toaster.pop("success", $translate.instant("scheduler_remove"), $translate.instant("scheduler_remove_success"));
					}, function(error) {
						toaster.pop("error", $translate.instant("scheduler_remove"), $translate.instant("scheduler_remove_error"));
					});
				});
			};

			sagremorFactory.removeTrigger = function(trigger) {
				return sagremorConfirm.open($translate.instant("trigger_remove"), $translate.instant("trigger_remove_confirm")).then(function(result) {
					return angharadFactory.removeTrigger(trigger.name).then(function() {
						sharedData.remove("angharadTriggers", trigger.name);
						toaster.pop("success", $translate.instant("trigger_remove"), $translate.instant("trigger_remove_success"));
					}, function(error) {
						toaster.pop("error", $translate.instant("trigger_remove"), $translate.instant("trigger_remove_error"));
					});
				});
			};

			sagremorFactory.getBenoicElement = function(device, type, name) {
				var elements = sharedData.get("benoicDevices", device);
				if (!!elements && !!elements.element) {
					switch (type) {
						case "switch":
							if (!!elements.element.switches && elements.element.switches[name]) {
								return elements.element.switches[name];
							}
							break;
						case "dimmer":
							if (!!elements.element.dimmers && elements.element.dimmers[name]) {
								return elements.element.dimmers[name];
							}
							break;
						case "heater":
							if (!!elements.element.heaters && elements.element.heaters[name]) {
								return elements.element.heaters[name];
							}
							break;
						case "sensor":
							if (!!elements.element.sensors && elements.element.sensors[name]) {
								return elements.element.sensors[name];
							}
							break;
					}
				}
				return false;
			};

			sagremorFactory.getCarleonElement = function(service, name) {
				if (!!service && !!name) {
					var cService = sharedData.get("carleonServices", service);
					var element = _.find(cService.element, function(elt) {
						return elt.name === name;
					});
					return element || false;
				}
				return false;
			};

			sagremorFactory.removeMonitor = function(monitor) {
				_.remove(sagremorParams.currentProfile.monitorList, function(curMonitor) {
					return curMonitor.name === monitor.name;
				});
				return angharadFactory.saveCurrentProfile().then(function() {
					toaster.pop("success", $translate.instant("monitor_save"), $translate.instant("monitor_save_success"));
				}, function() {
					toaster.pop("error", $translate.instant("monitor_save"), $translate.instant("monitor_save_error"));
				});
			};

			return sagremorFactory;
		}
	);
