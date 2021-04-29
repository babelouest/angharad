angular.module("sagremorApp")
    .controller("ScriptBindElementModalCtrl",
    function($scope, $uibModalInstance, $translate, toaster, DTOptionsBuilder, sharedData, sagGenericInjectorManager, angharadFactory, script) {
        var self = this;
        
        this.script = script;
        
        this.elementList = [];
        this.options = {};
        this.tmpCheck = [];
        
        function init() {
			if (!self.script.options.tags) {
				self.script.options.tags = [];
			}
			if ($translate.use()) {
				self.options = DTOptionsBuilder.newOptions()
								.withLanguageSource("components/core/i18n/datatables."+$translate.use()+".json")
								.withOption("order", [])
								.withOption("scrollY", "200px")
								.withOption("paging", false);
			}
			
			_.forEach(sharedData.all("benoicDevices"), function (device) {
				if (device.enabled && device.connected) {
					_.forEach(device.element.switches, function(element, name) {
						var selected = !!_.find(self.script.options.tags, function (tag) {
							return tag === "SGMR$SCR$B$" + device.name + "$switch$" + name;
						});
						var elt = {
							submodule: "benoic", 
							device: device.name,
							type: "switch",
							typeTitle: $translate.instant("switch_title"),
							name: name,
							display: element.display,
							selected: selected
						}
						self.elementList.push(elt);
						self.tmpCheck.push(selected);
					});
					_.forEach(device.element.dimmers, function(element, name) {
						var selected = !!_.find(self.script.options.tags, function (tag) {
							return tag === "SGMR$SCR$B$" + device.name + "$dimmer$" + name;
						});
						var elt = {
							submodule: "benoic", 
							device: device.name,
							type: "dimmer",
							typeTitle: $translate.instant("dimmer_title"),
							name: name,
							display: element.display,
							selected: selected
						}
						self.elementList.push(elt);
						self.tmpCheck.push(selected);
					});
					_.forEach(device.element.heaters, function(element, name) {
						var selected = !!_.find(self.script.options.tags, function (tag) {
							return tag === "SGMR$SCR$B$" + device.name + "$heater$" + name;
						});
						var elt = {
							submodule: "benoic", 
							device: device.name,
							type: "heater",
							typeTitle: $translate.instant("heater_title"),
							name: name,
							display: element.display,
							selected: selected
						}
						self.elementList.push(elt);
						self.tmpCheck.push(selected);
					});
					_.forEach(device.element.sensors, function(element, name) {
						var selected = !!_.find(self.script.options.tags, function (tag) {
							return tag === "SGMR$SCR$B$" + device.name + "$sensor$" + name;
						});
						var elt = {
							submodule: "benoic", 
							device: device.name,
							type: "sensor",
							typeTitle: $translate.instant("sensor_title"),
							name: name,
							display: element.display,
							selected: selected
						}
						self.elementList.push(elt);
						self.tmpCheck.push(selected);
					});
				}
			});
			
			_.forEach(sharedData.all("angharadSchedulers"), function (scheduler) {
				var selected = !!_.find(self.script.options.tags, function (tag) {
					return tag === "SGMR$SCR$A$scheduler$" + scheduler.name;
				});
				var elt = {
					submodule: "angharad", 
					type: "scheduler",
					typeTitle: $translate.instant("scheduler_title"),
					name: scheduler.name,
					display: scheduler.name,
					selected: selected
				}
				self.elementList.push(elt);
				self.tmpCheck.push(selected);
			});

			_.forEach(sharedData.all("carleonServices"), function (service, serviceName) {
				var inject = _.find(sagGenericInjectorManager.components, function (inject) {
					return inject.type === serviceName && service.enabled;
				});
				_.forEach(service.element, function (element) {
					var selected = !!_.find(self.script.options.tags, function (tag) {
						return tag === "SGMR$SCR$C$" + serviceName + "$" + element.name;
					});
					var elt = {
						submodule: "carleon", 
						type: serviceName,
						typeTitle: $translate.instant(inject.groupTitle),
						name: element.name, 
						display: element.name, 
						selected: selected
					};
					self.elementList.push(elt);
					self.tmpCheck.push(selected);
				});
			});
        }
        
        this.selectRow = function (index) {
			self.elementList[index].selected = !self.elementList[index].selected;
			self.tmpCheck[index] = self.elementList[index].selected;
		};
		
		this.cancel = function () {
			$uibModalInstance.dismiss("cancel");
		};
		
		this.ok = function () {
			// Script tag format: SGMR$SCR$<submodule>[$<device>]$<type>$name
			
			// First, remove all tags starting with SGMR$SCR$ for the current script
			_.remove(self.script.options.tags, function (tag) {
				return tag.startsWith("SGMR$SCR$");
			});
			
			// Second, add a tag for all selected elements
			var selectedElements = _.filter(self.elementList, function (element) {
				return element.selected;
			});
			_.forEach(selectedElements, function (element) {
				var tag = "SGMR$SCR$";
				if (element.submodule === "benoic") {
					tag += "B$" + element.device + "$" + element.type + "$" + element.name;
				} else if (element.submodule === "carleon") {
					tag += "C$" + element.type + "$" + element.name;
				} else {
					tag += "A$" + element.type + "$" + element.name;
				}
				self.script.options.tags.push(tag);
			});
			angharadFactory.setScript(self.script.name, self.script).then(function () {
				sharedData.add("angharadScripts", self.script.name, self.script);
				toaster.pop("success", $translate.instant("script_save"), $translate.instant("script_save_success"));
			}, function () {
				toaster.pop("error", $translate.instant("script_save"), $translate.instant("script_save_error"));
			})["finally"](function () {
				$uibModalInstance.dismiss("close");
			});
		};
		
        init();
    }
);
