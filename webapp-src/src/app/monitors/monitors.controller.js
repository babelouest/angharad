angular.module("sagremorApp")
    .controller("monitorsCtrl",
    function($scope, $translate, toaster, sagremorService, angharadFactory, carleonFactory, sagremorParams, sagremorEdit) {
      
        var self = this;
        
        this.monitorList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {

			$translate(["edit", "remove", "add_to_dashboard"]).then(function (results) {
				self.menu = [
					{
						name: "remove", 
						display: results.remove, 
						action: function (param) {
							sagremorService.removeMonitor(param).then(function () {
								$scope.$broadcast("angharadProfileChanged");
							});
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							param.type = "monitor";
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				self.updateMonitors();
			});
		};
        
        $scope.$on("angharadProfileChanged", function () {
            self.updateMonitors();
        });
        
        this.updateMonitors = function () {
			var profile = sagremorParams.currentProfile;
			if (!!profile && !!profile.monitorList) {
				self.monitorList = profile.monitorList;
			}
        };
        
        this.newMonitor = function () {
			return sagremorEdit.open($translate.instant("monitor_new_title"), $translate.instant("monitor_new_value"), $translate.instant("monitor_new_placeholder")).then(function (result) {
				addMonitor(result.value);
			});
		};
		
		function addMonitor(name) {
			if (sagremorParams.currentProfile) {
				var exist = _.find(sagremorParams.currentProfile.monitorList, function (monitor) {
					return monitor.name === name;
				});
				if (!sagremorParams.currentProfile.monitorList) {
					sagremorParams.currentProfile.monitorList = [];
				}
				if (!exist && !!name) {
					var monitor = {name: name, elements: []};
					sagremorParams.currentProfile.monitorList.push(monitor);
					angharadFactory.saveCurrentProfile().then(function() {
						toaster.pop("success", $translate.instant("monitor_save"), $translate.instant("monitor_save_success"));
					}, function () {
						toaster.pop("error", $translate.instant("monitor_save"), $translate.instant("monitor_save_error"));
					})["finally"](function () {
						$scope.$broadcast("angharadProfileChanged");
					});
				}
			}
		}
        
        this.init();
        
    }
);
