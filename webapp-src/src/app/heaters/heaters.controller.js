angular.module("sagremorApp")
    .controller("heatersCtrl",
    function($scope, $location, $translate, sagremorService, benoicFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.heaterList = [];
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
							sagremorService.editHeater(param);
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
				
				self.updateHeaters();
			});
		};
        
        $scope.$on("benoicDevicesChanged", function () {
            self.updateHeaters();
        });
        
        $scope.$on("benoicHeatersChanged", function () {
            self.updateHeaters();
        });
        
        this.updateHeaters = function () {
            var devices = sharedData.all("benoicDevices");
			self.heaterList = [];
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.heaters) {
                        var heaters = devices[key].element.heaters[se];
                        heaters.device = deviceName;
                        heaters.name = se;
                        self.heaterList.push(heaters);
                    }
                }
            }
        };
        
        $scope.$on("refreshDevices", function () {
            self.refreshElements();
        });
        
        this.refreshElements = function () {
			// Add all elements that are not in self.heaterList
			var devices = sharedData.all("benoicDevices");
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.heaters) {
						var index = 0;
						var found = _.find(self.heaterList, function (element) {
							return element.name === se && element.device === deviceName;
						});
						if (!found) {
							var heater = devices[key].element.heaters[se];
							heater.device = deviceName;
							heater.name = se;
							self.heaterList.push(heater);
						} else {
							var heater = devices[key].element.heaters[se];
							heater.device = deviceName;
							heater.name = se;
							var index = _.indexOf(self.heaterList, _.find(self.heaterList, {device: deviceName, name: se}));
							self.heaterList.splice(index, 1, heater);
						}
                    }
                }
            }
            
            // Remove all elements from self.heaterList that are no longer in devices
            _.remove(self.heaterList, function (heater) {
				var found = _.find(sharedData.all("benoicDevices"), function (device, deviceName) {
					return !device.element || _.find(device.element.heaters, function (element, eltName) {
						return eltName === heater.name && deviceName === heater.device;
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
