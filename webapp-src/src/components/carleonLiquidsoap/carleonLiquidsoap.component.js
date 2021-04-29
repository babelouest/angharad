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
.directive("radio", function($sce) {
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
})
.factory("carleonLiquidsoapFactory", function($http, $uibModal, $translate, toaster, angharadConfig, angharadBackendService, sagremorConfirm, sharedData) {
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

})
.config(function run($translatePartialLoaderProvider) {
	$translatePartialLoaderProvider.addPart("carleonLiquidsoap");
})
.run(function(sagGenericInjectorManager, carleonLiquidsoapFactory) {
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
});
