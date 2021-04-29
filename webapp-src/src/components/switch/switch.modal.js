angular.module("sagremorApp")
    .controller("SwitchesModalCtrl",
    function($scope, $uibModalInstance, $translate, toaster, sagremorConstant, benoicFactory, switcher) {
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
    }
);
