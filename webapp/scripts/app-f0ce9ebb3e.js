
carleonLiquidsoapController.$inject = ["$scope", "$sce", "$q", "$window", "$translatePartialLoader", "$translate", "$interval", "angharadConfig", "carleonFactory", "carleonLiquidsoapFactory", "sagremorService", "sagremorParams", "toaster"];
carleonMockController.$inject = ["$translatePartialLoader", "$translate", "carleonMockFactory", "sagremorParams", "toaster"];
carleonMotionController.$inject = ["$scope", "$http", "$translatePartialLoader", "$translate", "angharadConfig", "angharadFactory", "carleonFactory", "carleonMotionFactory", "sagremorService", "sagremorParams", "toaster"];
carleonMpdController.$inject = ["$scope", "$q", "$window", "$translatePartialLoader", "$translate", "$interval", "angharadConfig", "carleonFactory", "carleonMpdFactory", "benoicFactory", "sagremorParams", "sharedData", "toaster"];
sagContainerController.$inject = ["$rootScope", "$translate", "toaster", "sharedData", "angharadFactory"];
sagDimmerController.$inject = ["benoicFactory", "sagremorParams", "$translate"];
sagSchedulerController.$inject = ["$translate", "toaster", "angharadFactory", "sagremorParams", "sharedData"];
sagTriggerController.$inject = ["$translate", "toaster", "angharadFactory", "sagremorParams", "sharedData"];
sagHeaterController.$inject = ["benoicFactory", "sagremorParams", "$translate"];
sagMonitorController.$inject = ["$scope", "$q", "$translate", "toaster", "angharadFactory", "benoicFactory", "carleonFactory", "sagremorParams", "sagremorConstant", "sharedData"];
sagScriptController.$inject = ["$rootScope", "$translate", "toaster", "angharadFactory", "sagremorParams"];
sagSensorController.$inject = ["benoicFactory", "sagremorParams", "$translate"];
sagSwitchController.$inject = ["benoicFactory", "sagremorParams", "$translate"];
topLeftMenuCtrl.$inject = ["$scope", "$translate", "sagremorParams", "sagGenericInjectorManager", "sharedData"];
topRightMenuCtrl.$inject = ["$scope", "$rootScope", "$location", "$http", "$translate", "$cookies", "angharadFactory", "sagremorConstant", "sagremorParams", "angharadConfig"];angular.module("sagremorApp", [
"ui.bootstrap",
"ui.router", 
"ngCookies",
"angularMoment",
"frapontillo.bootstrap-switch", 
"toaster", 
"ui.bootstrap.contextMenu", 
"pascalprecht.translate",
"ngSanitize",
"ui.bootstrap.datetimepicker",
"datatables",
"angular.img",
"nvd3",
"oauth"
])
.constant("_", window._)
.constant("sagremorConstant", {
    scriptActionElements: [
        {name: "switch", label: "script_action_benoic_switch", submodule: "benoic"},
        {name: "dimmer", label: "script_action_benoic_dimmer", submodule: "benoic"},
        {name: "heater", label: "script_action_benoic_heater", submodule: "benoic"}
    ],
    conditionList: [
        {name: "switch", label: "event_condition_benoic_switch", submodule: "benoic"},
        {name: "dimmer", label: "event_condition_benoic_dimmer", submodule: "benoic"},
        {name: "heater", label: "event_condition_benoic_heater", submodule: "benoic"},
        {name: "sensor", label: "event_condition_benoic_sensor", submodule: "benoic"}
    ],
    monitoredEveryEnum: [
        {value: 60, label: "monitor_1_minute"},
        {value: 120, label: "monitor_2_minutes"},
        {value: 300, label: "monitor_5_minutes"},
        {value: 600, label: "monitor_10_minutes"},
        {value: 900, label: "monitor_15_minutes"},
        {value: 1200, label: "monitor_20_minutes"},
        {value: 1800, label: "monitor_30_minutes"},
        {value: 3600, label: "monitor_1_hour"},
        {value: 7200, label: "monitor_2_hours"},
        {value: 10800, label: "monitor_3_hours"},
        {value: 14400, label: "monitor_4_hours"},
        {value: 18000, label: "monitor_5_hours"},
        {value: 21600, label: "monitor_6_hours"},
        {value: 43200, label: "monitor_12_hours"},
        {value: 86400, label: "monitor_1_day"}
    ],
    schedulerRepeatEveryEnum: [
        {value: 0, label: "event_scheduler_repeat_every_minutes"},
        {value: 1, label: "event_scheduler_repeat_every_hours"},
        {value: 2, label: "event_scheduler_repeat_every_days"},
        {value: 3, label: "event_scheduler_repeat_every_days_of_week"},
        {value: 4, label: "event_scheduler_repeat_every_months"},
        {value: 5, label: "event_scheduler_repeat_every_year"},
    ],
    langList: [
        {name: "fr", display: "Fr"},
        {name: "en", display: "En"}
    ],
    durationList: [
        {value: 0, label: "duration_last_hour"},
        {value: 1, label: "duration_last_2_hours"},
        {value: 2, label: "duration_last_6_hours"},
        {value: 3, label: "duration_last_12_hours"},
        {value: 4, label: "duration_last_day"},
        {value: 5, label: "duration_last_2_days"},
        {value: 6, label: "duration_last_3_days"},
        {value: 7, label: "duration_last_7_days"},
        {value: 8, label: "duration_last_month"},
    ],
    colorList: [
		{ style:"background:Aqua", value:"#00FFFF", label:"Aqua"},
		{ style:"background:Brown", value:"#A52A2A", label:"Brown"},
		{ style:"background:Blue", value:"#0000FF", label:"Blue"},
		{ style:"background:Chartreuse", value:"#7FFF00", label:"Chartreuse"},
		{ style:"background:DarkGreen", value:"#006400", label:"Dark Green"},
		{ style:"background:DarkOrange", value:"#FF8C00", label:"Dark Orange"},
		{ style:"background:DarkRed", value:"#8B0000", label:"Dark Red"},
		{ style:"background:DarkTurquoise", value:"#00CED1", label:"Dark Turquoise"},
		{ style:"background:DarkViolet", value:"#9400D3", label:"Dark Violet"},
		{ style:"background:Green", value:"#008000", label:"Green"},
		{ style:"background:LawnGreen", value:"#7CFC00", label:"Lawn Green"},
		{ style:"background:LightGreen", value:"#90EE90", label:"Light Green"},
		{ style:"background:LightSeaGreen", value:"#20B2AA", label:"Light Sea Green"},
		{ style:"background:LightSlateGray", value:"#778899", label:"Light Slate Gray"},
		{ style:"background:Lime", value:"#00FF00", label:"Lime"},
		{ style:"background:Magenta", value:"#FF00FF", label:"Magenta"},
		{ style:"background:MediumBlue", value:"#0000CD", label:"Medium Blue"},
		{ style:"background:MediumSpringGreen", value:"#00FA9A", label:"Medium Spring Green"},
		{ style:"background:Navy", value:"#000080", label:"Navy"},
		{ style:"background:Orange", value:"#FFA500", label:"Orange"},
		{ style:"background:Purple", value:"#800080", label:"Purple"},
		{ style:"background:Red", value:"#FF0000", label:"Red"},
		{ style:"background:Teal", value:"#008080", label:"Teal"},
		{ style:"background:Turquoise", value:"#40E0D0", label:"Turquoise"},
		{ style:"background:Violet", value:"#EE82EE", label:"Violet"},
		{ style:"background:Yellow", value:"#FFFF00", label:"Yellow"},
		{ style:"background:YellowGreen", value:"#9ACD32", label:"Yellow Green"}
    ]

})
.config(["$translateProvider", "$translatePartialLoaderProvider", function ($translateProvider, $translatePartialLoaderProvider) {
	$translateProvider.useLoader("$translatePartialLoader", {
		urlTemplate: "components/{part}/i18n/{lang}.json"
	});
	$translatePartialLoaderProvider.addPart("core");
	$translateProvider
	// see: http://angular-translate.github.io/docs/#/guide/09_language-negotiation
	.registerAvailableLanguageKeys(['en', 'fr'], {
	  'en_US': 'en',
	  'en_UK': 'en',
	  'en_CA': 'en',
	  'fr_FR': 'fr',
	  'fr_CA': 'fr',
	  'fr_BE': 'fr',
	  '*': 'en'
	  })
	.fallbackLanguage('en')
	.determinePreferredLanguage();

	// interpolation for pluralization
	$translateProvider.useCookieStorage();
	$translateProvider.useSanitizeValueStrategy("escape");
}])
.factory("sharedData", function() {
    var sharedData = {};
    
    var add = function(share, name, data) {
        if (!sharedData[share]) {
            sharedData[share] = {};
        }
        sharedData[share][name] = data;
    };
    
    var get = function(share, name) {
        if (!!sharedData[share] && !!sharedData[share][name]) {
            return sharedData[share][name];
        } else {
            return undefined;
        }
    };
    
    var remove = function(share, name) {
        return delete sharedData[share][name];
    };
    
    var removeAll = function (share) {
		sharedData[share] = {};
	};
    
    var all = function(share) {
        return sharedData[share];
    };
    
    return {
        all: all,
        get: get,
        add: add,
        remove: remove,
        removeAll: removeAll
    };
})
.factory("sagremorParams", function () {
    var params = {};
    
    return params;
})
.factory("sagremorConfirm", ["$uibModal", function ($uibModal) {
    
    var open = function (title, message) {
        return modalInstance = $uibModal.open({
            animation: true,
            templateUrl: "components/modals/confirm.modal.html",
            controller: "ConfirmModalCtrl",
            controllerAs: "ConfirmModalCtrl",
            size: "sm",
            resolve: {
                title: function () {
                    return title;
                },
                message: function () {
                    return message;
                }
            }
        }).result;
    };
    
    return {open: open};
}])
.factory("sagremorEdit", ["$uibModal", function ($uibModal) {
    
    var open = function (title, message, placeholder) {
        return modalInstance = $uibModal.open({
            animation: true,
            templateUrl: "components/modals/edit.modal.html",
            controller: "EditModalCtrl",
            controllerAs: "EditModalCtrl",
            size: "sm",
            resolve: {
                title: function () {
                    return title;
                },
                message: function () {
                    return message;
                },
                placeholder: function () {
                    return placeholder;
                }
            }
        }).result;
    };
    
    return {open: open};
}])
.service("sagGenericInjectorManager", function () {
    var self = this;
    self.components = [];
    
    this.add = function (config) {
		if (!self.get(config.type)) {
			self.components.push(config);
		}
    };
    
    this.get = function (type) {
		return _.find(self.components, function (component) {
			return component.type === type;
		});
	};
});

angular.module("sagremorApp")
    .config(["$locationProvider", function($locationProvider) {
        $locationProvider.html5Mode(true).hashPrefix('!');
    }])
    .config(["$stateProvider", "$urlRouterProvider", function ($stateProvider, $urlRouterProvider) {
        
        $stateProvider
            .state("dashboard", {
                url: "/",
                templateUrl: "app/dashboard/dashboard.html",
                controller: "DashboardCtrl",
                controllerAs: "DashboardCtrl"
            })
            .state("error", {
                url: "/error",
                templateUrl: "app/error/error.html",
                controller: "ErrorCtrl",
                controllerAs: "ErrorCtrl"
            })
            .state("switches", {
                url: "/switches",
                templateUrl: "app/switches/switches.html",
                controller: "switchesCtrl",
                controllerAs: "switchesCtrl"
            })
            .state("heaters", {
                url: "/heaters",
                templateUrl: "app/heaters/heaters.html",
                controller: "heatersCtrl",
                controllerAs: "heatersCtrl"
            })
            .state("sensors", {
                url: "/sensors",
                templateUrl: "app/sensors/sensors.html",
                controller: "sensorsCtrl",
                controllerAs: "sensorsCtrl"
            })
            .state("service", {
                url: "/service/:service",
                templateUrl: "app/service/service.html",
                controller: "serviceCtrl",
                controllerAs: "serviceCtrl"
            })
            .state("scripts", {
                url: "/scripts",
                templateUrl: "app/scripts/scripts.html",
                controller: "scriptsCtrl",
                controllerAs: "scriptsCtrl"
            })
            .state("events", {
                url: "/events",
                templateUrl: "app/events/events.html",
                controller: "eventsCtrl",
                controllerAs: "eventsCtrl"
            })
            .state("monitors", {
                url: "/monitors",
                templateUrl: "app/monitors/monitors.html",
                controller: "monitorsCtrl",
                controllerAs: "monitorsCtrl"
            })
            .state("logs", {
                url: "/logs",
                templateUrl: "app/logs/logs.html",
                controller: "logsCtrl",
                controllerAs: "logsCtrl"
            })
            .state("parameters", {
                url: "/parameters",
                templateUrl: "app/parameters/parameters.html",
                controller: "ParametersCtrl",
                controllerAs: "ParametersCtrl"
            });
            
        $urlRouterProvider.otherwise("/");
    }]);

angular.module("sagremorApp")
		.controller("DashboardCtrl",
				["$scope", "$location", "$translate", "$timeout", "toaster", "sharedData", "sagremorParams", "sagremorService", "sagremorEdit", "angharadFactory", "benoicFactory", "carleonFactory", "sagGenericInjectorManager", function($scope, $location, $translate, $timeout, toaster, sharedData, sagremorParams, sagremorService, sagremorEdit, angharadFactory, benoicFactory, carleonFactory, sagGenericInjectorManager) {

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

				}]);

/**
 * Workaround for using jquery 3 with gridstack (I F****ing hate bower !)
 */
$.fn.size = function() {
		return this.length;
};

angular.module("sagremorApp")
	.controller("ErrorCtrl",
	["$scope", "sagremorParams", function($scope, sagremorParams) {

		var self = this;

		this.errorMessage = "";

		this.init = function() {
			if (!!sagremorParams.errorMessage) {
				self.errorMessage = sagremorParams.errorMessage;
			}
		};

		$scope.$on("generalError", function () {
			self.init();
		});

		self.init();
	}]
);

angular.module("sagremorApp")
    .controller("eventsCtrl",
    ["$scope", "$translate", "sagremorService", "angharadFactory", "sharedData", "sagremorParams", function($scope, $translate, sagremorService, angharadFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.schedulerList = [];
        this.triggerList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {

			$translate(["edit", "remove", "add_to_dashboard"]).then(function (results) {
				self.menuScheduler = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editEvent(param, "scheduler");
						}
					},
					{
						name: "remove", 
						display: results.remove, 
						action: function (param) {
							sagremorService.removeScheduler(param).then(function () {
								$scope.$broadcast("angharadSchedulersChanged");
							});
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							param.type = !!param.next_time?"scheduler":"trigger";
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				self.menuTrigger = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editEvent(param, "trigger");
						}
					},
					{
						name: "remove", 
						display: results.remove, 
						action: function (param) {
							sagremorService.removeTrigger(param).then(function () {
								$scope.$broadcast("angharadTriggersChanged");
							});
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							param.type = !!param.next_time?"scheduler":"trigger";
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				
				self.updateSchedulers();
				self.updateTriggers();
			});
		};
        
        this.updateSchedulers = function () {
            var events = sharedData.all("angharadSchedulers");
            self.schedulerList = [];
            for (key in events) {
				self.schedulerList.push(events[key]);
            }
        };
        
        this.updateTriggers = function () {
            var events = sharedData.all("angharadTriggers");
            self.triggerList = [];
            for (key in events) {
				self.triggerList.push(events[key]);
            }
        };
        
        this.refreshEvents = function () {
			var schedulers = sharedData.all("angharadSchedulers");
            for (key in schedulers) {
				var found = _.find(self.schedulerList, function (scheduler) {
					return scheduler.name === key;
				});
				
				if (!found) {
					self.schedulerList.push(schedulers[key]);
				}
            }
            
            for (var index = self.schedulerList.length - 1; index >= 0; index--) {
				if (!sharedData.get("angharadSchedulers", self.schedulerList[index].name)) {
					self.schedulerList.splice(index, 1);
				}
			}

			var triggers = sharedData.all("angharadTriggers");
            for (key in triggers) {
				var found = _.find(self.triggerList, function (trigger) {
					return trigger.name === key;
				});
				
				if (!found) {
					self.triggerList.push(triggers[key]);
				}
            }
            
            for (var index = self.triggerList.length - 1; index >= 0; index--) {
				if (!sharedData.get("angharadTriggers", self.triggerList[index].name)) {
					self.triggerList.splice(index, 1);
				}
			}
		};
        
        this.newEvent = function () {
			sagremorService.editEvent(null);
		};
		
        $scope.$on("angharadSchedulersChanged", function () {
            self.updateSchedulers();
        });
        
        $scope.$on("angharadTriggersChanged", function () {
            self.updateTriggers();
        });
        
        $scope.$on("refreshAngharadEvents", function () {
            self.refreshEvents();
        });
        
        this.init();
        
    }]
);

angular.module("sagremorApp")
    .controller("heatersCtrl",
    ["$scope", "$location", "$translate", "sagremorService", "benoicFactory", "sharedData", "sagremorParams", function($scope, $location, $translate, sagremorService, benoicFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.heaterList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {
			if (!sagremorParams.loggedIn) {
				$location.path("/login");
			}
			$translate(["edit", "monitor", "add_to_dashboard"]).then(function (results) {
				self.menuSensor = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editHeater(param);
						}
					},
					{
						name: "monitor", 
						display: results.monitor, 
						action: function (param) {
							sagremorService.monitor(param);
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				
				self.updateHeaters();
			});
		};
        
        $scope.$on("benoicDevicesChanged", function () {
            self.updateHeaters();
        });
        
        $scope.$on("benoicHeatersChanged", function () {
            self.updateHeaters();
        });
        
        this.updateHeaters = function () {
            var devices = sharedData.all("benoicDevices");
			self.heaterList = [];
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.heaters) {
                        var heaters = devices[key].element.heaters[se];
                        heaters.device = deviceName;
                        heaters.name = se;
                        self.heaterList.push(heaters);
                    }
                }
            }
        };
        
        $scope.$on("refreshDevices", function () {
            self.refreshElements();
        });
        
        this.refreshElements = function () {
			// Add all elements that are not in self.heaterList
			var devices = sharedData.all("benoicDevices");
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.heaters) {
						var index = 0;
						var found = _.find(self.heaterList, function (element) {
							return element.name === se && element.device === deviceName;
						});
						if (!found) {
							var heater = devices[key].element.heaters[se];
							heater.device = deviceName;
							heater.name = se;
							self.heaterList.push(heater);
						} else {
							var heater = devices[key].element.heaters[se];
							heater.device = deviceName;
							heater.name = se;
							var index = _.indexOf(self.heaterList, _.find(self.heaterList, {device: deviceName, name: se}));
							self.heaterList.splice(index, 1, heater);
						}
                    }
                }
            }
            
            // Remove all elements from self.heaterList that are no longer in devices
            _.remove(self.heaterList, function (heater) {
				var found = _.find(sharedData.all("benoicDevices"), function (device, deviceName) {
					return !device.element || _.find(device.element.heaters, function (element, eltName) {
						return eltName === heater.name && deviceName === heater.device;
					});
				});
				
				if (!found) {
					return true;
				}
			});
		};
        
        this.init();
        
    }]
);

angular.module("sagremorApp")
    .controller("FiltersModalCtrl",
    ["$scope", "$q", "$uibModalInstance", "$translate", "toaster", "DTOptionsBuilder", "garethFactory", "sharedData", function($scope, $q, $uibModalInstance, $translate, toaster, DTOptionsBuilder, garethFactory, sharedData) {
        var self = this;
        
        this.filterList = sharedData.all("garethFilters");
        this.AlertSmtpList = sharedData.all("garethAlertsSmtp");
        this.AlertHttpList = sharedData.all("garethAlertsHttp");
        this.editFilter = false;
        this.addFilter = false;
        this.newClause = false;
        this.newAlertSmtp = false;
        this.newAlertHttp = false;
        this.newAlertHttpHeader = false;
        this.displayAlert = false;
        this.customFilter = {
			name: "",
			description: "",
			filter_clauses: [
			],
			filter_alerts: {
				smtp: [],
				http: []
			}
		};
		
		self.menu = [
			{
				name: "smtp", 
				display: $translate.instant("logs_menu_smtp"), 
				action: function () {
					self.addAlertSmtp();
				}
			},
			{
				name: "http", 
				display: $translate.instant("logs_menu_http"), 
				action: function () {
					self.addAlertHttp();
				}
			}
		];
		
		self.menuSelect = function(menuItem, element) {
			menuItem.action(element);
		};
    
		this.customClause = {
			element: "",
			operator: "",
			value: ""
		};
		
		this.customHttpAlert = {
			name: "",
			method: "",
			url: "",
			body: "",
			http_headers: [
			]
		};
		this.customHttpAlertHeader = {
			key: "",
			value: ""
		};
		this.existingHttpAlert = "";
		this.customHttpAlertMethodSelect = "GET";
		
		this.customSmtpAlert = {
			name: "",
			host: "",
			port: 0,
			tls: false,
			check_ca: false,
			user: "",
			password: "",
			from: "",
			to: "",
			cc: "",
			bcc: "",
			subject: "",
			body: ""
		};
		this.existingSmtpAlert = "";
        
        function init() {
        }
        
		$scope.$on("garethChange", function () {
			self.filterList = sharedData.all("garethFilters");
			self.AlertList = sharedData.all("garethAlerts");
		});
        
        this.newFilter = function () {
			self.editFilter = true;
			self.addFilter = true;
			self.customFilter = {
				name: "",
				description: "",
				filter_clauses: [
				],
				filter_alerts: {
					smtp: [],
					http: []
				}
			};
		};
		
		this.addClause = function () {
			self.newClause = true;
		};
		
		this.isClauseValid = function () {
			return !!self.customClause.element && !!self.customClause.operator && !!self.customClause.value;
		};
		
		this.addNewClause = function () {
			if (self.customClause.element === "priority") {
				self.customClause.value = parseInt(self.customClause.value);
			}
			self.customFilter.filter_clauses.push(self.customClause);
			self.customClause = {
				element: "",
				operator: "",
				value: ""
			};
			self.newClause = false;
		};
		
		this.removeClause = function (index){
			self.customFilter.filter_clauses.splice(index, 1);
		};
		
		this.cancelAddClause = function () {
			self.newClause = false;
		};
		
		this.getClauseElement = function (element) {
			return $translate.instant("clause_option_" + element);
		};
        
		this.getClauseOperator = function (operator) {
			switch (operator) {
				case "=":
					return $translate.instant("clause_operator_equal");
					break;
				case "!=":
					return $translate.instant("clause_operator_different");
					break;
				case "<":
					return $translate.instant("clause_operator_lower");
					break;
				case "<=":
					return $translate.instant("clause_operator_lower_equal");
					break;
				case ">":
					return $translate.instant("clause_operator_higher");
					break;
				case ">=":
					return $translate.instant("clause_operator_higher_equal");
					break;
				case "contains":
					return $translate.instant("clause_operator_contains");
					break;
				case "notcontains":
					return $translate.instant("clause_operator_notcontains");
					break;
			}
		};
		
		this.getClauseValue = function (clause) {
			if (clause.element === "priority") {
				switch (clause.value) {
					case 0:
						return $translate.instant("logs_priority_none");
						break;
					case 1:
						return $translate.instant("logs_priority_low");
						break;
					case 2:
						return $translate.instant("logs_priority_medium");
						break;
					case 3:
						return $translate.instant("logs_priority_high");
						break;
					case 4:
						return $translate.instant("logs_priority_critical");
						break;
				}
			} else {
				return clause.value;
			}
		};
		
		this.addAlertSmtp = function () {
			self.newAlertSmtp = true;
		};
		
		this.addAlertHttp = function () {
			self.newAlertHttp = true;
		};
		
		this.isSmtpAlertValid = function () {
			if (!!self.existingSmtpAlert) {
				return true;
			}
			var found = _.find(self.AlertSmtpList, function (alert) {
				return alert.name === self.customSmtpAlert.name;
			});
			if (found) {
				return false;
			}
			return !!self.customSmtpAlert.name && 
					!!self.customSmtpAlert.host && 
					!!self.customSmtpAlert.from && 
					!!self.customSmtpAlert.to && 
					!!self.customSmtpAlert.subject && 
					!!self.customSmtpAlert.body;
		};
		
		this.cancelAddSmtpAlert = function () {
			self.newAlertSmtp = false;
		};
		
		this.postNewSmtpAlert = function () {
			if (!!self.existingSmtpAlert) {
				self.customFilter.filter_alerts.smtp.push(self.existingSmtpAlert.name);
				self.existingSmtpAlert = "";
				self.newAlertSmtp = false;
			} else {
				garethFactory.addAlert("smtp", self.customSmtpAlert).then(function () {
					self.customFilter.filter_alerts.smtp.push(self.customSmtpAlert.name);
					sharedData.add("garethAlertsSmtp", self.customSmtpAlert.name, self.customSmtpAlert);
					toaster.pop("success", $translate.instant("logs_alert"), $translate.instant("logs_alert_save_success"));
					$scope.$broadcast("garethChange");
				}, function () {
					toaster.pop("error", $translate.instant("logs_alert"), $translate.instant("logs_alert_save_error"));
				})["finally"](function () {
					self.customSmtpAlert = {
						name: "",
						host: "",
						port: 0,
						tls: false,
						check_ca: false,
						user: "",
						password: "",
						from: "",
						to: "",
						subject: "",
						body: ""
					};
					self.newAlertSmtp = false;
				});
			}
		};
		
		this.addAlertHttpHeader = function () {
			self.newAlertHttpHeader = true;
		};
		
		this.cancelAddAlertHttpHeader = function () {
			self.newAlertHttpHeader = false;
		};
		
		this.addNewAlertHttpHeader = function () {
			self.customHttpAlert.http_headers.push(self.customHttpAlertHeader);
			self.customHttpAlertHeader = {
				key: "",
				value: ""
			};
			self.newAlertHttpHeader = false;
		};
		
		this.isHttpAlertValid = function () {
			if (!!self.existingHttpAlert) {
				return true;
			}
			var found = _.find(self.AlertHttpList, function (alert) {
				return alert.name === self.customHttpAlert.name;
			});
			if (found) {
				return false;
			}
			return !!self.customHttpAlert.name && 
					(!!self.customHttpAlert.method || !!self.customHttpAlertMethodSelect) && 
					!!self.customHttpAlert.url;
		};
		
		this.postNewHttpAlert = function () {
			if (!!self.existingHttpAlert) {
				self.customFilter.filter_alerts.http.push(self.existingHttpAlert.name);
				self.existingHttpAlert = "";
				self.newAlertHttp = false;
			} else {
				if (self.customHttpAlertMethodSelect !== "custom") {
					self.customHttpAlert.method = self.customHttpAlertMethodSelect;
				}
				garethFactory.addAlert("http", self.customHttpAlert).then(function () {
					self.customFilter.filter_alerts.http.push(self.customHttpAlert.name);
					sharedData.add("garethAlertsHttp", self.customHttpAlert.name, self.customHttpAlert);
					toaster.pop("success", $translate.instant("logs_alert"), $translate.instant("logs_alert_save_success"));
					$scope.$broadcast("garethChange");
				}, function () {
					toaster.pop("error", $translate.instant("logs_alert"), $translate.instant("logs_alert_save_error"));
				})["finally"](function () {
					self.customHttpAlertHeader = {
						key: "",
						value: ""
					};
					self.customHttpAlert = {
						name: "",
						method: "",
						url: "",
						body: "",
						http_headers: [
						]
					};
					self.newAlertHttp = false;
					self.newAlertHttpHeader = false;
				});
			}
		};
		
		this.cancelAddHttpAlert = function () {
			self.newAlertHttp = false;
		};
		
		this.removeAlert = function (type, index) {
			self.customFilter.filter_alerts[type].splice(index, 1);
		};
		
		this.isFilterValid = function () {
			return !!self.customFilter.name && self.customFilter.filter_clauses.length > 0;
		};
		
		this.cancelNewFilter = function () {
			self.editFilter = false;
			self.addFilter = false;
		};
		
		this.postNewFilter = function () {
			var promise = null;
			if (self.addFilter) {
				promise = garethFactory.addFilter(self.customFilter);
			} else {
				promise = garethFactory.setFilter(self.customFilter.name, self.customFilter)
			}
			promise.then(function () {
				sharedData.add("garethFilters", self.customFilter.name, self.customFilter);
				toaster.pop("success", $translate.instant("logs_filter"), $translate.instant("logs_filter_save_success"));
				$scope.$broadcast("garethChange");
			}, function () {
				toaster.pop("error", $translate.instant("logs_filter"), $translate.instant("logs_filter_save_error"));
			})["finally"](function () {
				self.editFilter = false;
				self.addFilter = false;
			});
		};
		
		this.removeCustomFilter = function (filter) {
			garethFactory.removeFilter(filter.name).then(function () {
				sharedData.remove("garethFilters", filter.name);
				toaster.pop("success", $translate.instant("logs_filter"), $translate.instant("logs_filter_remove_success"));
				$scope.$broadcast("garethChange");
			}, function () {
				toaster.pop("error", $translate.instant("logs_filter"), $translate.instant("logs_filter_remove_error"));
			});
		};
		
		this.editCustomFilter = function (filter) {
			self.customFilter = filter;
			self.editFilter = true;
			self.addFilter = false;
		};
		
		this.showAlerts = function () {
			self.displayAlert = true;
		};
		
		this.showFilters = function () {
			self.displayAlert = false;
		};
		
		this.removeSmtpAlert = function (alert) {
			garethFactory.removeAlert("smtp", alert.name).then(function () {
				sharedData.remove("garethAlertsSmtp", alert.name);
				toaster.pop("success", $translate.instant("logs_alert"), $translate.instant("logs_alert_remove_success"));
				$scope.$broadcast("garethChange");
			}, function () {
				toaster.pop("error", $translate.instant("logs_alert"), $translate.instant("logs_alert_remove_error"));
			});
		};
		
		this.removeHttpAlert = function (alert) {
			garethFactory.removeAlert("http", alert.name).then(function () {
				sharedData.remove("garethAlertsHttp", alert.name);
				toaster.pop("success", $translate.instant("logs_alert"), $translate.instant("logs_alert_remove_success"));
				$scope.$broadcast("garethChange");
			}, function () {
				toaster.pop("error", $translate.instant("logs_alert"), $translate.instant("logs_alert_remove_error"));
			});
		};
		
		this.close = function () {
			$uibModalInstance.close("ok");
		};
        
        init();
    }]
);

angular.module("sagremorApp")
    .controller("logsCtrl",
    ["$scope", "$translate", "$uibModal", "toaster", "DTOptionsBuilder", "garethFactory", "sharedData", function($scope, $translate, $uibModal, toaster, DTOptionsBuilder, garethFactory, sharedData) {
      
        var self = this;
        
        this.messageList = [];
        this.filterList = [];
        this.options = {};
        
        this.searchOptions = {
			limit: 20,
			offset: 0
		};
		
		$scope.$on("garethChange", function () {
			self.getFilters();
			self.search();
		});
        
        this.init = function () {
			if ($translate.use()) {
				self.options = DTOptionsBuilder.newOptions()
								.withLanguageSource("components/core/i18n/datatables."+$translate.use()+".json")
								.withOption("order", [1, "desc"]);
				self.getFilters();
				self.search();
			}
		};
		
		
		this.getPriority = function (message) {
			switch (message.priority) {
				case "NONE":
					return $translate.instant("logs_priority_none");
					break;
				case "LOW":
					return $translate.instant("logs_priority_low");
					break;
				case "MEDIUM":
					return $translate.instant("logs_priority_medium");
					break;
				case "HIGH":
					return $translate.instant("logs_priority_high");
					break;
				case "CRITICAL":
					return $translate.instant("logs_priority_critical");
					break;
				default:
					return $translate.instant("logs_priority_not_found");
					break;
			}
		};
		
		this.search = function () {
			if (self.searchOptions.limit > 0 && self.searchOptions.offset >= 0) {
				if (!!self.searchOptions.filter) {
					garethFactory.getFilteredMessages(self.searchOptions.filter.name, self.searchOptions.limit, self.searchOptions.offset).then(function (result) {
						self.messageList = result;
					});
				} else {
					garethFactory.getAllMessages(self.searchOptions.limit, self.searchOptions.offset).then(function (result) {
						self.messageList = result;
					});
				}
			} else {
				toaster.pop("error", $translate.instant("logs_search"), $translate.instant("logs_search_error"));
			}
		};
		
		this.getFilters = function () {
			self.filterList = sharedData.all("garethFilters");
		};
		
		this.filterManagement = function () {
			var modalInstance = $uibModal.open({
				animation: true,
				templateUrl: "app/logs/filters.modal.html",
				controller: "FiltersModalCtrl",
				controllerAs: "FiltersModalCtrl",
				size: "md",
			});
		};
		
		$scope.$on("changeLang", function () {
			self.options = DTOptionsBuilder.newOptions()
							.withLanguageSource("components/core/i18n/datatables."+$translate.use()+".json");
		});
		
        this.init();
        
    }]
);

