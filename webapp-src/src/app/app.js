angular.module("sagremorApp", [
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
.config(function ($translateProvider, $translatePartialLoaderProvider) {
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
})
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
.factory("sagremorConfirm", function ($uibModal) {
    
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
})
.factory("sagremorEdit", function ($uibModal) {
    
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
})
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
