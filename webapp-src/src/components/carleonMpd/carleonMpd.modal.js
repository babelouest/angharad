angular.module("sagremorApp")
		.controller("carleonMpdModalCtrl", 
		function($rootScope, $uibModalInstance, $translate, sharedData, toaster, carleonMpdFactory, mpd) {
			var self = this;

			this.messages = {};
			this.add = true;
			this.mpd = {
				name: "",
				description: "",
				host: "",
				port: 0,
				password: "",
				device: "",
				switch: ""
			}
			this.switches = [];
			this.switchAttached = {};

		function init() {
			if (!!mpd) {
				self.mpd = _.find(sharedData.get("carleonServices", "service-mpd").element, function (elt) {
					return elt.name === mpd.name;
				});
				self.add = false;
			} else {
				self.mpd = {
					file_list: [],
					stream_list: []
				};
			}
			_.forEach(sharedData.all("benoicDevices"), function (device) {
				if (device.enabled && device.connected) {
					_.forEach(device.element.switches, function(element, name) {
						var elt = {
							device: device.name,
							type: "switch",
							name: name,
							display: element.display
						}
						if (elt.name === self.mpd.switch && elt.device === self.mpd.device) {
							self.switchAttached = elt;
						}
						self.switches.push(elt);
					});
				}
			});
		}
				
		this.isMpdValid = function () {
			var found = _.find(sharedData.get("carleonServices", "service-mpd").element, function (element) {
				return element.name === self.mpd.name;
			});
			return (!found || !self.add) && !!self.mpd.name && !!self.mpd.host;
		};
				
		this.save = function () {
			if (self.add) {
				carleonMpdFactory.addMpdService(self.mpd).then(function () {
					self.mpd.type = "service-mpd";
					self.mpd.service = carleonMpdFactory;
					sharedData.get("carleonServices", "service-mpd").element.push(self.mpd);
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_mpd_save"), $translate.instant("carleon_mpd_save_success"));
				}, function () {
					toaster.pop("error", $translate.instant("carleon_mpd_save"), $translate.instant("carleon_mpd_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			} else {
				carleonMpdFactory.setMpdService(self.mpd.name, self.mpd).then(function () {
					self.mpd.type = "service-mpd";
					self.mpd.service = carleonMpdFactory;
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_mpd_save"), $translate.instant("carleon_mpd_save_success"));
				}, function () {
					toaster.pop("error", $translate.instant("carleon_mpd_save"), $translate.instant("carleon_mpd_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			}
		};
		
		this.cancel = function () {
			$uibModalInstance.dismiss("close");
		};
		
		this.changeSwitch = function () {
			if (self.switchAttached) {
				self.mpd.device = self.switchAttached.device;
				self.mpd.switch = self.switchAttached.name;
			}
		}
		
		init();
		}
);