angular.module("sagremorApp")
    .controller("ParametersCtrl",
    ["$scope", "$rootScope", "$q", "$location", "$translate", "$cookies", "$window", "toaster", "angharadFactory", "benoicFactory", "carleonFactory", "sharedData", "sagremorConfirm", "sagremorParams", function($scope, $rootScope, $q, $location, $translate, $cookies, $window, toaster, angharadFactory, benoicFactory, carleonFactory, sharedData, sagremorConfirm, sagremorParams) {
      
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
    }]
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

angular.module("sagremorApp")
    .controller("monitorsCtrl",
    ["$scope", "$translate", "toaster", "sagremorService", "angharadFactory", "carleonFactory", "sagremorParams", "sagremorEdit", function($scope, $translate, toaster, sagremorService, angharadFactory, carleonFactory, sagremorParams, sagremorEdit) {
      
        var self = this;
        
        this.monitorList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {

			$translate(["edit", "remove", "add_to_dashboard"]).then(function (results) {
				self.menu = [
					{
						name: "remove", 
						display: results.remove, 
						action: function (param) {
							sagremorService.removeMonitor(param).then(function () {
								$scope.$broadcast("angharadProfileChanged");
							});
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							param.type = "monitor";
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				self.updateMonitors();
			});
		};
        
        $scope.$on("angharadProfileChanged", function () {
            self.updateMonitors();
        });
        
        this.updateMonitors = function () {
			var profile = sagremorParams.currentProfile;
			if (!!profile && !!profile.monitorList) {
				self.monitorList = profile.monitorList;
			}
        };
        
        this.newMonitor = function () {
			return sagremorEdit.open($translate.instant("monitor_new_title"), $translate.instant("monitor_new_value"), $translate.instant("monitor_new_placeholder")).then(function (result) {
				addMonitor(result.value);
			});
		};
		
		function addMonitor(name) {
			if (sagremorParams.currentProfile) {
				var exist = _.find(sagremorParams.currentProfile.monitorList, function (monitor) {
					return monitor.name === name;
				});
				if (!sagremorParams.currentProfile.monitorList) {
					sagremorParams.currentProfile.monitorList = [];
				}
				if (!exist && !!name) {
					var monitor = {name: name, elements: []};
					sagremorParams.currentProfile.monitorList.push(monitor);
					angharadFactory.saveCurrentProfile().then(function() {
						toaster.pop("success", $translate.instant("monitor_save"), $translate.instant("monitor_save_success"));
					}, function () {
						toaster.pop("error", $translate.instant("monitor_save"), $translate.instant("monitor_save_error"));
					})["finally"](function () {
						$scope.$broadcast("angharadProfileChanged");
					});
				}
			}
		}
        
        this.init();
        
    }]
);

angular.module("sagremorApp")
    .controller("scriptsCtrl",
    ["$scope", "$translate", "sagremorService", "angharadFactory", "sharedData", "sagremorParams", function($scope, $translate, sagremorService, angharadFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.scriptList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {
					$translate(["edit", "monitor", "bind_to_element", "add_to_dashboard"]).then(function (results) {
						self.menuScript = [
							{
								name: "edit", 
								display: $translate.instant("edit"), 
								action: function (param) {
									sagremorService.editScript(param);
								}
							},
							{
								name: "remove", 
								display: $translate.instant("remove"),
								action: function (param) {
									sagremorService.removeScript(param).then(function () {
										$scope.$broadcast("angharadScriptsChanged");
									});
								}
							},
							{
								name: "bind_to_element", 
								display: $translate.instant("bind_to_element"),
								action: function (param) {
									sagremorService.bindToElement(param);
								}
							},
							{
								name: "add_to_dashboard", 
								display: $translate.instant("add_to_dashboard"),
								action: function (param) {
									param.type = "script";
									sagremorService.addToDashboard(param).then(function () {
										$scope.$broadcast("refreshDashboard");
									});
								}
							}
						];
						self.updateScripts();
					});
				};
        
        $scope.$on("angharadScriptsChanged", function () {
            self.updateScripts();
        });
        
        this.updateScripts = function () {
            var scripts = sharedData.all("angharadScripts");
            self.scriptList = [];
            for (key in scripts) {
				self.scriptList.push(scripts[key]);
            }
        };
        
        $scope.$on("refreshAngharadScripts", function () {
            self.refreshScripts();
        });
        
        this.refreshScripts = function () {
					var scripts = sharedData.all("angharadScripts");
								for (key in scripts) {
						var found = _.find(self.scriptList, function (script) {
							return script.name === key;
						});
						
						if (!found) {
							self.scriptList.push(scripts[key]);
						}
								}
								
								for (var index = self.scriptList.length - 1; index >= 0; index--) {
						if (!sharedData.get("angharadScripts", self.scriptList[index].name)) {
							self.scriptList.splice(index, 1);
						}
					}
				};
        
        this.newScript = function () {
					sagremorService.editScript(null);
				};
		
        this.init();
        
    }]
);

angular.module("sagremorApp")
    .controller("sensorsCtrl",
    ["$scope", "$location", "$translate", "sagremorService", "benoicFactory", "sharedData", "sagremorParams", function($scope, $location, $translate, sagremorService, benoicFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.sensorList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {
			if (!sagremorParams.loggedIn) {
				$location.path("/login");
			}
			$translate(["edit", "monitor", "add_to_dashboard"]).then(function (results) {
				self.menuSensor = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editSensor(param);
						}
					},
					{
						name: "monitor", 
						display: results.monitor, 
						action: function (param) {
							sagremorService.monitor(param);
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				
				self.updateSensors();
			});
		};
        
        $scope.$on("benoicDevicesChanged", function () {
            self.updateSensors();
        });
        
        $scope.$on("benoicSensorsChanged", function () {
            self.updateSensors();
        });
        
        this.updateSensors = function () {
            var devices = sharedData.all("benoicDevices");
			self.sensorList = [];
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.sensors) {
                        var sensors = devices[key].element.sensors[se];
                        sensors.device = deviceName;
                        sensors.name = se;
                        self.sensorList.push(sensors);
                    }
                }
            }
        };
        
        $scope.$on("refreshDevices", function () {
            self.refreshElements();
        });
        
        this.refreshElements = function () {
			// Add all elements that are not in self.sensorList
			var devices = sharedData.all("benoicDevices");
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.sensors) {
						var index = 0;
						var found = _.find(self.sensorList, function (element) {
							return element.name === se && element.device === deviceName;
						});
						if (!found) {
							var sensor = devices[key].element.sensors[se];
							sensor.device = deviceName;
							sensor.name = se;
							self.sensorList.push(sensor);
						} else {
							var sensor = devices[key].element.sensors[se];
							sensor.device = deviceName;
							sensor.name = se;
							var index = _.indexOf(self.sensorList, _.find(self.sensorList, {device: deviceName, name: se}));
							self.sensorList.splice(index, 1, sensor);
						}
                    }
                }
            }
            
            // Remove all elements from self.sensorList that are no longer in devices
            _.remove(self.sensorList, function (sensor) {
				var found = _.find(sharedData.all("benoicDevices"), function (device, deviceName) {
					return !device.element || _.find(device.element.sensors, function (element, eltName) {
						return eltName === sensor.name && deviceName === sensor.device;
					});
				});
				
				if (!found) {
					return true;
				}
			});
		};
        
        this.init();
        
    }]
);

angular.module("sagremorApp")
    .controller("serviceCtrl",
    ["$scope", "$stateParams", "$location", "$translate", "$uibModal", "toaster", "sagGenericInjectorManager", "sagremorService", "carleonFactory", "sharedData", "sagremorParams", function($scope, $stateParams, $location, $translate, $uibModal, toaster, sagGenericInjectorManager, sagremorService, carleonFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.serviceList = [];
        this.title = ""
        
        this.sagremorParams = sagremorParams;
        this.currentInjectors = _.filter(sagGenericInjectorManager.components, function (component) {
			return component.leftMenu && component.leftMenu.target === $stateParams.service;
		});
		this.size = 1;
        
        this.init = function () {
			if (!sagremorParams.loggedIn) {
				$location.path("/login");
			}
			loadServices();
		};
		
		function loadServices () {
			_.forEach(self.currentInjectors, function (currentInjector) {
				var cService = sharedData.get("carleonServices", currentInjector.type);
				if (!!cService && cService.enabled && !_.find(self.serviceList, {type: currentInjector.type})) {
					var service = {type: currentInjector.type};
					self.title = currentInjector.leftMenu.title;
					service.size = currentInjector.size || 1
					service.serviceGroup = {title: currentInjector.groupTitle, list: [], service: currentInjector.service};
					service.menu = [
						{
							name: "edit", 
							display: $translate.instant("edit"), 
							action: function (param) {
								param.service.editService(param);
							}
						},
						{
							name: "remove", 
							display: $translate.instant("remove"), 
							action: function (param) {
								param.service.removeService(param).then(function () {
									$scope.$broadcast("carleonServicesChanged");
								});
							}
						},
						{
							name: "add_to_dashboard", 
							display: $translate.instant("add_to_dashboard"), 
							action: function (param) {
								if (sagremorService.addToDashboard(param)) {
									$scope.$broadcast("refreshDashboard");
								}
							}
						}
					];
					self.serviceList.push(service);
				}
			});
			_.forEach(self.currentInjectors, function (currentInjector) {
				var cService = sharedData.get("carleonServices", currentInjector.type);
				if (!!cService && cService.enabled && !!cService.element) {
					var serviceGroup = _.find(self.serviceList, {type: currentInjector.type});
					if (!!serviceGroup && !serviceGroup.list) {
						serviceGroup.list = [];
					}
					!serviceGroup || _.forEach(cService.element, function (element) {
						var exist = _.find(serviceGroup.list, function (elt) {
							return elt.name === element.name;
						});
						if (!exist) {
							var elt = {name: element.name, type: element.type, description: element.description, service: currentInjector.service};
							serviceGroup.list.push(elt);
						}
					});
					
					!serviceGroup || _.forEach(serviceGroup.list, function (elt, index) {
						var exist = _.find(cService.element, function (serviceElt) {
							return serviceElt.name === elt.name;
						});
						if (!exist) {
							serviceGroup.list.splice(index, 1);
						}
					});
				}
			});
		}
		
		this.addService = function (service) {
			service.addService();
		};
		
		$scope.$on("carleonServicesChanged", function () {
			loadServices();
		});
        
		$scope.$on("refreshCarleonServices", function () {
			loadServices();
		});
	
        this.init();
        
    }]
);

angular.module("sagremorApp")
    .controller("switchesCtrl",
    ["$scope", "$location", "$translate", "sagremorService", "benoicFactory", "sharedData", "sagremorParams", function($scope, $location, $translate, sagremorService, benoicFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.switchList = [];
        this.dimmerList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {
			if (!sagremorParams.loggedIn) {
				$location.path("/login");
			}
			$translate(["edit", "monitor", "add_to_dashboard"]).then(function (results) {
				self.menuSwitcher = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editSwitcher(param);
						}
					},
					{
						name: "monitor", 
						display: results.monitor, 
						action: function (param) {
							sagremorService.monitor(param);
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				self.menuDimmer = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editDimmer(param);
						}
					},
					{
						name: "monitor", 
						display: results.monitor, 
						action: function (param) {
							sagremorService.monitor(param);
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				
				self.updateSwitchers();
				self.updateDimmers();
			});
		};
        
        $scope.$on("benoicDevicesChanged", function () {
            self.updateSwitchers();
            self.updateDimmers();
        });
        
        $scope.$on("benoicSwitchesChanged", function () {
            self.updateSwitchers();
            self.updateDimmers();
        });
        
        this.updateSwitchers = function () {
            var devices = sharedData.all("benoicDevices");
			self.switchList = [];
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (sw in devices[key].element.switches) {
                        var switcher = devices[key].element.switches[sw];
                        switcher.device = deviceName;
                        switcher.name = sw;
                        self.switchList.push(switcher);
                    }
                }
            }
        };
        
        this.updateDimmers = function () {
            var devices = sharedData.all("benoicDevices");
			self.dimmerList = [];
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (di in devices[key].element.dimmers) {
                        var dimmer = devices[key].element.dimmers[di];
                        dimmer.device = deviceName;
                        dimmer.name = di;
                        self.dimmerList.push(dimmer);
                    }
                }
            }
        };
        
        $scope.$on("refreshDevices", function () {
            self.refreshElements();
        });
        
        this.refreshElements = function () {
			// Add all elements that are not in self.dimmerList
			var devices = sharedData.all("benoicDevices");
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.dimmers) {
						var index = 0;
						var found = _.find(self.dimmerList, function (element) {
							return element.name === se && element.device === deviceName;
						});
						if (!found) {
							var dimmer = devices[key].element.dimmers[se];
							dimmer.device = deviceName;
							dimmer.name = se;
							self.dimmerList.push(dimmer);
						} else {
							var dimmer = devices[key].element.dimmers[se];
							dimmer.device = deviceName;
							dimmer.name = se;
							var index = _.indexOf(self.dimmerList, _.find(self.dimmerList, {device: deviceName, name: se}));
							self.dimmerList.splice(index, 1, dimmer);
						}
                    }
                }
            }
            
            // Remove all elements from self.dimmerList that are no longer in devices
            _.remove(self.dimmerList, function (dimmer) {
				var found = _.find(sharedData.all("benoicDevices"), function (device, deviceName) {
					return _.find(device.element.dimmers, function (element, eltName) {
						return eltName === dimmer.name && deviceName === dimmer.device;
					});
				});
				
				if (!found) {
					return true;
				}
			});
			
			// Add all elements that are not in self.switchList
			var devices = sharedData.all("benoicDevices");
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.switches) {
						var index = 0;
						var found = _.find(self.switchList, function (element) {
							return element.name === se && element.device === deviceName;
						});
						if (!found) {
							var switcher = devices[key].element.dimmers[se];
							switcher.device = deviceName;
							switcher.name = se;
							self.switchList.push(switcher);
						} else {
							var switcher = devices[key].element.switches[se];
							switcher.device = deviceName;
							switcher.name = se;
							var index = _.indexOf(self.switchList, _.find(self.switchList, {device: deviceName, name: se}));
							self.switchList.splice(index, 1, switcher);
						}
                    }
                }
            }
            
            // Remove all elements from self.switchList that are no longer in devices
            _.remove(self.switchList, function (switcher) {
				var found = _.find(sharedData.all("benoicDevices"), function (device, deviceName) {
					return !device.element || _.find(device.element.dimmers, function (element, eltName) {
						return eltName === dimmer.name && deviceName === dimmer.device;
					});
				});
				
				if (!found) {
					return true;
				}
			});
		};
        
        this.init();
        
    }]
);

angular.module("sagremorApp")
    .controller("usersCtrl",
    ["$scope", "$translate", "toaster", "DTOptionsBuilder", "angharadFactory", "sagremorConfirm", function($scope, $translate, toaster, DTOptionsBuilder, angharadFactory, sagremorConfirm) {
      
        var self = this;
        
        this.userList = [];
        this.tokenList = [];
        
        this.options = {};
        
        this.newUser = false;
        this.user = {
			login: "",
			password: "",
			enabled: true
		};
		this.usersEnabled = true;
		
		this.searchOptions = {
			login: "",
			enabled: "",
		};
        
        this.init = function () {
			if ($translate.use()) {
				self.options = DTOptionsBuilder.newOptions()
								.withLanguageSource("components/core/i18n/datatables."+$translate.use()+".json");
			}
			self.getUserList();
			self.getTokenList();
		};
		
		this.getUserList = function () {
			angharadFactory.getUserList().then(function (result) {
				self.userList = result;
			}, function (error) {
				if (error.status === 404) {
					self.usersEnabled = false;
				} else {
					toaster.pop("error", $translate.instant("users_get_user_list"), $translate.instant("users_get_user_list_error"));
				}
			});
		};
		
		this.addUser = function () {
			self.newUser = true;
		};
		
		this.cancelNewUser = function () {
			self.newUser = false;
			self.user = {
				login: "",
				password: "",
				enabled: true
			};
		};
		
		this.isNewUserValid = function () {
			var found = _.find(self.userList, {login: self.user.login});
			
			if (found) {
				return false;
			}
			
			return !!self.user.login && self.user.login.length > 0 && !!self.user.password && self.user.password.length > 0;
		};
		
		this.createNewUser = function () {
			angharadFactory.addUser(self.user).then(function () {
				self.userList.push(self.user);
				self.newUser = false;
				self.user = {
					login: "",
					password: "",
					enabled: true
				};
				toaster.pop("success", $translate.instant("users_save_user"), $translate.instant("users_save_user_success"));
			}, function () {
				toaster.pop("error", $translate.instant("users_save_user"), $translate.instant("users_save_user_error"));
			});
		};
		
		this.displayChangePassword = function (user) {
			user.changePassword = true;
		};
		
		this.cancelChangePassword = function (user) {
			user.changePassword = false;
			user.password = "";
		};
		
		this.changePassword = function (user) {
			angharadFactory.setUser(user.login, user).then(function () {
				user.password = "";
				user.changePassword = false;
				toaster.pop("success", $translate.instant("users_save_user"), $translate.instant("users_save_user_success"));
			}, function () {
				toaster.pop("error", $translate.instant("users_save_user"), $translate.instant("users_save_user_error"));
			});
		};
		
		this.enableUser = function (user) {
			angharadFactory.setUser(user.login, {enabled: user.enabled}).then(function () {
				toaster.pop("success", $translate.instant("users_save_user"), $translate.instant("users_save_user_success"));
			}, function () {
				toaster.pop("error", $translate.instant("users_save_user"), $translate.instant("users_save_user_error"));
			});
		};
		
		this.removeUser = function (user) {
			return sagremorConfirm.open($translate.instant("users_user_remove"), $translate.instant("users_user_remove_confirm", {login: user.login})).then(function () {
				return angharadFactory.removeUser(user.login).then(function () {
					_.remove(self.userList, {login: user.login});
					toaster.pop("success", $translate.instant("users_user_remove"), $translate.instant("users_user_remove_success"));
				}, function () {
					toaster.pop("error", $translate.instant("users_user_remove"), $translate.instant("users_user_remove_error"));
				});
			});
		};
		
		this.getTokenList = function () {
			var options = {
				login: self.searchOptions.login
			};
			if (self.searchOptions.enabled === "true") {
				options.enabled = true;
			} else if (self.searchOptions.enabled === "false") {
				options.enabled = false;
			}
			
			angharadFactory.getTokenList(options).then(function (result) {
				self.tokenList = result;
			}, function () {
				toaster.pop("error", $translate.instant("users_get_token_list"), $translate.instant("users_get_token_list_error"));
			});
		};
		
		this.displayToken = function (token) {
			return token.substring(0, 4) + "****-****-****-****-********" + token.substring(32);
		};
		
		this.revokeToken = function (token) {
			return sagremorConfirm.open($translate.instant("users_token_revoke"), $translate.instant("users_token_revoke_confirm")).then(function () {
				return angharadFactory.revokeToken(token.token).then(function () {
					token.enabled = false;
					toaster.pop("success", $translate.instant("users_token_revoke"), $translate.instant("users_token_revoke_success"));
				}, function () {
					toaster.pop("error", $translate.instant("users_token_revoke"), $translate.instant("users_token_revoke_error"));
				});
			});
		};
		
        this.init();
        
		$scope.$on("changeLang", function () {
			self.options = DTOptionsBuilder.newOptions()
							.withLanguageSource("components/core/i18n/datatables."+$translate.use()+".json");
		});
    }]
);

angular.module("sagremorApp").directive("sagGenericInjector", ["$compile", "sagGenericInjectorManager", "sharedData", function ($compile, sagGenericInjectorManager, sharedData) {
    
    var template = "<directive element=\"elt\"></directive>";
    
    var templateNotFound = "<div class=\"container has-error\" data-translate=\"injector_not_found\"></div>";
     
    var templateDisabled = "<div class=\"container has-error\" data-translate=\"injector_disabled\"></div>";
     
    return{
        scope: {
            type: "=",
            elt: "="
        },
        link: function(scope, element) {
            var config = _.find(sagGenericInjectorManager.components, {type: scope.type});
			var service = sharedData.get("carleonServices", scope.type);
			if (config.carleonService && service && service.enabled) {
				content = $compile(template.replace(/directive/g, config.directive))(scope);
			} else if (config.carleonService && service && !service.enabled) {
				content = $compile(templateDisabled)(scope);
			} else if (!config.carleonService) {
				content = $compile(template.replace(/directive/g, config.directive))(scope);
			} else {
				content = $compile(templateNotFound)(scope);
			}
            element.append(content);
        }
    }}]);

function carleonLiquidsoapController ($scope, $sce, $q, $window, $translatePartialLoader, $translate, $interval, angharadConfig, carleonFactory, carleonLiquidsoapFactory, sagremorService, sagremorParams, toaster) {
    var ctrl = this;
    
    this.radio = {};
	this.status = "";
    this.onAir = {
		title: "",
		artist: "",
		albumartist: "",
		year: "",
		album: ""
	};
	this.list = [];
	this.listSelected = {
		title: "",
		artist: "",
		albumartist: "",
		year: "",
		album: ""
	};
    
    this.onAirAccordionOpen = false;
    this.listAccordionOpen = false;
    
    function init() {
		carleonLiquidsoapFactory.getLiquidsoapService(ctrl.element.name).then(function (result) {
			ctrl.radio = result;
		}, function () {
			toaster.pop("error", $translate.instant("carleon_liquidsoap_init"), $translate.instant("carleon_liquidsoap_init_error"));
		});
    }
    
    this.commandRadio = function (command) {
		carleonLiquidsoapFactory.command(ctrl.element.name, command).then(function (result) {
			toaster.pop("success", $translate.instant("carleon_liquidsoap_command"), $translate.instant("carleon_liquidsoap_command_success"));
			ctrl.refreshStatus();
		}, function () {
			toaster.pop("error", $translate.instant("carleon_liquidsoap_command"), $translate.instant("carleon_liquidsoap_command_error"));
		});
	};
    
	$scope.$watch("$ctrl.controlAccordionOpen", function(isOpen){
		if (isOpen) {
			ctrl.refreshStatus();
		}
	});
	
	$scope.$watch("$ctrl.onAirAccordionOpen", function(isOpen){
		if (isOpen) {
			ctrl.refreshOnAir();
		}
	});
	
	$scope.$watch("$ctrl.listAccordionOpen", function(isOpen){
		if (isOpen) {
			ctrl.refreshList();
		}
	});
	
	this.refreshStatus = function () {
		carleonLiquidsoapFactory.command(ctrl.element.name, "status").then(function (result) {
			ctrl.status = result.value;
		}, function () {
			toaster.pop("error", $translate.instant("carleon_liquidsoap_status"), $translate.instant("carleon_liquidsoap_status_error"));
		});
	};
	
	this.refreshOnAir = function () {
		carleonLiquidsoapFactory.onAir(ctrl.element.name).then(function (result) {
			ctrl.onAir = result;
		}, function () {
			toaster.pop("error", $translate.instant("carleon_liquidsoap_on_air"), $translate.instant("carleon_liquidsoap_on_air_error"));
		});
	};
	
	this.refreshList = function () {
		carleonLiquidsoapFactory.list(ctrl.element.name).then(function (result) {
			ctrl.list = result;
		}, function () {
			toaster.pop("error", $translate.instant("carleon_liquidsoap_list"), $translate.instant("carleon_liquidsoap_list_error"));
		});
	};
	
    init();
}

angular.module("sagremorApp").component("serviceLiquidsoap", {
    templateUrl: "components/carleonLiquidsoap/carleonLiquidsoap.template.html",
    controller: carleonLiquidsoapController,
    bindings: {
        element: "="
    }
})
.directive("radio", ["$sce", function($sce) {
	// Using directive from Hols: http://stackoverflow.com/a/23663352
	return {
		restrict: "A",
		scope: { src: "=" },
		replace: true,
		template: "<audio data-ng-src=\"{{url}}\" controls></audio>",
		link: function (scope) {
			scope.$watch("src", function (newVal, oldVal) {
				if (newVal !== undefined) {
					scope.url = $sce.trustAsResourceUrl(newVal);
				}
			});
		}
	};
}])
.factory("carleonLiquidsoapFactory", ["$http", "$uibModal", "$translate", "toaster", "angharadConfig", "angharadBackendService", "sagremorConfirm", "sharedData", function($http, $uibModal, $translate, toaster, angharadConfig, angharadBackendService, sagremorConfirm, sharedData) {
    var urlBase = angharadConfig.baseUrl + angharadConfig.prefixCarleon;
    var carleonLiquidsoapFactory = {};

	/* Liquidsoap service */
    carleonLiquidsoapFactory.getLiquidsoapServiceList = function () {
        return angharadBackendService.httpRequest("GET", urlBase + "service-liquidsoap/");
    };

    carleonLiquidsoapFactory.getLiquidsoapService = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "service-liquidsoap/" + name);
    };

    carleonLiquidsoapFactory.addLiquidsoapService = function (liquidsoapService) {
        return angharadBackendService.httpRequest("POST", urlBase + "service-liquidsoap/", liquidsoapService);
    };

    carleonLiquidsoapFactory.setLiquidsoapService = function (name, liquidsoapService) {
        return angharadBackendService.httpRequest("PUT", urlBase + "service-liquidsoap/" + name, liquidsoapService);
    };

    carleonLiquidsoapFactory.removeLiquidsoapService = function (name) {
        return angharadBackendService.httpRequest("DELETE", urlBase + "service-liquidsoap/" + name);
    };

    carleonLiquidsoapFactory.list = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "service-liquidsoap/" + name + "/list");
    };

    carleonLiquidsoapFactory.onAir = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "service-liquidsoap/" + name + "/on_air");
    };

    carleonLiquidsoapFactory.command = function (name, command) {
        return angharadBackendService.httpRequest("GET", urlBase + "service-liquidsoap/" + name + "/command/" + command);
    };

	carleonLiquidsoapFactory.addService = function () {
		var modalInstance = $uibModal.open({
			animation: true,
			templateUrl: "components/carleonLiquidsoap/carleonLiquidsoap.modal.html",
			controller: "carleonLiquidsoapModalCtrl",
			controllerAs: "carleonLiquidsoapModalCtrl",
			size: "md",
			resolve: {
				element: function () {
					return null;
				}
			}
		});
	};
	
	carleonLiquidsoapFactory.editService = function (service) {
		var modalInstance = $uibModal.open({
			animation: true,
			templateUrl: "components/carleonLiquidsoap/carleonLiquidsoap.modal.html",
			controller: "carleonLiquidsoapModalCtrl",
			controllerAs: "carleonLiquidsoapModalCtrl",
			size: "md",
			resolve: {
				element: function () {
					return service;
				}
			}
		});
	};
	
	carleonLiquidsoapFactory.removeService = function (service) {
		return sagremorConfirm.open($translate.instant("carleon_liquidsoap_remove"), $translate.instant("carleon_liquidsoap_remove_confirm")).then(function () {
			return carleonLiquidsoapFactory.removeLiquidsoapService(service.name).then(function () {
				var injector = sharedData.get("carleonServices", "service-liquidsoap");
				_.remove(injector.element, function (element) {
					return element.name === service.name;
				});
				sharedData.add("carleonServices", "service-liquidsoap", injector);
				toaster.pop("success", $translate.instant("carleon_liquidsoap_remove"), $translate.instant("carleon_liquidsoap_remove_success"));
			}, function () {
				toaster.pop("error", $translate.instant("carleon_liquidsoap_remove"), $translate.instant("carleon_liquidsoap_remove_error"));
			});
		});
	};
	
    return carleonLiquidsoapFactory;

}])
.config(["$translatePartialLoaderProvider", function run($translatePartialLoaderProvider) {
	$translatePartialLoaderProvider.addPart("carleonLiquidsoap");
}])
.run(["sagGenericInjectorManager", "carleonLiquidsoapFactory", function(sagGenericInjectorManager, carleonLiquidsoapFactory) {
    sagGenericInjectorManager.add({
        type: "service-liquidsoap",
        directive: "service-liquidsoap",
        groupTitle: "carleon_liquidsoap_title",
        service: carleonLiquidsoapFactory,
        carleonService: true,
        size: 2,
        leftMenu: {
			title: "carleon_music_title",
			target: "music"
		},
		widgetHeight: 4,
		widgetWidth: 3,
		icon: "music",
		commands: {
		}
    });
}]);

