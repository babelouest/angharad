function topLeftMenuCtrl ($scope, $translate, sagremorParams, sagGenericInjectorManager, sharedData) {
    var self = this;
    
    this.serviceList = [];
    this.sagremorParams = sagremorParams;
    
    function init() {
		self.loadMenu();
	}
	
	this.loadMenu = function () {
		self.serviceList = [];
		_.forEach(sagGenericInjectorManager.components, function (inject) {
			var service = sharedData.get("carleonServices", inject.type);
			if (inject.carleonService && inject.leftMenu && !!service && service.enabled) {
				var menu = inject.leftMenu;
				menu.icon = inject.icon;
				if (!_.find(self.serviceList, {target: menu.target})) {
					self.serviceList.push(menu);
				}
			}
		});
	};
    
    /**
     * Because I couldn't find how to do it with otherwise...
     */
    this.selectTab = function (tabName) {
        $(".sag-menu").removeClass("active");
        $(tabName).addClass("active");
    };
    
	$scope.$on("submodulesChanged", function () {
		self.sagremorParams = sagremorParams;
	});
	
	$scope.$on("carleonServicesChanged", function () {
		self.loadMenu();
	});

    init();
}

angular.module("sagremorApp").component("topLeftMenu", {
    templateUrl: "components/topLeftMenu/topLeftMenu.template.html",
    controller: topLeftMenuCtrl,
    controllerAs: "topLeftMenuCtrl"
});
