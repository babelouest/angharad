angular.module("sagremorApp")
		.controller("DashboardCtrl",
				function($scope, $location, $translate, $timeout, toaster, sharedData, sagremorParams, sagremorService, sagremorEdit, angharadFactory, benoicFactory, carleonFactory, sagGenericInjectorManager) {

						var self = this;

						this._timeout = null;
						this.sagremorParams = sagremorParams;
						this.dashboardWidgets = [];
						this.dashboardWidgetsDisplay = [];
						this.profileName = "";
						this.isInit = true;
						this.connected = false;

						this.options = {
								cellHeight: 100,
								verticalHargin: 10,
								disableResize: false
						};

						this.init = function() {
								self.isInit = true;
								self.connected = !!sharedData.get("oauthToken", "token");
								getDashboardElements();
								self.menu = [{
										name: "remove_from_dashboard",
										display: $translate.instant("remove_from_dashboard"),
										action: function(param) {
												self.removeFromDashboard(param);
										}
								}];
						};

						this.menuSelect = function(menuItem, element) {
								menuItem.action(element);
						};

						this.addTitleLine = function() {
								return sagremorEdit.open($translate.instant("dashboard_add_line"), $translate.instant("dashboard_add_line_value"), $translate.instant("dashboard_add_line_placeholder")).then(function(result) {
										newDashboardSeparator(result.value);
								});
						};

						function getDashboardElements() {
								self.dashboardWidgetsDisplay = [];
								var profile = sagremorParams.currentProfile;
								if (!!profile) {
										self.profileName = profile.name;
										if (!!profile.addTo && !!profile.addTo.D && profile.addTo.D.length > 0) {
												getDashboardElementsCurrentProfile();
										}
								} else {
										self.profileName = "";
								}
								for (key in self.dashboardWidgets) {
										if (!!self.dashboardWidgets[key]) {
												self.dashboardWidgetsDisplay.push(self.dashboardWidgets[key]);
										}
								}
						}

						function getDashboardElementsCurrentProfile() {
								var profile = sagremorParams.currentProfile;
								if (!!profile && !!profile.addTo && !!profile.addTo.D) {
										_.forEach(profile.addTo.D, function(element) {
												if (!!element.device) {
														if (sagremorParams.benoicEnabled) {
																addBenoicElementToDashboard(element, element.tag);
														}
												} else if (element.type === "script") {
														addScriptToDashboard(element, element.tag);
												} else if (element.type === "scheduler" || element.type === "trigger") {
														addEventToDashboard(element, element.tag);
												} else if (element.type === "separator") {
														addDashboardSeparator(element.name, element.tag);
												} else if (element.type === "monitor") {
														addMonitorToDashboard(element, element.tag);
												} else if (sagremorParams.carleonEnabled) {
														addCarleonElementToDashboard(element, element.tag);
												}
										});
								}
								self.isInit = false;
						}

						function addBenoicElementToDashboard(element, tag) {
								var tagParams = tag.split("$");
								if (tagParams.length >= 4) {
										var x = tagParams[2];
										var y = tagParams[3];
										var curHeight = 1;
										if (element.type === "dimmer" || element.type === "heater") {
												curHeight = 2;
										}
										var found = _.find(self.dashboardWidgets, function(widget) {
												return widget && widget.type === element.type && widget.device === element.device && widget.name === element.name;
										});
										var bElt = sagremorService.getBenoicElement(element.device, element.type, element.name);
										if (!!bElt) {
												if (!found) {
														var icon = "";
														switch (element.type) {
																case "switch":
																		icon = "toggle-on";
																		break;
																case "dimmer":
																		icon = "lightbulb-o";
																		break;
																case "heater":
																		icon = "fire";
																		break;
																case "sensor":
																		icon = "line-chart";
																		break;
														}
														bElt.device = element.device;
														bElt.name = element.name;
														var index = y * 10 + x;
														var dashboardElement = {
																type: element.type,
																device: element.device,
																name: element.name,
																element: bElt,
																x: x,
																y: y,
																width: 3,
																height: curHeight,
																tag: tag,
																icon: icon
														};
														self.dashboardWidgets.push(dashboardElement);
												} else {
														_.forEach(self.dashboardWidgets, function(widget) {
																if (widget && widget.type === element.type && widget.device === element.device && widget.name === element.name) {
																		bElt.device = element.device;
																		bElt.name = element.name;
																		widget.element = bElt;
																}
														});
												}
										}
								}
						}

						function addScriptToDashboard(element, tag) {
								var elt = sharedData.get("angharadScripts", element.name);
								var found = _.find(self.dashboardWidgets, function(widget) {
										return widget && widget.type === element.type && widget.name === element.name;
								});
								if (!!elt && !found) {
										var tagParams = tag.split("$");
										if (tagParams.length >= 4) {
												var x = tagParams[2];
												var y = tagParams[3];
												var curHeight = 1;
												var icon = "tasks";
												var index = y * 10 + x;
												var dashboardElement = {
														type: element.type,
														name: element.name,
														element: elt,
														x: x,
														y: y,
														width: 3,
														height: curHeight,
														tag: tag,
														icon: icon
												};
												self.dashboardWidgets.push(dashboardElement);
										}
								}
						}

						function addEventToDashboard(element, tag) {
								var elt = false;
								var icon = "";
								var found = _.find(self.dashboardWidgets, function(widget) {
										return widget.type === element.type && widget.name === element.name;
								});
								if (element.type === "scheduler") {
										elt = sharedData.get("angharadSchedulers", element.name);
										icon = "calendar";
								} else {
										elt = sharedData.get("angharadTriggers", element.name);
										icon = "bell";
								}
								if (!!elt && !found) {
										var tagParams = tag.split("$");
										if (tagParams.length >= 4) {
												var x = tagParams[2];
												var y = tagParams[3];
												var curHeight = element.type === "scheduler" ? 2 : 1;
												var index = y * 10 + x;
												var dashboardElement = {
														type: element.type,
														name: element.name,
														element: elt,
														x: x,
														y: y,
														width: 3,
														height: curHeight,
														tag: tag,
														icon: icon
												};
												self.dashboardWidgets.push(dashboardElement);
										}
								}
						}

						function addMonitorToDashboard(element, tag) {
								var elt = _.find(sagremorParams.currentProfile.monitorList, function(monitor) {
										return monitor.name === element.name;
								});
								var found = _.find(self.dashboardWidgets, function(widget) {
										return widget && widget.type === element.type && widget.name === element.name;
								});
								if (!!elt && !found) {
										var tagParams = tag.split("$");
										if (tagParams.length >= 4) {
												var x = tagParams[2];
												var y = tagParams[3];
												var curHeight = 4;
												var icon = "bar-chart";
												var index = y * 10 + x;
												var dashboardElement = {
														type: element.type,
														name: element.name,
														element: elt,
														x: x,
														y: y,
														width: 6,
														height: curHeight,
														tag: tag,
														icon: icon
												};
												self.dashboardWidgets.push(dashboardElement);
										}
								}
						}

						function addCarleonElementToDashboard(element, tag) {
								var injector = _.find(sagGenericInjectorManager.components, function(inject) {
										return inject.type === element.type;
								});
								var service = sharedData.get("carleonServices", element.type);
								var elt = !!service && !!service.element && _.find(service.element, function(cElt) {
										return cElt.name === element.name;
								});
								var found = _.find(self.dashboardWidgets, function(widget) {
										return widget && widget.type === element.type && widget.name === element.name;
								});
								if (!!elt && !found) {
										var tagParams = tag.split("$");
										if (tagParams.length >= 4) {
												var x = tagParams[2];
												var y = tagParams[3];
												var curHeight = injector.widgetHeight;
												var curWidth = injector.widgetWidth;
												var icon = injector.icon;
												var index = y * 10 + x;
												var dashboardElement = {
														type: element.type,
														name: element.name,
														element: element,
														x: x,
														y: y,
														width: curWidth,
														height: curHeight,
														tag: tag,
														icon: icon
												};
												self.dashboardWidgets.push(dashboardElement);
										}
								}
						}

						function addDashboardSeparator(value, tag) {
								var tagParams = tag.split("$");
								if (tagParams.length >= 4) {
										var y = tagParams[3];
										var index = y * 10;
										var dashboardElement = {
												type: "separator",
												name: value,
												x: 0,
												y: y,
												width: 20,
												height: 1,
												tag: tag
										};
										self.dashboardWidgets.push(dashboardElement);
								}
						}

						function newDashboardSeparator(value) {
								var dashboardElement = {
										type: "separator",
										name: value,
										x: 0,
										y: 0,
										width: 10,
										height: 1
								};
								var profile = sagremorParams.currentProfile;
								if (!profile.addTo) {
										profile.addTo = {
												D: []
										};
								}
								if (!profile.addTo.D) {
										profile.addTo.D = [];
								}
								self.dashboardWidgetsDisplay.push(dashboardElement);
								var newElement = {
										type: "separator",
										name: value,
										tag: "SGMR$D$0$0"
								};
								profile.addTo.D.push(newElement);
								angharadFactory.setProfile(profile.name, profile).then(function() {
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
						}

						this.removeFromDashboard = function(w) {
								var profile = sagremorParams.currentProfile;
								if (!!profile && !!profile.addTo && !!profile.addTo.D) {
										_.forEach(profile.addTo.D, function(element, key) {
												if (element && element.type === w.type && element.name === w.name && w.tag === element.tag) {
														profile.addTo.D.splice(key, 1);
												}
										});
								}
								var index = self.dashboardWidgetsDisplay.indexOf(w);
								self.dashboardWidgetsDisplay.splice(index, 1);
								sagremorParams.currentProfile = profile;
								angharadFactory.setProfile(profile.name, profile).then(function() {
										toaster.pop("success", $translate.instant("profile_save"), $translate.instant("profile_save_success"));
								}, function() {
										toaster.pop("error", $translate.instant("profile_save"), $translate.instant("profiles_save_error"));
								});
						};

						this.onChange = function(event, items) {
								if (self._timeout) {
										// if there is already a timeout in process cancel it
										$timeout.cancel(self._timeout);
								}
								self._timeout = $timeout(function() {
										if (!self.isInit) {
												if (sagremorParams.carleonEnabled) {
														var changed = false;
														_.forEach(items, function(item) {
																var element = _.find(self.dashboardWidgetsDisplay, function(widget) {
																		return widget.type === $(item.el).attr("data-sag-type") &&
																				widget.name === $(item.el).attr("data-sag-name") &&
																				(!widget.device || widget.device === $(item.el).attr("data-sag-device"));
																});
																if (!!element) {
																		var newTag = "SGMR$D$" + item.x + "$" + item.y;
																		if (updateTag(element, newTag)) {
																				changed = true;
																				element.tag = newTag;
																		}
																}
														});
														if (changed) {
																var dashboardWidgetsDisplay = _(sagremorParams.currentProfile.addTo.D).chain().sortBy(function(widget) {
																		var splitted = widget.tag.split("$");
																		return splitted[2];
																}).sortBy(function(widget) {
																		var splitted = widget.tag.split("$");
																		return splitted[3];
																}).value();
																sagremorParams.currentProfile.addTo.D = dashboardWidgetsDisplay;

																angharadFactory.setProfile(sagremorParams.currentProfile.name, sagremorParams.currentProfile).then(function() {
																		toaster.pop("success", $translate.instant("profile_save"), $translate.instant("profile_save_success"));
																}, function() {
																		toaster.pop("error", $translate.instant("profile_save"), $translate.instant("profiles_save_error"));
																});
														}
												}
										}
										self._timeout = null;
								}, 5000);
						};

						function updateTag(element, newTag) {
								if (newTag !== element.tag) {
										var profile = sagremorParams.currentProfile;
										if (!!profile) {
												if (!profile.addTo) {
														profile.addTo = {
																D: []
														};
												}
												if (!profile.addTo.D) {
														profile.addTo.D = [];
												}
												var elt = _.find(profile.addTo.D, function(e) {
														return element.tag == e.tag;
												});
												if (!!elt) {
														elt.tag = newTag;
												} else {
														element.tag = newTag;
														profile.addTo.D.push(element);
												}
												return true;
										}
								}
								return false;
						}

						function getProfileElementFromTag(tag) {
								var profile = sagremorParams.currentProfile;
								if (!!profile && !!profile.addTo && !!profile.addTo.D) {
										var elt = _.find(profile.addTo.D, function(element) {
												return element.tag == tag;
										});
										return elt || false;
								}
								return false;
						}

						$scope.$on("angharadProfileUpdated", function() {
								self.isInit = true;
								self.dashboardWidgets = [];
								getDashboardElements();
						});

						$scope.$on("refreshDashboard", function() {
								self.isInit = true;
								getDashboardElements();
						});

						$scope.$on("benoicDevicesChanged", function() {
								self.isInit = true;
								getDashboardElements();
						});

						$scope.$on("carleonServicesChanged", function() {
								self.isInit = true;
								getDashboardElements();
						});

						$scope.$on('oauth:login', function(event, token) {
								self.connected = !!sharedData.get("oauthToken", "token");
						});
						
						$scope.$on('oauth:refresh', function(event, token) {
								self.connected = !!sharedData.get("oauthToken", "token");
						});
						
						$scope.$on('oauth:loggedOut', function(event, token) {
								self.connected = !!sharedData.get("oauthToken", "token");
						});
						
						$scope.$on('oauth:denied', function(event, token) {
								self.connected = !!sharedData.get("oauthToken", "token");
						});
						
						$scope.$on('oauth:expired', function(event, token) {
								self.connected = !!sharedData.get("oauthToken", "token");
						});
						
						$scope.$on('oauth:invalid', function(event, token) {
								self.connected = !!sharedData.get("oauthToken", "token");
						});
						
						this.init();

				});

/**
 * Workaround for using jquery 3 with gridstack (I F****ing hate bower !)
 */
$.fn.size = function() {
		return this.length;
};