angular.module("sagremorApp")
    .controller("carleonLiquidsoapModalCtrl", 
    ["$rootScope", "$uibModalInstance", "$translate", "sharedData", "toaster", "carleonLiquidsoapFactory", "element", function($rootScope, $uibModalInstance, $translate, sharedData, toaster, carleonLiquidsoapFactory, element) {
        var self = this;

        this.messages = {};
        this.add = true;
        this.element = {
			name: "",
			description: "",
			radio_url: "",
			radio_type: "",
			control_enabled: false,
			control_host: "",
			control_port: 0,
			control_request_name: ""
		}

        function init() {
			if (!!element) {
				self.element = _.find(sharedData.get("carleonServices", "service-liquidsoap").element, function (elt) {
					return elt.name === element.name;
				});
				self.add = false;
			}
        }
        
        this.isLiquidsoapValid = function () {
			var found = _.find(sharedData.get("carleonServices", "service-liquidsoap").element, function (element) {
				return element.name === self.element.name;
			});
			return (!found || !self.add) 
					&& !!self.element.name
					&& !!self.element.radio_url 
					&& (!self.element.control_enabled 
						|| (!!self.element.control_host && !!self.element.control_port && !!self.element.control_request_name));
		};
        
        this.save = function () {
			if (self.add) {
				carleonLiquidsoapFactory.addLiquidsoapService(self.element).then(function () {
					self.element.type = "service-liquidsoap";
					self.element.service = carleonLiquidsoapFactory;
					sharedData.get("carleonServices", "service-liquidsoap").element.push(self.element);
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_liquidsoap_save"), $translate.instant("carleon_liquidsoap_save_success"));
				}, function () {
					toaster.pop("error", $translate.instant("carleon_liquidsoap_save"), $translate.instant("carleon_liquidsoap_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			} else {
				carleonLiquidsoapFactory.setLiquidsoapService(self.element.name, self.element).then(function () {
					self.element.type = "service-liquidsoap";
					self.element.service = carleonLiquidsoapFactory;
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_liquidsoap_save"), $translate.instant("carleon_liquidsoap_save_success"));
				}, function () {
					toaster.pop("error", $translate.instant("carleon_liquidsoap_save"), $translate.instant("carleon_liquidsoap_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			}
		};
		
        this.cancel = function () {
			$uibModalInstance.dismiss("close");
		};

        init();
    }]
);

function carleonMockController ($translatePartialLoader, $translate, carleonMockFactory, sagremorParams, toaster) {
    var ctrl = this;
    
    ctrl.adminMode = sagremorParams.adminMode;
    ctrl.param = {command: "exec1", param1: "test", param2: 42, param3: 7.3};
    ctrl.commandList = ["exec1", "exec2", "exec3", "exec4"];
    ctrl.messages = {};
    
    function init() {
    }
    
    ctrl.command = function () {
		carleonMockFactory.commandMock(ctrl.element.name, ctrl.param.command, ctrl.param.param1, ctrl.param.param2, ctrl.param.param3).then(function (result) {
			toaster.pop("success", $translate.instant("carleon_mock_command"), $translate.instant("carleon_mock_command_success"));
		}, function (error) {
			toaster.pop("error", $translate.instant("carleon_mock_command"), $translate.instant("carleon_mock_command_error"));
		});
	};
	
    init();
}

angular.module("sagremorApp").component("carleonMock", {
    templateUrl: "components/carleonMock/carleonMock.template.html",
    controller: carleonMockController,
    bindings: {
        element: "="
    }
})
.factory("carleonMockFactory", ["$http", "$uibModal", "$translate", "toaster", "angharadConfig", "angharadBackendService", "sagremorConfirm", "sharedData", function($http, $uibModal, $translate, toaster, angharadConfig, angharadBackendService, sagremorConfirm, sharedData) {
    var urlBase = angharadConfig.baseUrl + angharadConfig.prefixCarleon;
    var mockFactory = {};

	/* Mock service */
    mockFactory.getMockList = function () {
        return angharadBackendService.httpRequest("GET", urlBase + "mock-service/");
    };

    mockFactory.getMock = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "mock-service/" + name);
    };

    mockFactory.addMock = function (mock) {
        return angharadBackendService.httpRequest("POST", urlBase + "mock-service/", mock);
    };

    mockFactory.setMock = function (name, mock) {
        return angharadBackendService.httpRequest("PUT", urlBase + "mock-service/" + name, mock);
    };

    mockFactory.removeMock = function (name) {
        return angharadBackendService.httpRequest("DELETE", urlBase + "mock-service/" + name);
    };

    mockFactory.commandMock = function (name, command, param1, param2, param3) {
		if (command === "exec3" || command === "exec4") {
			return angharadBackendService.httpRequest("GET", urlBase + "mock-service/" + name + "/command/" + command);
		} else if (command === "exec2") {
			return angharadBackendService.httpRequest("GET", urlBase + "mock-service/" + name + "/command/" + command + "/" + param1);
		} else {
			return angharadBackendService.httpRequest("GET", urlBase + "mock-service/" + name + "/command/" + command + "/" + param1 + "/" + param2 + "/" + param3);
		}
    };

	mockFactory.addService = function () {
		var modalInstance = $uibModal.open({
			animation: true,
			templateUrl: "components/carleonMock/carleonMock.modal.html",
			controller: "CarleonMockModalCtrl",
			controllerAs: "CarleonMockModalCtrl",
			size: "sm",
			resolve: {
				mock: function () {
					return null;
				}
			}
		});
	};
	
	mockFactory.editService = function (service) {
		var modalInstance = $uibModal.open({
			animation: true,
			templateUrl: "components/carleonMock/carleonMock.modal.html",
			controller: "CarleonMockModalCtrl",
			controllerAs: "CarleonMockModalCtrl",
			size: "sm",
			resolve: {
				mock: function () {
					return service;
				}
			}
		});
	};
	
	mockFactory.removeService = function (service) {
		return sagremorConfirm.open($translate.instant("carleon_mock_remove"), $translate.instant("carleon_mock_confirm")).then(function () {
			return mockFactory.removeMock(service.name).then(function () {
				var injector = sharedData.get("carleonServices", "mock-service");
				_.remove(injector.element, function (element) {
					return element.name === service.name;
				});
				sharedData.add("carleonServices", "mock-service", injector);
				toaster.pop("success", $translate.instant("carleon_mock_remove"), $translate.instant("carleon_mock_remove_success"));
			}, function () {
				toaster.pop("error", $translate.instant("carleon_mock_remove"), $translate.instant("carleon_mock_remove_error"));
			});
		});
	};
	
    return mockFactory;

}])
.config(["$translatePartialLoaderProvider", function run($translatePartialLoaderProvider) {
	$translatePartialLoaderProvider.addPart("carleonMock");
}])
.run(["sagGenericInjectorManager", "carleonMockFactory", function(sagGenericInjectorManager, carleonMockFactory) {
    sagGenericInjectorManager.add({
        type: "mock-service",
        directive: "carleon-mock",
        groupTitle: "carleon_mock_title",
        service: carleonMockFactory,
        carleonService: true,
        size: 1,
        leftMenu: {
			title: "carleon_mock_title",
			target: "carleonMock"
		},
		widgetHeight: 3,
		widgetWidth: 3,
		icon: "thumbs-o-up",
		commands: {
			exec1: {
				title: "carleon_mock_command_exec1_title",
				parameters: {
					param1: "carleon_mock_command_exec1_parameter_param1_title",
					param2: "carleon_mock_command_exec1_parameter_param2_title",
					param3: "carleon_mock_command_exec1_parameter_param3_title"
				},
				result: {
					value1: {
						type: "integer",
						title: "carleon_mock_command_exec1_result_value1_title"
					},
					value2: {
						type: "boolean",
						title: "carleon_mock_command_exec1_result_value2_title"
					}
				}
			},
			exec2: {
				title: "carleon_mock_command_exec2_title",
				parameters: {
					param1: "carleon_mock_command_exec2_parameter_param1_title"
				},
				result: {
					type: "string",
					title: "carleon_mock_command_exec2_result_title"
				}
			},
			exec3: {
				title: "carleon_mock_command_exec3_title",
				parameters: {},
				result: {
					type: "string",
					title: "carleon_mock_command_exec3_result_title"
				}
			},
			exec4: {
				title: "carleon_mock_command_exec4_title",
				parameters: {},
				result: {
					value1: {
						type: "integer",
						title: "carleon_mock_command_exec4_result_value1_title"
					},
					value2: {
						type: "boolean",
						title: "carleon_mock_command_exec4_result_value2_title"
					}
				}
			}
		}
    });
}]);

angular.module("sagremorApp")
    .controller("CarleonMockModalCtrl", 
    ["$rootScope", "$uibModalInstance", "$translate", "sharedData", "toaster", "carleonMockFactory", "mock", function($rootScope, $uibModalInstance, $translate, sharedData, toaster, carleonMockFactory, mock) {
        var self = this;

        this.messages = {};
        this.add = true;

        function init() {
			if (!!mock) {
				self.mock = mock;
				self.add = false;
			} else {
				self.mock = {};
			}
        }
        
        this.cancel = function () {
			$uibModalInstance.dismiss("close");
		};

        this.save = function () {
			if (self.add) {
				carleonMockFactory.addMock(self.mock).then(function () {
					self.mock.type = "mock-service";
					self.mock.service = carleonMockFactory;
					sharedData.get("carleonServices", "mock-service").element.push(self.mock);
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_mock_save"), $translate.instant("carleon_mock_save_success"));
				}, function (error) {
					toaster.pop("error", $translate.instant("carleon_mock_save"), $translate.instant("carleon_mock_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			} else {
				carleonMockFactory.setMock(self.mock.name, self.mock).then(function () {
					var mocks = sharedData.get("carleonServices", "mock-service").element;
					_.forEach(mocks, function (mock) {
						if (mock.name === self.mock.name) {
							mock.description = self.mock.description;
						}
					});
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_mock_save"), $translate.instant("carleon_mock_save_success"));
				}, function (error) {
					toaster.pop("error", $translate.instant("carleon_mock_save"), $translate.instant("carleon_mock_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			}
		};

        init();
    }]
);

function carleonMotionController ($scope, $http, $translatePartialLoader, $translate, angharadConfig, angharadFactory, carleonFactory, carleonMotionFactory, sagremorService, sagremorParams, toaster) {
    var ctrl = this;
    
    this.urlBaseImages = angharadConfig.baseUrl + angharadConfig.prefixCarleon + "service-motion/" + ctrl.element.name + "/image/";
    this.urlBaseStream = angharadConfig.baseUrl + angharadConfig.prefixCarleon + "service-motion/" + ctrl.element.name + "/stream/";
    this.name = ctrl.element.name;

    this.images = [];
    this.fileListName = [];
    this.fileList = {};
    this.selectedFileList = "";
    this.offset = 0;
    
    this.streamList = [];
    this.selectedStream = false;
    this.streamUri = "";
    
    this.online = false;
    
    this.profileParams = {};
    
    this.init = function () {
		ctrl.refresh();
		ctrl.getFromProfile();
    }
    
    this.getFromProfile = function () {
		if (sagremorParams.currentProfile && _.has(sagremorParams.currentProfile, "carleon.serviceMotion."+ctrl.element.name)) {
			ctrl.profileParams = sagremorParams.currentProfile.carleon.serviceMotion[ctrl.element.name];
		} else if (!!sagremorParams.currentProfile) {
			if (!sagremorParams.currentProfile.carleon) {
				sagremorParams.currentProfile.carleon = {};
			}
			if (!sagremorParams.currentProfile.carleon.serviceMotion) {
				sagremorParams.currentProfile.carleon.serviceMotion = {};
			}
			if (!sagremorParams.currentProfile.carleon.serviceMotion[ctrl.element.name]) {
				sagremorParams.currentProfile.carleon.serviceMotion[ctrl.element.name] = {
					selectedFileList: "",
					selectedStream: ""
				}
			}
		}
	}
    
    this.openPopupImage = function (imgArray, index) {
		carleonMotionFactory.imagePopup(imgArray, index);
	};
    
    this.refresh = function () {
		ctrl.fileListName = [];
		ctrl.streamList = [];
		carleonMotionFactory.status(ctrl.element.name, ctrl.offset).then(function (response) {
			ctrl.online = response.online;
			ctrl.fileList = response.file_list;
			_.forEach(response.file_list, function (fileList, name) {
				ctrl.fileListName.push(name);
			});
			if (!!sagremorParams.currentProfile && !!sagremorParams.currentProfile.carleon.serviceMotion[ctrl.element.name].selectedFileList) {
				ctrl.selectedFileList = sagremorParams.currentProfile.carleon.serviceMotion[ctrl.element.name].selectedFileList;
				ctrl.changeSelectedFileList();
			} else if (ctrl.fileListName.length === 1) {
				ctrl.selectedFileList = ctrl.fileListName[0];
				ctrl.changeSelectedFileList();
			}
			
			_.forEach(response.stream_list, function (stream) {
				var image = {
					name: ctrl.element.name,
					title: stream.name,
					stream: stream.uri,
					snapshotUri: stream.snapshot_uri
				};
				ctrl.streamList.push(image);
			});
		});
	};
	
	this.canPrevious = function () {
		return ctrl.offset > 0;
	};
	
	this.canNext = function () {
		return ctrl.fileList[ctrl.selectedFileList].length == 20;
	};
    
	this.previousList = function () {
		ctrl.offset -= 20;
		ctrl.refresh();
	};
	
	this.nextList = function () {
		ctrl.offset += 20;
		ctrl.refresh();
	};
	
    this.changeSelectedFileList = function () {
		ctrl.images = [];
		_.forEach(ctrl.fileList[ctrl.selectedFileList], function (file) {
			var image = {
				thumb: ctrl.urlBaseImages + ctrl.selectedFileList + "/" + file + "?thumbnail=true",
				img: ctrl.urlBaseImages + ctrl.selectedFileList + "/" + file,
				title: file
			}
			ctrl.images.push(image);
		});
		sagremorParams.currentProfile.carleon.serviceMotion[ctrl.element.name].selectedStream = "";
		sagremorParams.currentProfile.carleon.serviceMotion[ctrl.element.name].selectedFileList = ctrl.selectedFileList;
		angharadFactory.saveCurrentProfile().then(function () {
			//toaster.pop("success", $translate.instant("profile_save"), $translate.instant("profile_save_success"));
		}, function () {
			toaster.pop("error", $translate.instant("profile_save"), $translate.instant("profile_save_error"));
		});
	};
	
	this.openStream = function () {
		carleonMotionFactory.imagePopup(ctrl.streamList, 0);
	};
	
	$scope.$on("refreshCarleonServices", function () {
		ctrl.refresh();
	});
	
	$scope.$on("carleonServicesChanged", function () {
		ctrl.init();
	});
	
	$scope.$on("angharadProfileChanged", function () {
		ctrl.getFromProfile();
	});
	
    this.init();
}

angular.module("sagremorApp").component("serviceMotion", {
    templateUrl: "components/carleonMotion/carleonMotion.template.html",
    controller: carleonMotionController,
    bindings: {
        element: "="
    }
})
.factory("carleonMotionFactory", ["$http", "$uibModal", "$translate", "toaster", "angharadConfig", "angharadBackendService", "sagremorConfirm", "sharedData", function($http, $uibModal, $translate, toaster, angharadConfig, angharadBackendService, sagremorConfirm, sharedData) {
    var urlBase = angharadConfig.baseUrl + angharadConfig.prefixCarleon;
    var carleonMotionFactory = {};

	/* Motion service */
    carleonMotionFactory.getMotionServiceList = function () {
        return angharadBackendService.httpRequest("GET", urlBase + "service-motion/");
    };

    carleonMotionFactory.getMotionService = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "service-motion/" + name);
    };

    carleonMotionFactory.addMotionService = function (motionService) {
        return angharadBackendService.httpRequest("POST", urlBase + "service-motion/", motionService);
    };

    carleonMotionFactory.setMotionService = function (name, motionService) {
        return angharadBackendService.httpRequest("PUT", urlBase + "service-motion/" + name, motionService);
    };

    carleonMotionFactory.removeMotionService = function (name) {
        return angharadBackendService.httpRequest("DELETE", urlBase + "service-motion/" + name);
    };

    carleonMotionFactory.status = function (name, offset) {
        return angharadBackendService.httpRequest("GET", urlBase + "service-motion/" + name + "/status", null, {offset: offset});
    };

    carleonMotionFactory.image = function (name, file_list, file_name, thumbnail) {
		var urlParam = {};
		if (thumbnail) {
			urlParam.thumbnail = "";
		}
        return angharadBackendService.httpRequest("GET", urlBase + "service-motion/" + name + "/image/" + file_list + "/" + file_name, null, urlParam);
    };

    carleonMotionFactory.stream = function (name, stream_name) {
        return angharadBackendService.httpRequest("GET", urlBase + "service-motion/" + name + "/stream/" + stream_name);
    };

    carleonMotionFactory.snapshot = function (name, stream_name) {
        return angharadBackendService.httpRequest("PUT", urlBase + "service-motion/" + name + "/stream/" + stream_name + "/snapshot");
    };

	carleonMotionFactory.addService = function () {
		var modalInstance = $uibModal.open({
			animation: true,
			templateUrl: "components/carleonMotion/carleonMotion.modal.html",
			controller: "carleonMotionModalCtrl",
			controllerAs: "carleonMotionModalCtrl",
			size: "md",
			resolve: {
				motion: function () {
					return null;
				}
			}
		});
	};
	
	carleonMotionFactory.editService = function (service) {
		var modalInstance = $uibModal.open({
			animation: true,
			templateUrl: "components/carleonMotion/carleonMotion.modal.html",
			controller: "carleonMotionModalCtrl",
			controllerAs: "carleonMotionModalCtrl",
			size: "md",
			resolve: {
				motion: function () {
					return service;
				}
			}
		});
	};
	
	carleonMotionFactory.removeService = function (service) {
		return sagremorConfirm.open($translate.instant("carleon_motion_remove"), $translate.instant("carleon_motion_confirm")).then(function () {
			return carleonMotionFactory.removeMotionService(service.name).then(function () {
				var injector = sharedData.get("carleonServices", "service-motion");
				_.remove(injector.element, function (element) {
					return element.name === service.name;
				});
				sharedData.add("carleonServices", "service-motion", injector);
				toaster.pop("success", $translate.instant("carleon_motion_remove"), $translate.instant("carleon_motion_remove_success"));
			}, function () {
				toaster.pop("error", $translate.instant("carleon_motion_remove"), $translate.instant("carleon_motion_remove_error"));
			});
		});
	};
	
	carleonMotionFactory.imagePopup = function (imgArray, index) {
		var modalInstance = $uibModal.open({
			animation: true,
			templateUrl: "components/carleonMotion/carleonMotion.image.modal.html",
			controller: "carleonMotionImageModalCtrl",
			controllerAs: "carleonMotionImageModalCtrl",
			size: "lg",
			resolve: {
				imgArray: function () {
					return imgArray;
				},
				index: function () {
					return index;
				}
			}
		});
	};
        
    return carleonMotionFactory;

}])
.config(["$translatePartialLoaderProvider", function run($translatePartialLoaderProvider) {
	$translatePartialLoaderProvider.addPart("carleonMotion");
}])
.run(["sagGenericInjectorManager", "carleonMotionFactory", function(sagGenericInjectorManager, carleonMotionFactory) {
    sagGenericInjectorManager.add({
        type: "service-motion",
        directive: "service-motion",
        groupTitle: "carleon_motion_title",
        service: carleonMotionFactory,
        carleonService: true,
        size: 3,
        leftMenu: {
			title: "carleon_motion_title",
			target: "carleonMotion"
		},
		widgetHeight: 5,
		widgetWidth: 6,
		icon: "camera",
		commands: {
			online: {
				title: "carleon_motion_command_online_title",
				parameters: {},
				result: {
					type: "boolean",
					title: "carleon_motion_command_online_result_title"
				}
			},
			snapshot: {
				title: "carleon_motion_command_snapshot_title",
				parameters: {
					stream: "carleon_motion_command_snapshot_parameter_stream_title",
				},
				result: {
					type: "boolean",
					title: "carleon_motion_command_snapshot_result_title"
				}
			}
		}
    });
}]);

angular.module("sagremorApp")
    .controller("carleonMotionImageModalCtrl",
    ["$scope", "$uibModalInstance", "$translate", "toaster", "carleonMotionFactory", "imgArray", "index", function($scope, $uibModalInstance, $translate, toaster, carleonMotionFactory, imgArray, index) {
        var self = this;
        
        this.imgArray = imgArray;
        this.index = index;
        this.currentImage = {
			name: "",
			title: "",
			img: "",
			stream: "",
			snapshotUri: ""
		};
        
        function init() {
			self.currentImage = self.imgArray[self.index];
			$scope.$broadcast("carleonMotionImageIframe", self.currentImage);
        }
        
        this.close = function () {
            $uibModalInstance.dismiss("close");
        };
        
        this.displayTitle = function () {
			return $translate.instant("image_title") + self.currentImage.title;
		};
		
		this.changeImage = function (delta) {
			self.index += delta;
			self.currentImage = self.imgArray[self.index];
			$scope.$broadcast("carleonMotionImageIframe", self.currentImage);
		};
		
		this.canPrevious = function () {
			return self.index > 0;
		};
		
		this.canNext = function () {
			return self.index < self.imgArray.length - 1;
		};
		
		this.snapshot = function () {
			carleonMotionFactory.snapshot(self.currentImage.name, self.currentImage.title).then(function () {
				toaster.pop("success", $translate.instant("carleon_motion_snapshot"), $translate.instant("carleon_motion_snapshot_success"));
			}, function () {
				toaster.pop("error", $translate.instant("carleon_motion_snapshot"), $translate.instant("carleon_motion_snapshot_error"));
			});
		};
		
		this.showIndex = function () {
			return (self.index + 1) + " / " + self.imgArray.length;
		};
		
        init();
    }]
);

angular.module("sagremorApp")
    .controller("carleonMotionModalCtrl", 
    ["$rootScope", "$uibModalInstance", "$translate", "sharedData", "toaster", "carleonMotionFactory", "motion", function($rootScope, $uibModalInstance, $translate, sharedData, toaster, carleonMotionFactory, motion) {
        var self = this;

        this.messages = {};
        this.add = true;
        this.addFileList = false;
        this.addStream = false;
        this.newFileList = {};
        this.newStream = {};

        function init() {
			if (!!motion) {
				self.motion = _.find(sharedData.get("carleonServices", "service-motion").element, function (elt) {
					return elt.name === motion.name;
				});
				self.add = false;
			} else {
				self.motion = {
					file_list: [],
					stream_list: []
				};
			}
        }
        
        this.addNewFileList = function () {
			self.addFileList = true;
		};
		
		this.removeFileList = function (index) {
			self.motion.file_list.splice(index, 1);
		};
		
		this.saveNewFileList = function () {
			self.motion.file_list.push(self.newFileList);
			self.newFileList = {};
			self.addFileList = false;
		};
		
		this.cancelNewFileList = function () {
			self.newFileList = {};
			self.addFileList = false;
		};
		
		this.isFileListValid = function () {
			return !!self.newFileList.name && !!self.newFileList.path && !!self.newFileList.thumbnail_path;
		};
		
		this.addNewStream = function () {
			self.addStream = true;
		};
        
		this.removeStream = function (index) {
			self.motion.stream_list.splice(index, 1);
		};
		
		this.saveNewStream = function () {
			self.motion.stream_list.push(self.newStream);
			self.newStream = {};
			self.addStream = false;
		};
		
		this.cancelNewStream = function () {
			self.newStream = {};
			self.addStream = false;
		};
		
		this.isStreamValid = function () {
			return !!self.newStream.name && !!self.newStream.uri;
		};
		
		this.isCameraValid = function () {
			var found = _.find(sharedData.get("carleonServices", "service-motion").element, function (element) {
				return element.name === self.motion.name;
			});
			return (!found || !self.add) && !!self.motion.name && !!self.motion.config_uri;
		};
		
        this.cancel = function () {
			$uibModalInstance.dismiss("close");
		};

        this.save = function () {
			if (self.add) {
				carleonMotionFactory.addMotionService(self.motion).then(function () {
					self.motion.type = "service-motion";
					self.motion.service = carleonMotionFactory;
					sharedData.get("carleonServices", "service-motion").element.push(self.motion);
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_motion_save"), $translate.instant("carleon_motion_save_success"));
				}, function () {
					toaster.pop("error", $translate.instant("carleon_motion_save"), $translate.instant("carleon_motion_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			} else {
				carleonMotionFactory.setMotionService(self.motion.name, self.motion).then(function () {
					self.motion.type = "service-motion";
					self.motion.service = carleonMotionFactory;
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_motion_save"), $translate.instant("carleon_motion_save_success"));
				}, function () {
					toaster.pop("error", $translate.instant("carleon_motion_save"), $translate.instant("carleon_motion_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			}
		};

        init();
    }]
);

function carleonMpdController($scope, $q, $window, $translatePartialLoader, $translate, $interval, angharadConfig, carleonFactory, carleonMpdFactory, benoicFactory, sagremorParams, sharedData, toaster) {
	var ctrl = this;

	this.mpd = {};
	this.mpdElement = {};
	this.playlists = [];
	this.selectedPlaylist = false;
	this._interval = null;
	this.hasFocus = true;

	function init() {
		ctrl.mpdElement = _.find(sharedData.get("carleonServices", "service-mpd").element, function (mpd) {
			return mpd.name === ctrl.element.name;
		});
		
		var promises = [
			carleonMpdFactory.getMpdStatus(ctrl.element.name),
			carleonMpdFactory.getMpdPlaylists(ctrl.element.name)
		];

		$q.all(promises).then(function(responses) {
			ctrl.mpd = responses[0];
			ctrl.playlists = responses[1];
			if (ctrl.mpd.state === "play" || ctrl.mpd.state === "pause") {
				startRefreshStatusInterval();
			}
		}, function() {
			toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_load_error", {
				name: ctrl.element.name
			}));
		});
	}

	function startRefreshStatusInterval() {
		ctrl._interval = $interval(function() {
			if (ctrl.hasFocus) {
				ctrl.refreshStatus();
			}
		}, 1000 * 10);
	}

	this.refreshStatus = function() {
		carleonMpdFactory.getMpdStatus(ctrl.element.name).then(function(response) {
			ctrl.mpd = response;
			if ((ctrl.mpd.state === "play" || ctrl.mpd.state === "pause") && ctrl._interval == null) {
				startRefreshStatusInterval();
			} else if (ctrl._interval != null && ctrl.mpd.state !== "play" && ctrl.mpd.state !== "pause") {
				$interval.cancel(ctrl._interval);
				ctrl._interval = null;
			}
		}, function() {
			toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_load_error", {
				name: ctrl.element.name
			}));
		});
	};

	this.displayVolume = function() {
		return ctrl.mpd.volume + "%";
	};

	this.setVolume = function(delta) {
		var newVolume = ctrl.mpd.volume + delta;
		if (newVolume >= 0 && newVolume <= 100) {
			carleonMpdFactory.setMpdVolume(ctrl.element.name, newVolume).then(function() {
				ctrl.mpd.volume = newVolume;
			}, function() {
				toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_volume_error"));
			});
		}
	};

	this.displayVolumeCommand = function(delta) {
		return (delta > 0 ? "+" : "") + delta + " %"
	};

	this.setAction = function(action) {
		var actionUrl = action;
		if (action === "repeat") {
			actionUrl += ("/" + (ctrl.mpd.repeat?"0":"1"));
			ctrl.mpd.repeat = !ctrl.mpd.repeat;
		}
		if (action === "random") {
			actionUrl += ("/" + (ctrl.mpd.random?"0":"1"));
			ctrl.mpd.random = !ctrl.mpd.random;
		}
		if (action === "play" && ctrl.mpdElement.device && ctrl.mpdElement.switch) {
			benoicFactory.setElement(ctrl.mpdElement.device, "switch", ctrl.mpdElement.switch, "1");
		}
		if (action === "stop" && ctrl.mpdElement.device && ctrl.mpdElement.switch) {
			benoicFactory.setElement(ctrl.mpdElement.device, "switch", ctrl.mpdElement.switch, "0");
		}
		carleonMpdFactory.setMpdAction(ctrl.element.name, actionUrl).then(function() {
			ctrl.refreshStatus();
		}, function() {
			toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_volume_error"));
		});
	};

	this.loadPlaylist = function() {
		if (!!ctrl.selectedPlaylist) {
			if (ctrl.mpdElement.device && ctrl.mpdElement.switch) {
				benoicFactory.setElement(ctrl.mpdElement.device, "switch", ctrl.mpdElement.switch, "1");
			}
			carleonMpdFactory.loadMpdPlaylist(ctrl.element.name, ctrl.selectedPlaylist).then(function() {
				ctrl.refreshStatus();
			}, function() {
				toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_load_playlist_error"));
			});
		}
	};

	$scope.$on("refreshCarleonServices", function() {
		ctrl.refreshStatus();
	});

	$scope.$on("carleonServicesChanged", function() {
		init();
	});

	$scope.$on("$destroy", function() {
		$interval.cancel(ctrl._interval);
		ctrl._interval = null;
	});

	$window.onblur = function() {
		ctrl.hasFocus = false;
	};

	$window.onfocus = function() {
		ctrl.hasFocus = true;
	};

	init();
}

angular.module("sagremorApp").component("serviceMpd", {
		templateUrl: "components/carleonMpd/carleonMpd.template.html",
		controller: carleonMpdController,
		bindings: {
			element: "="
		}
	})
	.factory("carleonMpdFactory", ["$http", "$uibModal", "$translate", "toaster", "angharadConfig", "angharadBackendService", "sagremorConfirm", "sharedData", function($http, $uibModal, $translate, toaster, angharadConfig, angharadBackendService, sagremorConfirm, sharedData) {
		var urlBase = angharadConfig.baseUrl + angharadConfig.prefixCarleon;
		var carleonMpdFactory = {};

		/* Mpd service */
		carleonMpdFactory.getMpdServiceList = function() {
			return angharadBackendService.httpRequest("GET", urlBase + "service-mpd/");
		};

		carleonMpdFactory.getMpdService = function(name) {
			return angharadBackendService.httpRequest("GET", urlBase + "service-mpd/" + name);
		};

		carleonMpdFactory.addMpdService = function(mpdService) {
			return angharadBackendService.httpRequest("POST", urlBase + "service-mpd/", mpdService);
		};

		carleonMpdFactory.setMpdService = function(name, mpdService) {
			return angharadBackendService.httpRequest("PUT", urlBase + "service-mpd/" + name, mpdService);
		};

		carleonMpdFactory.removeMpdService = function(name) {
			return angharadBackendService.httpRequest("DELETE", urlBase + "service-mpd/" + name);
		};

		carleonMpdFactory.getMpdStatus = function(name) {
			return angharadBackendService.httpRequest("GET", urlBase + "service-mpd/" + name + "/status");
		};

		carleonMpdFactory.getMpdPlaylists = function(name) {
			return angharadBackendService.httpRequest("GET", urlBase + "service-mpd/" + name + "/playlists");
		};

		carleonMpdFactory.setMpdAction = function(name, action) {
			return angharadBackendService.httpRequest("PUT", urlBase + "service-mpd/" + name + "/action/" + action);
		};

		carleonMpdFactory.loadMpdPlaylist = function(name, playlist) {
			return angharadBackendService.httpRequest("PUT", urlBase + "service-mpd/" + name + "/playlist/" + playlist);
		};

		carleonMpdFactory.setMpdVolume = function(name, volume) {
			return angharadBackendService.httpRequest("PUT", urlBase + "service-mpd/" + name + "/volume/" + volume);
		};

		carleonMpdFactory.addService = function() {
			var modalInstance = $uibModal.open({
				animation: true,
				templateUrl: "components/carleonMpd/carleonMpd.modal.html",
				controller: "carleonMpdModalCtrl",
				controllerAs: "carleonMpdModalCtrl",
				size: "md",
				resolve: {
					mpd: function() {
						return null;
					}
				}
			});
		};

		carleonMpdFactory.editService = function(service) {
			var modalInstance = $uibModal.open({
				animation: true,
				templateUrl: "components/carleonMpd/carleonMpd.modal.html",
				controller: "carleonMpdModalCtrl",
				controllerAs: "carleonMpdModalCtrl",
				size: "md",
				resolve: {
					mpd: function() {
						return service;
					}
				}
			});
		};

		carleonMpdFactory.removeService = function(service) {
			return sagremorConfirm.open($translate.instant("carleon_mpd_remove"), $translate.instant("carleon_mpd_confirm")).then(function() {
				return carleonMpdFactory.removeMpdService(service.name).then(function() {
					var injector = sharedData.get("carleonServices", "service-mpd");
					_.remove(injector.element, function(element) {
						return element.name === service.name;
					});
					sharedData.add("carleonServices", "service-mpd", injector);
					toaster.pop("success", $translate.instant("carleon_mpd_remove"), $translate.instant("carleon_mpd_remove_success"));
				}, function() {
					toaster.pop("error", $translate.instant("carleon_mpd_remove"), $translate.instant("carleon_mpd_remove_error"));
				});
			});
		};

		return carleonMpdFactory;

	}])
	.config(["$translatePartialLoaderProvider", function run($translatePartialLoaderProvider) {
		$translatePartialLoaderProvider.addPart("carleonMpd");
	}])
	.run(["sagGenericInjectorManager", "carleonMpdFactory", function(sagGenericInjectorManager, carleonMpdFactory) {
		sagGenericInjectorManager.add({
			type: "service-mpd",
			directive: "service-mpd",
			groupTitle: "carleon_mpd_title",
			service: carleonMpdFactory,
			carleonService: true,
			size: 2,
			leftMenu: {
				title: "carleon_music_title",
				target: "music"
			},
			widgetHeight: 4,
			widgetWidth: 3,
			icon: "music",
			commands: {
				action: {
					title: "carleon_mpd_command_action_title",
					parameters: {
						action: "carleon_mpd_command_action_parameter_action_title",
					},
					result: {
						type: "boolean",
						title: "carleon_mpd_command_action_result_title"
					}
				},
				load_playlist: {
					title: "carleon_mpd_command_playlist_title",
					parameters: {
						playlist: "carleon_mpd_command_playlist_parameter_plsylist_title",
					},
					result: {
						type: "boolean",
						title: "carleon_mpd_command_playlist_result_title"
					}
				},
				set_volume: {
					title: "carleon_mpd_command_volume_title",
					parameters: {
						volume: "carleon_mpd_command_volume_parameter_volume_title",
					},
					result: {
						type: "boolean",
						title: "carleon_mpd_command_volume_result_title"
					}
				}
			}
		});
	}]);

angular.module("sagremorApp")
		.controller("carleonMpdModalCtrl", 
		["$rootScope", "$uibModalInstance", "$translate", "sharedData", "toaster", "carleonMpdFactory", "mpd", function($rootScope, $uibModalInstance, $translate, sharedData, toaster, carleonMpdFactory, mpd) {
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
		}]
);

function sagContainerController ($rootScope, $translate, toaster, sharedData, angharadFactory) {
    var ctrl = this;
    
    this.scripts = [];
    
    function init() {
		var tag = "SGMR$SCR$";
		switch (ctrl.type) {
			case "switch":
				tag += "B$" + ctrl.element.device + "$switch$" + ctrl.element.name;
				break;
			case "dimmer":
				tag += "B$" + ctrl.element.device + "$dimmer$" + ctrl.element.name;
				break;
			case "heater":
				tag += "B$" + ctrl.element.device + "$heater$" + ctrl.element.name;
				break;
			case "sensor":
				tag += "B$" + ctrl.element.device + "$sensor$" + ctrl.element.name;
				break;
			case "scheduler":
				tag += "A$scheduler$" + ctrl.element.name;
				break;
			default:
				// Carleon service
				tag += "C$" + ctrl.type + "$" + ctrl.element.name;
				break;
		}
		ctrl.scripts = _.filter(sharedData.all("angharadScripts"), function (script) {
			return !!_.find(script.options.tags, function (oneTag) {
				return oneTag === tag;
			});
		});
    }
    
    this.runScript = function (script) {
		var loaderToast = toaster.pop({type: "wait", title: $translate.instant("angharad_loading_script_title"), body: $translate.instant("angharad_loading_script_message"), timeout: 0, showCloseButton: false});
		angharadFactory.runScript(script.name).then(function () {
			toaster.pop("success", $translate.instant("script_run", {name: script.name}), $translate.instant("script_run_success"));
		}, function (error) {
			toaster.pop("error", $translate.instant("script_run", {name: script.name}), $translate.instant("script_run_error"));
		})["finally"](function () {
			$rootScope.$broadcast("refresh");
			toaster.clear(loaderToast);
		});
	};
    
    ctrl.menuSelect = function(menuItem, element) {
        menuItem.action(element);
    };
    
    init();
}

angular.module("sagremorApp").component("sagContainer", {
    templateUrl: "components/container/container.template.html",
    controller: sagContainerController,
    transclude: true,
    bindings: {
        type: "=",
        menu: "=",
        title: "=",
        size: "=",
        element: "=",
        dashboard: "=",
        sagParams: "=",
        icon: "="
    }
});

angular.module("sagremorApp")
  .factory("angharadBackendService",
  ["$http", "$q", "$rootScope", "AccessToken", "sagremorParams", "sharedData", function($http, $q, $rootScope, AccessToken, sagremorParams, sharedData) {
    var angharadBackendFactory = {};
    
    angharadBackendFactory.httpRequest = function (method, url, data, params) {
      var deferred = $q.defer();
      
      var token = sharedData.get("oauthToken", "token");
      var now = new Date();
      if (token.expires_at.valueOf() < now.valueOf()) {
        // This is a lousy way to refresh the token if the refresh wasn't called in time, for example after sleep mode
        AccessToken.forceRefresh().then(function (token) {
          $http.defaults.headers.common.Authorization = "Bearer " + token.access_token;
          $http({method: method, url: url, data: data, params: params}).then(function (response) {
            deferred.resolve(response.data);
          }, function (error) {
            deferred.reject(error);
          });
        }, function (error) {
          deferred.reject(error);
        });
      } else {
        $http({method: method, url: url, data: data, params: params}).then(function (response) {
          deferred.resolve(response.data);
        }, function (error) {
          deferred.reject(error);
        });
      }
      
      return deferred.promise;
    };
    
    return angharadBackendFactory;
  }])
  .factory("angharadFactory", 
  ["$http", "$q", "angharadConfig", "angharadBackendService", "sagremorParams", function($http, $q, angharadConfig, angharadBackendService, sagremorParams) {

    var urlBase = angharadConfig.baseUrl + angharadConfig.prefixAngharad;
    var dataFactory = {};
    
    dataFactory.getSumboduleList = function () {
      return angharadBackendService.httpRequest("GET", urlBase + "submodule/");
    };

    dataFactory.getSubmodule = function (name) {
      return angharadBackendService.httpRequest("GET", urlBase + "submodule/" + name);
    };

    dataFactory.enableSubmodule = function (name, enabled) {
      return angharadBackendService.httpRequest("GET", urlBase + "submodule/" + name + "/enable/" + (enabled?"1":"0"));
    };

    dataFactory.getScriptList = function () {
      return angharadBackendService.httpRequest("GET", urlBase + "script/");
    };

    dataFactory.getScript = function (name) {
      return angharadBackendService.httpRequest("GET", urlBase + "script/" + name);
    };

    dataFactory.addScript = function (script) {
      return angharadBackendService.httpRequest("POST", urlBase + "script/", script);
    };

    dataFactory.setScript = function (name, script) {
      return angharadBackendService.httpRequest("PUT", urlBase + "script/" + name, script);
    };

    dataFactory.removeScript = function (name) {
      return angharadBackendService.httpRequest("DELETE", urlBase + "script/" + name);
    };

    dataFactory.runScript = function (name) {
      return angharadBackendService.httpRequest("GET", urlBase + "script/" + name + "/run");
    };

    dataFactory.getSchedulerList = function () {
      return angharadBackendService.httpRequest("GET", urlBase + "scheduler/");
    };

    dataFactory.getScheduler = function (name) {
      return angharadBackendService.httpRequest("GET", urlBase + "scheduler/" + name);
    };

    dataFactory.enableScheduler = function (name, enabled) {
      return angharadBackendService.httpRequest("GET", urlBase + "scheduler/" + name + "/enable/" + (enabled?"1":"0"));
    };

    dataFactory.addScheduler = function (scheduler) {
      return angharadBackendService.httpRequest("POST", urlBase + "scheduler/", scheduler);
    };

    dataFactory.setScheduler = function (name, scheduler) {
      return angharadBackendService.httpRequest("PUT", urlBase + "scheduler/" + name, scheduler);
    };

    dataFactory.removeScheduler = function (name) {
      return angharadBackendService.httpRequest("DELETE", urlBase + "scheduler/" + name);
    };

    dataFactory.getTriggerList = function () {
      return angharadBackendService.httpRequest("GET", urlBase + "trigger/");
    };

    dataFactory.getTrigger = function (name) {
      return angharadBackendService.httpRequest("GET", urlBase + "trigger/" + name);
    };

    dataFactory.enableTrigger = function (name, enabled) {
      return angharadBackendService.httpRequest("GET", urlBase + "trigger/" + name + "/enable/" + (enabled?"1":"0"));
    };

    dataFactory.addTrigger = function (trigger) {
      return angharadBackendService.httpRequest("POST", urlBase + "trigger/", trigger);
    };

    dataFactory.setTrigger = function (name, trigger) {
      return angharadBackendService.httpRequest("PUT", urlBase + "trigger/" + name, trigger);
    };

    dataFactory.removeTrigger = function (name) {
      return angharadBackendService.httpRequest("DELETE", urlBase + "trigger/" + name);
    };

    /* Profile endpoints */
    dataFactory.getProfileList = function () {
      return angharadBackendService.httpRequest("GET", urlBase + "profile");
    };

    dataFactory.getProfile = function (profile_id) {
      return angharadBackendService.httpRequest("GET", urlBase + "profile/" + profile_id);
    };

    dataFactory.setProfile = function (profile_id, profile) {
      return angharadBackendService.httpRequest("PUT", urlBase + "profile/" + profile_id, profile);
    };

    dataFactory.removeProfile = function (profile_id) {
      return angharadBackendService.httpRequest("DELETE", urlBase + "profile/" + profile_id);
    };
    
    /* Users endpoints */
    dataFactory.getUserList = function () {
      return angharadBackendService.httpRequest("GET", urlBase + "user/");
    };

    dataFactory.addUser = function (user) {
      return angharadBackendService.httpRequest("POST", urlBase + "user/", user);
    };

    dataFactory.setUser = function (name, user) {
      return angharadBackendService.httpRequest("PUT", urlBase + "user/" + name, user);
    };

    dataFactory.removeUser = function (name) {
      return angharadBackendService.httpRequest("DELETE", urlBase + "user/" + name);
    };
    
    /* Token endpoints */
    dataFactory.getTokenList = function (options) {
      return angharadBackendService.httpRequest("GET", urlBase + "token/", null, options);
    };

    dataFactory.revokeToken = function (token) {
      return angharadBackendService.httpRequest("POST", urlBase + "token/", {token: token});
    };

    
    dataFactory.saveCurrentProfile = function () {
      var profile = sagremorParams.currentProfile;
      var deferred = $q.defer();
      if (!!profile) {
        return dataFactory.setProfile(profile.name, profile);
      } else {
        deferred.reject(error);
        return deferred.promise;
      }
    };

    return dataFactory;
  }]
);

angular.module("sagremorApp")
    .factory("benoicFactory", 
    ["$http", "angharadConfig", "angharadBackendService", function($http, angharadConfig, angharadBackendService) {

    var urlBase = angharadConfig.baseUrl + angharadConfig.prefixBenoic;
    var dataFactory = {};

    dataFactory.getDeviceTypes = function () {
        return angharadBackendService.httpRequest("GET", urlBase + "deviceTypes/");
    };

    dataFactory.reloadDeviceTypes = function () {
        return angharadBackendService.httpRequest("PUT", urlBase + "deviceTypes/reload");
    };

    dataFactory.getDeviceList = function () {
        return angharadBackendService.httpRequest("GET", urlBase + "device/");
    };

    dataFactory.getDevice = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name);
    };
    
    dataFactory.addDevice = function (device) {
        return angharadBackendService.httpRequest("POST", urlBase + "device/", device);
    };

    dataFactory.setDevice = function (device) {
        return angharadBackendService.httpRequest("PUT", urlBase + "device/" + device.name, device);
    };

    dataFactory.removeDevice = function (name) {
        return angharadBackendService.httpRequest("DELETE", urlBase + "device/" + name);
    };
    
    dataFactory.connectDevice = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name + "/connect");
    };

    dataFactory.disconnectDevice = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name + "/disconnect");
    };

    dataFactory.pingDevice = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name + "/ping");
    };

    dataFactory.getDeviceOverview = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name + "/overview");
    };

    dataFactory.setElement = function (deviceName, elementType, elementName, elementValue, additionalOptions) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + deviceName + "/" + elementType + "/" + elementName + "/" + elementValue, null, additionalOptions);
    };

    dataFactory.updateElement = function (deviceName, elementType, elementName, element) {
        return angharadBackendService.httpRequest("PUT", urlBase + "device/" + deviceName + "/" + elementType + "/" + elementName, element);
    };

    dataFactory.getMonitor = function (deviceName, elementType, elementName, from) {
        return angharadBackendService.httpRequest("GET", urlBase + "monitor/" + deviceName + "/" + elementType + "/" + elementName, null, !!from?{from: from}:null);
    };
    
    dataFactory.addTag = function (deviceName, elementType, elementName, tag) {
        return angharadBackendService.httpRequest("PUT", urlBase + "device/" + deviceName + "/" + elementType + "/" + elementName + "/" + tag);
    };

    dataFactory.removeTag = function (deviceName, elementType, elementName, tag) {
        return angharadBackendService.httpRequest("DELETE", urlBase + "device/" + deviceName + "/" + elementType + "/" + elementName + "/" + tag);
    };

    return dataFactory;
}]);

angular.module("sagremorApp")
    .factory("carleonFactory", 
    ["$http", "$q", "angharadConfig", "angharadBackendService", "sagremorParams", function($http, $q, angharadConfig, angharadBackendService, sagremorParams) {

    var urlBase = angharadConfig.baseUrl + angharadConfig.prefixCarleon;
    var dataFactory = {};

    /* Base Carleon endpoints */
    dataFactory.getServiceList = function () {
        return angharadBackendService.httpRequest("GET", urlBase + "service");
    };

    dataFactory.reloadServiceList = function () {
        return angharadBackendService.httpRequest("PUT", urlBase + "service/reload");
    };

    dataFactory.enableService = function (service_name, enable) {
        return angharadBackendService.httpRequest("PUT", urlBase + "service/" + service_name + "/enable/" + (enable?"1":"0"));
    };

    dataFactory.elementCleanup = function (service_name, element) {
        return angharadBackendService.httpRequest("PUT", urlBase + "service/" + service_name + "/" + element + "/cleanup");
    };

    dataFactory.addTag = function (service_name, element, tag) {
        return angharadBackendService.httpRequest("PUT", urlBase + "service/" + service_name + "/" + element + "/" + tag);
    };

    dataFactory.removeTag = function (service_name, element, tag) {
        return angharadBackendService.httpRequest("DELETE", urlBase + "service/" + service_name + "/" + element + "/" + tag);
    };

    return dataFactory;
}]);

angular.module("sagremorApp")
	.factory("garethFactory", 
	["angharadConfig", "angharadBackendService", "sagremorParams", function(angharadConfig, angharadBackendService, sagremorParams) {

	var urlBase = angharadConfig.baseUrl + angharadConfig.prefixGareth;
	var dataFactory = {};

	/* Gareth endpoints */
	dataFactory.getAllMessages = function (limit, offset) {
		return angharadBackendService.httpRequest("GET", urlBase + "message", null, {limit: limit, offset: offset});
	};

	dataFactory.getFilteredMessages = function (filter, limit, offset) {
		return angharadBackendService.httpRequest("GET", urlBase + "message/" + filter, null, {limit: limit, offset: offset});
	};

	/* Filters */
	dataFactory.getFilterList = function () {
		return angharadBackendService.httpRequest("GET", urlBase + "filter");
	};

	dataFactory.getFilter = function (name) {
		return angharadBackendService.httpRequest("GET", urlBase + "filter/" + name);
	};

	dataFactory.addFilter = function (filter) {
		return angharadBackendService.httpRequest("POST", urlBase + "filter/", filter);
	};

	dataFactory.setFilter = function (name, filter) {
		return angharadBackendService.httpRequest("PUT", urlBase + "filter/" + name, filter);
	};

	dataFactory.removeFilter = function (name) {
		return angharadBackendService.httpRequest("DELETE", urlBase + "filter/" + name);
	};

	/* Alerts */
	dataFactory.getAlertList = function () {
		return angharadBackendService.httpRequest("GET", urlBase + "alert");
	};

	dataFactory.getAlert = function (type, name) {
		return angharadBackendService.httpRequest("GET", urlBase + "alert/" + type + "/" + name);
	};

	dataFactory.addAlert = function (type, alert) {
		return angharadBackendService.httpRequest("POST", urlBase + "alert/" + type, alert);
	};

	dataFactory.setAlert = function (type, name, alert) {
		return angharadBackendService.httpRequest("PUT", urlBase + "alert/" + type + "/" + name, alert);
	};

	dataFactory.removeAlert = function (type, name) {
		return angharadBackendService.httpRequest("DELETE", urlBase + "alert/" + type + "/" + name);
	};

	return dataFactory;
}]);

angular.module("sagremorApp")
	.controller("sagremorCtrl",
		["$scope", "$rootScope", "$interval", "$window", "$http", "$q", "$location", "$cookies", "$translate", "toaster", "angharadFactory", "benoicFactory", "carleonFactory", "garethFactory", "sharedData", "sagremorParams", "angharadConfig", function($scope, $rootScope, $interval, $window, $http, $q, $location, $cookies, $translate, toaster, angharadFactory, benoicFactory, carleonFactory, garethFactory, sharedData, sagremorParams, angharadConfig) {
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
		}]
	);

angular.module("sagremorApp")
	.factory("sagremorService",
		["$uibModal", "$translate", "toaster", "sharedData", "sagremorParams", "sagremorConfirm", "angharadFactory", "benoicFactory", "carleonFactory", function($uibModal, $translate, toaster, sharedData, sagremorParams, sagremorConfirm, angharadFactory, benoicFactory, carleonFactory) {
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
		}]
	);

function sagDimmerController (benoicFactory, sagremorParams, $translate) {
    var ctrl = this;
    
    ctrl.element.valueChecked = (ctrl.element.value === 1);
    ctrl.adminMode = sagremorParams.adminMode;
    ctrl.element.newDisplay = ctrl.element.display;
    
    function init() {
        ctrl.element.type = "dimmer";
    }
    
    ctrl.setDimmer = function (value) {
        if (value === 0 && ctrl.element.value === 0) {
			benoicFactory.setElement(ctrl.element.device, "dimmer", ctrl.element.name, 101).then(function (result) {
				ctrl.element.value = result.value;
			});
		} else if (value >= 0 && value <= 100) {
			benoicFactory.setElement(ctrl.element.device, "dimmer", ctrl.element.name, value).then(function () {
				ctrl.element.value = value;
			});
		}
    };
    
    ctrl.addDimmer = function (value) {
		if (ctrl.element.value + value >= 0 && ctrl.element.value + value <= 100) {
			benoicFactory.setElement(ctrl.element.device, "dimmer", ctrl.element.name, ctrl.element.value + value).then(function () {
				ctrl.element.value += value;
			});
		}
	}
    
    ctrl.Dimmervalue = function () {
		return ctrl.element.value + "%"
	};
    
    init();
}

angular.module("sagremorApp").component("sagDimmer", {
    templateUrl: "components/dimmer/dimmer.template.html",
    controller: sagDimmerController,
    bindings: {
        element: "="
    }
})
.run(["sagGenericInjectorManager", function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "dimmer",
        directive: "sag-dimmer",
        carleonService: false
    });
}]);

angular.module("sagremorApp")
    .controller("DimmersModalCtrl",
    ["$scope", "$uibModalInstance", "$translate", "toaster", "sagremorConstant", "benoicFactory", "dimmer", function($scope, $uibModalInstance, $translate, toaster, sagremorConstant, benoicFactory, dimmer) {
        var self = this;
        
        this.dimmer = dimmer;
        this.dimmer.newDisplay = this.dimmer.display;
        
        this.monitorEveryEnum = sagremorConstant.monitoredEveryEnum;
        
        function init() {
			self.dimmer.menu = false;
            
            _.forEach(self.monitorEveryEnum, function(monitorEvery) {
                $translate(monitorEvery.label).then(function (trLabel) {
                    monitorEvery.trLabel = trLabel;
                });
            });
        }
        
        this.cancel = function () {
            $uibModalInstance.dismiss("cancel");
        };
        
        this.tr = function (id) {
			return $translate.instant(id);
		};
        
        this.saveDimmer = function () {
            self.dimmer.display = self.dimmer.newDisplay;
            self.dimmer.monitor = self.dimmer.monitorChecked?1:0;
            benoicFactory.updateElement(self.dimmer.device, "dimmer", self.dimmer.name, self.dimmer).then(function (response) {
                $scope.$broadcast("benoicDimmersChanged");
                toaster.pop("success", $translate.instant("dimmer_save"), $translate.instant("dimmer_save_success"));
            }, function (error) {
                toaster.pop("error", $translate.instant("dimmer_save"), $translate.instant("dimmer_save_error"));
            })["finally"](function () {
                $uibModalInstance.dismiss("close");
            });
        };
        
        init();
    }]
);

angular.module("sagremorApp")
    .controller("EventModalCtrl",
    ["$scope", "$rootScope", "$uibModalInstance", "$translate", "toaster", "sagremorConstant", "sagremorService", "angharadFactory", "sharedData", "sagGenericInjectorManager", "event", "eventType", function($scope, $rootScope, $uibModalInstance, $translate, toaster, sagremorConstant, sagremorService, angharadFactory, sharedData, sagGenericInjectorManager, event, eventType) {
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
    }]);

function sagSchedulerController ($translate, toaster, angharadFactory, sagremorParams, sharedData) {
    var ctrl = this;
    
    function init() {
    }
    
    this.enableScheduler = function () {
		angharadFactory.enableScheduler(ctrl.element.name, ctrl.element.enabled).then(function (result) {
			sharedData.add("angharadSchedulers", result.name, result);
			ctrl.element = result;
			toaster.pop("success", $translate.instant("scheduler_enable"), $translate.instant("scheduler_enable_success"));
		}, function () {
			toaster.pop("error", $translate.instant("scheduler_enable"), $translate.instant("scheduler_enable_error"));
		});
	};
    
    init();
}

angular.module("sagremorApp").component("sagScheduler", {
    templateUrl: "components/event/scheduler.template.html",
    controller: sagSchedulerController,
    bindings: {
        element: "="
    }
})
.run(["sagGenericInjectorManager", function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "scheduler",
        directive: "sag-scheduler",
        carleonService: false
    });
}])
.filter;

