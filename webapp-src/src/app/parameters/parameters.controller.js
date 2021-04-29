angular.module("sagremorApp")
    .controller("ParametersCtrl",
    function($scope, $rootScope, $q, $location, $translate, $cookies, $window, toaster, angharadFactory, benoicFactory, carleonFactory, sharedData, sagremorConfirm, sagremorParams) {
      
		var self = this;

		this.submodules;

		this.deviceList = [];
		this.deviceTypes = [];
		this.deviceTypesEnabled = [];
		this.deviceAdded = false;
		this.deviceUpdated = false;

		this.newDeviceName = "";
		this.newDeviceDescription = "";
		this.newDeviceType = "";
		this.newDeviceConnect = true;

		this.deviceOptionList = [];
		this.deviceOptionListDisplay = false;

		this.profileList = [];
		this.currentProfile = false;
		this.profileAdded = false;
		
		this.serviceList = [];
		
		this.sagremorParams = sagremorParams;

		this.init = function() {
			if (!sagremorParams.loggedIn) {
				$location.path("/login");
			}

			self.submodules = sharedData.all("submodules");
			self.deviceList = sharedData.all("benoicDevices");
			self.initDeviceTypes();
			self.serviceList = sharedData.all("carleonServices");
			self.selectedLang = $translate.use();
			self.profileList = sagremorParams.profiles;
			self.currentProfile = sagremorParams.currentProfile;
		};

		$scope.$on("submodulesChanged", function () {
			self.submodules = sharedData.all("submodules");
		});

		$scope.$on("benoicDevicesChanged", function () {
			self.deviceList = sharedData.all("benoicDevices");
		});

		$scope.$on("benoicDeviceTypesChanged", function () {
			self.initDeviceTypes();
		});

		$scope.$on("carleonServicesChanged", function () {
			self.serviceList = sharedData.all("carleonServices");
		});

		this.addDevice = function () {
			self.deviceAdded = true;
		};

		this.isDeviceValid = function () {
			if (!self.newDeviceName || self.newDeviceName.length > 64) {
				return false;
			}

			for (var name in self.deviceList) {
				if (self.newDeviceName === name) {
					return false;
				}
			}

			if (!self.newDeviceDescription || self.newDeviceDescription > 128) {
				return false;
			}

			if (!self.newDeviceType) {
				return false;
			}

			for (var key in self.deviceOptionList) {
				var option = self.deviceOptionList[key];

				if (!option.optional && !option.value) {
					return false;
				}
			}

			return true;
		};

		this.postNewDevice = function () {

			var curOptions = {};
			_.forEach(self.deviceOptionList, function (option) {
				if (option.type === "boolean") {
					curOptions[option.name] = !!option.value;
				} else if (option.type === "numeric") {
					curOptions[option.name] = parseFloat(option.value);
				} else {
					curOptions[option.name] = option.value;
				}
			});
			var deviceName = self.newDeviceName;
			benoicFactory.addDevice({name: deviceName, display: self.newDeviceName, description: self.newDeviceDescription, enabled: true, type_uid: self.newDeviceType, options: curOptions}).then(function (response) {
				if (self.newDeviceConnect) {
					self.connectDevice({name: deviceName, connected: true}).then(function(result) {
						toaster.pop("success", $translate.instant("device_add"), $translate.instant("device_add_success"));
					});
				} else {
					toaster.pop("success", $translate.instant("device_add"), $translate.instant("device_add_success"));
				}
			}, function (error) {
				toaster.pop("error", $translate.instant("device_add"), $translate.instant("device_add_error"));
			})["finally"](function () {
				self.deviceAdded = false;
				self.deviceList = sharedData.all("benoicDevices");
				self.newDeviceName = "";
				self.newDeviceDescription = "";
				self.newDeviceType = "";
				self.newDeviceConnect = true;
				self.deviceOptionList = [];
				$rootScope.$broadcast("reinitBenoic");
			});
		};

		this.cancelNewDevice = function () {
			self.deviceAdded = false;
			self.newDeviceName = "";
			self.newDeviceDescription = "";
			self.newDeviceType = "";
			self.deviceOptionList = [];
		};

		this.setDeviceOptions = function () {
			if (!!self.newDeviceType) {
				self.deviceOptionList = _.find(self.deviceTypes, function(type) { return type.uid === self.newDeviceType; }).options;
				self.deviceOptionListDisplay = true;
			} else {
				self.deviceOptionListDisplay = false;
			}
		};

		this.connectDevice = function (device) {
			if (device.connected) {
				return benoicFactory.connectDevice(device.name).then(function (response) {
					benoicFactory.getDeviceOverview(device.name).then(function (result) {
						sharedData.get("benoicDevices", device.name).element = result;
						toaster.pop("success", $translate.instant("device_connect"), $translate.instant("device_connect_success"));
					}, function (error) {
						toaster.pop("error", $translate.instant("device_connect"), $translate.instant("device_connect_error"));
					});
				}, function (error) {
					toaster.pop("error", $translate.instant("device_connect"), $translate.instant("device_connect_error"));
					device.connected = false;
				});
			} else {
				return benoicFactory.disconnectDevice(device.name).then(function (response) {
					toaster.pop("success", $translate.instant("device_disconnect"), $translate.instant("device_disconnect_success"));
					var curDevice = sharedData.get("benoicDevices", device.name);
					if (!!curDevice) {
						curDevice.element = {};
					}
				}, function (error) {
					toaster.pop("error", $translate.instant("device_disconnect"), $translate.instant("device_disconnect_error"));
					device.connected = true;
				});
			}
		};

		this.editDevice = function (device) {
			device.newDescription = device.description;
			device.update = true;
			self.deviceUpdated = true;
			self.deviceOptionListDisplay = true;
			self.deviceOptionList = _.find(self.deviceTypes, {uid: device.type_uid}).options;
			self.newDeviceType = device.type_uid;
			_.forEach(self.deviceOptionList, function (option) {
				option.value = device.options[option.name];
			});
		};

		this.saveDevice = function (device) {
			device.description = device.newDescription;
			_.forEach(self.deviceOptionList, function (option) {
				device.options[option.name] = option.value;
			});
			
			benoicFactory.setDevice(device).then(function (response) {
				toaster.pop("success", $translate.instant("device_save"), $translate.instant("device_save_success"));
			}, function (error) {
				toaster.pop("error", $translate.instant("device_save"), $translate.instant("device_save_error"));
			})["finally"](function () {
				self.deviceUpdated = false;
				self.deviceOptionListDisplay = false;
				self.deviceOptionList = [];
				device.update = false;
			});
		};

		this.cancelEditDevice = function (device) {
			device.newDescription = device.description;
			self.deviceUpdated = false;
			self.deviceOptionListDisplay = false;
			self.deviceOptionList = [];
			device.update = false;
		};

		this.removeDevice = function (device) {
			sagremorConfirm.open($translate.instant("device_remove"), $translate.instant("device_remove_confirm")).then (function(result) {
				benoicFactory.removeDevice(device.name).then(function (response) {
					sharedData.remove("benoicDevices", device.name);
					self.deviceList = sharedData.all("benoicDevices");
					toaster.pop("success", $translate.instant("device_remove"), $translate.instant("device_remove_success"));
				}, function (error) {
					toaster.pop("error", $translate.instant("device_remove"), $translate.instant("device_remove_error"));
				});
			});
		};

		this.initDeviceTypes = function () {
			self.deviceTypes = sharedData.all("benoicDeviceTypes");
			self.deviceTypesEnabled = _.filter(sharedData.all("benoicDeviceTypes"), {enabled: true});
			_.forEach(self.deviceTypes, function (type) {
				$translate(type.uid + "_device_type").then(function(translate) {
					type.translate = translate;
				}, function (error) {
					type.translate = type.name;
				});
			});
		};
		
		this.reloadBenoicDeviceTypes = function () {
			benoicFactory.reloadDeviceTypes().then(function (result) {
				sharedData.removeAll("benoicDeviceTypes");
				_.forEach(result, function (deviceType) {
					sharedData.add("benoicDeviceTypes", deviceType.uid, deviceType);
				});
				self.initDeviceTypes();
				$rootScope.$broadcast("refresh");
			});
		};

		this.reloadCarleonServices = function () {
			carleonFactory.reloadServiceList().then(function (result) {
				sharedData.removeAll("carleonServices");
				for (key in result) {
					_.forEach(result[key].element, function (element) {
						element.type = result[key].name;
					});
					sharedData.add("carleonServices", result[key].name, result[key]);
				}
				$rootScope.$broadcast("refresh");
			});
		};

		this.submoduleEnable = function(submodule) {
			return sagremorConfirm.open($translate.instant("param_submodule_enable"), $translate.instant("param_submodule_enable_confirm")).then(function () {
				if (submodule === "benoic") {
					angharadFactory.enableSubmodule(submodule, self.submodules.benoic.enabled).then(function (response) {
						sagremorParams.benoicEnabled = self.submodules.benoic.enabled;
						$rootScope.$broadcast("submodulesChanged");
						$rootScope.$broadcast("refreshDashboard")
						if (self.submodules.benoic.enabled) {
							$rootScope.$broadcast("reinitBenoic");
						} else {
							$rootScope.$broadcast("closeBenoic");
						}
						toaster.pop("success", $translate.instant("submodules"), $translate.instant("submodules_enable_success"));
					}, function (error) {
						toaster.pop("error", $translate.instant("submodules"), $translate.instant("submodules_enable_error"));
					});
				} else if (submodule === "carleon") {
					angharadFactory.enableSubmodule(submodule, self.submodules.carleon.enabled).then(function (response) {
						sagremorParams.carleonEnabled = self.submodules.carleon.enabled;
						$rootScope.$broadcast("submodulesChanged");
						$rootScope.$broadcast("refreshDashboard")
						if (self.submodules.carleon.enabled) {
							$rootScope.$broadcast("reinitCarleon");
						} else {
							$rootScope.$broadcast("closeCarleon");
						}
						toaster.pop("success", $translate.instant("submodules"), $translate.instant("submodules_enable_success"));
					}, function (error) {
						toaster.pop("error", $translate.instant("submodules"), $translate.instant("submodules_enable_error"));
					});
				} else if (submodule === "gareth") {
					angharadFactory.enableSubmodule(submodule, self.submodules.gareth.enabled).then(function (response) {
						sagremorParams.garethEnabled = self.submodules.gareth.enabled;
						$rootScope.$broadcast("submodulesChanged");
						$rootScope.$broadcast("refreshDashboard")
						if (self.submodules.gareth.enabled) {
							$rootScope.$broadcast("reinitGareth");
						} else {
							$rootScope.$broadcast("closeGareth");
						}
						toaster.pop("success", $translate.instant("submodules"), $translate.instant("submodules_enable_success"));
					}, function (error) {
						toaster.pop("error", $translate.instant("submodules"), $translate.instant("submodules_enable_error"));
					});
				}
			}, function () {
				self.submodules[submodule].enabled = !self.submodules[submodule].enabled;
			});
		};

		this.addProfile = function () {
			self.newProfileName = "";
			self.newProfileDescription = "";
			self.newProfileDefault = false;
			self.profileAdded = true;
		};

		this.cancelNewProfile = function () {
			self.profileAdded = false;
		};
		
		this.isProfileValid = function () {
			var result = self.newProfileName.length > 0;
			_.forEach(self.profileList, function (profile) {
				if (profile.name === self.newProfileName) {
					result = false;
				}
			});
			return result;
		};
		
		this.saveNewProfile = function () {
			var newProfile = {name: self.newProfileName, description: self.newProfileDescription, default: self.newProfileDefault};
			angharadFactory.setProfile(self.newProfileName, newProfile).then(function () {
				toaster.pop("success", $translate.instant("profiles"), $translate.instant("profiles_add_success"));
				$scope.$broadcast("angharadProfileChanged");
				self.profileList.push(newProfile);
			}, function (error) {
				toaster.pop("error", $translate.instant("profiles"), $translate.instant("profiles_add_error"));
			})["finally"](function () {
				self.profileAdded = false;
			});
		};

		this.removeProfile = function (profile) {
			sagremorConfirm.open($translate.instant("profile_remove"), $translate.instant("profile_remove_confirm")).then (function(result) {
				angharadFactory.removeProfile(profile.name).then(function () {
					toaster.pop("success", $translate.instant("profiles"), $translate.instant("profiles_remove_success"));
					$scope.$broadcast("angharadProfileChanged");
					_.remove(self.profileList, function (curProfile) {
						return profile.name === curProfile.name;
					});
				}, function (error) {
					toaster.pop("error", $translate.instant("profiles"), $translate.instant("profiles_remove_error"));
				});
			});
		};
		
		this.cleanProfile = function (profile) {
			sagremorConfirm.open($translate.instant("profile_clean"), $translate.instant("profile_clean_confirm")).then (function(result) {
				var cleanProfile = {name: profile.name, description: profile.description, default: profile.default};
				angharadFactory.setProfile(profile.name, cleanProfile).then(function (result) {
					for (key in sagremorParams.profiles) {
						if (sagremorParams.profiles[key].name === cleanProfile.name) {
							sagremorParams.profiles[key] = cleanProfile;
						}
					}
					if (sagremorParams.currentProfile.name === cleanProfile.name) {
						sagremorParams.currentProfile = cleanProfile;
					}
					$scope.$broadcast("angharadProfileChanged");
					toaster.pop("success", $translate.instant("profiles"), $translate.instant("profiles_save_success"));
					profile = cleanProfile;
				}, function (error) {
					toaster.pop("error", $translate.instant("profiles"), $translate.instant("profiles_save_error"));
				});
			});
		};

		this.editProfile = function (profile) {
			profile.update = true;
			profile.newName = profile.name;
			profile.newDescription = profile.description;
		};

		this.cancelEditProfile = function (profile) {
			profile.update = false;
			profile.name = profile.newName;
			profile.description = profile.newDescription;
		};

		this.isUpdateProfileValid = function (profile) {
			var result = profile.newName.length > 0;
			_.forEach(self.profileList, function (curProfile) {
				if (profile.name !== curProfile.name && profile.newName === curProfile.name) {
					// Duplicate name, avoid
					result = false;
				}
			});
			return result;
		};

		this.saveProfile = function (profile) {
			if (profile.name !== profile.savedName) {
				// Remve old profile, then add new one
				var savedName = profile.newName;
				profile.name = profile.newName;
				profile.description = profile.newDescription;
				delete profile.newName;
				delete profile.newDescription;
				delete profile.update;
				var promises = {
					remove: angharadFactory.removeProfile(savedName),
					add: angharadFactory.setProfile(profile.name, profile)
				}

				$q.all(promises).then(function (results) {
					$scope.$broadcast("angharadProfileChanged");
					toaster.pop("success", $translate.instant("profiles"), $translate.instant("profiles_save_success"));
				}, function (error) {
					toaster.pop("error", $translate.instant("profiles"), $translate.instant("profiles_save_error"));
				});
			} else {
			  // Update profile
				delete profile.savedName;
				delete profile.savedDescription;
				delete profile.update;
				angharadFactory.setProfile(profile.name, profile).then(function (result) {
					$scope.$broadcast("angharadProfileChanged");
					toaster.pop("success", $translate.instant("profiles"), $translate.instant("profiles_save_success"));
				}, function (error) {
					toaster.pop("error", $translate.instant("profiles"), $translate.instant("profiles_save_error"));
				});
			}
		};
		
		this.setDefaultProfile = function (profile) {
			if (!profile.default) {
				// Update just this one
				delete profile.savedName;
				delete profile.savedDescription;
				delete profile.update;
				angharadFactory.setProfile(profile.name, profile).then(function (result) {
					$scope.$broadcast("angharadProfileChanged");
					toaster.pop("success", $translate.instant("profiles"), $translate.instant("profiles_save_success"));
				}, function (error) {
					toaster.pop("error", $translate.instant("profiles"), $translate.instant("profiles_save_error"));
				});
			} else {
				// Set this one to default, and set the other default to false
				delete profile.update;
				angharadFactory.setProfile(profile.name, profile).then(function (result) {
					_.forEach(self.profileList, function (curProfile) {
						if (curProfile.default && curProfile.name !== profile.name) {
							curProfile.default = false;
							angharadFactory.setProfile(curProfile.name, curProfile).then(function (result) {
							}, function (error) {
								toaster.pop("error", $translate.instant("profiles"), $translate.instant("profiles_save_error"));
							});
						}
					});
					$scope.$broadcast("angharadProfileChanged");
					toaster.pop("success", $translate.instant("profiles"), $translate.instant("profiles_save_success"));
				}, function (error) {
					toaster.pop("error", $translate.instant("profiles"), $translate.instant("profiles_save_error"));
				});
			}
		};
		
		this.useProfile = function (profile) {
			sagremorParams.currentProfile = profile;
			var exp = new $window.Date();
			exp = new $window.Date(exp.getFullYear() + 10, exp.getMonth(), exp.getDate());
			$cookies.put("ANGHARAD_PROFILE", profile.name, {expires: exp});
			$rootScope.$broadcast("angharadProfileUpdated");
		};
		
		$scope.$on("angharadProfileChanged", function () {
			self.profileList = sagremorParams.profiles;
		});

		$scope.$on("angharadProfileUpdated", function () {
			self.currentProfile = sagremorParams.currentProfile;
		});

		this.init();
    }
).filter("deviceTypeName", [
    "sharedData",
    function(sharedData) {
        return function(input) {
          var types = sharedData.all("benoicDeviceTypes");
          for (key in types) {
            if (types[key].uid === input) {
              return types[key].name;
            }
          }
          return false;
        };
    }]);
