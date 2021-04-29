angular.module("sagremorApp")
    .controller("carleonLiquidsoapModalCtrl", 
    function($rootScope, $uibModalInstance, $translate, sharedData, toaster, carleonLiquidsoapFactory, element) {
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
    }
);
