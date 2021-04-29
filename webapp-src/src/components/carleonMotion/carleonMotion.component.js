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
.factory("carleonMotionFactory", function($http, $uibModal, $translate, toaster, angharadConfig, angharadBackendService, sagremorConfirm, sharedData) {
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

})
.config(function run($translatePartialLoaderProvider) {
	$translatePartialLoaderProvider.addPart("carleonMotion");
})
.run(function(sagGenericInjectorManager, carleonMotionFactory) {
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
});
