angular.module("sagremorApp")
    .controller("scriptsCtrl",
    function($scope, $translate, sagremorService, angharadFactory, sharedData, sagremorParams) {
      
        var self = this;
        
        this.scriptList = [];
        this.sagremorParams = sagremorParams;
        
        this.init = function () {
					$translate(["edit", "monitor", "bind_to_element", "add_to_dashboard"]).then(function (results) {
						self.menuScript = [
							{
								name: "edit", 
								display: $translate.instant("edit"), 
								action: function (param) {
									sagremorService.editScript(param);
								}
							},
							{
								name: "remove", 
								display: $translate.instant("remove"),
								action: function (param) {
									sagremorService.removeScript(param).then(function () {
										$scope.$broadcast("angharadScriptsChanged");
									});
								}
							},
							{
								name: "bind_to_element", 
								display: $translate.instant("bind_to_element"),
								action: function (param) {
									sagremorService.bindToElement(param);
								}
							},
							{
								name: "add_to_dashboard", 
								display: $translate.instant("add_to_dashboard"),
								action: function (param) {
									param.type = "script";
									sagremorService.addToDashboard(param).then(function () {
										$scope.$broadcast("refreshDashboard");
									});
								}
							}
						];
						self.updateScripts();
					});
				};
        
        $scope.$on("angharadScriptsChanged", function () {
            self.updateScripts();
        });
        
        this.updateScripts = function () {
            var scripts = sharedData.all("angharadScripts");
            self.scriptList = [];
            for (key in scripts) {
				self.scriptList.push(scripts[key]);
            }
        };
        
        $scope.$on("refreshAngharadScripts", function () {
            self.refreshScripts();
        });
        
        this.refreshScripts = function () {
					var scripts = sharedData.all("angharadScripts");
								for (key in scripts) {
						var found = _.find(self.scriptList, function (script) {
							return script.name === key;
						});
						
						if (!found) {
							self.scriptList.push(scripts[key]);
						}
								}
								
								for (var index = self.scriptList.length - 1; index >= 0; index--) {
						if (!sharedData.get("angharadScripts", self.scriptList[index].name)) {
							self.scriptList.splice(index, 1);
						}
					}
				};
        
        this.newScript = function () {
					sagremorService.editScript(null);
				};
		
        this.init();
        
    }
);
