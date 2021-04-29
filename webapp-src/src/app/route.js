angular.module("sagremorApp")
    .config(function($locationProvider) {
        $locationProvider.html5Mode(true).hashPrefix('!');
    })
    .config(function ($stateProvider, $urlRouterProvider) {
        
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
    });
