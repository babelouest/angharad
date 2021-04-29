angular.module("sagremorApp")
    .controller("MonitorModalCtrl",
    function($scope, $uibModalInstance, $translate, toaster, benoicFactory, sagremorConstant, element) {
        var self = this;
        
        this.element = element;
        this.data = [];
        this.durationList = sagremorConstant.durationList;
        this.duration = {value: 4};
        
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
					axisLabel: element.options.unit,
					axisLabelDistance: 30
				}
			}
		};
		
        function init() {
			self.getMonitor();
        }
        
        this.getMonitor = function () {
			var from = new Date();
			
			switch (self.duration.value) {
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
			
            benoicFactory.getMonitor(element.device, element.type, element.name, Math.round(from.getTime() / 1000)).then(function (result) {
                self.data = [];
                var myData = {
					key: element.display,
					values: []
                };
                if (!!result && result.length > 0) {
					_.forEach(result, function (monitor) {
						var curDate = new Date(monitor.timestamp * 1000);
						var value = {timestamp: curDate, value: parseFloat(monitor.value)};
						myData.values.push(value);
					});
					self.data.push(myData);
				}
            });
		}
        
        this.tr = function (value) {
			return $translate.instant(value);
		};
        
        this.close = function () {
            $uibModalInstance.dismiss("close");
        };
        
        this.displayTitle = function () {
			return $translate.instant("monitor_title") + element.display;
		};
		
        init();
    }
);
