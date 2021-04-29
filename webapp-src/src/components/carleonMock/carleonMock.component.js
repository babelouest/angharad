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
.factory("carleonMockFactory", function($http, $uibModal, $translate, toaster, angharadConfig, angharadBackendService, sagremorConfirm, sharedData) {
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

})
.config(function run($translatePartialLoaderProvider) {
	$translatePartialLoaderProvider.addPart("carleonMock");
})
.run(function(sagGenericInjectorManager, carleonMockFactory) {
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
});