function sagTriggerController ($translate, toaster, angharadFactory, sagremorParams, sharedData) {
    var ctrl = this;
    
    function init() {
    }
    
    this.enableTrigger = function () {
		angharadFactory.enableTrigger(ctrl.element.name, ctrl.element.enabled).then(function (result) {
			sharedData.add("angharadTrigger", result.name, result);
			ctrl.element = result;
			toaster.pop("success", $translate.instant("trigger_enable"), $translate.instant("trigger_enable_success"));
		}, function () {
			toaster.pop("error", $translate.instant("trigger_enable"), $translate.instant("trigger_enable_error"));
		});
	};
    
    init();
}

angular.module("sagremorApp").component("sagTrigger", {
    templateUrl: "components/event/trigger.template.html",
    controller: sagTriggerController,
    bindings: {
        element: "="
    }
})
.run(["sagGenericInjectorManager", function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "trigger",
        directive: "sag-trigger",
        carleonService: false
    });
}])
.filter;

angular.module("sagremorApp")
.controller("GridstackController", ["$scope", function($scope) {

    var gridstack = null;

    this.init = function(element, options) {
      gridstack = element.gridstack(options).data("gridstack");
      return gridstack;
    };

    this.removeItem = function(element) {
      if(gridstack) {
        return gridstack.removeWidget(element, false);
      }
      return null;
    };

    this.addItem = function(element) {
      if(gridstack) {
        gridstack.makeWidget(element);
        return element;
      }
      return null;
    };

}]);

angular.module('sagremorApp')

.directive('gridstack', ['$timeout', function($timeout) {

    return {
      restrict: "A",
      controller: 'GridstackController',
      scope: {
        onChange: '&',
        onDragStart: '&',
        onDragStop: '&',
        onResizeStart: '&',
        onResizeStop: '&',
        options: '='
      },
      link: function (scope, element, attrs, controller, ngModel) {

        controller.init(element, scope.options);

        element.on('change', function (e, items) {
          $timeout(function() {
            scope.$apply();
            scope.onChange({event: e, items: items});
          });
        });

        element.on('dragstart', function(e, ui) {
          scope.onDragStart({event: e, ui: ui});
        });

        element.on('dragstop', function(e, ui) {
          $timeout(function() {
            scope.$apply();
            scope.onDragStop({event: e, ui: ui});
          });
        });

      }
    };

}]).directive('gridstackItem', ['$timeout', function($timeout) {

    return {
      restrict: "A",
      controller: 'GridstackController',
      require: '^gridstack',
      scope: {
        gridstackItem: '=',
        onItemAdded: '&',
        onItemRemoved: '&',
        gsItemX: '=',
        gsItemY: '=',
        gsItemWidth: '=',
        gsItemHeight: '=',
        gsItemAutopos: '='
      },
      link: function (scope, element, attrs, controller) {

        $(element).attr('data-gs-x', scope.gsItemX);
        $(element).attr('data-gs-y', scope.gsItemY);
        $(element).attr('data-gs-width', scope.gsItemWidth);
        $(element).attr('data-gs-height', scope.gsItemHeight);
        $(element).attr('data-gs-auto-position', scope.gsItemAutopos);
        var widget = controller.addItem(element);
        var item = element.data('_gridstack_node');
        $timeout(function() {
          scope.onItemAdded({item: item});
        });

        scope.$watch(function(){ return $(element).attr('data-gs-x'); }, function(val) {
          scope.gsItemX = val;
        });

        scope.$watch(function(){ return $(element).attr('data-gs-y'); }, function(val) {
          scope.gsItemY = val;
        });

        scope.$watch(function(){ return $(element).attr('data-gs-width'); }, function(val) {
          scope.gsItemWidth = val;
        });

        scope.$watch(function(){ return $(element).attr('data-gs-height'); }, function(val) {
          scope.gsItemHeight = val;
        });

        element.bind('$destroy', function() {
          var item = element.data('_gridstack_node');
          scope.onItemRemoved({item: item});
          controller.removeItem(element);
        });

      }

    };

  }]);

function sagHeaterController (benoicFactory, sagremorParams, $translate) {
    var ctrl = this;
    
    ctrl.element.valueChecked = (ctrl.element.value === 1);
    ctrl.adminMode = sagremorParams.adminMode;
    ctrl.element.newDisplay = "";
    ctrl.availableModes = [];
    
    function init() {
        ctrl.element.type = "heater";
        if (!!ctrl.element) {
			ctrl.element.newDisplay = ctrl.element.display;
			ctrl.availableModes = ctrl.element.value.availableModes;
		}
    }
    
    ctrl.setHeater = function (value) {
		if (value == null) {
			value = ctrl.element.value.command;
		} else {
			value = ctrl.element.value.command + value;
		}
        benoicFactory.setElement(ctrl.element.device, "heater", ctrl.element.name, value, {mode: ctrl.element.value.mode}).then(function () {
            ctrl.element.value.command = value;
        });
    };
    
    ctrl.displayCommandValue = function () {
		var command = !!ctrl.element?(Math.round(ctrl.element.value.command * 100) / 100):0;
        if (!!ctrl.element && !!ctrl.element.options.unit) {
			command += " " + ctrl.element.options.unit
		}
		return command;
	};
	
	ctrl.displayCommand = function (command) {
		var value = "";
		if (command < 0) {
			value = command;
		} else {
			value = "+" + command;
		}
        if (!!ctrl.element && !!ctrl.element.options.unit) {
			value += " " + ctrl.element.options.unit
		}
		return value
	};
	
	ctrl.modeDisplay = function (value) {
		return $translate.instant("heater_mode_" + value.toLowerCase());
	};
    
    init();
}

angular.module("sagremorApp").component("sagHeater", {
    templateUrl: "components/heater/heater.template.html",
    controller: sagHeaterController,
    bindings: {
        element: "="
    }
})
.run(["sagGenericInjectorManager", function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "heater",
        directive: "sag-heater",
        carleonService: false
    });
}]);

angular.module("sagremorApp")
    .controller("HeatersModalCtrl",
    ["$scope", "$uibModalInstance", "$translate", "toaster", "sagremorConstant", "benoicFactory", "heater", function($scope, $uibModalInstance, $translate, toaster, sagremorConstant, benoicFactory, heater) {
        var self = this;
        
        this.heater = heater;
        this.heater.newDisplay = this.heater.display;
        
        this.monitorEveryEnum = sagremorConstant.monitoredEveryEnum;
        
        function init() {
			self.heater.menu = false;
            
            _.forEach(self.monitorEveryEnum, function(monitorEvery) {
                $translate(monitorEvery.label).then(function (trLabel) {
                    monitorEvery.trLabel = trLabel;
                });
            });
        }
        
        this.cancel = function () {
            $uibModalInstance.dismiss("cancel");
        };
        
        this.tr = function (id) {
			return $translate.instant(id);
		};
        
        this.saveHeater = function () {
            self.heater.display = self.heater.newDisplay;
            self.heater.monitor = self.heater.monitorChecked?1:0;
            benoicFactory.updateElement(self.heater.device, "heater", self.heater.name, self.heater).then(function (response) {
                $scope.$broadcast("benoicHeatersChanged");
                toaster.pop("success", $translate.instant("heater_save"), $translate.instant("heater_save_success"));
            }, function (error) {
                toaster.pop("error", $translate.instant("heater_save"), $translate.instant("heater_save_error"));
            })["finally"](function () {
                $uibModalInstance.dismiss("close");
            });
        };
        
        init();
    }]
);

angular.module("sagremorApp")
    .controller("ConfirmModalCtrl",
    ["$uibModalInstance", "title", "message", function($uibModalInstance, title, message) {
        var self = this;
        
        this.title = title;
        this.message = message;
        
        this.cancel = function () {
            $uibModalInstance.dismiss("cancel");
        };
        
        this.ok = function () {
            $uibModalInstance.close("ok");
        };
        
    }]
);

angular.module("sagremorApp")
    .controller("EditModalCtrl",
    ["$uibModalInstance", "title", "message", "placeholder", function($uibModalInstance, title, message, placeholder) {
        var self = this;
        
        this.title = title;
        this.message = message;
        this.placeholder = placeholder;
        this.value = "";
        
        this.cancel = function () {
            $uibModalInstance.dismiss("cancel");
        };
        
        this.ok = function () {
            $uibModalInstance.close({reason: "close", value: self.value});
        };
        
    }]
);

angular.module("sagremorApp")
    .controller("ImageModalCtrl",
    ["$scope", "$uibModalInstance", "$translate", "title", "src", function($scope, $uibModalInstance, $translate, title, src) {
        var self = this;
        
        this.title = title;
        this.src = src;
        
        function init() {
        }
        
        this.close = function () {
            $uibModalInstance.dismiss("close");
        };
        
        this.displayTitle = function () {
			return $translate.instant("image_title") + self.title;
		};
		
        init();
    }]
);

angular.module("sagremorApp")
    .controller("MonitorModalCtrl",
    ["$scope", "$uibModalInstance", "$translate", "toaster", "benoicFactory", "sagremorConstant", "element", function($scope, $uibModalInstance, $translate, toaster, benoicFactory, sagremorConstant, element) {
        var self = this;
        
        this.element = element;
        this.data = [];
        this.durationList = sagremorConstant.durationList;
        this.duration = {value: 4};
        
		this.options = {
			chart: {
				type: "lineChart",
				interpolate:"basis",
				x: function(d) { return d.timestamp; },
				y: function(d) { return d.value; },
				showValues: true,
				valueFormat: function(d){
					return d3.format(",.2f")(d);
				},
				transitionDuration: 500,
				xScale : d3.time.scale(),
				xAxis: {
					axisLabel: $translate.instant("monitor_date_axis")
				},
				yAxis: {
					axisLabel: element.options.unit,
					axisLabelDistance: 30
				}
			}
		};
		
        function init() {
			self.getMonitor();
        }
        
        this.getMonitor = function () {
			var from = new Date();
			
			switch (self.duration.value) {
				case 0:
					from.setTime(from.getTime() - 1*60*60*1000);
					break;
				case 1:
					from.setTime(from.getTime() - 2*60*60*1000);
					break;
				case 2:
					from.setTime(from.getTime() - 6*60*60*1000);
					break;
				case 3:
					from.setTime(from.getTime() - 12*60*60*1000);
					break;
				case 4:
					from.setTime(from.getTime() - 24*60*60*1000);
					break;
				case 5:
					from.setTime(from.getTime() - 48*60*60*1000);
					break;
				case 6:
					from.setTime(from.getTime() - 72*60*60*1000);
					break;
				case 7:
					from.setTime(from.getTime() - 168*60*60*1000);
					break;
				case 8:
					from.setMonth(from.getMonth() - 1);
					break;
			}
			
            benoicFactory.getMonitor(element.device, element.type, element.name, Math.round(from.getTime() / 1000)).then(function (result) {
                self.data = [];
                var myData = {
					key: element.display,
					values: []
                };
                if (!!result && result.length > 0) {
					_.forEach(result, function (monitor) {
						var curDate = new Date(monitor.timestamp * 1000);
						var value = {timestamp: curDate, value: parseFloat(monitor.value)};
						myData.values.push(value);
					});
					self.data.push(myData);
				}
            });
		}
        
        this.tr = function (value) {
			return $translate.instant(value);
		};
        
        this.close = function () {
            $uibModalInstance.dismiss("close");
        };
        
        this.displayTitle = function () {
			return $translate.instant("monitor_title") + element.display;
		};
		
        init();
    }]
);

angular.module("sagremorApp")
    .controller("ScriptBindElementModalCtrl",
    ["$scope", "$uibModalInstance", "$translate", "toaster", "DTOptionsBuilder", "sharedData", "sagGenericInjectorManager", "angharadFactory", "script", function($scope, $uibModalInstance, $translate, toaster, DTOptionsBuilder, sharedData, sagGenericInjectorManager, angharadFactory, script) {
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
    }]
);

function sagMonitorController ($scope, $q, $translate, toaster, angharadFactory, benoicFactory, carleonFactory, sagremorParams, sagremorConstant, sharedData) {
    var ctrl = this;
    
    this.elementList = [];
    this.elementListDisplayed = [];
    this.colorList = [];
    this.newElement = false;
    this.newElementColor = false;
    this.durationList = sagremorConstant.durationList;
    this.duration = {value: 4};
    
    this.data = [];
    this.labels = [];
    this.series = [];
    
    this.datasetOverride = [{
		fill: false
	}];
	

	this.options = {
		chart: {
			type: "lineChart",
			interpolate:"basis",
			x: function(d) { return d.timestamp; },
			y: function(d) { return d.value; },
			showValues: true,
			valueFormat: function(d){
				return d3.format(",.2f")(d);
			},
			transitionDuration: 500,
			xScale : d3.time.scale(),
			xAxis: {
				axisLabel: $translate.instant("monitor_date_axis")
			},
			yAxis: {
				axisLabel: "",
				axisLabelDistance: 30
			}
		}
	};

    function init() {
		updateDevices();
		ctrl.displayMonitor();
    }
    
    this.tr = function (value) {
		return $translate.instant(value);
	};
	
    $scope.$on("updateMonitor-"+ctrl.element.name, function () {
		ctrl.displayMonitor();
	});
	
	function updateDevices() {
		var devices = sharedData.all("benoicDevices");
		
		_.forEach(devices, function (device, deviceName) {
			if (!!device.element) {
				_.forEach(device.element.switches, function (element, name) {
					if (element.monitored && element.enabled) {
						var elt = {
							name: name,
							display: element.display,
							device: deviceName,
							type: "switch"
						};
						ctrl.elementList.push(elt);
					}
				});
			}
		});
			
		_.forEach(devices, function (device, deviceName) {
			if (!!device.element) {
				_.forEach(device.element.dimmers, function (element, name) {
					if (element.monitored && element.enabled) {
						var elt = {
							name: name,
							display: element.display,
							device: deviceName,
							type: "dimmer"
						};
						ctrl.elementList.push(elt);
					}
				});
			}
		});
			
		_.forEach(devices, function (device, deviceName) {
			if (!!device.element) {
				_.forEach(device.element.heaters, function (element, name) {
					if (element.monitored && element.enabled) {
						var elt = {
							name: name,
							display: element.display,
							device: deviceName,
							type: "heater"
						};
						ctrl.elementList.push(elt);
					}
				});
			}
		});
			
		_.forEach(devices, function (device, deviceName) {
			if (!!device.element) {
				_.forEach(device.element.sensors, function (element, name) {
					if (element.monitored && element.enabled) {
						var elt = {
							name: name,
							display: element.display,
							device: deviceName,
							type: "sensor"
						};
						ctrl.elementList.push(elt);
					}
				});
			}
		});
	}
	
    this.addElement = function () {
		ctrl.newElement.color = ctrl.newElementColor;
		ctrl.element.elements.push(ctrl.newElement);
		angharadFactory.saveCurrentProfile().then(function () {
			ctrl.displayMonitor();
		}, function () {
			toaster.pop("error", $translate.instant("profile_save"), $translate.instant("profile_save_error"));
		})["finally"](function () {
		});
	};
	
	this.displayMonitor = function() {
		var from = new Date();
		
		switch (ctrl.duration.value) {
			case 0:
				from.setTime(from.getTime() - 1*60*60*1000);
				break;
			case 1:
				from.setTime(from.getTime() - 2*60*60*1000);
				break;
			case 2:
				from.setTime(from.getTime() - 6*60*60*1000);
				break;
			case 3:
				from.setTime(from.getTime() - 12*60*60*1000);
				break;
			case 4:
				from.setTime(from.getTime() - 24*60*60*1000);
				break;
			case 5:
				from.setTime(from.getTime() - 48*60*60*1000);
				break;
			case 6:
				from.setTime(from.getTime() - 72*60*60*1000);
				break;
			case 7:
				from.setTime(from.getTime() - 168*60*60*1000);
				break;
			case 8:
				from.setMonth(from.getMonth() - 1);
				break;
		}
		ctrl.colorList = angular.copy(sagremorConstant.colorList);
		ctrl.elementListDisplayed = angular.copy(ctrl.elementList);
		var httpMonitor = [];
		
		_.forEach(ctrl.element.elements, function (element) {
			httpMonitor.push(benoicFactory.getMonitor(element.device, element.type, element.name, Math.round(from.getTime() / 1000)));
		});
		
		ctrl.data = [];
		$q.all(httpMonitor).then(function (results) {
			for (key in results) {
				_.remove(ctrl.elementListDisplayed, function (curElement) {
					return curElement.device === ctrl.element.elements[key].device &&
							curElement.type === ctrl.element.elements[key].type &&
							curElement.name === ctrl.element.elements[key].name;
				});
				ctrl.series.push(ctrl.element.elements[key].display);
                var myData = {
					key: ctrl.element.elements[key].display,
					values: []
                };
                if (!!results[key] && results[key].length > 0) {
					_.forEach(results[key], function (monitor) {
						var curDate = new Date(monitor.timestamp * 1000);
						var value = {timestamp: curDate, value: parseFloat(monitor.value)};
						myData.values.push(value);
					});
					ctrl.data.push(myData);
				}
			}
		}, function (error) {
		});
	}
	
	this.newElementValid = function () {
		return !!ctrl.newElement;
	};
	
	this.removeSerie = function (serie) {
		if (ctrl.element.elements.indexOf(serie) >= 0) {
			ctrl.element.elements.splice(ctrl.element.elements.indexOf(serie), 1);
			angharadFactory.saveCurrentProfile().then(function () {
				ctrl.displayMonitor();
			}, function () {
				toaster.pop("error", $translate.instant("profile_save"), $translate.instant("profile_save_error"));
			})["finally"](function () {
				ctrl.newElementColor = false;
				ctrl.newElement = false;
			});
		}
	};
    
    init();
}

angular.module("sagremorApp").component("sagMonitor", {
    templateUrl: "components/monitor/monitor.template.html",
    controller: sagMonitorController,
    bindings: {
        element: "="
    }
})
.run(["sagGenericInjectorManager", function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "monitor",
        directive: "sag-monitor",
        carleonService: false
    });
}]);

function sagScriptController ($rootScope, $translate, toaster, angharadFactory, sagremorParams) {
    var ctrl = this;
    
    this.loaderToast = null;
    
    function init() {
    }
    
    this.runScript = function () {
			toaster.clear(self.loaderToast);
			self.loaderToast = toaster.pop({type: "wait", title: $translate.instant("angharad_loading_script_title"), body: $translate.instant("angharad_loading_script_message"), timeout: 0, showCloseButton: false});
			angharadFactory.runScript(ctrl.element.name).then(function () {
				toaster.pop("success", $translate.instant("script_run", {name: ctrl.element.name}), $translate.instant("script_run_success"));
			}, function (error) {
				toaster.pop("error", $translate.instant("script_run", {name: ctrl.element.name}), $translate.instant("script_run_error"));
			})["finally"](function () {
				toaster.clear(self.loaderToast);
				$rootScope.$broadcast("refresh");
			});
		};
    
    init();
}

angular.module("sagremorApp").component("sagScript", {
    templateUrl: "components/script/script.template.html",
    controller: sagScriptController,
    bindings: {
        element: "="
    }
})
.run(["sagGenericInjectorManager", function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "script",
        directive: "sag-script",
        carleonService: false
    });
}]);

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
    ["$scope", "$rootScope", "$uibModalInstance", "$translate", "toaster", "sagremorConstant", "sagremorService", "angharadFactory", "sharedData", "sagGenericInjectorManager", "script", function($scope, $rootScope, $uibModalInstance, $translate, toaster, sagremorConstant, sagremorService, angharadFactory, sharedData, sagGenericInjectorManager, script) {
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
    }]);

function sagSensorController (benoicFactory, sagremorParams, $translate) {
    var ctrl = this;
    
    ctrl.adminMode = sagremorParams.adminMode;
    ctrl.element.newDisplay = ctrl.element.display;
    ctrl.messages = {};
    
    function init() {
        ctrl.element.type = "sensor";
    }
    
    ctrl.elementDisplayValue = function() {
		var value = ctrl.element.value;
		if (!isNaN(parseFloat(value)) && isFinite(value)) {
			value = (Math.round(value * 100) / 100);
		}
        if (!!ctrl.element && !!ctrl.element.options.unit) {
			value += " " + ctrl.element.options.unit
		}
		return value;
	};
    
    init();
}

angular.module("sagremorApp").component("sagSensor", {
    templateUrl: "components/sensor/sensor.template.html",
    controller: sagSensorController,
    bindings: {
        element: "="
    }
})
.run(["sagGenericInjectorManager", function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "sensor",
        directive: "sag-sensor",
        carleonService: false
    });
}]);

angular.module("sagremorApp")
    .controller("SensorsModalCtrl",
    ["$scope", "$uibModalInstance", "$translate", "toaster", "sagremorConstant", "benoicFactory", "sensor", function($scope, $uibModalInstance, $translate, toaster, sagremorConstant, benoicFactory, sensor) {
        var self = this;
        
        this.sensor = sensor;
        this.sensor.newDisplay = this.sensor.display;
        
        this.monitorEveryEnum = sagremorConstant.monitoredEveryEnum;
        
        function init() {
			self.sensor.menu = false;
            
            _.forEach(self.monitorEveryEnum, function(monitorEvery) {
                $translate(monitorEvery.label).then(function (trLabel) {
                    monitorEvery.trLabel = trLabel;
                });
            });
        }
        
        this.tr = function (id) {
			return $translate.instant(id);
		};
        
        this.cancel = function () {
            $uibModalInstance.dismiss("cancel");
        };
        
        this.saveSensor = function () {
            self.sensor.display = self.sensor.newDisplay;
            self.sensor.monitor = self.sensor.monitorChecked?1:0;
            benoicFactory.updateElement(self.sensor.device, "sensor", self.sensor.name, self.sensor).then(function (response) {
                $scope.$broadcast("benoicSensorsChanged");
                toaster.pop("success", $translate.instant("sensor_save"), $translate.instant("sensor_save_success"));
            }, function (error) {
                toaster.pop("error", $translate.instant("sensor_save"), $translate.instant("sensor_save_error"));
            })["finally"](function () {
                $uibModalInstance.dismiss("close");
            });
        };
        
        init();
    }]
);

function sagSwitchController (benoicFactory, sagremorParams, $translate) {
    var ctrl = this;
    
    ctrl.element.valueChecked = (ctrl.element.value === 1);
    ctrl.adminMode = sagremorParams.adminMode;
    ctrl.element.newDisplay = ctrl.element.display;
    ctrl.messages = {};
    
    function init() {
        ctrl.element.type = "switch";
    }
    
    ctrl.toggleSwitcher = function () {
        ctrl.element.value = ctrl.element.valueChecked?1:0;
        benoicFactory.setElement(ctrl.element.device, 'switch', ctrl.element.name, ctrl.element.value);
    };
    
    init();
}

angular.module('sagremorApp').component('sagSwitch', {
    templateUrl: 'components/switch/switch.template.html',
    controller: sagSwitchController,
    bindings: {
        element: '='
    }
})
.run(["sagGenericInjectorManager", function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "switch",
        directive: "sag-switch",
        carleonService: false
    });
}]);

angular.module("sagremorApp")
    .controller("SwitchesModalCtrl",
    ["$scope", "$uibModalInstance", "$translate", "toaster", "sagremorConstant", "benoicFactory", "switcher", function($scope, $uibModalInstance, $translate, toaster, sagremorConstant, benoicFactory, switcher) {
        var self = this;
        
        this.switcher = switcher;
        this.switcher.newDisplay = this.switcher.display;
        
        this.monitorEveryEnum = sagremorConstant.monitoredEveryEnum;
        
        function init() {
			self.switcher.menu = false;
            
            _.forEach(self.monitorEveryEnum, function(monitorEvery) {
                $translate(monitorEvery.label).then(function (trLabel) {
                    monitorEvery.trLabel = trLabel;
                });
            });
        }
        
        this.cancel = function () {
            $uibModalInstance.dismiss("cancel");
        };
        
        this.tr = function (id) {
			return $translate.instant(id);
		};
        
        this.saveSwitcher = function () {
            self.switcher.display = self.switcher.newDisplay;
            self.switcher.monitor = self.switcher.monitorChecked?1:0;
            benoicFactory.updateElement(self.switcher.device, "switch", self.switcher.name, self.switcher).then(function (response) {
                $scope.$broadcast("benoicSwitchesChanged");
                toaster.pop("success", $translate.instant("switch_save"), $translate.instant("switch_save_success"));
            }, function (error) {
                toaster.pop("error", $translate.instant("switch_save"), $translate.instant("switch_save_error"));
            })["finally"](function () {
                $uibModalInstance.dismiss("close");
            });
        };
        
        init();
    }]
);

