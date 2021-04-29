angular.module("sagremorApp")
    .controller("DimmersModalCtrl",
    function($scope, $uibModalInstance, $translate, toaster, sagremorConstant, benoicFactory, dimmer) {
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
    }
);
