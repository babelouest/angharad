angular.module("sagremorApp")
    .controller("serviceCtrl",
    function($scope, $stateParams, $location, $translate, $uibModal, toaster, sagGenericInjectorManager, sagremorService, carleonFactory, sharedData, sagremorParams) {
      
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
        
    }
);