function topLeftMenuCtrl ($scope, $translate, sagremorParams, sagGenericInjectorManager, sharedData) {
    var self = this;
    
    this.serviceList = [];
    this.sagremorParams = sagremorParams;
    
    function init() {
		self.loadMenu();
	}
	
	this.loadMenu = function () {
		self.serviceList = [];
		_.forEach(sagGenericInjectorManager.components, function (inject) {
			var service = sharedData.get("carleonServices", inject.type);
			if (inject.carleonService && inject.leftMenu && !!service && service.enabled) {
				var menu = inject.leftMenu;
				menu.icon = inject.icon;
				if (!_.find(self.serviceList, {target: menu.target})) {
					self.serviceList.push(menu);
				}
			}
		});
	};
    
    /**
     * Because I couldn't find how to do it with otherwise...
     */
    this.selectTab = function (tabName) {
        $(".sag-menu").removeClass("active");
        $(tabName).addClass("active");
    };
    
	$scope.$on("submodulesChanged", function () {
		self.sagremorParams = sagremorParams;
	});
	
	$scope.$on("carleonServicesChanged", function () {
		self.loadMenu();
	});

    init();
}

angular.module("sagremorApp").component("topLeftMenu", {
    templateUrl: "components/topLeftMenu/topLeftMenu.template.html",
    controller: topLeftMenuCtrl,
    controllerAs: "topLeftMenuCtrl"
});

function topRightMenuCtrl($scope, $rootScope, $location, $http, $translate, $cookies, angharadFactory, sagremorConstant, sagremorParams, angharadConfig) {
	var self = this;

	self.sagremorParams = sagremorParams;

	self.selectedLang = $translate.use();

	self.langList = sagremorConstant.langList;

	self.profiles = sagremorParams.profiles;
	self.currentProfileName = !!sagremorParams.currentProfile ? sagremorParams.currentProfile.name : "";

	self.oauth2 = angharadConfig.oauth2;

	if ($location.search().type) {
		self.oauth2.responseType = $location.search().type;
	}

	self.changeLang = function() {
		$translate.use(self.selectedLang).then(function() {
			$rootScope.$broadcast("changeLang");
		});
	};

	self.changeProfile = function() {
		_.forEach(sagremorParams.profiles, function(profile) {
			if (profile.name === self.currentProfileName) {
				sagremorParams.currentProfile = profile;
				var now = new Date();
				var exp = new Date(now.getFullYear() + 10, now.getMonth(), now.getDate());
				$cookies.put("ANGHARAD_PROFILE", self.currentProfileName, {
					expires: exp
				});
				$rootScope.$broadcast("angharadProfileUpdated");
				$rootScope.$broadcast("refreshDashboard");
			}
		});
	};

	self.refresh = function() {
		$rootScope.$broadcast("refresh");
	};

	self.reconnect = function() {
		$rootScope.$broadcast("reconnect");
	};

	$scope.$on("angharadProfileChanged", function() {
		self.profiles = sagremorParams.profiles;
		self.currentProfileName = !!sagremorParams.currentProfile ? sagremorParams.currentProfile.name : "";
	});

	$scope.$on("angharadProfileUpdated", function() {
		self.profiles = sagremorParams.profiles;
		self.currentProfileName = !!sagremorParams.currentProfile ? sagremorParams.currentProfile.name : "";
	});
}

angular.module("sagremorApp").component("topRightMenu", {
	templateUrl: "components/topRightMenu/topRightMenu.template.html",
	controller: topRightMenuCtrl,
	controllerAs: "topRightMenuCtrl"
});

