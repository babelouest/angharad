angular.module("sagremorApp")
    .controller("SensorsModalCtrl",
    function($scope, $uibModalInstance, $translate, toaster, sagremorConstant, benoicFactory, sensor) {
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
    }
);
