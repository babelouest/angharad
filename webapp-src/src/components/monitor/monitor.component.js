function sagMonitorController ($scope, $q, $translate, toaster, angharadFactory, benoicFactory, carleonFactory, sagremorParams, sagremorConstant, sharedData) {
    var ctrl = this;
    
    this.elementList = [];
    this.elementListDisplayed = [];
    this.colorList = [];
    this.newElement = false;
    this.newElementColor = false;
    this.durationList = sagremorConstant.durationList;
    this.duration = {value: 4};
    
    this.data = [];
    this.labels = [];
    this.series = [];
    
    this.datasetOverride = [{
		fill: false
	}];
	

	this.options = {
		chart: {
			type: "lineChart",
			interpolate:"basis",
			x: function(d) { return d.timestamp; },
			y: function(d) { return d.value; },
			showValues: true,
			valueFormat: function(d){
				return d3.format(",.2f")(d);
			},
			transitionDuration: 500,
			xScale : d3.time.scale(),
			xAxis: {
				axisLabel: $translate.instant("monitor_date_axis")
			},
			yAxis: {
				axisLabel: "",
				axisLabelDistance: 30
			}
		}
	};

    function init() {
		updateDevices();
		ctrl.displayMonitor();
    }
    
    this.tr = function (value) {
		return $translate.instant(value);
	};
	
    $scope.$on("updateMonitor-"+ctrl.element.name, function () {
		ctrl.displayMonitor();
	});
	
	function updateDevices() {
		var devices = sharedData.all("benoicDevices");
		
		_.forEach(devices, function (device, deviceName) {
			if (!!device.element) {
				_.forEach(device.element.switches, function (element, name) {
					if (element.monitored && element.enabled) {
						var elt = {
							name: name,
							display: element.display,
							device: deviceName,
							type: "switch"
						};
						ctrl.elementList.push(elt);
					}
				});
			}
		});
			
		_.forEach(devices, function (device, deviceName) {
			if (!!device.element) {
				_.forEach(device.element.dimmers, function (element, name) {
					if (element.monitored && element.enabled) {
						var elt = {
							name: name,
							display: element.display,
							device: deviceName,
							type: "dimmer"
						};
						ctrl.elementList.push(elt);
					}
				});
			}
		});
			
		_.forEach(devices, function (device, deviceName) {
			if (!!device.element) {
				_.forEach(device.element.heaters, function (element, name) {
					if (element.monitored && element.enabled) {
						var elt = {
							name: name,
							display: element.display,
							device: deviceName,
							type: "heater"
						};
						ctrl.elementList.push(elt);
					}
				});
			}
		});
			
		_.forEach(devices, function (device, deviceName) {
			if (!!device.element) {
				_.forEach(device.element.sensors, function (element, name) {
					if (element.monitored && element.enabled) {
						var elt = {
							name: name,
							display: element.display,
							device: deviceName,
							type: "sensor"
						};
						ctrl.elementList.push(elt);
					}
				});
			}
		});
	}
	
    this.addElement = function () {
		ctrl.newElement.color = ctrl.newElementColor;
		ctrl.element.elements.push(ctrl.newElement);
		angharadFactory.saveCurrentProfile().then(function () {
			ctrl.displayMonitor();
		}, function () {
			toaster.pop("error", $translate.instant("profile_save"), $translate.instant("profile_save_error"));
		})["finally"](function () {
		});
	};
	
	this.displayMonitor = function() {
		var from = new Date();
		
		switch (ctrl.duration.value) {
			case 0:
				from.setTime(from.getTime() - 1*60*60*1000);
				break;
			case 1:
				from.setTime(from.getTime() - 2*60*60*1000);
				break;
			case 2:
				from.setTime(from.getTime() - 6*60*60*1000);
				break;
			case 3:
				from.setTime(from.getTime() - 12*60*60*1000);
				break;
			case 4:
				from.setTime(from.getTime() - 24*60*60*1000);
				break;
			case 5:
				from.setTime(from.getTime() - 48*60*60*1000);
				break;
			case 6:
				from.setTime(from.getTime() - 72*60*60*1000);
				break;
			case 7:
				from.setTime(from.getTime() - 168*60*60*1000);
				break;
			case 8:
				from.setMonth(from.getMonth() - 1);
				break;
		}
		ctrl.colorList = angular.copy(sagremorConstant.colorList);
		ctrl.elementListDisplayed = angular.copy(ctrl.elementList);
		var httpMonitor = [];
		
		_.forEach(ctrl.element.elements, function (element) {
			httpMonitor.push(benoicFactory.getMonitor(element.device, element.type, element.name, Math.round(from.getTime() / 1000)));
		});
		
		ctrl.data = [];
		$q.all(httpMonitor).then(function (results) {
			for (key in results) {
				_.remove(ctrl.elementListDisplayed, function (curElement) {
					return curElement.device === ctrl.element.elements[key].device &&
							curElement.type === ctrl.element.elements[key].type &&
							curElement.name === ctrl.element.elements[key].name;
				});
				ctrl.series.push(ctrl.element.elements[key].display);
                var myData = {
					key: ctrl.element.elements[key].display,
					values: []
                };
                if (!!results[key] && results[key].length > 0) {
					_.forEach(results[key], function (monitor) {
						var curDate = new Date(monitor.timestamp * 1000);
						var value = {timestamp: curDate, value: parseFloat(monitor.value)};
						myData.values.push(value);
					});
					ctrl.data.push(myData);
				}
			}
		}, function (error) {
		});
	}
	
	this.newElementValid = function () {
		return !!ctrl.newElement;
	};
	
	this.removeSerie = function (serie) {
		if (ctrl.element.elements.indexOf(serie) >= 0) {
			ctrl.element.elements.splice(ctrl.element.elements.indexOf(serie), 1);
			angharadFactory.saveCurrentProfile().then(function () {
				ctrl.displayMonitor();
			}, function () {
				toaster.pop("error", $translate.instant("profile_save"), $translate.instant("profile_save_error"));
			})["finally"](function () {
				ctrl.newElementColor = false;
				ctrl.newElement = false;
			});
		}
	};
    
    init();
}

angular.module("sagremorApp").component("sagMonitor", {
    templateUrl: "components/monitor/monitor.template.html",
    controller: sagMonitorController,
    bindings: {
        element: "="
    }
})
.run(function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "monitor",
        directive: "sag-monitor",
        carleonService: false
    });
});