angular.module("sagremorApp").run(["$templateCache", function($templateCache) {$templateCache.put("components/topLeftMenu/topLeftMenu.template.html","<div data-ng-if=\"topLeftMenuCtrl.sagremorParams.loggedIn\"><ul class=\"nav nav-tabs nav-stacked nav-pills topnav\"><li id=\"tabDashboard\" class=\"sag-menu active text-center\"><a data-ui-sref=\"dashboard\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabDashboard\')\" title=\"{{\'menu_home\'|translate}}\"><i class=\"fa fa-home fa-fw fa-3x\"></i></a></li><li id=\"tabSwitches\" class=\"sag-menu text-center\" data-ng-if=\"topLeftMenuCtrl.sagremorParams.benoicEnabled\"><a data-ui-sref=\"switches\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabSwitches\')\" title=\"{{\'menu_lights\'|translate}}\"><i class=\"fa fa-lightbulb-o fa-fw fa-3x\"></i></a></li><li id=\"tabHeaters\" class=\"sag-menu text-center\" data-ng-if=\"topLeftMenuCtrl.sagremorParams.benoicEnabled\"><a data-ui-sref=\"heaters\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabHeaters\')\" title=\"{{\'menu_heaters\'|translate}}\"><i class=\"fa fa-fire fa-3x\"></i></a></li><li id=\"tabSensors\" class=\"sag-menu text-center\" data-ng-if=\"topLeftMenuCtrl.sagremorParams.benoicEnabled\"><a data-ui-sref=\"sensors\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabSensors\')\" title=\"{{\'menu_sensors\'|translate}}\"><i class=\"fa fa-line-chart fa-fw fa-3x\"></i></a></li><li id=\"tabService{{service.target}}\" class=\"sag-menu text-center\" data-ng-repeat=\"service in topLeftMenuCtrl.serviceList\" data-ng-if=\"topLeftMenuCtrl.sagremorParams.carleonEnabled\"><a data-ui-sref=\"service({service: service.target})\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabService\' + service.target)\" title=\"{{service.name|translate}}\"><i data-ng-class=\"{\'fa fa-{{service.icon}} fa-fw fa-3x\': true}\"></i></a></li><li id=\"tabScripts\" class=\"sag-menu text-center\"><a data-ui-sref=\"scripts\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabScripts\')\" title=\"{{\'menu_scripts\'|translate}}\"><i class=\"fa fa-tasks fa-fw fa-3x\"></i></a></li><li id=\"tabEvents\" class=\"sag-menu text-center\"><a data-ui-sref=\"events\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabEvents\')\" title=\"{{\'menu_events\'|translate}}\"><i class=\"fa fa-calendar fa-fw fa-3x\"></i></a></li><li id=\"tabMonitors\" class=\"sag-menu text-center\"><a data-ui-sref=\"monitors\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabMonitors\')\" title=\"{{\'menu_monitor\'|translate}}\"><i class=\"fa fa-bar-chart fa-fw fa-3x\"></i></a></li><li id=\"tabLogs\" class=\"sag-menu text-center\" data-ng-if=\"topLeftMenuCtrl.sagremorParams.garethEnabled\" title=\"{{\'menu_logs\'|translate}}\"><a data-ui-sref=\"logs\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabLogs\')\"><i class=\"fa fa-history fa-fw fa-3x\"></i></a></li><li id=\"tabParameters\" class=\"sag-menu text-center\"><a data-ui-sref=\"parameters\" data-toggle=\"tab\" data-ng-click=\"topLeftMenuCtrl.selectTab(\'#tabParameters\')\" title=\"{{\'menu_parameters\'|translate}}\"><i class=\"fa fa-cogs fa-fw fa-3x\"></i></a></li></ul></div>");
$templateCache.put("components/topRightMenu/topRightMenu.template.html","<div class=\"top-right-menu\" data-uib-dropdown=\"\"><button class=\"btn btn-primary text-left\" type=\"button\" data-uib-dropdown-toggle=\"\"><i class=\"fa fa-user fa-3x\" aria-hidden=\"true\"></i></button><ul class=\"container-fluid dropdown-menu dropdown-menu-right top-right-menu-list\" data-uib-dropdown-menu=\"\" data-uib-dropdown-menu-auto-close=\"outsideClick\" data-ng-click=\"$event.stopPropagation()\"><li class=\"row control-group\" data-ng-if=\"topRightMenuCtrl.sagremorParams.loggedIn\"><div class=\"col-xs-6 controls\"><label for=\"admin-mode\" data-translate=\"top_right_menu_admin_mode\"></label></div><div class=\"col-xs-6 controls\"><input class=\"form-control\" id=\"admin-mode\" name=\"admin-mode\" data-toggle=\"toggle\" data-bs-switch=\"\" data-switch-size=\"mini\" type=\"checkbox\" data-ng-model=\"topRightMenuCtrl.sagremorParams.adminMode\"></div></li><li class=\"row control-group\"><div class=\"col-xs-6 controls\"><label for=\"sag-lang\" data-translate=\"top_right_menu_lang\"></label></div><div class=\"col-xs-6 controls\"><select class=\"form-control\" id=\"sag-lang\" name=\"sag-lang\" data-ng-model=\"topRightMenuCtrl.selectedLang\" data-ng-options=\"lang.name as lang.display for lang in topRightMenuCtrl.langList\" data-ng-change=\"topRightMenuCtrl.changeLang()\"></select></div></li><li class=\"row control-group\" data-ng-if=\"topRightMenuCtrl.sagremorParams.loggedIn\"><div class=\"col-xs-6 controls\"><label for=\"sag-lang\" data-translate=\"top_right_menu_profile\"></label></div><div class=\"col-xs-6 controls\"><select class=\"form-control\" id=\"sag-lang\" name=\"sag-lang\" data-ng-model=\"topRightMenuCtrl.currentProfileName\" data-ng-options=\"profile.name as profile.name for profile in topRightMenuCtrl.profiles\" data-ng-change=\"topRightMenuCtrl.changeProfile()\"></select></div></li><li class=\"row control-group\" data-ng-if=\"topRightMenuCtrl.sagremorParams.loggedIn\"><div class=\"col-xs-6 controls\"><label for=\"sag-refresh\" data-translate=\"top_right_menu_refresh\"></label></div><div class=\"col-xs-6 controls\"><button id=\"sag-refresh\" name=\"sag-refresh\" type=\"button\" class=\"btn btn-primary btn-sm\" data-ng-click=\"topRightMenuCtrl.refresh()\"><i class=\"fa fa-refresh\" aria-hidden=\"true\"></i></button></div></li><li class=\"row control-group\" data-ng-if=\"!topRightMenuCtrl.sagremorParams.loggedIn\"><div class=\"col-xs-6 controls\"><label for=\"sag-refresh\" data-translate=\"top_right_menu_refresh\"></label></div><div class=\"col-xs-6 controls\"><button id=\"sag-refresh\" name=\"sag-refresh\" type=\"button\" class=\"btn btn-primary btn-sm\" data-ng-click=\"topRightMenuCtrl.reconnect()\"><i class=\"fa fa-refresh\" aria-hidden=\"true\"></i></button></div></li><li class=\"row control-group\"><data-oauth data-site=\"{{topRightMenuCtrl.oauth2.site}}\" data-client-id=\"{{topRightMenuCtrl.oauth2.clientId}}\" data-response-type=\"{{topRightMenuCtrl.oauth2.responseType}}\" data-redirect-uri=\"{{topRightMenuCtrl.oauth2.redirectUri}}\" data-profile-uri=\"{{topRightMenuCtrl.oauth2.profileUri}}\" data-scope=\"{{topRightMenuCtrl.oauth2.scope}}\" data-authorize-path=\"{{topRightMenuCtrl.oauth2.authorizePath}}\" data-token-path=\"{{topRightMenuCtrl.oauth2.tokenPath}}\" data-template=\"/app/login/login.html\" data-storage=\"localStorage\" data-disable-check-session=\"true\"></data-oauth></li></ul></div>");
$templateCache.put("components/script/script.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal-title\" data-translate=\"script_modal_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\" data-ng-class=\"{\'has-error\': !!ScriptModalCtrl.error.message}\"><label for=\"scriptName\" class=\"col-xs-4\" data-translate=\"script_name\"></label><div class=\"col-xs-8 controls\"><input type=\"text\" name=\"scriptName\" id=\"scriptName\" data-ng-model=\"ScriptModalCtrl.script.name\" class=\"modal-input-text form-control\" size=\"16\" maxlength=\"64\" data-ng-disabled=\"!ScriptModalCtrl.add\" placeholder=\"{{\'script_name_placeholder\' | translate}}\"></div></div><div class=\"row control-group\"><label for=\"scriptDecription\" class=\"col-xs-4\" data-translate=\"script_description\"></label><div class=\"col-xs-8 controls\"><input type=\"text\" name=\"scriptDecription\" id=\"scriptDecription\" data-ng-model=\"ScriptModalCtrl.script.description\" class=\"modal-input-text form-control\" size=\"16\" maxlength=\"128\" placeholder=\"{{\'script_description_placeholder\' | translate}}\"></div></div><hr><h4 data-translate=\"script_actions\"></h4><div data-ng-repeat=\"action in ScriptModalCtrl.script.actions\"><div class=\"row\" data-ng-if=\"action.submodule === \'benoic\'\"><div class=\"col-xs-4\"><strong data-ng-bind=\"ScriptModalCtrl.getBenoicElementDisplay(action)\"></strong></div><div class=\"col-xs-4\"><strong data-ng-bind=\"ScriptModalCtrl.getBenoicElementValue(action)\"></strong></div><div class=\"col-xs-2\"><button type=\"button\" class=\"btn\" data-ng-click=\"ScriptModalCtrl.removeAction($index)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></div></div><div class=\"row\" data-ng-if=\"action.submodule === \'carleon\'\"><div class=\"col-xs-4\"><strong data-ng-bind=\"ScriptModalCtrl.carleonCommand(action.command, action.parameters.service)\"></strong></div><div class=\"col-xs-4\"><div class=\"row\" data-ng-repeat=\"(name, value) in action.parameters\" data-ng-if=\"name !== \'service\'\"><strong><span data-ng-bind=\"ScriptModalCtrl.carleonParameter(name, action.parameters.service, action.command)\"></span> <span>:</span> <span data-ng-bind=\"value\"></span></strong></div></div><div class=\"col-xs-2\"><button type=\"button\" class=\"btn\" data-ng-click=\"ScriptModalCtrl.removeAction($index)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></div></div></div><hr><div class=\"row\"><div class=\"col-xs-12\"><button type=\"button\" class=\"btn btn-primary\" data-ng-click=\"ScriptModalCtrl.addAction()\" data-translate=\"script_add_action\"></button></div></div><div data-ng-if=\"ScriptModalCtrl.newAction\"><hr><div class=\"row control-group\" data-ng-class=\"{\'has-error\': !ScriptModalCtrl.newActionType}\"><label for=\"newActionElement\" class=\"col-xs-4\" data-translate=\"script_action\"></label><div class=\"col-xs-8 controls\"><select id=\"newActionElement\" name=\"newActionElement\" class=\"form-control\" data-ng-model=\"ScriptModalCtrl.newActionType\" data-ng-options=\"element as ScriptModalCtrl.tr(element.label) for element in ScriptModalCtrl.scriptActionElements track by element.name\" data-ng-change=\"ScriptModalCtrl.changeActionType()\"></select></div></div><div data-ng-if=\"ScriptModalCtrl.newActionType.name === \'switch\'\"><div class=\"row control-group\" data-ng-class=\"{\'has-error\': !ScriptModalCtrl.newAction.switcher.name}\"><label for=\"newActionSwitch\" class=\"col-xs-4\" data-translate=\"script_switch_select\"></label><div class=\"col-xs-8 controls\"><select id=\"newActionSwitch\" name=\"newActionSwitch\" class=\"form-control\" data-ng-model=\"ScriptModalCtrl.tmpElement\" data-ng-change=\"ScriptModalCtrl.setActionBenoicElement()\" data-ng-options=\"switch as switch.display for switch in ScriptModalCtrl.benoicElements.switches track by ScriptModalCtrl.trackBenoicElement(switch, \'switch\')\"></select></div></div><div class=\"row\"><label for=\"newActionSwitchValue\" class=\"col-xs-4\" data-translate=\"script_switch_value\"></label><div class=\"col-xs-8\"><input type=\"checkbox\" id=\"newActionSwitchValue\" name=\"newActionSwitchValue\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"ScriptModalCtrl.newAction.command\" data-ng-checked=\"false\" data-ng-true-value=\"1\" data-ng-false-value=\"0\"></div></div></div><div data-ng-if=\"ScriptModalCtrl.newActionType.name === \'dimmer\'\"><div class=\"row control-group\" data-ng-class=\"{\'has-error\': !ScriptModalCtrl.newAction.dimmer.name}\"><label for=\"newActionDimmer\" class=\"col-xs-4\" data-translate=\"script_dimmer_select\"></label><div class=\"col-xs-8 controls\"><select id=\"newActionDimmer\" name=\"newActionDimmer\" class=\"form-control\" data-ng-model=\"ScriptModalCtrl.tmpElement\" data-ng-change=\"ScriptModalCtrl.setActionBenoicElement()\" data-ng-options=\"dimmer as dimmer.display for dimmer in ScriptModalCtrl.benoicElements.dimmers track by ScriptModalCtrl.trackBenoicElement(dimmer, \'dimmer\')\"></select></div></div><div class=\"row\"><label for=\"newActionDimmerValue\" class=\"col-xs-4\" data-translate=\"script_dimmer_value\"></label><div class=\"col-xs-8\"><input id=\"newActionDimmerValue\" class=\"form-control\" name=\"newActionDimmerValue\" type=\"number\" min=\"0\" max=\"100\" data-ng-model=\"ScriptModalCtrl.newAction.command\"></div></div></div><div data-ng-if=\"ScriptModalCtrl.newActionType.name === \'heater\'\"><div class=\"row control-group\" data-ng-class=\"{\'has-error\': !ScriptModalCtrl.newAction.heater.name}\"><label for=\"newActionHeater\" class=\"col-xs-4\" data-translate=\"script_heater_select\"></label><div class=\"col-xs-8 controls\"><select id=\"newActionHeater\" name=\"newActionHeater\" class=\"form-control\" data-ng-model=\"ScriptModalCtrl.tmpElement\" data-ng-change=\"ScriptModalCtrl.setActionBenoicElement()\" data-ng-options=\"heater as heater.display for heater in ScriptModalCtrl.benoicElements.heaters track by ScriptModalCtrl.trackBenoicElement(heater, \'heater\')\"></select></div></div><div class=\"row\"><label for=\"newActionHeaterValue\" class=\"col-xs-4\" data-translate=\"script_heater_command\"></label><div class=\"col-xs-8\"><input id=\"newActionHeaterValue\" class=\"form-control\" name=\"newActionHeaterValue\" type=\"number\" min=\"0\" max=\"100\" data-ng-model=\"ScriptModalCtrl.newAction.command\"> <span data-ng-bind=\"ScriptModalCtrl.tmpElement.options.unit\"></span></div></div><div class=\"row control-group\" data-ng-class=\"{\'has-error\': !ScriptModalCtrl.newAction.heater.mode}\"><label for=\"newActionHeaterMode\" class=\"col-xs-4\" data-translate=\"script_heater_mode_title\"></label><div class=\"col-xs-8 controls\"><select class=\"form-control\" id=\"newActionHeaterMode\" name=\"newActionHeaterMode\" data-ng-model=\"ScriptModalCtrl.newAction.parameters.mode\" data-ng-options=\"mode as ScriptModalCtrl.heaterModeDisplay(mode) for mode in ScriptModalCtrl.tmpElement.availableModes\"><option value=\"\" data-translate=\"heater_mode_keep\"></option></select></div></div></div><div data-ng-if=\"ScriptModalCtrl.newActionType.submodule === \'carleon\'\"><div class=\"row control-group\" data-ng-class=\"{\'has-error\': false}\"><label for=\"newActionCarleonElement\" class=\"col-xs-4\" data-translate=\"script_element_select\"></label><div class=\"col-xs-8 controls\"><select id=\"newActionCarleonElement\" name=\"newActionCarleonElement\" class=\"form-control\" data-ng-model=\"ScriptModalCtrl.tmpElement\" data-ng-change=\"ScriptModalCtrl.setActionCarleonElement()\" data-ng-options=\"element as element.name for element in ScriptModalCtrl.carleonConditionElementsList[ScriptModalCtrl.newActionType.name] track by ScriptModalCtrl.trackCarleonElement(element, ScriptModalCtrl.newActionType.name)\"></select></div></div><div class=\"row control-group\" data-ng-class=\"{\'has-error\': false}\" data-ng-repeat=\"parameter in ScriptModalCtrl.carleonConditionCommandsParameters[ScriptModalCtrl.newActionType.name]\"><label for=\"{{ScriptModalCtrl.newActionType.name + \'$\' + parameter.name}}\" class=\"col-xs-4\" data-ng-bind=\"parameter.title\"></label><div class=\"col-xs-8 controls\"><input type=\"text\" class=\"form-control\" name=\"{{ScriptModalCtrl.newActionType.name + \'$\' + parameter.name}}\" id=\"{{ScriptModalCtrl.newActionType.name + \'$\' + parameter.name}}\" data-ng-if=\"parameter.type === \'string\'\" data-ng-model=\"ScriptModalCtrl.newAction.parameters[parameter.name]\"> <input type=\"number\" class=\"form-control\" name=\"{{ScriptModalCtrl.newActionType.name + \'$\' + parameter.name}}\" id=\"{{ScriptModalCtrl.newActionType.name + \'$\' + parameter.name}}\" data-ng-if=\"parameter.type === \'integer\'\" data-ng-model=\"ScriptModalCtrl.newAction.parameters[parameter.name]\"> <input type=\"number\" class=\"form-control\" name=\"{{ScriptModalCtrl.newActionType.name + \'$\' + parameter.name}}\" id=\"{{ScriptModalCtrl.newActionType.name + \'$\' + parameter.name}}\" data-ng-if=\"parameter.type === \'real\'\" step=\".01\" data-ng-model=\"ScriptModalCtrl.newAction.parameters[parameter.name]\"></div></div></div><hr><div class=\"row\"><div class=\"col-xs-6\"><button type=\"button\" class=\"btn btn-primary\" data-ng-click=\"ScriptModalCtrl.saveNewAction()\" data-translate=\"btn_add\" data-ng-disabled=\"!ScriptModalCtrl.isActionValid()\" data-uib-tooltip=\"{{ScriptModalCtrl.error.messageAction}}\"></button></div><div class=\"col-xs-6\"><button type=\"button\" class=\"btn btn-primary\" data-ng-click=\"ScriptModalCtrl.cancelAddAction()\" data-translate=\"btn_cancel\"></button></div></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"ScriptModalCtrl.save()\" data-translate=\"btn_ok\" data-ng-disabled=\"!ScriptModalCtrl.isScriptValid()\" data-uib-tooltip=\"{{ScriptModalCtrl.error.message}}\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"ScriptModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"btn_cancel\"></button></div></div>");
$templateCache.put("components/script/script.template.html","<div class=\"text-center\"><button type=\"button\" data-ng-click=\"$ctrl.runScript()\" class=\"btn btn-primary script-button\" data-ng-bind=\"$ctrl.element.name\"></button></div>");
$templateCache.put("app/dashboard/dashboard.html","<div><h1 data-translate=\"dashboard_title\"></h1><h2 data-ng-bind=\"DashboardCtrl.profileName\"></h2><div class=\"row\" data-ng-if=\"DashboardCtrl.sagremorParams.adminMode\"><button type=\"button\" class=\"btn btn-primary\" name=\"addTitleLine\" id=\"addTitleLine\" data-ng-click=\"DashboardCtrl.addTitleLine()\" data-translate=\"dashboard_button_add_title_line\"></button><hr></div><div class=\"row\"><div class=\"col-md-12\"><div data-gridstack=\"\" class=\"grid-stack grid1\" data-options=\"DashboardCtrl.options\" data-on-change=\"DashboardCtrl.onChange(event, items)\"><div data-gridstack-item=\"\" data-ng-repeat=\"w in DashboardCtrl.dashboardWidgetsDisplay\" class=\"grid-stack-item\" data-gs-no-resize=\"true\" data-gs-item-x=\"w.x\" data-gs-item-y=\"w.y\" data-gs-item-width=\"w.width\" data-gs-item-height=\"w.height\" data-gs-min-width=\"3\" data-gs-item-autopos=\"0\" data-sag-type=\"{{w.type}}\" data-sag-device=\"{{w.device}}\" data-sag-name=\"{{w.name}}\" data-sag-profile=\"{{w.profile}}\"><div class=\"grid-stack-item-content\" data-ng-if=\"w.type !== \'separator\'\"><data-sag-container data-menu=\"DashboardCtrl.menu\" data-size=\"1\" data-title=\"w.element.display || w.element.name\" data-element=\"w\" data-dashboard=\"true\" data-type=\"w.type\" data-sag-params=\"DashboardCtrl.sagremorParams\" data-icon=\"w.icon\"><div data-sag-generic-injector=\"\" data-type=\"w.type\" data-elt=\"w.element\"></div></data-sag-container></div><div data-ng-if=\"w.type === \'separator\'\"><div class=\"sag-container-header-menu\" data-uib-dropdown=\"\"><button class=\"btn btn-primary text-left\" data-ng-if=\"DashboardCtrl.sagremorParams.adminMode\" type=\"button\" data-uib-dropdown-toggle=\"\" data-ng-click=\"\">#</button><ul data-uib-dropdown-menu=\"\"><li><a href=\"\" data-ng-click=\"DashboardCtrl.removeFromDashboard(w)\" data-translate=\"remove_from_dashboard\"></a></li></ul><fieldset class=\"grid-stack-item-content\" dir=\"ltr\"><legend data-ng-bind=\"w.name\" class=\"sag-separator\"></legend></fieldset></div></div></div></div></div></div><div class=\"row\" data-ng-if=\"!DashboardCtrl.connected\"><div class=\"col-md-12\"><h3 data-translate=\"connect_message\"></h3></div></div></div>");
$templateCache.put("app/error/error.html","<div><div class=\"container\"><h1 data-translate=\"{{ErrorCtrl.errorMessage}}\"></h1></div></div>");
$templateCache.put("app/events/events.html","<div><h1 data-translate=\"events_title\"></h1><div class=\"row\" data-ng-if=\"eventsCtrl.sagremorParams.adminMode\"><button type=\"button\" class=\"btn btn-primary\" name=\"addEvent\" id=\"addEvent\" data-ng-click=\"eventsCtrl.newEvent()\" data-translate=\"events_button_add\"></button></div><hr data-ng-if=\"eventsCtrl.schedulerList.length > 0\"><h2 data-translate=\"schedulers_title\" data-ng-if=\"eventsCtrl.schedulerList.length > 0\"></h2><div class=\"row\"><div data-ng-repeat=\"scheduler in eventsCtrl.schedulerList track by $index\" class=\"col-sm-3\"><data-sag-container data-menu=\"eventsCtrl.menuScheduler\" data-size=\"1\" data-title=\"scheduler.name\" data-type=\"\'scheduler\'\" data-element=\"scheduler\" data-icon=\"\'calendar\'\"><data-sag-scheduler data-element=\"scheduler\"></data-sag-scheduler></data-sag-container></div></div><hr data-ng-if=\"eventsCtrl.triggerList.length > 0\"><h2 data-translate=\"triggers_title\" data-ng-if=\"eventsCtrl.triggerList.length > 0\"></h2><div class=\"row\"><div data-ng-repeat=\"trigger in eventsCtrl.triggerList track by $index\" class=\"col-sm-3\"><data-sag-container data-menu=\"eventsCtrl.menuTrigger\" data-size=\"1\" data-title=\"trigger.name\" data-type=\"\'trigger\'\" data-element=\"trigger\" data-icon=\"\'bell\'\"><data-sag-trigger data-element=\"trigger\"></data-sag-trigger></data-sag-container></div></div></div>");
$templateCache.put("app/heaters/heaters.html","<div><h1 data-translate=\"heaters_title\"></h1><div class=\"row\"><div data-ng-repeat=\"heater in heatersCtrl.heaterList\" class=\"col-sm-3\" data-ng-if=\"heater.enabled || heatersCtrl.sagremorParams.adminMode\"><data-sag-container data-menu=\"heatersCtrl.menuSensor\" data-size=\"1\" data-title=\"heater.display\" data-type=\"\'heater\'\" data-element=\"heater\" data-icon=\"\'fire\'\"><data-sag-heater data-element=\"heater\"></data-sag-heater></data-sag-container></div></div></div>");
$templateCache.put("app/login/login.html","<div class=\"col-xs-6 controls\"><label data-translate=\"top_right_menu_login\" data-ng-if=\"responseType !== \'password\' && show==\'logged-out\'\"></label> <label data-translate=\"top_right_menu_logout\" data-ng-if=\"show==\'logged-in\'\"></label> <label data-translate=\"top_right_menu_access_denied\" data-ng-if=\"responseType !== \'password\' && show==\'denied\'\"></label></div><div class=\"col-xs-6 controls\"><button type=\"button\" class=\"btn btn-primary btn-sm\" data-ng-if=\"responseType !== \'password\' && show==\'logged-out\' || show==\'denied\'\" data-ng-click=\"login()\"><i class=\"fa fa-sign-in\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn btn-primary btn-sm\" data-ng-if=\"show==\'logged-in\'\" data-ng-click=\"logout()\"><i class=\"fa fa-sign-out\" aria-hidden=\"true\"></i></button></div><div class=\"col-xs-12\"><div data-ng-show=\"responseType === \'password\' && show !== \'logged-in\'\"><form><div><label>Login</label> <input type=\"text\" data-ng-model=\"typedLogin\" placeholder=\"login\"></div><div><label>Password</label> <input type=\"password\" data-ng-model=\"typedPassword\" placeholder=\"password\"></div><div><label>Keep connection</label> <input type=\"checkbox\" data-ng-model=\"typedKeepConnection\"></div><input type=\"submit\" class=\"btn btn-primary btn-sm\" value=\"Submit\" data-ng-click=\"checkPassword()\"></form></div></div>");
$templateCache.put("app/logs/filters.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal_title\" data-translate=\"logs_filter_modal_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><div class=\"col-xs-4 controls\" data-ng-if=\"!FiltersModalCtrl.displayAlert\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.newFilter()\"><i class=\"fa fa-plus\" aria-hidden=\"true\"></i> <span data-translate=\"logs_filter_new_button\"></span></button></div><div class=\"col-xs-4 controls\" data-ng-if=\"!FiltersModalCtrl.displayAlert\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.showAlerts()\"><i class=\"fa fa-bolt\" aria-hidden=\"true\"></i> <span data-translate=\"logs_alerts_delete_button\"></span></button></div><div class=\"col-xs-4 controls\" data-ng-if=\"FiltersModalCtrl.displayAlert\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.showFilters()\"><i class=\"fa fa-bolt\" aria-hidden=\"true\"></i> <span data-translate=\"logs_filters_show_button\"></span></button></div></div><div data-ng-if=\"FiltersModalCtrl.editFilter && !FiltersModalCtrl.displayAlert\"><hr><h3 data-translate=\"filter_table_edit\" data-ng-if=\"!FiltersModalCtrl.addFilter\"></h3><h3 data-translate=\"filter_table_new\" data-ng-if=\"FiltersModalCtrl.addFilter\"></h3><table class=\"table table-hover\"><thead><tr><th data-translate=\"filter_table_name\"></th><th data-translate=\"filter_table_description\"></th></tr></thead><tbody><tr><td><input type=\"text\" class=\"form-control\" name=\"filterName\" id=\"filterName\" data-ng-model=\"FiltersModalCtrl.customFilter.name\" data-ng-disabled=\"!FiltersModalCtrl.addFilter\"></td><td><input type=\"text\" class=\"form-control\" name=\"filterDescription\" id=\"filterDescription\" data-ng-model=\"FiltersModalCtrl.customFilter.description\"></td></tr></tbody></table><div class=\"row control-group\"><div class=\"col-xs-4 controls\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.addClause()\"><i class=\"fa fa-plus\" aria-hidden=\"true\"></i> <span data-translate=\"logs_clause_new_button\"></span></button></div><div class=\"col-xs-4 controls\"><div data-uib-dropdown=\"\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.addAlert()\" data-uib-dropdown-toggle=\"\"><i class=\"fa fa-plus\" aria-hidden=\"true\"></i> <span data-translate=\"logs_alert_new_button\"></span></button><ul data-uib-dropdown-menu=\"\"><li data-ng-repeat=\"menuItem in FiltersModalCtrl.menu\"><a href=\"\" data-ng-click=\"FiltersModalCtrl.menuSelect(menuItem, $ctrl.element)\" data-ng-bind=\"menuItem.display\"></a></li></ul></div></div></div><h3 data-translate=\"filter_table_clauses\"></h3><table class=\"table table-hover\"><thead><tr><th data-translate=\"clause_table_element\"></th><th data-translate=\"clause_table_operator\"></th><th data-translate=\"clause_table_value\"></th><th></th><th></th></tr></thead><tbody><tr data-ng-if=\"FiltersModalCtrl.newClause\"><td><select class=\"form-control\" data-ng-model=\"FiltersModalCtrl.customClause.element\"><option value=\"priority\" data-translate=\"clause_option_priority\"></option><option value=\"source\" data-translate=\"clause_option_source\"></option><option value=\"message\" data-translate=\"clause_option_message\"></option><option value=\"tag\" data-translate=\"clause_option_tag\"></option></select></td><td><select class=\"form-control\" data-ng-model=\"FiltersModalCtrl.customClause.operator\"><option value=\"=\" data-translate=\"clause_operator_equal\"></option><option value=\"!=\" data-translate=\"clause_operator_different\"></option><option value=\"<\" data-translate=\"clause_operator_lower\"></option><option value=\"<=\" data-translate=\"clause_operator_lower_equal\"></option><option value=\">\" data-translate=\"clause_operator_higher\"></option><option value=\">=\" data-translate=\"clause_operator_higher_equal\"></option><option value=\"contains\" data-translate=\"clause_operator_contains\"></option><option value=\"notcontains\" data-translate=\"clause_operator_notcontains\"></option></select></td><td><select class=\"form-control\" data-ng-model=\"FiltersModalCtrl.customClause.value\" data-ng-if=\"FiltersModalCtrl.customClause.element === \'priority\'\"><option value=\"0\" data-translate=\"logs_priority_none\"></option><option value=\"1\" data-translate=\"logs_priority_low\"></option><option value=\"2\" data-translate=\"logs_priority_medium\"></option><option value=\"3\" data-translate=\"logs_priority_high\"></option><option value=\"4\" data-translate=\"logs_priority_critical\"></option></select><input type=\"text\" data-ng-model=\"FiltersModalCtrl.customClause.value\" data-ng-if=\"FiltersModalCtrl.customClause.element !== \'priority\'\"></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.addNewClause()\" data-ng-disabled=\"!FiltersModalCtrl.isClauseValid()\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.cancelAddClause()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></td></tr><tr data-ng-repeat=\"clause in FiltersModalCtrl.customFilter.filter_clauses track by $index\"><td data-ng-bind=\"FiltersModalCtrl.getClauseElement(clause.element)\"></td><td data-ng-bind=\"FiltersModalCtrl.getClauseOperator(clause.operator)\"></td><td data-ng-bind=\"FiltersModalCtrl.getClauseValue(clause)\"></td><td></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.removeClause($index)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></td></tr></tbody></table><h3 data-translate=\"filter_table_alerts\"></h3><table class=\"table table-hover\"><thead><tr><th data-translate=\"alert_table_name\"></th><th data-translate=\"alert_table_type\"></th><th></th></tr></thead><tbody><tr data-ng-repeat=\"alert in FiltersModalCtrl.customFilter.filter_alerts.smtp track by $index\"><td data-ng-bind=\"alert\"></td><td>SMTP</td><td><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.removeAlert(\'smtp\', $index)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></td></tr><tr data-ng-repeat=\"alert in FiltersModalCtrl.customFilter.filter_alerts.http track by $index\"><td data-ng-bind=\"alert\"></td><td>HTTP</td><td><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.removeAlert(\'http\', $index)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></td></tr></tbody></table><div data-ng-if=\"FiltersModalCtrl.newAlertSmtp\"><div class=\"row control-group\"><h3 data-translate=\"alert_smtp_title\"></h3><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_select\"></label></div><div class=\"controls col-xs-6\"><select id=\"logsSearchFilter\" class=\"form-control\" data-ng-model=\"FiltersModalCtrl.existingSmtpAlert\" data-ng-options=\"alert.name for alert in FiltersModalCtrl.AlertSmtpList\" data-ng-disabled=\"!FiltersModalCtrl.AlertSmtpList || FiltersModalCtrl.AlertSmtpList.length === 0\"><option value=\"\"></option></select></div></div><div class=\"row control-group\"><div class=\"controls col-xs-12 text-center\"><label class=\"control-label\" data-translate=\"alert_or\"></label></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_name\"></label></div><div class=\"controls col-xs-6\"><input required=\"\" type=\"text\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.name\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_host\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" required=\"\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.host\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_port\"></label></div><div class=\"controls col-xs-6\"><input type=\"number\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.port\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_tls\"></label></div><div class=\"controls col-xs-6\"><input type=\"checkbox\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.tls\" data-ng-checked=\"false\" data-switch-active=\"{{!FiltersModalCtrl.existingSmtpAlert}}\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_check_ca\"></label></div><div class=\"controls col-xs-6\"><input type=\"checkbox\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.check_ca\" data-ng-checked=\"false\" data-switch-active=\"{{!FiltersModalCtrl.existingSmtpAlert}}\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_user\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.user\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_password\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.password\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_from\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" data-uib-tooltip=\"{{\'alert_smtp_emails_tooltip\' | translate}}\" required=\"\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.from\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_to\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" required=\"\" data-uib-tooltip=\"{{\'alert_smtp_emails_tooltip\' | translate}}\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.to\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_cc\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" data-uib-tooltip=\"{{\'alert_smtp_emails_tooltip\' | translate}}\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.cc\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_bcc\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" data-uib-tooltip=\"{{\'alert_smtp_emails_tooltip\' | translate}}\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.bcc\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_subject\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" data-uib-tooltip=\"{{\'alert_subject_body_tooltip\' | translate}}\" required=\"\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.subject\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_smtp_body\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" data-uib-tooltip=\"{{\'alert_subject_body_tooltip\' | translate}}\" required=\"\" data-ng-model=\"FiltersModalCtrl.customSmtpAlert.body\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingSmtpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"></div><div class=\"controls col-xs-6\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.postNewSmtpAlert()\" data-ng-disabled=\"!FiltersModalCtrl.isSmtpAlertValid()\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.cancelAddSmtpAlert()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></div></div><hr></div><div data-ng-if=\"FiltersModalCtrl.newAlertHttp\"><h3 data-translate=\"alert_http_title\"></h3><div class=\"row control-group required\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_http_select\"></label></div><div class=\"controls col-xs-6\"><select id=\"logsSearchFilter\" class=\"form-control\" data-ng-model=\"FiltersModalCtrl.existingHttpAlert\" data-ng-options=\"alert.name for alert in FiltersModalCtrl.AlertHttpList\" data-ng-disabled=\"!FiltersModalCtrl.AlertHttpList || FiltersModalCtrl.AlertHttpList.length === 0\"><option value=\"\"></option></select></div></div><div class=\"row control-group\"><div class=\"controls col-xs-12 text-center\"><label class=\"control-label\" data-translate=\"alert_or\"></label></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_http_name\"></label></div><div class=\"controls col-xs-6\"><input required=\"\" type=\"text\" data-ng-model=\"FiltersModalCtrl.customHttpAlert.name\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingHttpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_http_method\"></label></div><div class=\"controls col-xs-6\"><select required=\"\" data-ng-model=\"FiltersModalCtrl.customHttpAlertMethodSelect\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingHttpAlert\"><option value=\"GET\">GET</option><option value=\"POST\">POST</option><option value=\"PUT\">PUT</option><option value=\"DELETE\">DELETE</option><option value=\"custom\" data-translate=\"alert_http_method_other\"></option></select></div></div><div class=\"row control-group\" data-ng-if=\"FiltersModalCtrl.customHttpAlertMethodSelect === \'custom\'\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_http_method_custom\"></label></div><div class=\"controls col-xs-6\"><input type=\"text\" data-ng-model=\"FiltersModalCtrl.customHttpAlert.method\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingHttpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_http_url\"></label></div><div class=\"controls col-xs-6\"><input required=\"\" type=\"text\" data-ng-model=\"FiltersModalCtrl.customHttpAlert.url\" class=\"form-control\" data-ng-disabled=\"FiltersModalCtrl.existingHttpAlert\"></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_http_body\"></label></div><div class=\"controls col-xs-6\"><textarea data-uib-tooltip=\"{{\'alert_subject_body_tooltip\' | translate}}\" required=\"\" class=\"form-control\" data-ng-model=\"FiltersModalCtrl.customHttpAlert.body\">\n						</textarea></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_http_headers\"></label></div><div class=\"controls col-xs-6\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.addAlertHttpHeader()\"><i class=\"fa fa-plus\" aria-hidden=\"true\"></i></button></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-translate=\"alert_http_header_key\"></label></div><div class=\"controls col-xs-6\"><label data-translate=\"alert_http_header_value\"></label></div></div><div class=\"row control-group\" data-ng-if=\"FiltersModalCtrl.newAlertHttpHeader && !FiltersModalCtrl.existingHttpAlert\"><div class=\"controls col-xs-4\"><input type=\"text\" data-ng-model=\"FiltersModalCtrl.customHttpAlertHeader.key\" class=\"form-control\"></div><div class=\"controls col-xs-4\"><input type=\"text\" data-ng-model=\"FiltersModalCtrl.customHttpAlertHeader.value\" class=\"form-control\"></div><div class=\"controls col-xs-4\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.addNewAlertHttpHeader()\" data-ng-disabled=\"!FiltersModalCtrl.customHttpAlertHeader.key || !FiltersModalCtrl.customHttpAlertHeader.value\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.cancelAddAlertHttpHeader()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></div></div><div class=\"row control-group\" data-ng-repeat=\"header in FiltersModalCtrl.customHttpAlert.http_headers\"><div class=\"controls col-xs-6\"><label class=\"control-label\" data-ng-bind=\"header.key\"></label></div><div class=\"controls col-xs-6\"><label class=\"control-label\" data-ng-bind=\"header.value\"></label></div></div><div class=\"row control-group\"><div class=\"controls col-xs-6\"></div><div class=\"controls col-xs-6\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.postNewHttpAlert()\" data-ng-disabled=\"!FiltersModalCtrl.isHttpAlertValid()\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.cancelAddHttpAlert()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></div></div><hr></div><div class=\"row control-group\"><div class=\"col-xs-6\"></div><div class=\"col-xs-3\"><button type=\"button\" class=\"btn btn-primary pull-right\" data-ng-click=\"FiltersModalCtrl.postNewFilter()\" data-ng-disabled=\"!FiltersModalCtrl.isFilterValid()\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i> <span data-translate=\"btn_ok\"></span></button></div><div class=\"col-xs-3\"><button type=\"button\" class=\"btn btn-primary pull-right\" data-ng-click=\"FiltersModalCtrl.cancelNewFilter()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i> <span data-translate=\"btn_cancel\"></span></button></div></div></div><div data-ng-if=\"!FiltersModalCtrl.editFilter && !FiltersModalCtrl.displayAlert\"><table class=\"table table-hover\"><thead><tr><th data-translate=\"filter_table_name\"></th><th data-translate=\"filter_table_description\"></th><th data-translate=\"filter_table_clauses\"></th><th data-translate=\"filter_table_alerts\"></th><th></th></tr></thead><tr data-ng-repeat=\"filter in FiltersModalCtrl.filterList track by $index\"><td data-ng-bind=\"filter.name\"></td><td data-ng-bind=\"filter.description\"></td><td><div class=\"row-fluid\" data-ng-repeat=\"clause in filter.filter_clauses\"><span data-ng-bind=\"FiltersModalCtrl.getClauseElement(clause.element)\"></span> <span data-ng-bind=\"FiltersModalCtrl.getClauseOperator(clause.operator)\"></span> <span data-ng-bind=\"FiltersModalCtrl.getClauseValue(clause)\"></span></div></td><td><div class=\"row\" data-ng-if=\"filter.filter_alerts.smtp.length\"><div class=\"col-xs-3\"><strong><span data-translate=\"logs_alert_list_smtp\"></span></strong></div><div class=\"col-xs-9\"><span class=\"pull-right\" data-ng-bind=\"filter.filter_alerts.smtp.join(\', \')\"></span></div></div><div class=\"row\" data-ng-if=\"filter.filter_alerts.http.length\"><div class=\"col-xs-3\"><strong><span data-translate=\"logs_alert_list_http\"></span></strong></div><div class=\"col-xs-9\"><span class=\"pull-right\" data-ng-bind=\"filter.filter_alerts.http.join(\', \')\"></span></div></div></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.editCustomFilter(filter)\"><i class=\"fa fa-pencil\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.removeCustomFilter(filter)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></td></tr></table></div><div data-ng-if=\"FiltersModalCtrl.displayAlert\"><h4 data-translate=\"logs_menu_smtp\"></h4><div class=\"row\" data-ng-repeat=\"alertSmtp in FiltersModalCtrl.AlertSmtpList\"><div class=\"col-xs-2\"></div><div class=\"col-xs-4\" data-ng-bind=\"alertSmtp.name\"></div><div class=\"col-xs-4\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.removeSmtpAlert(alertSmtp)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></div></div><h4 data-translate=\"logs_menu_http\"></h4><div class=\"row\" data-ng-repeat=\"alertHttp in FiltersModalCtrl.AlertHttpList\"><div class=\"col-xs-2\"></div><div class=\"col-xs-4\" data-ng-bind=\"alertHttp.name\"></div><div class=\"col-xs-4\"><button type=\"button\" class=\"btn\" data-ng-click=\"FiltersModalCtrl.removeHttpAlert(alertHttp)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></div></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"FiltersModalCtrl.close()\" data-translate=\"btn_close\"></button></div></div>");
$templateCache.put("app/logs/logs.html","<div><h1 data-translate=\"logs_title\"></h1><div class=\"row control-group\"><div class=\"col-xs-1 controls\"><label for=\"logsSearchLimit\" data-translate=\"logs_search_limit\"></label></div><div class=\"col-xs-2 controls\"><input type=\"number\" min=\"1\" class=\"form-control\" name=\"logsSearchLimit\" id=\"logsSearchLimit\" data-ng-model=\"logsCtrl.searchOptions.limit\"></div><div class=\"col-xs-1 controls\"><label for=\"logsSearchOffset\" data-translate=\"logs_search_offset\"></label></div><div class=\"col-xs-2 controls\"><input type=\"number\" min=\"0\" class=\"form-control\" name=\"logsSearchOffset\" id=\"logsSearchOffset\" data-ng-model=\"logsCtrl.searchOptions.offset\"></div><div class=\"col-xs-1 controls\"><label for=\"logsSearchFilter\" data-translate=\"logs_search_filter\"></label></div><div class=\"col-xs-2 controls\"><select id=\"logsSearchFilter\" class=\"form-control\" data-ng-model=\"logsCtrl.searchOptions.filter\" data-ng-options=\"filter.name for filter in logsCtrl.filterList\" data-ng-disabled=\"logsCtrl.filterList.length === 0\"><option value=\"\"></option></select></div><div class=\"col-xs-3\"><button type=\"button\" class=\"btn btn-primary pull-right\" data-ng-click=\"logsCtrl.search()\" style=\"margin-left: 5px; margin-bottom: 5px;\"><i class=\"fa fa-search\" aria-hidden=\"true\"></i> <span data-translate=\"logs_search_button\"></span></button> <button type=\"button\" class=\"btn btn-primary pull-right\" data-ng-click=\"logsCtrl.filterManagement()\" style=\"margin-left: 5px; margin-bottom: 5px;\"><i class=\"fa fa-filter\" aria-hidden=\"true\"></i> <span data-translate=\"logs_filter_button\"></span></button></div></div><hr><table data-datatable=\"ng\" data-dt-options=\"logsCtrl.options\" class=\"table table-hover table-striped table-bordered dataTable\"><thead><tr><th data-translate=\"logs_table_date\"></th><th data-translate=\"logs_table_priority\"></th><th data-translate=\"logs_table_source\"></th><th data-translate=\"logs_table_text\"></th><th data-translate=\"logs_table_tags\"></th></tr></thead><tbody><tr data-ng-repeat=\"message in logsCtrl.messageList\"><td>{{message.date | date : \'medium\'}}</td><td>{{logsCtrl.getPriority(message)}}</td><td>{{message.source}}</td><td>{{message.text}}</td><td><span data-ng-repeat=\"tag in message.tags\">{{tag}}</span></td></tr></tbody></table></div>");
$templateCache.put("app/parameters/parameters.html","<div><h1 data-translate=\"parameters_title\"></h1><h3 data-translate=\"param_submodule_title\"></h3><div class=\"row\"><div class=\"col-xs-2\"><label for=\"submodule-benoic\" data-translate=\"param_submodule_benoic\"></label></div></div><h4 data-translate=\"param_benoic_device_type_list\"></h4><div class=\"row\" data-ng-repeat=\"type in ParametersCtrl.deviceTypes track by $index\"><div class=\"col-xs-2\" data-ng-bind=\"type.name\"></div><div class=\"col-xs-2\"><input data-toggle=\"toggle\" data-bs-switch=\"\" data-switch-size=\"mini\" type=\"checkbox\" data-ng-model=\"type.enabled\" data-switch-active=\"false\"></div></div><hr><div class=\"row\"><div class=\"col-xs-2\"><label for=\"submodule-carleon\" data-translate=\"param_submodule_carleon\"></label></div></div><h4 data-translate=\"param_carleon_service_list\"></h4><div class=\"row\" data-ng-repeat=\"service in ParametersCtrl.serviceList track by $index\"><div class=\"col-xs-2\" data-ng-bind=\"service.name\"></div><div class=\"col-xs-2\"><input data-toggle=\"toggle\" data-bs-switch=\"\" data-switch-size=\"mini\" type=\"checkbox\" data-ng-model=\"service.enabled\" data-switch-active=\"false\"></div></div><hr><div class=\"row\"><div class=\"col-xs-2\"><label for=\"submodule-gareth\" data-translate=\"param_submodule_gareth\"></label></div></div><div data-ng-if=\"ParametersCtrl.sagremorParams.benoicEnabled\"><h3 data-translate=\"param_device_list\"></h3><form name=\"deviceList\" class=\"css-form\"><button type=\"button\" data-ng-click=\"ParametersCtrl.addDevice()\" class=\"btn\"><i class=\"fa fa-plus\" aria-hidden=\"true\"></i></button><table class=\"table table-hover\"><thead><tr><th data-translate=\"device_name\"></th><th data-translate=\"device_description\"></th><th data-translate=\"device_type\"></th><th data-translate=\"device_connected\"></th><th data-translate=\"device_action\"></th></tr></thead><tbody><tr data-ng-repeat=\"(name, device) in ParametersCtrl.deviceList track by name\" data-ng-class=\"{\'device-disabled\': !device.enabled}\"><td>{{device.name}}</td><td data-ng-if=\"!device.update\">{{device.description}}</td><td data-ng-if=\"device.update\"><input type=\"text\" name=\"deviceDesc\" id=\"deviceDesc\" data-ng-model=\"device.newDescription\" value=\"{{device.newDescription}}\"></td><td>{{device.type_uid + \'_device_type\' | translate}}</td><td><input data-toggle=\"toggle\" data-bs-switch=\"\" data-switch-size=\"mini\" type=\"checkbox\" data-ng-model=\"device.connected\" data-ng-change=\"ParametersCtrl.connectDevice(device)\"></td><td data-ng-if=\"!device.update\"><button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.editDevice(device)\"><i class=\"fa fa-pencil\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.removeDevice(device)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></td><td data-ng-if=\"device.update\"><button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.saveDevice(device)\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.cancelEditDevice(device)\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></td></tr><tr data-ng-if=\"ParametersCtrl.deviceAdded\"><td><input type=\"text\" data-ng-model=\"ParametersCtrl.newDeviceName\" data-ng-required=\"\" data-ng-maxlength=\"64\" maxlength=\"64\" placeholder=\"{{\'device_name_placeholder\' | translate}}\"></td><td><input type=\"text\" data-ng-model=\"ParametersCtrl.newDeviceDescription\" data-ng-maxlength=\"128\" maxlength=\"128\" placeholder=\"{{\'device_description_placeholder\' | translate}}\"></td><td><select data-ng-required=\"\" data-ng-model=\"ParametersCtrl.newDeviceType\" data-ng-options=\"device.uid as device.translate for device in ParametersCtrl.deviceTypesEnabled\" data-ng-change=\"ParametersCtrl.setDeviceOptions()\"></select></td><td><input data-toggle=\"toggle\" data-bs-switch=\"\" data-switch-size=\"mini\" type=\"checkbox\" data-ng-model=\"ParametersCtrl.newDeviceConnect\"></td><td></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.postNewDevice()\" data-ng-disabled=\"!ParametersCtrl.isDeviceValid()\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.cancelNewDevice()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></td></tr></tbody></table><h3 data-ng-if=\"(ParametersCtrl.deviceAdded || ParametersCtrl.deviceUpdated) && ParametersCtrl.deviceOptionListDisplay\" data-translate=\"device_options_title\"></h3><div data-ng-if=\"(ParametersCtrl.deviceAdded || ParametersCtrl.deviceUpdated) && ParametersCtrl.deviceOptionListDisplay\" data-ng-repeat=\"(optionName, option) in ParametersCtrl.deviceOptionList track by optionName\" class=\"container\"><div class=\"row\"><div class=\"col-xs-3\" data-ng-class=\"{\'device-option-required\': !option.optional}\">{{ParametersCtrl.newDeviceType + \"_\" + option.name | translate}}</div><div class=\"col-xs-3\" data-ng-if=\"option.type === \'string\'\"><input type=\"text\" data-ng-model=\"option.value\" data-ng-required=\"!option.optional\" placeholder=\"{{ParametersCtrl.newDeviceType + \'_\' + option.name + \'_placeholder\' | translate}}\"></div><div class=\"col-xs-3\" data-ng-if=\"option.type === \'numeric\'\"><input type=\"number\" data-ng-model=\"option.value\" data-ng-required=\"!option.optional\" placeholder=\"{{ParametersCtrl.newDeviceType + \'_\' + option.name + \'_placeholder\' | translate}}\"></div><div class=\"col-xs-3\" data-ng-if=\"option.type === \'boolean\'\"><input data-toggle=\"toggle\" type=\"checkbox\" data-ng-model=\"option.value\" data-ng-required=\"!option.optional\"></div></div></div></form></div><div data-ng-if=\"ParametersCtrl.sagremorParams.carleonEnabled\"><h3 data-translate=\"param_profiles_list\"></h3><button type=\"button\" data-ng-click=\"ParametersCtrl.addProfile()\" class=\"btn\"><i class=\"fa fa-plus\" aria-hidden=\"true\"></i></button><table class=\"table table-hover\"><thead><tr><th data-translate=\"profile_name\"></th><th data-translate=\"profile_description\"></th><th data-translate=\"profile_default\"></th><th data-translate=\"profile_use\"></th><th data-translate=\"profile_action\"></th></tr></thead><tbody><tr data-ng-repeat=\"profile in ParametersCtrl.profileList track by profile.name\"><td data-ng-if=\"!profile.update\">{{profile.name}}</td><td data-ng-if=\"!profile.update\">{{profile.description}}</td><td data-ng-if=\"profile.update\"><input type=\"text\" name=\"profileName\" id=\"profileName\" data-ng-model=\"profile.newName\" placeholder=\"{{\'profile_name_placeholder\' | translate}}\"></td><td data-ng-if=\"profile.update\"><input type=\"text\" name=\"profileDescription\" id=\"profileDescription\" data-ng-model=\"profile.newDescription\" placeholder=\"{{\'profile_description_placeholder\' | translate}}\"></td><td><input data-toggle=\"toggle\" data-bs-switch=\"\" data-switch-size=\"mini\" type=\"checkbox\" data-ng-model=\"profile.default\" data-ng-change=\"ParametersCtrl.setDefaultProfile(profile)\"></td><td><button type=\"button\" class=\"btn\" name=\"useProfile\" id=\"useProfile\" data-ng-click=\"ParametersCtrl.useProfile(profile)\" data-ng-if=\"profile.name !== ParametersCtrl.currentProfile.name\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <span data-ng-if=\"profile.name === ParametersCtrl.currentProfile.name\" data-translate=\"profile_used\"></span></td><td data-ng-if=\"!profile.update\"><button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.editProfile(profile)\"><i class=\"fa fa-pencil\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.cleanProfile(profile)\"><i class=\"fa fa-eraser\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.removeProfile(profile)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></td><td data-ng-if=\"profile.update\"><button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.saveProfile(profile)\" data-ng-disabled=\"!ParametersCtrl.isUpdateProfileValid(profile)\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.cancelEditProfile(profile)\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></td></tr><tr data-ng-if=\"ParametersCtrl.profileAdded\"><td><input type=\"text\" data-ng-model=\"ParametersCtrl.newProfileName\" data-ng-required=\"\" data-ng-maxlength=\"64\" maxlength=\"64\" placeholder=\"{{\'profile_name_placeholder\' | translate}}\"></td><td><input type=\"text\" data-ng-model=\"ParametersCtrl.newProfileDescription\" data-ng-maxlength=\"128\" maxlength=\"128\" placeholder=\"{{\'profile_description_placeholder\' | translate}}\"></td><td><input data-toggle=\"toggle\" data-bs-switch=\"\" data-switch-size=\"mini\" type=\"checkbox\" data-ng-model=\"ParametersCtrl.newProfileDefault\"></td><td></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.saveNewProfile()\" data-ng-disabled=\"!ParametersCtrl.isProfileValid()\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"ParametersCtrl.cancelNewProfile()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></td></tr></tbody></table></div></div>");
$templateCache.put("app/monitors/monitors.html","<div><h1 data-translate=\"monitors_title\"></h1><div class=\"row\" data-ng-if=\"monitorsCtrl.sagremorParams.adminMode\"><button type=\"button\" class=\"btn btn-primary\" name=\"addMonitor\" id=\"addMonitor\" data-ng-click=\"monitorsCtrl.newMonitor()\" data-translate=\"monitors_button_add\"></button></div><hr><div class=\"row\"><div data-ng-repeat=\"monitor in monitorsCtrl.monitorList track by monitor.name\" class=\"col-sm-6\"><data-sag-container data-menu=\"monitorsCtrl.menu\" data-title=\"monitor.name\" data-type=\"\'monitor\'\" data-element=\"monitor\" data-icon=\"\'bar-chart\'\"><data-sag-monitor data-element=\"monitor\"></data-sag-monitor></data-sag-container></div></div></div>");
$templateCache.put("app/scripts/scripts.html","<div><h1 data-translate=\"scripts_title\"></h1><div class=\"row\" data-ng-if=\"scriptsCtrl.sagremorParams.adminMode\"><button type=\"button\" class=\"btn btn-primary\" name=\"addScript\" id=\"addScript\" data-ng-click=\"scriptsCtrl.newScript()\" data-translate=\"scripts_button_add\"></button></div><hr><div class=\"row\"><div data-ng-repeat=\"script in scriptsCtrl.scriptList track by script.name\" class=\"col-sm-3\"><data-sag-container data-menu=\"scriptsCtrl.menuScript\" data-size=\"1\" data-title=\"script.name\" data-type=\"\'script\'\" data-element=\"script\" data-icon=\"\'tasks\'\"><data-sag-script data-element=\"script\"></data-sag-script></data-sag-container></div></div></div>");
$templateCache.put("app/sensors/sensors.html","<div><h1 data-translate=\"sensors_title\"></h1><div class=\"row\"><div data-ng-repeat=\"sensor in sensorsCtrl.sensorList\" class=\"col-sm-3\" data-ng-if=\"sensor.enabled || sensorsCtrl.sagremorParams.adminMode\"><data-sag-container data-menu=\"sensorsCtrl.menuSensor\" data-size=\"1\" data-title=\"sensor.display\" data-type=\"\'sensor\'\" data-element=\"sensor\" data-icon=\"\'line-chart\'\"><data-sag-sensor data-element=\"sensor\"></data-sag-sensor></data-sag-container></div></div></div>");
$templateCache.put("app/switches/switches.html","<div><h1 data-translate=\"switches_title\"></h1><h2 data-translate=\"switch_title\"></h2><div class=\"row\"><div data-ng-repeat=\"switcher in switchesCtrl.switchList\" class=\"col-sm-3\" data-ng-if=\"switcher.enabled || switchesCtrl.sagremorParams.adminMode\"><data-sag-container data-menu=\"switchesCtrl.menuSwitcher\" data-size=\"1\" data-title=\"switcher.display\" data-type=\"\'switch\'\" data-element=\"switcher\" data-icon=\"\'toggle-on\'\"><data-sag-switch data-element=\"switcher\"></data-sag-switch></data-sag-container></div></div><h2 data-translate=\"dimmer_title\"></h2><div class=\"row\"><div data-ng-repeat=\"dimmer in switchesCtrl.dimmerList\" class=\"col-sm-3\" data-ng-if=\"dimmer.enabled || switchesCtrl.sagremorParams.adminMode\"><data-sag-container data-menu=\"switchesCtrl.menuDimmer\" data-size=\"1\" data-title=\"dimmer.display\" data-type=\"\'dimmer\'\" data-element=\"dimmer\" data-icon=\"\'lightbulb-o\'\"><data-sag-dimmer data-element=\"dimmer\"></data-sag-dimmer></data-sag-container></div></div></div>");
$templateCache.put("app/users/users.html","<div class=\"container\"><h1 data-translate=\"users_title\"></h1><h2 data-translate=\"users_user_list\"></h2><h4 data-ng-if=\"!usersCtrl.usersEnabled\" data-translate=\"users_user_disabled\"></h4><button type=\"button\" data-ng-click=\"usersCtrl.addUser()\" class=\"btn btn-primary\" data-translate=\"users_add_user\" data-ng-if=\"usersCtrl.usersEnabled\"></button><table class=\"table table-hover\" data-ng-if=\"usersCtrl.usersEnabled\"><thead><tr><th data-translate=\"users_table_login\"></th><th data-translate=\"users_table_password\"></th><th data-translate=\"users_table_enabled\"></th><th></th></tr></thead><tbody><tr data-ng-repeat=\"user in usersCtrl.userList\"><td data-ng-bind=\"user.login\"></td><td data-ng-if=\"!user.changePassword\">***</td><td data-ng-if=\"!!user.changePassword\"><input type=\"password\" data-ng-model=\"user.password\" maxlength=\"128\"></td><td><input type=\"checkbox\" data-bs-switch=\"\" class=\"form-control\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-change=\"usersCtrl.enableUser(user)\" data-ng-model=\"user.enabled\"></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"usersCtrl.displayChangePassword(user)\" data-ng-if=\"!user.changePassword\" title=\"{{ \'users_user_change_password\' | translate }}\"><i class=\"fa fa-key\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"usersCtrl.removeUser(user)\" data-ng-if=\"!user.changePassword\" title=\"{{ \'users_user_delete\' | translate }}\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"usersCtrl.changePassword(user)\" data-ng-disabled=\"user.password.length === 0\" data-ng-if=\"!!user.changePassword\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"usersCtrl.cancelChangePassword(user)\" data-ng-if=\"!!user.changePassword\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></td></tr><tr data-ng-if=\"usersCtrl.newUser\"><td><input type=\"text\" data-ng-model=\"usersCtrl.user.login\" maxlength=\"128\"></td><td><input type=\"password\" data-ng-model=\"usersCtrl.user.password\" maxlength=\"128\"></td><td><input type=\"checkbox\" data-bs-switch=\"\" class=\"form-control\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"usersCtrl.user.enabled\"></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"usersCtrl.createNewUser()\" data-ng-disabled=\"!usersCtrl.isNewUserValid()\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"usersCtrl.cancelNewUser()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></td></tr></tbody></table><hr><h2 data-translate=\"users_token_list\"></h2><div class=\"row control-group\"><div class=\"col-xs-1 controls\"><label for=\"tokenSearchLogin\" data-translate=\"users_token_search_login\"></label></div><div class=\"col-xs-2 controls\"><input type=\"text\" min=\"1\" class=\"form-control\" name=\"tokenSearchLogin\" id=\"tokenSearchLogin\" data-ng-model=\"usersCtrl.searchOptions.login\" maxlength=\"128\"></div><div class=\"col-xs-1 controls\"><label for=\"tokenSearchEnabled\" data-translate=\"users_token_search_enabled\"></label></div><div class=\"col-xs-2 controls\"><select id=\"tokenSearchEnabled\" class=\"form-control\" data-ng-model=\"usersCtrl.searchOptions.enabled\"><option value=\"\" data-translate=\"users_token_search_enabled_all\"></option><option value=\"true\" data-translate=\"users_token_search_enabled_true\"></option><option value=\"false\" data-translate=\"users_token_search_enabled_false\"></option></select></div><div class=\"col-xs-3\"><button type=\"button\" class=\"btn btn-primary pull-right\" data-ng-click=\"usersCtrl.getTokenList()\" style=\"margin-left: 5px; margin-bottom: 5px;\"><i class=\"fa fa-search\" aria-hidden=\"true\"></i> <span data-translate=\"users_search_button\"></span></button></div></div><table class=\"table table-hover dataTable table-striped\" data-dt-options=\"usersCtrl.options\" data-datatable=\"ng\"><thead><tr><th data-translate=\"users_table_token_login\"></th><th data-translate=\"users_table_token_value\"></th><th data-translate=\"users_table_token_validity\"></th><th data-translate=\"users_table_token_lastseen\"></th><th data-translate=\"users_table_token_revoke\"></th></tr></thead><tbody><tr data-ng-repeat=\"token in usersCtrl.tokenList\" data-ng-class=\"{ \'text-muted\' : !token.enabled}\"><td data-ng-bind=\"token.login\"></td><td data-ng-bind=\"usersCtrl.displayToken(token.token)\"></td><td data-ng-bind=\"token.validity | date : \'medium\'\" data-ng-if=\"token.enabled\"></td><td data-translate=\"users_token_revoked\" data-ng-if=\"!token.enabled\"></td><td data-ng-bind=\"token.last_seen | date : \'medium\'\"></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"usersCtrl.revokeToken(token)\" title=\"{{ \'users_token_revoke\' | translate }}\" data-ng-disabled=\"!token.enabled\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></td></tr></tbody></table></div>");
$templateCache.put("app/service/service.html","<div><h1 data-translate=\"{{serviceCtrl.title}}\"></h1><div data-ng-repeat=\"service in serviceCtrl.serviceList\"><h3 data-translate=\"{{service.serviceGroup.title}}\"></h3><div class=\"row\" data-ng-if=\"serviceCtrl.sagremorParams.adminMode\"><div class=\"span6\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"serviceCtrl.addService(service.serviceGroup.service)\" data-translate=\"service_add\"></button></div></div><div class=\"row\"><data-sag-container data-ng-repeat=\"element in service.list\" data-menu=\"service.menu\" data-title=\"element.display || element.name\" data-type=\"element.type\" data-element=\"element\" data-icon=\"service.currentInjector.icon\" data-ng-class=\"{\'col-md-2\': !service.size || service.size === 1, \'col-md-4\': service.size === 2, \'col-md-6\': service.size === 3}\"><div data-sag-generic-injector=\"\" data-type=\"element.type\" data-elt=\"element\"></div></data-sag-container></div></div></div>");
$templateCache.put("components/carleonLiquidsoap/carleonLiquidsoap.modal.html","<div class=\"modal-content\"><div class=\"modal-carleon-liquidsoap\"><h3 class=\"modal-title\" data-translate=\"carleon_liquidsoap_modal_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_liquidsoap_modal_name\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonLiquidsoapName\" id=\"carleonLiquidsoapName\" class=\"modal-input-text form-control\" data-ng-model=\"carleonLiquidsoapModalCtrl.element.name\" data-ng-disabled=\"!carleonLiquidsoapModalCtrl.add\" size=\"10\" maxlength=\"64\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_liquidsoap_modal_description\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonLiquidsoapDescription\" id=\"carleonLiquidsoapDescription\" class=\"modal-input-text form-control\" data-ng-model=\"carleonLiquidsoapModalCtrl.element.description\" size=\"10\" maxlength=\"128\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_liquidsoap_radio_url\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonLiquidsoapRadioUrl\" id=\"carleonLiquidsoapRadioUrl\" class=\"modal-input-text form-control\" data-ng-model=\"carleonLiquidsoapModalCtrl.element.radio_url\" size=\"10\" maxlength=\"512\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_liquidsoap_radio_type\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonLiquidsoapRadioType\" id=\"carleonLiquidsoapRadioType\" class=\"modal-input-text form-control\" data-ng-model=\"carleonLiquidsoapModalCtrl.element.radio_type\" size=\"10\" maxlength=\"64\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_liquidsoap_control_enabled\"></span><div class=\"col-xs-6 controls\"><input class=\"form-control\" id=\"carleonLiquidsoapControlEnabled\" name=\"carleonLiquidsoapControlEnabled\" data-toggle=\"toggle\" data-bs-switch=\"\" data-switch-size=\"mini\" type=\"checkbox\" data-ng-model=\"carleonLiquidsoapModalCtrl.element.control_enabled\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_liquidsoap_control_host\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonLiquidsoapControlHost\" id=\"carleonLiquidsoapControlHost\" class=\"modal-input-text form-control\" data-ng-model=\"carleonLiquidsoapModalCtrl.element.control_host\" size=\"10\" maxlength=\"128\" data-ng-disabled=\"!carleonLiquidsoapModalCtrl.element.control_enabled\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_liquidsoap_control_port\"></span><div class=\"col-xs-6 controls\"><input type=\"number\" name=\"carleonLiquidsoapControlPort\" id=\"carleonLiquidsoapControlPort\" class=\"modal-input-text form-control\" data-ng-model=\"carleonLiquidsoapModalCtrl.element.control_port\" size=\"10\" step=\"1\" data-ng-disabled=\"!carleonLiquidsoapModalCtrl.element.control_enabled\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_liquidsoap_control_request_name\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonLiquidsoapControlRequestName\" id=\"carleonLiquidsoapControlRequestName\" class=\"modal-input-text form-control\" data-ng-model=\"carleonLiquidsoapModalCtrl.element.control_request_name\" size=\"10\" maxlength=\"128\" data-ng-disabled=\"!carleonLiquidsoapModalCtrl.element.control_enabled\"></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"carleonLiquidsoapModalCtrl.save()\" data-translate=\"btn_ok\" data-ng-disabled=\"!carleonLiquidsoapModalCtrl.isLiquidsoapValid()\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"carleonLiquidsoapModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"btn_cancel\"></button></div></div>");
$templateCache.put("components/carleonLiquidsoap/carleonLiquidsoap.template.html","<div class=\"container-fluid\"><div class=\"row control-group\"><div data-radio=\"\" data-src=\"$ctrl.radio.radio_url\"></div></div><div class=\"row control-group\"><data-uib-accordion><div data-uib-accordion-group=\"\" data-heading=\"{{\'carleon_liquidsoap_control_header\' | translate}}\" data-is-open=\"$ctrl.controlAccordionOpen\"><div class=\"row control-group\"><div class=\"col-xs-6\"><button type=\"button\" class=\"btn\" data-ng-click=\"$ctrl.commandRadio(\'stop\')\" data-ng-if=\"$ctrl.status === \'on\'\"><i class=\"fa fa-stop\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"$ctrl.commandRadio(\'start\')\" data-ng-if=\"$ctrl.status === \'off\'\"><i class=\"fa fa-play\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"$ctrl.commandRadio(\'skip\')\" data-ng-if=\"$ctrl.status === \'on\'\"><i class=\"fa fa-step-forward\" aria-hidden=\"true\"></i></button></div></div></div><div data-uib-accordion-group=\"\" data-heading=\"{{\'carleon_liquidsoap_on_air_header\' | translate}}\" data-is-open=\"$ctrl.onAirAccordionOpen\"><div class=\"row control-group\"><dl><dt data-translate=\"carleon_liquidsoap_song_title\"></dt><dd data-ng-bind=\"$ctrl.onAir.title\"></dd><dt data-translate=\"carleon_liquidsoap_song_artist\"></dt><dd data-ng-bind=\"$ctrl.onAir.artist\"></dd><dt data-translate=\"carleon_liquidsoap_song_album\"></dt><dd data-ng-bind=\"$ctrl.onAir.album\"></dd><dt data-translate=\"carleon_liquidsoap_song_album_artist\"></dt><dd data-ng-bind=\"$ctrl.onAir.albumartist\"></dd><dt data-translate=\"carleon_liquidsoap_song_year\"></dt><dd data-ng-bind=\"$ctrl.onAir.year\"></dd></dl></div></div><div data-uib-accordion-group=\"\" data-heading=\"{{\'carleon_liquidsoap_list_header\' | translate}}\" data-is-open=\"$ctrl.listAccordionOpen\"><div class=\"row control-group\"><select class=\"form-control\" data-ng-model=\"$ctrl.listSelected\" data-ng-options=\"song as song.artist + \' - \' + song.title for song in $ctrl.list track by song.artist + \' - \' + song.title\"></select></div><div class=\"row control-group\"><dl><dt data-translate=\"carleon_liquidsoap_song_title\"></dt><dd data-ng-bind=\"$ctrl.listSelected.title\"></dd><dt data-translate=\"carleon_liquidsoap_song_artist\"></dt><dd data-ng-bind=\"$ctrl.listSelected.artist\"></dd><dt data-translate=\"carleon_liquidsoap_song_album\"></dt><dd data-ng-bind=\"$ctrl.listSelected.album\"></dd><dt data-translate=\"carleon_liquidsoap_song_album_artist\"></dt><dd data-ng-bind=\"$ctrl.listSelected.albumartist\"></dd><dt data-translate=\"carleon_liquidsoap_song_year\"></dt><dd data-ng-bind=\"$ctrl.listSelected.year\"></dd></dl></div></div></data-uib-accordion></div></div>");
$templateCache.put("components/carleonMock/carleonMock.modal.html","<div class=\"modal-content\"><div class=\"modal-carleon-mock\"><h3 class=\"modal-title\" data-translate=\"carleon_mock_modal_title\"></h3></div><div class=\"modal-body\"><div class=\"row-fluid\"><span class=\"col-xs-6\" data-translate=\"carleon_mock_modal_name\"></span><div class=\"col-xs-6\"><input type=\"text\" name=\"carleonMockName\" id=\"carleonMockName\" data-ng-model=\"CarleonMockModalCtrl.mock.name\" data-ng-disabled=\"!CarleonMockModalCtrl.add\" class=\"modal-input-text\" size=\"10\"></div></div><div class=\"row\"><span class=\"col-xs-6\" data-translate=\"carleon_mock_modal_description\"></span><div class=\"col-xs-6\"><input type=\"text\" name=\"carleonMockDescription\" id=\"carleonMockDescription\" data-ng-model=\"CarleonMockModalCtrl.mock.description\" class=\"modal-input-text\" size=\"10\"></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"CarleonMockModalCtrl.save()\" data-translate=\"btn_ok\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"CarleonMockModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"btn_cancel\"></button></div></div></div>");
$templateCache.put("components/carleonMock/carleonMock.template.html","<div class=\"text-center\"><div class=\"row\"><div class=\"span6\">command</div><div class=\"span6\"><select id=\"mockCommand\" data-ng-model=\"$ctrl.param.command\" data-ng-options=\"command for command in $ctrl.commandList\"></select></div></div><div class=\"row\" data-ng-if=\"$ctrl.param.command === \'exec1\' || $ctrl.param.command === \'exec2\'\"><div class=\"span6\">param1</div><div class=\"span6\"><input type=\"text\" data-ng-model=\"$ctrl.param.param1\" size=\"4\"></div></div><div class=\"row\" data-ng-if=\"$ctrl.param.command === \'exec1\'\"><div class=\"span6\">param2</div><div class=\"span6\"><input type=\"number\" data-ng-model=\"$ctrl.param.param2\" style=\"width: 7em\"></div></div><div class=\"row\" data-ng-if=\"$ctrl.param.command === \'exec1\'\"><div class=\"span6\">param3</div><div class=\"span6\"><input type=\"number\" step=\"0.01\" data-ng-model=\"$ctrl.param.param3\" style=\"width: 7em\"></div></div><button type=\"button\" data-ng-click=\"$ctrl.command()\">Execute</button></div>");
$templateCache.put("components/carleonMpd/carleonMpd.modal.html","<div class=\"modal-content\"><div class=\"modal-carleon-mpd\"><h3 class=\"modal-title\" data-translate=\"carleon_mpd_modal_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_mpd_modal_name\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonMpdName\" id=\"carleonMpdName\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMpdModalCtrl.mpd.name\" data-ng-disabled=\"!carleonMpdModalCtrl.add\" size=\"10\" maxlength=\"64\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_mpd_modal_description\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonMpdDescription\" id=\"carleonMpdDescription\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMpdModalCtrl.mpd.description\" size=\"10\" maxlength=\"128\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_mpd_modal_host\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonMpdDescription\" id=\"carleonMpdDescription\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMpdModalCtrl.mpd.host\" size=\"10\" maxlength=\"128\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_mpd_modal_port\"></span><div class=\"col-xs-6 controls\"><input type=\"number\" name=\"carleonMpdDescription\" id=\"carleonMpdDescription\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMpdModalCtrl.mpd.port\" size=\"10\" step=\"1\" maxlength=\"5\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_mpd_modal_password\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonMpdDescription\" id=\"carleonMpdDescription\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMpdModalCtrl.mpd.password\" size=\"10\" maxlength=\"128\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_mpd_modal_switch\"></span><div class=\"col-xs-6 controls\"><select id=\"newActionSwitch\" name=\"newActionSwitch\" class=\"form-control\" data-ng-model=\"carleonMpdModalCtrl.switchAttached\" data-ng-options=\"switch as switch.display for switch in carleonMpdModalCtrl.switches\" data-ng-change=\"carleonMpdModalCtrl.changeSwitch()\"></select></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"carleonMpdModalCtrl.save()\" data-translate=\"btn_ok\" data-ng-disabled=\"!carleonMpdModalCtrl.isMpdValid()\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"carleonMpdModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"btn_cancel\"></button></div></div>");
$templateCache.put("components/carleonMpd/carleonMpd.template.html","<div class=\"container-fluid\"><div class=\"row control-group\"><div class=\"row control-group\" data-ng-if=\"!!$ctrl.mpd.name && ($ctrl.mpd.state == \'pause\' || $ctrl.mpd.state == \'play\')\"><div class=\"col-xs-12 controls text-center\"><strong><span data-ng-bind=\"$ctrl.mpd.name\"></span></strong></div></div><div class=\"row control-group\" data-ng-if=\"!!$ctrl.mpd.title && ($ctrl.mpd.state == \'pause\' || $ctrl.mpd.state == \'play\')\"><div class=\"col-xs-2 controls\"><strong><span data-translate=\"carleon_mpd_playing_title\"></span></strong></div><div class=\"col-xs-10 controls\"><span data-ng-bind=\"$ctrl.mpd.title\"></span></div></div><div class=\"row control-group\" data-ng-if=\"!!$ctrl.mpd.artist && ($ctrl.mpd.state == \'pause\' || $ctrl.mpd.state == \'play\')\"><div class=\"col-xs-2 controls\"><strong><span data-translate=\"carleon_mpd_playing_artist\"></span></strong></div><div class=\"col-xs-10 controls\"><span data-ng-bind=\"$ctrl.mpd.artist\"></span></div></div><div class=\"row control-group\" data-ng-if=\"!!$ctrl.mpd.album && ($ctrl.mpd.state == \'pause\' || $ctrl.mpd.state == \'play\')\"><div class=\"col-xs-2 controls\"><strong><span data-translate=\"carleon_mpd_playing_album\"></span></strong></div><div class=\"col-xs-10 controls\"><span data-ng-bind=\"$ctrl.mpd.album\"></span></div></div><div class=\"row control-group\" data-ng-if=\"!!$ctrl.mpd.date && ($ctrl.mpd.state == \'pause\' || $ctrl.mpd.state == \'play\')\"><div class=\"col-xs-2 controls\"><strong><span data-translate=\"carleon_mpd_playing_date\"></span></strong></div><div class=\"col-xs-10 controls\"><span data-ng-bind=\"$ctrl.mpd.date\"></span></div></div><hr class=\"heater-hr\"><div class=\"clearfix control-group\"><div class=\"col-xs-4 controls\"><label for=\"mpdPlaylists\" data-translate=\"carleon_mpd_playlists_title\"></label></div><div class=\"col-xs-4 controls\"><select class=\"form-control\" name=\"mpdPlaylists\" value=\"mpdPlaylists\" data-ng-model=\"$ctrl.selectedPlaylist\" data-ng-options=\"playlist as playlist for playlist in $ctrl.playlists\"></select></div><div class=\"col-xs-4 controls\"><button type=\"button\" class=\"btn btn-primary\" data-ng-click=\"$ctrl.loadPlaylist()\" data-translate=\"carleon_mpd_playlist_load\"></button></div></div><hr class=\"heater-hr\"><div class=\"row control-group\"><div class=\"clearfix input-group\"><span class=\"input-group-btn\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"$ctrl.setAction(\'previous\')\"><i class=\"fa fa-backward\" aria-hidden=\"true\"></i></button></span> <span class=\"input-group-btn\" data-ng-if=\"$ctrl.mpd.state == \'pause\' || $ctrl.mpd.state == \'play\'\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"$ctrl.setAction(\'stop\')\"><i class=\"fa fa-stop\" aria-hidden=\"true\"></i></button></span> <span class=\"input-group-btn\" data-ng-if=\"$ctrl.mpd.state == \'pause\' || $ctrl.mpd.state == \'stop\'\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"$ctrl.setAction(\'play\')\"><i class=\"fa fa-play\" aria-hidden=\"true\"></i></button></span> <span class=\"input-group-btn\" data-ng-if=\"$ctrl.mpd.state == \'play\'\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"$ctrl.setAction(\'pause\')\"><i class=\"fa fa-pause\" aria-hidden=\"true\"></i></button></span> <span class=\"input-group-btn\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"$ctrl.setAction(\'next\')\"><i class=\"fa fa-forward\" aria-hidden=\"true\"></i></button></span> <span class=\"input-group-btn\"><button type=\"button\" data-ng-class=\"{\'btn btn-primary\': $ctrl.mpd.repeat, \'btn btn-default\': !$ctrl.mpd.repeat}\" data-ng-click=\"$ctrl.setAction(\'repeat\')\"><i class=\"fa fa-refresh\" aria-hidden=\"true\"></i></button> <button type=\"button\" data-ng-class=\"{\'btn btn-primary\': $ctrl.mpd.random, \'btn btn-default\': !$ctrl.mpd.random}\" data-ng-click=\"$ctrl.setAction(\'random\')\"><i class=\"fa fa-random\" aria-hidden=\"true\"></i></button></span></div></div><hr class=\"heater-hr\"><div class=\"row control-group\" data-ng-if=\"!!$ctrl.mpd.name\"><div class=\"col-xs-12 controls text-center\"><strong><span data-translate=\"carleon_mpd_control_volume\"></span></strong></div></div><div class=\"row control-group\"><div class=\"ring control-group\"><div class=\"quarter quarter-plus-1\" data-ng-click=\"$ctrl.setVolume(1)\"><span class=\"quarter-value\" data-ng-bind=\"$ctrl.displayVolumeCommand(1)\"></span></div><div class=\"quarter quarter-plus-5\" data-ng-click=\"$ctrl.setVolume(5)\"><span class=\"quarter-value\" data-ng-bind=\"$ctrl.displayVolumeCommand(5)\"></span></div><div class=\"quarter quarter-minus-1\" data-ng-click=\"$ctrl.setVolume(-1)\"><span class=\"quarter-value\" data-ng-bind=\"$ctrl.displayVolumeCommand(-1)\"></span></div><div class=\"quarter quarter-minus-5\" data-ng-click=\"$ctrl.setVolume(-5)\"><span class=\"quarter-value\" data-ng-bind=\"$ctrl.displayVolumeCommand(-5)\"></span></div><div class=\"cutout-heater\"><div class=\"ring-value\" data-ng-bind=\"$ctrl.displayVolume()\"></div></div></div></div></div></div>");
$templateCache.put("components/carleonMotion/carleonMotion.image.modal.html","<div class=\"container-fluid\"><div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal_title\" data-ng-bind=\"carleonMotionImageModalCtrl.displayTitle()\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><div class=\"col-xs-12\" data-ng-if=\"!!carleonMotionImageModalCtrl.currentImage.stream\"><img data-ng-src=\"{{carleonMotionImageModalCtrl.currentImage.stream}}\" alt=\"{{carleonMotionImageModalCtrl.currentImage.title}}\" class=\"img-responsive img-thumbnail\"></div><div class=\"col-xs-12\" data-ng-if=\"!!carleonMotionImageModalCtrl.currentImage.img\"><img data-http-src=\"{{carleonMotionImageModalCtrl.currentImage.img}}\" alt=\"{{carleonMotionImageModalCtrl.currentImage.title}}\" class=\"img-responsive img-thumbnail\"></div></div><div class=\"row control-group\"><div class=\"col-xs-5\"><button type=\"button\" class=\"btn pull-right\" data-ng-click=\"carleonMotionImageModalCtrl.changeImage(-1)\" data-ng-if=\"carleonMotionImageModalCtrl.canPrevious()\"><i class=\"fa fa-arrow-left\" aria-hidden=\"true\"></i></button></div><div class=\"col-xs-2 text-center\"><span data-ng-bind=\"carleonMotionImageModalCtrl.showIndex()\"></span></div><div class=\"col-xs-5\"><button type=\"button\" class=\"btn pull-left\" data-ng-click=\"carleonMotionImageModalCtrl.changeImage(1)\" data-ng-if=\"carleonMotionImageModalCtrl.canNext()\"><i class=\"fa fa-arrow-right\" aria-hidden=\"true\"></i></button></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"carleonMotionImageModalCtrl.snapshot()\" data-ng-if=\"!!carleonMotionImageModalCtrl.currentImage.snapshotUri\"><span data-translate=\"carleon_motion_snapshot\"></span> <i class=\"fa fa-camera\" aria-hidden=\"true\"></i></button> <button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"carleonMotionImageModalCtrl.close()\" data-translate=\"monitor_close\"></button></div></div></div>");
$templateCache.put("components/carleonMotion/carleonMotion.modal.html","<div class=\"modal-content\"><div class=\"modal-carleon-motion\"><h3 class=\"modal-title\" data-translate=\"carleon_motion_modal_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_motion_modal_name\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonMotionName\" id=\"carleonMotionName\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMotionModalCtrl.motion.name\" data-ng-disabled=\"!carleonMotionModalCtrl.add\" size=\"10\" maxlength=\"64\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_motion_modal_description\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonMotionDescription\" id=\"carleonMotionDescription\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMotionModalCtrl.motion.description\" size=\"10\" maxlength=\"128\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"carleon_motion_modal_config_uri\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"carleonMotionDescription\" id=\"carleonMotionDescription\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMotionModalCtrl.motion.config_uri\" size=\"10\" maxlength=\"128\"></div></div><hr><div class=\"row\"><h3 data-translate=\"carleon_motion_modal_file_list\"></h3><button type=\"button\" data-ng-click=\"carleonMotionModalCtrl.addNewFileList()\" class=\"btn\"><i class=\"fa fa-plus\" aria-hidden=\"true\"></i></button></div><div class=\"row control-group\"><table class=\"table table-hover controls\"><thead><tr><th data-translate=\"carleon_motion_file_list_name\"></th><th data-translate=\"carleon_motion_file_list_path\"></th><th data-translate=\"carleon_motion_file_list_thumbnail_path\"></th><th></th></tr></thead><tbody><tr data-ng-repeat=\"fileList in carleonMotionModalCtrl.motion.file_list track by $index\"><td data-ng-bind=\"fileList.name\"></td><td data-ng-bind=\"fileList.path\"></td><td data-ng-bind=\"fileList.thumbnail_path\"></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"carleonMotionModalCtrl.removeFileList($index)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></td></tr><tr data-ng-if=\"carleonMotionModalCtrl.addFileList\"><td><input type=\"text\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMotionModalCtrl.newFileList.name\" data-ng-required=\"\" data-ng-maxlength=\"64\" maxlength=\"64\"></td><td><input type=\"text\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMotionModalCtrl.newFileList.path\" data-ng-required=\"\" data-ng-maxlength=\"512\" maxlength=\"512\"></td><td><input type=\"text\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMotionModalCtrl.newFileList.thumbnail_path\" data-ng-required=\"\" data-ng-maxlength=\"512\" maxlength=\"512\"></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"carleonMotionModalCtrl.saveNewFileList()\" data-ng-disabled=\"!carleonMotionModalCtrl.isFileListValid()\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"carleonMotionModalCtrl.cancelNewFileList()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></td></tr></tbody></table></div><hr><div class=\"row\"><h3 data-translate=\"carleon_motion_modal_stream_list\"></h3><button type=\"button\" data-ng-click=\"carleonMotionModalCtrl.addNewStream()\" class=\"btn\"><i class=\"fa fa-plus\" aria-hidden=\"true\"></i></button></div><div class=\"row control-group\"><table class=\"table table-hover controls\"><thead><tr><th data-translate=\"carleon_motion_stream_list_name\"></th><th data-translate=\"carleon_motion_stream_list_uri\"></th><th data-translate=\"carleon_motion_stream_list_snapshot_uri\"></th><th></th></tr></thead><tbody><tr data-ng-repeat=\"streamList in carleonMotionModalCtrl.motion.stream_list track by $index\"><td data-ng-bind=\"streamList.name\"></td><td data-ng-bind=\"streamList.uri\"></td><td data-ng-bind=\"streamList.snapshot_uri\"></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"carleonMotionModalCtrl.removeStream($index)\"><i class=\"fa fa-trash\" aria-hidden=\"true\"></i></button></td></tr><tr data-ng-if=\"carleonMotionModalCtrl.addStream\"><td><input type=\"text\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMotionModalCtrl.newStream.name\" data-ng-required=\"\" data-ng-maxlength=\"64\" maxlength=\"64\"></td><td><input type=\"text\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMotionModalCtrl.newStream.uri\" data-ng-required=\"\" data-ng-maxlength=\"512\" maxlength=\"512\"></td><td><input type=\"text\" class=\"modal-input-text form-control\" data-ng-model=\"carleonMotionModalCtrl.newStream.snapshot_uri\" data-ng-maxlength=\"512\" maxlength=\"512\"></td><td><button type=\"button\" class=\"btn\" data-ng-click=\"carleonMotionModalCtrl.saveNewStream()\" data-ng-disabled=\"!carleonMotionModalCtrl.isStreamValid()\"><i class=\"fa fa-check\" aria-hidden=\"true\"></i></button> <button type=\"button\" class=\"btn\" data-ng-click=\"carleonMotionModalCtrl.cancelNewStream()\"><i class=\"fa fa-times\" aria-hidden=\"true\"></i></button></td></tr></tbody></table></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"carleonMotionModalCtrl.save()\" data-translate=\"btn_ok\" data-ng-disabled=\"!carleonMotionModalCtrl.isCameraValid()\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"carleonMotionModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"btn_cancel\"></button></div></div>");
$templateCache.put("components/carleonMotion/carleonMotion.template.html","<div class=\"container-fluid\"><div class=\"row control-group\"><div class=\"col-xs-4 controls\" data-ng-class=\"{\'text-success\': $ctrl.online, \'text-danger\': !$ctrl.online}\"><label data-translate=\"carleon_motion_online\" data-ng-if=\"$ctrl.online\"></label> <label data-translate=\"carleon_motion_offline\" data-ng-if=\"!$ctrl.online\"></label> <i class=\"fa fa-power-off\" aria-hidden=\"true\"></i></div><div class=\"col-xs-4 controls\" data-ng-if=\"$ctrl.fileListName.length > 1\"><div><label for=\"motionImagesList\" data-translate=\"carleon_motion_file_list\"></label></div></div><div class=\"col-xs-4 controls\" data-ng-if=\"$ctrl.fileListName.length > 1\"><select id=\"motionImagesList\" class=\"form-control\" data-ng-change=\"$ctrl.changeSelectedFileList()\" data-ng-model=\"$ctrl.selectedFileList\" data-ng-options=\"fileListName as fileListName for fileListName in $ctrl.fileListName track by fileListName\"></select></div><div class=\"col-xs-8 controls\" data-ng-if=\"$ctrl.fileListName.length <= 1\"><span data-ng-bind=\"$ctrl.selectedFileList\"></span></div></div><div class=\"row control-group\"><div class=\"col-xs-4\"><button type=\"button\" class=\"btn btn-primary\" data-ng-click=\"$ctrl.refresh()\"><span data-translate=\"carleon_motion_refresh\"></span> <i class=\"fa fa-refresh\" aria-hidden=\"true\"></i></button></div><div class=\"col-xs-4 controls\"><button type=\"button\" class=\"btn btn-primary\" data-ng-click=\"$ctrl.openStream()\"><span data-translate=\"carleon_motion_stream_open\"></span> <i class=\"fa fa-video-camera\" aria-hidden=\"true\"></i></button></div></div><hr><div class=\"row control-group\" data-ng-if=\"!!$ctrl.selectedFileList\"><div class=\"col-xs-3\" data-ng-repeat=\"image in $ctrl.images track by $index\"><a href=\"\" data-ng-click=\"$ctrl.openPopupImage($ctrl.images, $index)\" alt=\"{{image.title}}\"><img data-http-src=\"{{image.thumb}}\" title=\"{{image.title}}\" class=\"img-responsive\"></a> <span class=\"small text-center\" data-ng-bind=\"image.title\"></span></div></div><div class=\"row control-group\" data-ng-if=\"!!$ctrl.selectedFileList\"><div class=\"col-xs-5\"><button type=\"button\" class=\"btn pull-right\" data-ng-click=\"$ctrl.previousList()\" data-ng-if=\"$ctrl.canPrevious()\"><i class=\"fa fa-arrow-left\" aria-hidden=\"true\"></i></button></div><div class=\"col-xs-2 text-center\"><span data-ng-bind=\"$ctrl.offset\"></span></div><div class=\"col-xs-5\"><button type=\"button\" class=\"btn pull-left\" data-ng-click=\"$ctrl.nextList()\" data-ng-if=\"$ctrl.canNext()\"><i class=\"fa fa-arrow-right\" aria-hidden=\"true\"></i></button></div></div></div>");
$templateCache.put("components/container/container.template.html","<div data-ng-class=\"{\'sag-container\': !$ctrl.dashboard, \'sag-container-dashboard\': ($ctrl.dashboard && $ctrl.element.type !== \'script\')}\"><div class=\"row\"><div class=\"sag-container-header-menu col-xs-2\" data-ng-if=\"(!!$ctrl.menu && !$ctrl.dashboard) || $ctrl.dashboard && $ctrl.sagParams.adminMode\" data-uib-dropdown=\"\"><button class=\"btn btn-primary text-left\" type=\"button\" data-uib-dropdown-toggle=\"\" data-ng-click=\"\"><i data-ng-class=\"{\'fa fa-{{$ctrl.icon}} fa-fw\': !!$ctrl.icon, \'fa fa-bars\': !$ctrl.icon}\" aria-hidden=\"true\"></i></button><ul data-uib-dropdown-menu=\"\"><li data-ng-repeat=\"menuItem in $ctrl.menu\" data-ng-if=\"menuItem.name !== \'monitor\' || $ctrl.element.monitored\"><a href=\"\" data-ng-click=\"$ctrl.menuSelect(menuItem, $ctrl.element)\" data-ng-bind=\"menuItem.display\"></a></li></ul></div><div data-ng-if=\"$ctrl.element.type !== \'script\'\" class=\"sag-container-header-title col-xs-10\" data-ng-class=\"{\'sag-container-header-title-dashboard\': $ctrl.dashboard}\"><i data-ng-class=\"{\'fa fa-{{$ctrl.icon}}\': !!$ctrl.icon}\" aria-hidden=\"true\" data-ng-if=\"$ctrl.dashboard\"></i> <span data-toggle=\"tooltip\" data-ng-attr-title=\"{{$ctrl.title + \'\\n\' + $ctrl.element.description}}\"><span data-ng-bind=\"$ctrl.title\" class=\"text-center\"></span></span></div></div><div data-ng-transclude=\"\" class=\"sag-container-element\"></div><div class=\"row\"><div class=\"col-md-12\"><span data-ng-repeat=\"script in $ctrl.scripts\" class=\"script-small\"><button type=\"button\" class=\"btn btn-xs btn-primary\" data-ng-bind=\"script.name\" data-ng-click=\"$ctrl.runScript(script)\"></button></span></div></div></div>");
$templateCache.put("components/dimmer/dimmer.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal-title\" data-translate=\"dimmer_update_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"dimmer_update_name\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"dimmerDisplay\" id=\"dimmerDisplay\" data-ng-model=\"DimmersModalCtrl.dimmer.newDisplay\" class=\"modal-input-text form-control\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"dimmer_update_enabled\"></span><div class=\"col-xs-6 controls\"><input type=\"checkbox\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"DimmersModalCtrl.dimmer.enabled\"></div></div><div class=\"row\"><hr></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"dimmer_update_monitor\"></span><div class=\"col-xs-6 controls\"><input type=\"checkbox\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"DimmersModalCtrl.dimmer.monitored\"></div></div><div class=\"row control-group\" data-ng-if=\"DimmersModalCtrl.dimmer.monitored\"><span class=\"col-xs-6\" data-translate=\"dimmer_update_monitor_every\"></span><div class=\"col-xs-6 controls\"><select id=\"bddAddUserGrant\" class=\"form-control\" data-ng-model=\"DimmersModalCtrl.dimmer.monitored_every\" data-ng-options=\"monitorEvery.value as DimmersModalCtrl.tr(monitorEvery.label) for monitorEvery in DimmersModalCtrl.monitorEveryEnum\"></select></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"DimmersModalCtrl.saveDimmer()\" data-translate=\"dimmer_btn_ok\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"DimmersModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"dimmer_btn_cancel\"></button></div></div>");
$templateCache.put("components/dimmer/dimmer.template.html","<div data-ng-if=\"$ctrl.element.enabled || $ctrl.adminMode\"><div class=\"text-center\" data-ng-class=\"{\'text-italic\': $ctrl.adminMode}\"><div class=\"ring\"><div class=\"quarter quarter1\" data-ng-click=\"$ctrl.setDimmer(100)\" data-ng-class=\"{\'quarter-off\': $ctrl.element.value <= 75, \'quarter-almost-on\': $ctrl.element.value < 100 && $ctrl.element.value > 75, \'quarter-on\': $ctrl.element.value === 100}\"><span class=\"quarter-value\">100%</span></div><div class=\"quarter quarter2\" data-ng-click=\"$ctrl.setDimmer(25)\" data-ng-class=\"{\'quarter-off\': $ctrl.element.value === 0, \'quarter-almost-on\': $ctrl.element.value < 25 && $ctrl.element.value > 0, \'quarter-on\': $ctrl.element.value >= 25}\"><span class=\"quarter-value\">25%</span></div><div class=\"quarter quarter3\" data-ng-click=\"$ctrl.setDimmer(75)\" data-ng-class=\"{\'quarter-off\': $ctrl.element.value <= 50, \'quarter-almost-on\': $ctrl.element.value < 75 && $ctrl.element.value > 50, \'quarter-on\': $ctrl.element.value >= 75}\"><span class=\"quarter-value\"><br>75%</span></div><div class=\"quarter quarter4\" data-ng-click=\"$ctrl.setDimmer(50)\" data-ng-class=\"{\'quarter-off\': $ctrl.element.value <= 25, \'quarter-almost-on\': $ctrl.element.value < 50 && $ctrl.element.value > 25, \'quarter-on\': $ctrl.element.value >= 50}\"><span class=\"quarter-value\"><br>50%</span></div><div class=\"cutout\"><div class=\"ring-value\" data-ng-bind=\"$ctrl.Dimmervalue()\"></div></div></div><hr class=\"dimmer-hr\"><div class=\"clearfix input-group\"><span class=\"input-group-btn\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"$ctrl.addDimmer(-1)\"><i class=\"fa fa-minus\"></i></button></span> <span class=\"input-group-btn\"><button type=\"button\" data-ng-click=\"$ctrl.setDimmer(0)\" class=\"btn btn-default\"><i class=\"fa fa-power-off\" aria-hidden=\"true\"></i></button></span> <span class=\"input-group-btn\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"$ctrl.addDimmer(1)\"><i class=\"fa fa-plus\"></i></button></span></div></div></div>");
$templateCache.put("components/event/event.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal-title\" data-translate=\"event_modal_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\" data-ng-class=\"{\'has-error\': !!EventModalCtrl.error.nameInvalid}\"><label for=\"eventName\" class=\"col-xs-5\" data-translate=\"event_name\"></label><div class=\"col-xs-7 controls\"><input type=\"text\" name=\"eventName\" id=\"eventName\" data-ng-model=\"EventModalCtrl.event.name\" class=\"modal-input-text form-control\" size=\"16\" maxlength=\"64\" data-ng-disabled=\"!EventModalCtrl.add\" placeholder=\"{{\'event_name_placeholder\' | translate}}\"></div></div><div class=\"row control-group\"><label for=\"eventDecription\" class=\"col-xs-5\" data-translate=\"event_description\"></label><div class=\"col-xs-7 controls\"><input type=\"text\" name=\"eventDecription\" id=\"eventDecription\" data-ng-model=\"EventModalCtrl.event.description\" class=\"modal-input-text form-control\" size=\"16\" maxlength=\"128\" placeholder=\"{{\'event_description_placeholder\' | translate}}\"></div></div><div class=\"row control-group\"><label for=\"eventEnabled\" class=\"col-xs-5\" data-translate=\"event_enabled\"></label><div class=\"col-xs-7 controls\"><input type=\"checkbox\" id=\"eventEnabled\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.event.enabled\"></div></div><hr><div class=\"row control-group\"><label for=\"eventType\" class=\"col-xs-5\" data-translate=\"event_type_select\"></label><div class=\"col-xs-7 controls\"><select id=\"eventType\" class=\"form-control\" data-ng-model=\"EventModalCtrl.eventType\"><option value=\"scheduler\" data-translate=\"event_type_scheduler\"></option><option value=\"trigger\" data-translate=\"event_type_trigger\"></option></select></div></div><div class=\"row control-group\" data-ng-if=\"EventModalCtrl.eventType === \'scheduler\'\"><label for=\"schedulerDate\" class=\"col-xs-5\" data-translate=\"event_scheduler_date_select\"></label><div class=\"col-xs-7\"><div class=\"input-group\"><input type=\"text\" id=\"schedulerDate\" class=\"form-control\" data-datetime-picker=\"yyyy/MM/dd HH:mm\" data-ng-model=\"EventModalCtrl.myDate\" data-is-open=\"EventModalCtrl.isOpen\" data-button-bar=\"EventModalCtrl.datePickerOptions.buttonBar\"> <span class=\"input-group-btn\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"EventModalCtrl.openCalendar($event, prop)\"><i class=\"fa fa-calendar\"></i></button></span></div></div></div><div class=\"row control-group\" data-ng-if=\"EventModalCtrl.eventType === \'scheduler\'\"><label for=\"schedulerRepeat\" class=\"col-xs-5\" data-translate=\"event_scheduler_repeat\"></label><div class=\"col-xs-7 controls\"><input type=\"checkbox\" id=\"schedulerRepeat\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.scheduler.repeatSwitch\"></div></div><div class=\"row control-group\" data-ng-if=\"EventModalCtrl.eventType === \'scheduler\' && EventModalCtrl.scheduler.repeatSwitch\"><label for=\"schedulerRepeatEvery\" class=\"col-xs-5\" data-translate=\"event_scheduler_repeat_every\"></label><div class=\"col-xs-7 controls\"><select id=\"schedulerRepeatEvery\" class=\"form-control\" data-ng-model=\"EventModalCtrl.scheduler.repeat\" data-ng-options=\"repeatEvery.value*1 as EventModalCtrl.tr(repeatEvery.label) for repeatEvery in EventModalCtrl.schedulerRepeatEveryEnum\"></select></div></div><div class=\"row control-group\" data-ng-if=\"EventModalCtrl.eventType === \'scheduler\' && EventModalCtrl.scheduler.repeatSwitch && EventModalCtrl.scheduler.repeat !== 3\"><label for=\"schedulerRepeatEveryValue\" class=\"col-xs-5\" data-translate=\"event_scheduler_repeat_every_value\"></label><div class=\"col-xs-7 controls\"><input type=\"number\" id=\"schedulerRepeatEveryValue\" min=\"0\" step=\"1\" data-ng-model=\"EventModalCtrl.scheduler.repeat_value\"></div></div><div class=\"row control-group\" data-ng-if=\"EventModalCtrl.eventType === \'scheduler\' && EventModalCtrl.scheduler.repeatSwitch && EventModalCtrl.scheduler.repeat === 3\"><label for=\"schedulerRepeatEveryDayOfWeek\" class=\"col-xs-5\" data-translate=\"event_scheduler_repeat_every_value\"></label><div class=\"col-xs-7 controls\"><div class=\"row\"><div class=\"col-xs-6\"><label for=\"schedulerRepeatMonday\" data-translate=\"event_scheduler_repeat_every_day_of_week_monday\"></label></div><div class=\"col-xs-6\"><input type=\"checkbox\" id=\"schedulerRepeatMonday\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.scheduler.repeatEveryDayOfWeek.monday\"></div></div><div class=\"row\"><div class=\"col-xs-6\"><label for=\"schedulerRepeatTuesday\" data-translate=\"event_scheduler_repeat_every_day_of_week_tuesday\"></label></div><div class=\"col-xs-6\"><input type=\"checkbox\" id=\"schedulerRepeatTuesday\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.scheduler.repeatEveryDayOfWeek.tuesday\"></div></div><div class=\"row\"><div class=\"col-xs-6\"><label for=\"schedulerRepeatWednesday\" data-translate=\"event_scheduler_repeat_every_day_of_week_wednesday\"></label></div><div class=\"col-xs-6\"><input type=\"checkbox\" id=\"schedulerRepeatWednesday\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.scheduler.repeatEveryDayOfWeek.wednesday\"></div></div><div class=\"row\"><div class=\"col-xs-6\"><label for=\"schedulerRepeatThursday\" data-translate=\"event_scheduler_repeat_every_day_of_week_thursday\"></label></div><div class=\"col-xs-6\"><input type=\"checkbox\" id=\"schedulerRepeatThursday\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.scheduler.repeatEveryDayOfWeek.thursday\"></div></div><div class=\"row\"><div class=\"col-xs-6\"><label for=\"schedulerRepeatFriday\" data-translate=\"event_scheduler_repeat_every_day_of_week_friday\"></label></div><div class=\"col-xs-6\"><input type=\"checkbox\" id=\"schedulerRepeatFriday\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.scheduler.repeatEveryDayOfWeek.friday\"></div></div><div class=\"row\"><div class=\"col-xs-6\"><label for=\"schedulerRepeatSaturday\" data-translate=\"event_scheduler_repeat_every_day_of_week_saturday\"></label></div><div class=\"col-xs-6\"><input type=\"checkbox\" id=\"schedulerRepeatSaturday\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.scheduler.repeatEveryDayOfWeek.saturday\"></div></div><div class=\"row\"><div class=\"col-xs-6\"><label for=\"schedulerRepeatSunday\" data-translate=\"event_scheduler_repeat_every_day_of_week_sunday\"></label></div><div class=\"col-xs-6\"><input type=\"checkbox\" id=\"schedulerRepeatSunday\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.scheduler.repeatEveryDayOfWeek.sunday\"></div></div></div></div><div class=\"row control-group\" data-ng-if=\"EventModalCtrl.eventType === \'scheduler\' && !EventModalCtrl.scheduler.repeatSwitch\"><label for=\"schedulerRemoveAfter\" class=\"col-xs-5\" data-translate=\"event_scheduler_remove_after\"></label><div class=\"col-xs-7 controls\"><input type=\"checkbox\" id=\"schedulerRemoveAfter\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.scheduler.remove_after\"></div></div><div class=\"row control-group\" data-ng-if=\"EventModalCtrl.eventType === \'trigger\'\"><label for=\"triggerSource\" class=\"col-xs-5\" data-translate=\"event_trigger_source\"></label><div class=\"col-xs-7 controls\"><select id=\"triggerSource\" class=\"form-control\" data-ng-model=\"EventModalCtrl.trigger\" data-ng-options=\"element as element.display for element in EventModalCtrl.globalTriggerElementList track by EventModalCtrl.trackTriggerElement(element)\"></select></div><div class=\"control-group\"><label for=\"newTriggerMessage\" class=\"col-xs-5\" data-translate=\"event_trigger_message\"></label><div class=\"col-xs-5 controls\"></div><div class=\"col-xs-3\"><select id=\"newTriggerMessageOperator\" name=\"newTriggerMessageOperator\" class=\"form-control\" data-ng-model=\"EventModalCtrl.event.message_match\"><option value=\"0\"></option><option value=\"1\" data-translate=\"message_match_equal\"></option><option value=\"2\" data-translate=\"message_match_different\"></option><option value=\"3\" data-translate=\"message_match_contains\"></option><option value=\"4\" data-translate=\"message_match_not_contains\"></option><option value=\"5\" data-translate=\"message_match_empty\"></option><option value=\"6\" data-translate=\"message_match_not_empty\"></option></select></div><div class=\"col-xs-4\" data-ng-if=\"EventModalCtrl.event.message_match <= 4\"><input type=\"text\" class=\"form-control\" name=\"newTriggerMessageValue\" id=\"newTriggerMessageValue\" data-ng-model=\"EventModalCtrl.event.message\" placeholder=\"{{\'event_message_match_placeholder\' | translate}}\"></div></div></div><hr><div class=\"row control-group\"><label class=\"col-xs-5\" data-translate=\"event_script_list\"></label><div class=\"col-xs-7\"><div class=\"row\" data-ng-repeat=\"script in EventModalCtrl.scriptList track by $index\"><strong data-ng-bind=\"script.name\"></strong> <input type=\"checkbox\" id=\"scriptEnabled\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"script.enabled\"> <button type=\"button\" class=\"btn btn-default pull-right\" data-ng-click=\"EventModalCtrl.removeScript($index)\"><i class=\"fa fa-trash\"></i></button></div></div></div><div class=\"row control-group\"><label for=\"eventScriptSelect\" class=\"col-xs-5\" data-translate=\"event_script_select\"></label><div class=\"col-xs-7\"><select id=\"eventScriptSelect\" class=\"form-control\" data-ng-model=\"EventModalCtrl.newScript\" data-ng-options=\"script.name for script in EventModalCtrl.globalScriptList\"></select></div></div><div class=\"row control-group\"><label for=\"eventScriptAdd\" class=\"col-xs-5\" data-translate=\"event_add\"></label><div class=\"col-xs-7\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"EventModalCtrl.addScript(EventModalCtrl.newScript)\"><i class=\"fa fa-plus\"></i></button></div></div><hr><div class=\"row control-group\"><label class=\"col-xs-5\" data-translate=\"event_condition_list\"></label><div class=\"col-xs-7\"><div class=\"row\" data-ng-repeat=\"condition in EventModalCtrl.conditionList track by $index\"><strong data-ng-bind=\"EventModalCtrl.displayCondition(condition)\"></strong> <button type=\"button\" class=\"btn btn-default pull-right\" data-ng-click=\"EventModalCtrl.removeCondition($index)\"><i class=\"fa fa-trash\"></i></button></div></div></div><div class=\"row control-group\"><label for=\"eventScriptAdd\" class=\"col-xs-5\" data-translate=\"event_condition_select\"></label><div class=\"col-xs-7\"><select id=\"eventconditionAdd\" class=\"form-control\" data-ng-model=\"EventModalCtrl.newConditionType\" data-ng-options=\"condition as EventModalCtrl.tr(condition.label) for condition in EventModalCtrl.globalElementList\"></select></div></div><div class=\"row control-group\"><div data-ng-if=\"EventModalCtrl.newConditionType.name === \'switch\'\"><div data-ng-class=\"{\'has-error\': !EventModalCtrl.tmpElement}\"><label for=\"newConditionSwitch\" class=\"col-xs-5\" data-translate=\"script_switch_select\"></label><div class=\"col-xs-7 controls\"><select id=\"newConditionSwitch\" name=\"newConditionSwitch\" class=\"form-control\" data-ng-model=\"EventModalCtrl.tmpElement\" data-ng-change=\"EventModalCtrl.setConditionBenoicElement()\" data-ng-options=\"switch as switch.display for switch in EventModalCtrl.benoicElements.switches track by EventModalCtrl.trackBenoicElement(switch, \'switch\')\"></select></div></div><div><label for=\"newConditionSwitchValue\" class=\"col-xs-5\" data-translate=\"script_switch_value\"></label><div class=\"col-xs-7\"><input type=\"checkbox\" id=\"newConditionSwitchValue\" name=\"newConditionSwitchValue\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.newCondition.value\" data-ng-checked=\"false\" data-ng-true-value=\"1\" data-ng-false-value=\"0\"></div></div></div><div data-ng-if=\"EventModalCtrl.newConditionType.name === \'dimmer\'\"><div class=\"control-group\" data-ng-class=\"{\'has-error\': !EventModalCtrl.tmpElement}\"><label for=\"newConditionDimmer\" class=\"col-xs-5\" data-translate=\"script_dimmer_select\"></label><div class=\"col-xs-7 controls\"><select id=\"newConditionDimmer\" name=\"newConditionDimmer\" class=\"form-control\" data-ng-model=\"EventModalCtrl.tmpElement\" data-ng-change=\"EventModalCtrl.setConditionBenoicElement()\" data-ng-options=\"dimmer as dimmer.display for dimmer in EventModalCtrl.benoicElements.dimmers track by EventModalCtrl.trackBenoicElement(dimmer, \'dimmer\')\"></select></div></div><div class=\"form-group\"><label for=\"newConditionDimmerValue\" class=\"col-xs-5\" data-translate=\"script_dimmer_value\"></label><div class=\"col-xs-3\"><select id=\"newConditionOperator\" name=\"newConditionOperator\" class=\"form-control\" data-ng-model=\"EventModalCtrl.newCondition.condition\"><option value=\"==\">==</option><option value=\"<\">&lt;</option><option value=\"<=\">&lt;=</option><option value=\">\">&gt;</option><option value=\">=\">&gt;=</option></select></div><div class=\"col-xs-4\"><input id=\"newConditionDimmerValue\" class=\"form-control\" name=\"newConditionDimmerValue\" type=\"number\" min=\"0\" max=\"100\" size=\"3\" data-ng-model=\"EventModalCtrl.newCondition.value\"></div></div></div><div data-ng-if=\"EventModalCtrl.newConditionType.name === \'heater\'\"><div class=\"control-group\"><label for=\"newConditionHeater\" class=\"col-xs-5\" data-translate=\"script_heater_select\"></label><div class=\"col-xs-7 controls\"><select id=\"newConditionHeater\" name=\"newConditionHeater\" class=\"form-control\" data-ng-model=\"EventModalCtrl.tmpElement\" data-ng-change=\"EventModalCtrl.setConditionBenoicElement()\" data-ng-options=\"heater as heater.display for heater in EventModalCtrl.benoicElements.heaters track by EventModalCtrl.trackBenoicElement(heater, \'heater\')\"></select></div></div><div><label for=\"newConditionHeaterValue\" class=\"col-xs-5\" data-translate=\"script_heater_command\"></label><div class=\"col-xs-3\"><select id=\"newConditionOperator\" name=\"newConditionOperator\" class=\"form-control\" data-ng-disabled=\"!!EventModalCtrl.newCondition.commandMode\" data-ng-model=\"EventModalCtrl.newCondition.condition\"><option value=\"==\">==</option><option value=\"<\"><</option><option value=\"<=\"><=< option=\"\"><option value=\">\">&gt;</option><option value=\">=\">&gt;=</option></=<></option></select></div><div class=\"col-xs-4\"><input id=\"newConditionHeaterValue\" class=\"form-control\" name=\"newConditionHeaterValue\" type=\"number\" min=\"0\" max=\"100\" data-ng-disabled=\"!!EventModalCtrl.newCondition.commandMode\" data-ng-model=\"EventModalCtrl.newCondition.commandHeat\"></div></div><div class=\"control-group\"><div class=\"col-xs-12 text-center\"><label data-translate=\"condition_or\"></label></div></div><div class=\"control-group\"><label for=\"newConditionHeaterMode\" class=\"col-xs-5\" data-translate=\"script_heater_mode_title\"></label><div class=\"col-xs-3\"><select id=\"newConditionOperator\" name=\"newConditionOperator\" class=\"form-control\" data-ng-disabled=\"!!EventModalCtrl.newCondition.commandHeat\" data-ng-model=\"EventModalCtrl.newCondition.condition\"><option value=\"==\">=</option><option value=\"!=\">!=</option></select></div><div class=\"col-xs-4 controls\"><select class=\"form-control\" id=\"newConditionHeaterMode\" name=\"newConditionHeaterMode\" data-ng-disabled=\"!!EventModalCtrl.newCondition.commandHeat\" data-ng-model=\"EventModalCtrl.newCondition.commandMode\" data-ng-options=\"mode as EventModalCtrl.heaterModeDisplay(mode) for mode in EventModalCtrl.tmpElement.availableModes\"><option value=\"\"></option></select></div></div></div><div data-ng-if=\"EventModalCtrl.newConditionType.name === \'sensor\'\"><div class=\"control-group\" data-ng-class=\"{\'has-error\': !EventModalCtrl.tmpElement}\"><label for=\"newConditionSensor\" class=\"col-xs-5\" data-translate=\"script_sensor_select\"></label><div class=\"col-xs-7 controls\"><select id=\"newConditionSensor\" name=\"newConditionSensor\" class=\"form-control\" data-ng-model=\"EventModalCtrl.tmpElement\" data-ng-change=\"EventModalCtrl.setConditionBenoicElement()\" data-ng-options=\"sensor as sensor.display for sensor in EventModalCtrl.benoicElements.sensors track by EventModalCtrl.trackBenoicElement(sensor, \'sensor\')\"></select></div></div><div class=\"form-group\"><label for=\"newConditionSensorValue\" class=\"col-xs-5\" data-translate=\"script_sensor_value\"></label><div class=\"col-xs-3\"><select id=\"newConditionOperator\" name=\"newConditionOperator\" class=\"form-control\" data-ng-model=\"EventModalCtrl.newCondition.condition\"><option value=\"==\">==</option><option value=\"<\">&lt;</option><option value=\"<=\">&lt;=</option><option value=\">\">&gt;</option><option value=\">=\">&gt;=</option></select></div><div class=\"col-xs-4\"><input id=\"newConditionSensorValue\" class=\"form-control\" name=\"newConditionSensorValue\" type=\"number\" min=\"0\" max=\"100\" size=\"3\" step=\".1\" data-ng-model=\"EventModalCtrl.newCondition.value\"></div></div></div><div data-ng-if=\"EventModalCtrl.newConditionType.submodule === \'carleon\'\"><div class=\"control-group\" data-ng-class=\"{\'has-error\': !EventModalCtrl.tmpElement}\"><label for=\"newConditionCarleonElement\" class=\"col-xs-5\" data-translate=\"script_element_select\"></label><div class=\"col-xs-7 controls\"><select id=\"newConditionCarleonElement\" name=\"newConditionCarleonElement\" class=\"form-control\" data-ng-model=\"EventModalCtrl.tmpElement\" data-ng-change=\"EventModalCtrl.setActionCarleonElement()\" data-ng-options=\"element as element.name for element in EventModalCtrl.carleonConditionElementsList[EventModalCtrl.newConditionType.name] track by EventModalCtrl.trackCarleonElement(element, EventModalCtrl.newConditionType.name)\"></select></div></div><div class=\"control-group\" data-ng-repeat=\"parameter in EventModalCtrl.carleonConditionCommandsParameters[EventModalCtrl.newConditionType.name]\"><label for=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" class=\"col-xs-5\" data-ng-bind=\"parameter.title\"></label><div class=\"col-xs-7 controls\" data-ng-class=\"{\'has-error\': (parameter.required && !EventModalCtrl.newCondition.parameters[parameter.name] && EventModalCtrl.newCondition.parameters[parameter.name] !== 0)}\"><input type=\"text\" class=\"form-control\" name=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" id=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" data-ng-if=\"parameter.type === \'string\'\" data-ng-model=\"EventModalCtrl.newCondition.parameters[parameter.name]\"> <input type=\"number\" class=\"form-control\" name=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" id=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" data-ng-if=\"parameter.type === \'integer\'\" data-ng-model=\"EventModalCtrl.newCondition.parameters[parameter.name]\"> <input type=\"number\" class=\"form-control\" name=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" id=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" data-ng-if=\"parameter.type === \'real\'\" step=\".01\" data-ng-model=\"EventModalCtrl.newCondition.parameters[parameter.name]\"> <input type=\"checkbox\" id=\"newConditionSwitchValue\" name=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.newCondition.parameters[parameter.name]\" data-ng-checked=\"false\" data-ng-if=\"parameter.type === \'boolean\'\"></div></div><div class=\"control-group\"><label for=\"newConditionCarleonParameter\" class=\"col-xs-5\" data-translate=\"condition_result_select\"></label><div class=\"col-xs-7 controls\"><select id=\"newConditionCarleonParameter\" name=\"newConditionCarleonParameter\" class=\"form-control\" data-ng-init=\"EventModalCtrl.tmpResult = EventModalCtrl.carleonConditionResultParameters[EventModalCtrl.newConditionType.name][0]\" data-ng-model=\"EventModalCtrl.tmpResult\" data-ng-options=\"parameter as EventModalCtrl.tr(parameter.title) for parameter in EventModalCtrl.carleonConditionResultParameters[EventModalCtrl.newConditionType.name]\"></select></div></div><div class=\"control-group\" data-ng-if=\"EventModalCtrl.tmpResult\"><label for=\"{{EventModalCtrl.tmpResult.title}}\" class=\"col-xs-5\" data-translate=\"scheduler_condition_value\"></label><div class=\"col-xs-5 controls\"></div><div class=\"col-xs-3\"><select id=\"{{EventModalCtrl.tmpResult.title}}\" name=\"{{EventModalCtrl.tmpResult.title}}\" class=\"form-control\" data-ng-model=\"EventModalCtrl.newCondition.condition\" data-ng-if=\"EventModalCtrl.tmpResult.type === \'real\' || EventModalCtrl.tmpResult.type === \'integer\'\"><option value=\"==\">==</option><option value=\"<\">&lt;</option><option value=\"<=\">&lt;=</option><option value=\">\">&gt;</option><option value=\">=\">&gt;=</option></select><select id=\"newConditionOperator\" name=\"newConditionOperator\" class=\"form-control\" data-ng-model=\"EventModalCtrl.newCondition.condition\" data-ng-if=\"EventModalCtrl.tmpResult.type === \'string\'\"><option value=\"==\">==</option><option value=\"!=\">!=</option><option value=\"contains\" data-translate=\"condition_contains\"></option><option value=\"not contains\" data-translate=\"condition_not_contains\"></option></select><input type=\"checkbox\" id=\"{{EventModalCtrl.newConditionType.name}}\" name=\"{{EventModalCtrl.newConditionType.name}\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"EventModalCtrl.newCondition.value\" data-ng-checked=\"false\" data-ng-if=\"EventModalCtrl.tmpResult.type === \'boolean\'\"></div><div class=\"col-xs-4\"><input type=\"text\" class=\"form-control\" name=\"{{EventModalCtrl.newConditionType.name}}\" id=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" data-ng-if=\"EventModalCtrl.tmpResult.type === \'string\'\" data-ng-model=\"EventModalCtrl.newCondition.value\"> <input type=\"number\" class=\"form-control\" name=\"{{EventModalCtrl.newConditionType.name}}\" id=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" data-ng-if=\"EventModalCtrl.tmpResult.type === \'integer\'\" data-ng-model=\"EventModalCtrl.newCondition.value\"> <input type=\"number\" class=\"form-control\" name=\"{{EventModalCtrl.newConditionType.name}}\" id=\"{{EventModalCtrl.newConditionType.name + \'$\' + parameter.name}}\" data-ng-if=\"EventModalCtrl.tmpResult.type === \'real\'\" step=\".01\" data-ng-model=\"EventModalCtrl.newCondition.value\"></div></div></div></div><div class=\"row control-group\"><label for=\"eventScriptAdd\" class=\"col-xs-5\" data-translate=\"event_add\"></label><div class=\"col-xs-7\"><button type=\"button\" class=\"btn btn-default\" data-ng-click=\"EventModalCtrl.addCondition()\" data-ng-disabled=\"!EventModalCtrl.isConditionValid()\"><i class=\"fa fa-plus\"></i></button></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"EventModalCtrl.save()\" data-translate=\"btn_ok\" data-ng-disabled=\"!EventModalCtrl.isEventValid()\" data-uib-tooltip=\"{{EventModalCtrl.error.message}}\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"EventModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"btn_cancel\"></button></div></div>");
$templateCache.put("components/event/scheduler.template.html","<div class=\"text-center\"><div class=\"row\"><div class=\"span6\"><label for=\"schedulerEnabled\" data-translate=\"scheduler_enable_button\"></label> <input type=\"checkbox\" id=\"schedulerEnabled\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-change=\"$ctrl.enableScheduler()\" data-ng-model=\"$ctrl.element.enabled\"></div></div><div class=\"row\"><div class=\"span6\"><label data-translate=\"scheduler_next_time\"></label></div><div class=\"span6\"><span data-ng-bind=\"$ctrl.element.next_time * 1000 | date:\'yyyy/MM/dd HH:mm\'\" data-ng-if=\"$ctrl.element.enabled\"></span> <span data-translate=\"scheduler_disabled\" data-ng-if=\"!$ctrl.element.enabled\"></span></div></div></div>");
$templateCache.put("components/event/trigger.template.html","<div class=\"text-center\"><div class=\"row\"><div class=\"span6\"><label for=\"schedulerEnabled\" data-translate=\"scheduler_enable_button\"></label> <input type=\"checkbox\" id=\"schedulerEnabled\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-change=\"$ctrl.enableTrigger()\" data-ng-model=\"$ctrl.element.enabled\"></div></div></div>");
$templateCache.put("components/heater/heater.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal-title\" data-translate=\"heater_update_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"heater_update_name\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"heaterDisplay\" id=\"heaterDisplay\" data-ng-model=\"HeatersModalCtrl.heater.newDisplay\" class=\"modal-input-text form-control\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"heater_update_unit\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"heaterUnit\" id=\"heaterUnit\" data-ng-model=\"HeatersModalCtrl.heater.options.unit\" class=\"modal-input-text form-control\" placeholder=\"{{\'heater_unit_placeholder\' | translate}}\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"heater_update_enabled\"></span><div class=\"col-xs-6 controls\"><input type=\"checkbox\" data-bs-switch=\"\" class=\"form-control\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"HeatersModalCtrl.heater.enabled\"></div></div><div class=\"row\"><hr></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"heater_update_monitor\"></span><div class=\"col-xs-6 controls\"><input type=\"checkbox\" data-bs-switch=\"\" class=\"form-control\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"HeatersModalCtrl.heater.monitored\"></div></div><div class=\"row control-group\" data-ng-if=\"HeatersModalCtrl.heater.monitored\"><span class=\"col-xs-6\" data-translate=\"heater_update_monitor_every\"></span><div class=\"col-xs-6 controls\"><select id=\"bddAddUserGrant\" class=\"form-control\" data-ng-model=\"HeatersModalCtrl.heater.monitored_every\" data-ng-options=\"monitorEvery.value as HeatersModalCtrl.tr(monitorEvery.label) for monitorEvery in HeatersModalCtrl.monitorEveryEnum\"></select></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"HeatersModalCtrl.saveHeater()\" data-translate=\"heater_btn_ok\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"HeatersModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"heater_btn_cancel\"></button></div></div>");
$templateCache.put("components/heater/heater.template.html","<div class=\"container-fluid\" data-ng-class=\"{\'text-italic\': $ctrl.adminMode}\" data-ng-if=\"$ctrl.element.enabled || $ctrl.adminMode\"><div class=\"text-center row control-group\"><span class=\"heater-on\"><i class=\"fa fa-fire\" aria-hidden=\"true\" data-ng-if=\"$ctrl.element.value.on\"></i></span></div><div class=\"row control-group\"><div class=\"ring control-group\"><div class=\"quarter quarter-plus-1\" data-ng-click=\"$ctrl.setHeater(1)\"><span class=\"quarter-value\" data-ng-bind=\"$ctrl.displayCommand(1)\"></span></div><div class=\"quarter quarter-plus-5\" data-ng-click=\"$ctrl.setHeater(5)\"><span class=\"quarter-value\" data-ng-bind=\"$ctrl.displayCommand(5)\"></span></div><div class=\"quarter quarter-minus-1\" data-ng-click=\"$ctrl.setHeater(-1)\"><span class=\"quarter-value\" data-ng-bind=\"$ctrl.displayCommand(-1)\"></span></div><div class=\"quarter quarter-minus-5\" data-ng-click=\"$ctrl.setHeater(-5)\"><span class=\"quarter-value\" data-ng-bind=\"$ctrl.displayCommand(-5)\"></span></div><div class=\"cutout-heater\"><div class=\"ring-value\" data-ng-bind=\"$ctrl.displayCommandValue()\"></div></div></div></div><hr class=\"heater-hr\"><div class=\"row control-group\"><div class=\"col-xs-4 controls\"><label for=\"heaterMode\" data-translate=\"heater_mode_title\"></label></div><div class=\"col-xs-8 controls\"><select class=\"form-control\" name=\"heaterMode\" value=\"heaterMode\" data-ng-model=\"$ctrl.element.value.mode\" data-ng-change=\"$ctrl.setHeater(null)\" data-ng-options=\"mode as $ctrl.modeDisplay(mode) for mode in $ctrl.availableModes\"></select></div></div></div>");
$templateCache.put("components/modals/confirm.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal-title\">{{ConfirmModalCtrl.title}}</h3></div><div class=\"modal-body\"><div class=\"row\" style=\"margin: 5px;\"><span>{{ConfirmModalCtrl.message}}</span></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"ConfirmModalCtrl.ok()\">OK</button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"ConfirmModalCtrl.cancel()\">Cancel</button></div></div></div>");
$templateCache.put("components/modals/edit.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal-title\" data-ng-bind=\"EditModalCtrl.title\"></h3></div><div class=\"modal-body\"><div class=\"row\" style=\"margin: 5px;\"><span data-ng-bind=\"EditModalCtrl.message\"></span></div><div class=\"row control-group\" style=\"margin: 5px;\"><div class=\"controls\"><input type=\"text\" name=\"editInput\" id=\"editInput\" class=\"form-control\" data-ng-model=\"EditModalCtrl.value\" placeholder=\"{{EditModalCtrl.placeholder}}\"></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"EditModalCtrl.ok()\" data-translate=\"modal_button_ok\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"EditModalCtrl.cancel()\" data-translate=\"modal_button_cancel\"></button></div></div></div>");
$templateCache.put("components/modals/image.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal_title\" data-ng-bind=\"ImageModalCtrl.displayTitle()\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><div class=\"col-xs-1\"></div><div class=\"col-xs-10\"><img data-ng-src=\"{{ImageModalCtrl.src}}\" alt=\"{{ImageModalCtrl.title}}\" class=\"img-responsive img-thumbnail\"></div><div class=\"col-xs-1\"></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"ImageModalCtrl.close()\" data-translate=\"monitor_close\"></button></div></div>");
$templateCache.put("components/modals/monitor.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal_title\" data-ng-bind=\"MonitorModalCtrl.displayTitle()\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><div class=\"col-xs-6\"><label for=\"monitorSelectDuration\" data-translate=\"monitor_select_duration\"></label></div><div class=\"col-xs-6 controls\"><select id=\"monitorSelectDuration\" class=\"form-control\" data-ng-model=\"MonitorModalCtrl.duration\" data-ng-change=\"MonitorModalCtrl.getMonitor()\" data-ng-options=\"duration as MonitorModalCtrl.tr(duration.label) for duration in MonitorModalCtrl.durationList track by duration.value\"></select></div></div><div class=\"row\"><data-nvd3 data-options=\"MonitorModalCtrl.options\" data=\"MonitorModalCtrl.data\"></data-nvd3></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"MonitorModalCtrl.close()\" data-translate=\"monitor_close\"></button></div></div>");
$templateCache.put("components/modals/scriptBindElement.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal-title\" data-translate=\"script_bind_element_title\"></h3></div><div class=\"modal-body\"><div class=\"row\"><span data-translat=\"script_bind_element_list\"></span></div><div class=\"row\" style=\"padding: 5px;\"><table data-datatable=\"ng\" data-dt-options=\"ScriptBindElementModalCtrl.options\" class=\"table table-hover table-striped table-bordered dataTable\"><thead><tr><th data-translate=\"script_bind_table_select\"></th><th data-translate=\"script_bind_table_type\"></th><th data-translate=\"script_bind_table_display\"></th></tr></thead><tbody><tr data-ng-repeat=\"element in ScriptBindElementModalCtrl.elementList track by $index\" data-ng-click=\"ScriptBindElementModalCtrl.selectRow($index)\" data-ng-class=\"{\'selected\': element.selected}\"><td><input class=\"text-center\" id=\"elementSelect\" type=\"checkbox\" data-ng-model=\"ScriptBindElementModalCtrl.tmpCheck[$index]\"></td><td data-ng-bind=\"element.typeTitle\"></td><td data-ng-bind=\"element.display\"></td></tr></tbody></table></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"ScriptBindElementModalCtrl.ok()\" data-translate=\"modal_button_ok\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"ScriptBindElementModalCtrl.cancel()\" data-translate=\"modal_button_cancel\"></button></div></div></div>");
$templateCache.put("components/monitor/monitor.template.html","<div class=\"container-fluid\"><div class=\"row control-group\"><div class=\"col-xs-5\"><label for=\"monitorSelectDuration\" data-translate=\"monitor_select_duration\"></label></div><div class=\"col-xs-5 controls\"><select id=\"monitorSelectDuration\" class=\"form-control\" data-ng-model=\"$ctrl.duration\" data-ng-change=\"$ctrl.displayMonitor()\" data-ng-options=\"duration as $ctrl.tr(duration.label) for duration in $ctrl.durationList track by duration.value\"></select></div></div><div class=\"row control-group\"><div class=\"col-xs-1\"></div><div class=\"col-xs-2\"><label for=\"monitorNewElement\" data-translate=\"monitor_new_element\"></label></div><div class=\"col-xs-3 controls\"><select id=\"monitorNewElement\" class=\"form-control\" data-ng-model=\"$ctrl.newElement\" data-ng-options=\"element.display for element in $ctrl.elementListDisplayed\"></select></div><div class=\"col-xs-3 controls\"><button type=\"button\" data-ng-click=\"$ctrl.addElement()\" class=\"btn btn-primary\" data-ng-disabled=\"!$ctrl.newElementValid()\" data-translate=\"monitor_new_element_button\"></button></div></div><div class=\"row text-center\"><div class=\"col-xs-12\"><data-nvd3 data-options=\"$ctrl.options\" data=\"$ctrl.data\"></data-nvd3></div></div><div class=\"row text-center\"><span data-ng-repeat=\"serie in $ctrl.element.elements\" style=\"margin: 5px; color: white;\"><button type=\"button\" class=\"btn btn-primary\" data-ng-click=\"$ctrl.removeSerie(serie)\" data-ng-disabled=\"$ctrl.element.elements.length <= 1\"><i class=\"fa fa-trash-o\" aria-hidden=\"true\"></i> <span data-ng-bind=\"serie.display\"></span></button></span></div></div>");
$templateCache.put("components/sensor/sensor.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal-title\" data-translate=\"sensor_update_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"sensor_update_name\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"sensorDisplay\" id=\"sensorDisplay\" data-ng-model=\"SensorsModalCtrl.sensor.newDisplay\" class=\"modal-input-text form-control\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"sensor_update_unit\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"sensorUnit\" id=\"sensorUnit\" data-ng-model=\"SensorsModalCtrl.sensor.options.unit\" class=\"modal-input-text form-control\" placeholder=\"{{\'sensor_unit_placeholder\' | translate}}\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"sensor_update_enabled\"></span><div class=\"col-xs-6 controls\"><input type=\"checkbox\" data-bs-switch=\"\" class=\"form-control\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"SensorsModalCtrl.sensor.enabled\"></div></div><div class=\"row\"><hr></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"sensor_update_monitor\"></span><div class=\"col-xs-6\"><input type=\"checkbox\" data-bs-switch=\"\" class=\"form-control\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"SensorsModalCtrl.sensor.monitored\"></div></div><div class=\"row control-group\" data-ng-if=\"SensorsModalCtrl.sensor.monitored\"><span class=\"col-xs-6\" data-translate=\"sensor_update_monitor_every\"></span><div class=\"col-xs-6\"><select id=\"bddAddUserGrant\" class=\"form-control\" data-ng-model=\"SensorsModalCtrl.sensor.monitored_every\" data-ng-options=\"monitorEvery.value as SensorsModalCtrl.tr(monitorEvery.label) for monitorEvery in SensorsModalCtrl.monitorEveryEnum\"></select></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"SensorsModalCtrl.saveSensor()\" data-translate=\"sensor_btn_ok\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"SensorsModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"sensor_btn_cancel\"></button></div></div>");
$templateCache.put("components/sensor/sensor.template.html","<div data-ng-if=\"$ctrl.element.enabled || $ctrl.adminMode\"><div class=\"text-center\" data-ng-class=\"{\'text-italic\': $ctrl.adminMode}\"><div data-ng-bind=\"$ctrl.elementDisplayValue()\"></div></div></div>");
$templateCache.put("components/switch/switch.modal.html","<div class=\"modal-content\"><div class=\"modal-header\"><h3 class=\"modal-title\" data-translate=\"switch_update_title\"></h3></div><div class=\"modal-body\"><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"switch_update_name\"></span><div class=\"col-xs-6 controls\"><input type=\"text\" name=\"switcherDisplay\" id=\"switcherDisplay\" data-ng-model=\"SwitchesModalCtrl.switcher.newDisplay\" class=\"modal-input-text form-control\"></div></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"switch_update_enabled\"></span><div class=\"col-xs-6 controls\"><input type=\"checkbox\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"SwitchesModalCtrl.switcher.enabled\"></div></div><div class=\"row\"><hr></div><div class=\"row control-group\"><span class=\"col-xs-6\" data-translate=\"switch_update_monitor\"></span><div class=\"col-xs-6 controls\"><input type=\"checkbox\" class=\"form-control\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"SwitchesModalCtrl.switcher.monitored\"></div></div><div class=\"row control-group\" data-ng-if=\"SwitchesModalCtrl.switcher.monitored\"><span class=\"col-xs-6\" data-translate=\"switch_update_monitor_every\"></span><div class=\"col-xs-6 controls\"><select id=\"bddAddUserGrant\" class=\"form-control\" data-ng-model=\"SwitchesModalCtrl.switcher.monitored_every\" data-ng-options=\"monitorEvery.value as SwitchesModalCtrl.tr(monitorEvery.label) for monitorEvery in SwitchesModalCtrl.monitorEveryEnum\"></select></div></div></div><div class=\"modal-footer\"><button class=\"btn btn-primary\" type=\"button\" data-ng-click=\"SwitchesModalCtrl.saveSwitcher()\" data-translate=\"switch_btn_ok\"></button> <button class=\"btn btn-warning\" type=\"button\" data-ng-click=\"SwitchesModalCtrl.cancel()\" data-dismiss=\"modal\" data-translate=\"switch_btn_cancel\"></button></div></div>");
$templateCache.put("components/switch/switch.template.html","<div data-ng-if=\"$ctrl.element.enabled || $ctrl.adminMode\"><div class=\"text-center\" data-ng-class=\"{\'text-italic\': $ctrl.adminMode}\"><input type=\"checkbox\" data-bs-switch=\"\" data-toggle=\"toggle\" data-switch-size=\"mini\" data-ng-model=\"$ctrl.element.valueChecked\" data-ng-change=\"$ctrl.toggleSwitcher()\"></div></div>");}]);
//# sourceMappingURL=../maps/scripts/app-f0ce9ebb3e.js.map
