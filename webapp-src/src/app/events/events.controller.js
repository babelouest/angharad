angular.module("sagremorApp")
    .controller("eventsCtrl",
    function($scope, $translate, sagremorService, angharadFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.schedulerList = [];
        this.triggerList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {

			$translate(["edit", "remove", "add_to_dashboard"]).then(function (results) {
				self.menuScheduler = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editEvent(param, "scheduler");
						}
					},
					{
						name: "remove", 
						display: results.remove, 
						action: function (param) {
							sagremorService.removeScheduler(param).then(function () {
								$scope.$broadcast("angharadSchedulersChanged");
							});
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							param.type = !!param.next_time?"scheduler":"trigger";
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				self.menuTrigger = [
					{
						name: "edit", 
						display: results.edit, 
						action: function (param) {
							sagremorService.editEvent(param, "trigger");
						}
					},
					{
						name: "remove", 
						display: results.remove, 
						action: function (param) {
							sagremorService.removeTrigger(param).then(function () {
								$scope.$broadcast("angharadTriggersChanged");
							});
						}
					},
					{
						name: "add_to_dashboard", 
						display: results.add_to_dashboard, 
						action: function (param) {
							param.type = !!param.next_time?"scheduler":"trigger";
							sagremorService.addToDashboard(param).then(function () {
								$scope.$broadcast("refreshDashboard");
							});
						}
					}
				];
				
				self.updateSchedulers();
				self.updateTriggers();
			});
		};
        
        this.updateSchedulers = function () {
            var events = sharedData.all("angharadSchedulers");
            self.schedulerList = [];
            for (key in events) {
				self.schedulerList.push(events[key]);
            }
        };
        
        this.updateTriggers = function () {
            var events = sharedData.all("angharadTriggers");
            self.triggerList = [];
            for (key in events) {
				self.triggerList.push(events[key]);
            }
        };
        
        this.refreshEvents = function () {
			var schedulers = sharedData.all("angharadSchedulers");
            for (key in schedulers) {
				var found = _.find(self.schedulerList, function (scheduler) {
					return scheduler.name === key;
				});
				
				if (!found) {
					self.schedulerList.push(schedulers[key]);
				}
            }
            
            for (var index = self.schedulerList.length - 1; index >= 0; index--) {
				if (!sharedData.get("angharadSchedulers", self.schedulerList[index].name)) {
					self.schedulerList.splice(index, 1);
				}
			}

			var triggers = sharedData.all("angharadTriggers");
            for (key in triggers) {
				var found = _.find(self.triggerList, function (trigger) {
					return trigger.name === key;
				});
				
				if (!found) {
					self.triggerList.push(triggers[key]);
				}
            }
            
            for (var index = self.triggerList.length - 1; index >= 0; index--) {
				if (!sharedData.get("angharadTriggers", self.triggerList[index].name)) {
					self.triggerList.splice(index, 1);
				}
			}
		};
        
        this.newEvent = function () {
			sagremorService.editEvent(null);
		};
		
        $scope.$on("angharadSchedulersChanged", function () {
            self.updateSchedulers();
        });
        
        $scope.$on("angharadTriggersChanged", function () {
            self.updateTriggers();
        });
        
        $scope.$on("refreshAngharadEvents", function () {
            self.refreshEvents();
        });
        
        this.init();
        
    }
);
