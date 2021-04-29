angular.module("sagremorApp")
	.controller("sagremorCtrl",
		function($scope, $rootScope, $interval, $window, $http, $q, $location, $cookies, $translate, toaster, angharadFactory, benoicFactory, carleonFactory, garethFactory, sharedData, sagremorParams, angharadConfig) {
			var self = this;

			this.loaderToast;
			this.initComplete = false;
			this.benoicInitError = false;
			this.shoudRefresh = false;
			this.hasFocus = true;
			this.refreshTimeout = null;
			this.isInit = false;

			function init() {
				$translate("angharad_loading_title").then(function(title) {
					// Nothing to do here, just waiting for lang files to be loaded before starting
				});
			}

			function closeLoader(result) {
				toaster.clear(self.loaderToast);
				if (result) {
					toaster.pop({
						type: "success",
						title: $translate.instant("angharad_loading_title"),
						body: $translate.instant("init_message_loading_complete")
					});
				} else {
					toaster.pop({
						type: "error",
						title: $translate.instant("angharad_loading_title"),
						body: $translate.instant("init_message_loading_error")
					});
				}
			}

			function popLoader() {
        toaster.clear(self.loaderToast);
				self.loaderToast = toaster.pop({
					type: "wait",
					title: $translate.instant("angharad_loading_title"),
					body: $translate.instant("init_message_loading"),
					timeout: 0,
					showCloseButton: false
				});
			}

			function initParameters() {
				popLoader();
				getApiData();
				self.refreshTimeout = startRefreshTimeout();
				sagremorParams.adminMode = false;
				sagremorParams.loggedIn = true;
				self.isInit = true;
			}

			function refreshData() {
				popLoader();

				// Refresh benoic devices with overview
				benoicFactory.getDeviceList().then(function(result) {
					_.forEach(sharedData.all("benoicDevices"), function(device) {
						device.enabled = false;
						device.connected = false;
					});
					_.forEach(result, function(device) {
						sharedData.add("benoicDevices", device.name, device);
					});

					var devices = sharedData.all("benoicDevices");
					var devicePromises = {};
					_.forEach(sharedData.all("benoicDevices"), function(device, deviceName) {
						if (device.connected) {
							devicePromises[deviceName] = benoicFactory.getDeviceOverview(deviceName).then(function(result) {
								var curDevice = sharedData.get("benoicDevices", deviceName);
								if (!!curDevice) {
									curDevice.element = result;
								}
							}, function() {
								toaster.pop("error", $translate.instant("refresh"), $translate.instant("refresh_device_error", {
									name: deviceName
								}));
							});
						}
					});
					$q.all(devicePromises).then(function(results) {})["finally"](function() {
						$rootScope.$broadcast("refreshDevices");
					});
				}, function() {
					toaster.pop("error", $translate.instant("refresh"), $translate.instant("refresh_device_error"));
				})["finally"](function() {
					// Refresh scripts and events
					var promiseList = {
						scripts: angharadFactory.getScriptList(),
						schedulers: angharadFactory.getSchedulerList(),
						triggers: angharadFactory.getTriggerList(),
						profiles: angharadFactory.getProfileList()
					};

					$q.all(promiseList).then(function(result) {
						sharedData.removeAll("angharadScripts");
						sharedData.removeAll("angharadSchedulers");
						sharedData.removeAll("angharadTriggers");
						for (sc in result.scripts) {
							sharedData.add("angharadScripts", result.scripts[sc].name, result.scripts[sc]);
						}
						$rootScope.$broadcast("refreshAngharadScripts");
						for (sh in result.schedulers) {
							sharedData.add("angharadSchedulers", result.schedulers[sh].name, result.schedulers[sh]);
						}
						for (tr in result.triggers) {
							sharedData.add("angharadTriggers", result.triggers[tr].name, result.triggers[tr]);
						}
						$rootScope.$broadcast("refreshAngharadEvents");
						sagremorParams.profiles = result.profiles;
						$scope.$broadcast("angharadProfileChanged");
					}, function(error) {
						toaster.pop("error", $translate.instant("refresh"), $translate.instant("refresh_scripts_events_error"));
					})["finally"](function() {
						// Refresh carleon services elements

						carleonFactory.getServiceList().then(function(result) {
							sharedData.removeAll("carleonServices");
							for (key in result) {
								_.forEach(result[key].element, function(element) {
									element.type = result[key].name;
								});
								sharedData.add("carleonServices", result[key].name, result[key]);
							}
							$scope.$broadcast("refreshCarleonServices");

						}, function(error) {
							toaster.pop({
								type: "error",
								title: $translate.instant("refresh"),
								body: $translate.instant("refresh_carleon_error")
							});
						})["finally"](function() {
							// All done, refreshing dashboard now
							toaster.clear(self.loaderToast);
							$scope.$broadcast("refreshDashboard");
						});
					});
				});
			}

			function getApiData() {
				return self.initAngharadSubmodules().then(function(result) {
					var promises = [self.initAngharad()];
					sagremorParams.benoicEnabled = false;
					sagremorParams.carleonEnabled = false;
					sagremorParams.garethEnabled = false;
					for (key in result) {
						sharedData.add("submodules", result[key].name, result[key]);
						if (result[key].name === "benoic" && result[key].enabled) {
							sagremorParams.benoicEnabled = true;
							promises.push(self.initBenoic());
						} else if (result[key].name === "carleon" && result[key].enabled) {
							sagremorParams.carleonEnabled = true;
							promises.push(self.initCarleon());
						} else if (result[key].name === "gareth" && result[key].enabled) {
							sagremorParams.garethEnabled = true;
							promises.push(self.initGareth());
						}
					}
					$q.all(promises).then(function() {
						$scope.$broadcast("submodulesChanged");
					}, function() {
						toaster.pop({
							type: "error",
							title: $translate.instant("angharad_loading_title"),
							body: $translate.instant("init_message_loading_error")
						});
					})["finally"](function() {
						self.initComplete = true;
						$scope.$broadcast("initComplete");
					});
				}, function(error) {
					toaster.pop({
						type: "error",
						title: $translate.instant("angharad_loading_title"),
						body: $translate.instant("init_message_loading_error")
					});
				});
			}

			function startRefreshTimeout() {
				// Refresh every 5 minutes if the window has the focus
				// If the window has not the focus, set a flag to refresh the page when it will have the focus
				return $interval(function() {
					if (self.hasFocus) {
						refreshData();
						self.shouldRefresh = false;
					} else {
						self.shouldRefresh = true;
					}
				}, 1000 * 60 * 5);
			}

			$window.onblur = function() {
				self.hasFocus = false;
			};

			$window.onfocus = function() {
				self.hasFocus = true;
				if (self.shouldRefresh) {
					refreshData();
					self.shouldRefresh = false;
				}
			};

			this.initAngharadSubmodules = function() {
				return angharadFactory.getSumboduleList();
			};

			this.initBenoic = function() {
				var promiseList = {
					deviceTypesResult: benoicFactory.getDeviceTypes(),
					deviceResult: benoicFactory.getDeviceList()
				};

				return $q.all(promiseList).then(function(result) {
					var deviceTypesResult = result.deviceTypesResult;
					var deviceResult = result.deviceResult;

					// Handle device types
					_.forEach(deviceTypesResult, function(type) {
						sharedData.add("benoicDeviceTypes", type.name, type);
					});
					$scope.$broadcast("benoicDeviceTypesChanged");

					// Handle devices
					var deviceList = [];
					sharedData.removeAll("benoicDevices");
					_.forEach(deviceResult, function(curDevice) {
						if (curDevice.connected) {
							deviceList.push(benoicFactory.getDeviceOverview(curDevice.name).then(function(result) {
								curDevice.element = result;
							}, function() {
								curDevice.element = {};
								toaster.pop("error", $translate.instant("benoic_loading_title"), $translate.instant("benoic_loading_error", {
									name: curDevice.name
								}));
							}));
						}
						sharedData.add("benoicDevices", curDevice.name, curDevice);
					});
					$q.all(deviceList).then(function(responses) {})["finally"](function() {
						$rootScope.$broadcast("benoicDevicesChanged");
					});
				}, function(error) {
					toaster.pop("error", $translate.instant("benoic_loading_title"), $translate.instant("benoic_loading_error"));
				});
			}

			this.initCarleon = function() {
				sharedData.removeAll("carleonServices");

				return carleonFactory.getServiceList().then(function(result) {
					for (key in result) {
						_.forEach(result[key].element, function(element) {
							element.type = result[key].name;
						});
						sharedData.add("carleonServices", result[key].name, result[key]);
					}
				}, function(error) {
					toaster.pop({
						type: "error",
						title: $translate.instant("carleon_loading_title"),
						body: $translate.instant("carleon_loading_error")
					});
				})["finally"](function() {
					$rootScope.$broadcast("carleonServicesChanged");
				});

			};

			this.initGareth = function() {
				sharedData.removeAll("garethFilters");
				sharedData.removeAll("garethAlertsSmtp");
				sharedData.removeAll("garethAlertsHttp");
				var qList = {
					filters: garethFactory.getFilterList(),
					alerts: garethFactory.getAlertList()
				}

				return $q.all(qList).then(function(results) {
					for (key in results.filters) {
						sharedData.add("garethFilters", results.filters[key].name, results.filters[key]);
					}
					for (key in results.alerts.smtp) {
						sharedData.add("garethAlertsSmtp", results.alerts.smtp[key].name, results.alerts.smtp[key]);
					}
					for (key in results.alerts.http) {
						sharedData.add("garethAlertsHttp", results.alerts.http[key].name, results.alerts.http[key]);
					}
					$scope.$broadcast("garethChange");
				}, function(error) {
					toaster.pop({
						type: "error",
						title: $translate.instant("gareth_loading_title"),
						body: $translate.instant("gareth_loading_error")
					});
				});
			};

			this.setDefaultProfile = function() {
				sagremorParams.currentProfile = false;
				_.forEach(sagremorParams.profiles, function(profile) {
					if (profile.default) {
						sagremorParams.currentProfile = profile;
						var exp = new $window.Date();
						exp = new $window.Date(exp.getFullYear() + 10, exp.getMonth(), exp.getDate());
						$cookies.put("ANGHARAD_PROFILE", profile.name, {
							expires: exp
						});
					}
				});

				if (!sagremorParams.currentProfile) {
					// No profile, add default one
					var defaultProfile = {
						name: $translate.instant("profile_default"),
						default: true,
						data: {}
					};
					sagremorParams.profiles = [
						defaultProfile
					];
					sagremorParams.currentProfile = defaultProfile;
					angharadFactory.setProfile($translate.instant("profile_default"), defaultProfile).then(function() {
						toaster.pop("success", $translate.instant("profile_save"), $translate.instant("profile_save_success"));
					}, function(error) {
						toaster.pop("error", $translate.instant("profile_save"), $translate.instant("profile_save_error"));
					});
					var exp = new $window.Date();
					exp = new $window.Date(exp.getFullYear() + 10, exp.getMonth(), exp.getDate());
					$cookies.put("ANGHARAD_PROFILE", $translate.instant("profile_default"), {
						expires: exp
					});
					$scope.$broadcast("angharadProfileChanged");
				}
			};

			this.initAngharad = function() {
				var promiseList = {
					scripts: angharadFactory.getScriptList(),
					schedulers: angharadFactory.getSchedulerList(),
					triggers: angharadFactory.getTriggerList(),
					profiles: angharadFactory.getProfileList()
				};

				return $q.all(promiseList).then(function(result) {
					for (sc in result.scripts) {
						sharedData.add("angharadScripts", result.scripts[sc].name, result.scripts[sc]);
					}
					$scope.$broadcast("angharadScriptsChanged");
					for (sh in result.schedulers) {
						sharedData.add("angharadSchedulers", result.schedulers[sh].name, result.schedulers[sh]);
					}
					$scope.$broadcast("angharadSchedulersChanged");
					for (tr in result.triggers) {
						sharedData.add("angharadTriggers", result.triggers[tr].name, result.triggers[tr]);
					}
					$scope.$broadcast("angharadTriggersChanged");

					if (result.profiles.length === 0) {
						self.setDefaultProfile();
						$scope.$broadcast("angharadProfileChanged");
					} else {
						sagremorParams.profiles = result.profiles;
						var profile_name = $cookies.get("ANGHARAD_PROFILE");
						sagremorParams.currentProfile = false;

						if (!!profile_name) {
							_.forEach(result.profiles, function(profile) {
								if (profile.name === profile_name) {
									sagremorParams.currentProfile = profile;
									$scope.$broadcast("angharadProfileChanged");
								}
							});
							if (!sagremorParams.currentProfile) {
								self.setDefaultProfile();
							}
						} else {
							self.setDefaultProfile();
						}
					}
				}, function(error) {
					toaster.pop("error", $translate.instant("angharad_loading_title"), $translate.instant("angharad_loading_error"));
				});
			};

			this.closeSagremor = function() {
				sagremorParams.token = null;
				sagremorParams.loggedIn = false;
				$scope.isLogged = false;
				sagremorParams.profiles = [];
				sagremorParams.currentProfile = null;
				self.isInit = false;
				$rootScope.$broadcast("closeBenoic");
				$rootScope.$broadcast("closeCarleon");
				$rootScope.$broadcast("closeGareth");
				$rootScope.$broadcast("angharadProfileUpdated");
				$rootScope.$broadcast("refreshDashboard");
				sharedData.add("oauthToken", "token", false);
			};

			$scope.$on("reinitBenoic", function() {
				self.initBenoic();
			});

			$scope.$on("reinitCarleon", function() {
				self.initCarleon();
			});

			$scope.$on("reinitGareth", function() {
				self.initGareth();
			});

			$scope.$on("closeBenoic", function() {
				sharedData.removeAll("benoicDevices");
			});

			$scope.$on("closeCarleon", function() {
				sharedData.removeAll("carleonServices");
				sagremorParams.profiles = [];
				sagremorParams.currentProfile = false;
			});

			$scope.$on("closeGareth", function() {
				sharedData.removeAll("garethFilters");
				sharedData.removeAll("garethAlertsSmtp");
				sharedData.removeAll("garethAlertsHttp");
			});

			$scope.$on("refresh", function() {
				refreshData();
			});

			$scope.$on("reconnect", function() {
				$scope.$broadcast("oauth:external:refresh");
			});

			$scope.$on("loginSuccess", function() {
				initParameters();
			});

			$scope.$on("initComplete", function() {
				if (self.initComplete) {
					closeLoader(true);
				} else if (self.benoicInitError) {
					closeLoader(false);
				}
			});

			$scope.$on('oauth:login', function(event, token) {
				sharedData.add("oauthToken", "token", token);
				$http.defaults.headers.common.Authorization = "Bearer " + token.access_token;
				initParameters();
			});

			$scope.$on('oauth:refresh', function(event, token) {
				$http.defaults.headers.common.Authorization = "Bearer " + token.access_token;
				sharedData.add("oauthToken", "token", token);
				if (!self.isInit) {
					initParameters();
				}
			});

			$scope.$on('oauth:loggedOut', function(event, token) {
				self.closeSagremor();
			});

			$scope.$on('oauth:denied', function(event, token) {
				toaster.pop("error", $translate.instant("angharad_loading_title"), $translate.instant("access_denied"));
				self.closeSagremor();
			});

			$scope.$on('oauth:expired', function(event, token) {
				$scope.$broadcast("reconnect");
			});

			$scope.$on('oauth:invalid', function(event, message) {
				self.closeSagremor();
			});

			init();
		}
	);
