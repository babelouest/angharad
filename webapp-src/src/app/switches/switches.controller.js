angular.module("sagremorApp")
    .controller("switchesCtrl",
    function($scope, $location, $translate, sagremorService, benoicFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.switchList = [];
        this.dimmerList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {
			if (!sagremorParams.loggedIn) {
				$location.path("/login");
			}
			$translate(["edit", "monitor", "add_to_dashboard"]).then(function (results) {
				self.menuSwitcher = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editSwitcher(param);
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
				self.menuDimmer = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editDimmer(param);
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
				
				self.updateSwitchers();
				self.updateDimmers();
			});
		};
        
        $scope.$on("benoicDevicesChanged", function () {
            self.updateSwitchers();
            self.updateDimmers();
        });
        
        $scope.$on("benoicSwitchesChanged", function () {
            self.updateSwitchers();
            self.updateDimmers();
        });
        
        this.updateSwitchers = function () {
            var devices = sharedData.all("benoicDevices");
			self.switchList = [];
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (sw in devices[key].element.switches) {
                        var switcher = devices[key].element.switches[sw];
                        switcher.device = deviceName;
                        switcher.name = sw;
                        self.switchList.push(switcher);
                    }
                }
            }
        };
        
        this.updateDimmers = function () {
            var devices = sharedData.all("benoicDevices");
			self.dimmerList = [];
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (di in devices[key].element.dimmers) {
                        var dimmer = devices[key].element.dimmers[di];
                        dimmer.device = deviceName;
                        dimmer.name = di;
                        self.dimmerList.push(dimmer);
                    }
                }
            }
        };
        
        $scope.$on("refreshDevices", function () {
            self.refreshElements();
        });
        
        this.refreshElements = function () {
			// Add all elements that are not in self.dimmerList
			var devices = sharedData.all("benoicDevices");
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.dimmers) {
						var index = 0;
						var found = _.find(self.dimmerList, function (element) {
							return element.name === se && element.device === deviceName;
						});
						if (!found) {
							var dimmer = devices[key].element.dimmers[se];
							dimmer.device = deviceName;
							dimmer.name = se;
							self.dimmerList.push(dimmer);
						} else {
							var dimmer = devices[key].element.dimmers[se];
							dimmer.device = deviceName;
							dimmer.name = se;
							var index = _.indexOf(self.dimmerList, _.find(self.dimmerList, {device: deviceName, name: se}));
							self.dimmerList.splice(index, 1, dimmer);
						}
                    }
                }
            }
            
            // Remove all elements from self.dimmerList that are no longer in devices
            _.remove(self.dimmerList, function (dimmer) {
				var found = _.find(sharedData.all("benoicDevices"), function (device, deviceName) {
					return _.find(device.element.dimmers, function (element, eltName) {
						return eltName === dimmer.name && deviceName === dimmer.device;
					});
				});
				
				if (!found) {
					return true;
				}
			});
			
			// Add all elements that are not in self.switchList
			var devices = sharedData.all("benoicDevices");
            for (key in devices) {
                var deviceName = devices[key].name;
                if (devices[key].connected && devices[key].enabled) {
                    for (se in devices[key].element.switches) {
						var index = 0;
						var found = _.find(self.switchList, function (element) {
							return element.name === se && element.device === deviceName;
						});
						if (!found) {
							var switcher = devices[key].element.dimmers[se];
							switcher.device = deviceName;
							switcher.name = se;
							self.switchList.push(switcher);
						} else {
							var switcher = devices[key].element.switches[se];
							switcher.device = deviceName;
							switcher.name = se;
							var index = _.indexOf(self.switchList, _.find(self.switchList, {device: deviceName, name: se}));
							self.switchList.splice(index, 1, switcher);
						}
                    }
                }
            }
            
            // Remove all elements from self.switchList that are no longer in devices
            _.remove(self.switchList, function (switcher) {
				var found = _.find(sharedData.all("benoicDevices"), function (device, deviceName) {
					return !device.element || _.find(device.element.dimmers, function (element, eltName) {
						return eltName === dimmer.name && deviceName === dimmer.device;
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
