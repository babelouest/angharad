angular.module("sagremorApp")
    .controller("sensorsCtrl",
    function($scope, $location, $translate, sagremorService, benoicFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.sensorList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {
			if (!sagremorParams.loggedIn) {
				$location.path("/login");
			}
			$translate(["edit", "monitor", "add_to_dashboard"]).then(function (results) {
				self.menuSensor = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editSensor(param);
						}
					},
					{
						name: "monitor", 
						display: results.monitor, 
						action: function (param) {
							sagremorService.monitor(param);
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				
				self.updateSensors();
			});
		};
        
        $scope.$on("benoicDevicesChanged", function () {
            self.updateSensors();
        });
        
        $scope.$on("benoicSensorsChanged", function () {
            self.updateSensors();
        });
        
        this.updateSensors = function () {
            var devices = sharedData.all("benoicDevices");
			self.sensorList = [];
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.sensors) {
                        var sensors = devices[key].element.sensors[se];
                        sensors.device = deviceName;
                        sensors.name = se;
                        self.sensorList.push(sensors);
                    }
                }
            }
        };
        
        $scope.$on("refreshDevices", function () {
            self.refreshElements();
        });
        
        this.refreshElements = function () {
			// Add all elements that are not in self.sensorList
			var devices = sharedData.all("benoicDevices");
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.sensors) {
						var index = 0;
						var found = _.find(self.sensorList, function (element) {
							return element.name === se && element.device === deviceName;
						});
						if (!found) {
							var sensor = devices[key].element.sensors[se];
							sensor.device = deviceName;
							sensor.name = se;
							self.sensorList.push(sensor);
						} else {
							var sensor = devices[key].element.sensors[se];
							sensor.device = deviceName;
							sensor.name = se;
							var index = _.indexOf(self.sensorList, _.find(self.sensorList, {device: deviceName, name: se}));
							self.sensorList.splice(index, 1, sensor);
						}
                    }
                }
            }
            
            // Remove all elements from self.sensorList that are no longer in devices
            _.remove(self.sensorList, function (sensor) {
				var found = _.find(sharedData.all("benoicDevices"), function (device, deviceName) {
					return !device.element || _.find(device.element.sensors, function (element, eltName) {
						return eltName === sensor.name && deviceName === sensor.device;
					});
				});
				
				if (!found) {
					return true;
				}
			});
		};
        
        this.init();
        
    }
);
