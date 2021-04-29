function sagContainerController ($rootScope, $translate, toaster, sharedData, angharadFactory) {
    var ctrl = this;
    
    this.scripts = [];
    
    function init() {
		var tag = "SGMR$SCR$";
		switch (ctrl.type) {
			case "switch":
				tag += "B$" + ctrl.element.device + "$switch$" + ctrl.element.name;
				break;
			case "dimmer":
				tag += "B$" + ctrl.element.device + "$dimmer$" + ctrl.element.name;
				break;
			case "heater":
				tag += "B$" + ctrl.element.device + "$heater$" + ctrl.element.name;
				break;
			case "sensor":
				tag += "B$" + ctrl.element.device + "$sensor$" + ctrl.element.name;
				break;
			case "scheduler":
				tag += "A$scheduler$" + ctrl.element.name;
				break;
			default:
				// Carleon service
				tag += "C$" + ctrl.type + "$" + ctrl.element.name;
				break;
		}
		ctrl.scripts = _.filter(sharedData.all("angharadScripts"), function (script) {
			return !!_.find(script.options.tags, function (oneTag) {
				return oneTag === tag;
			});
		});
    }
    
    this.runScript = function (script) {
		var loaderToast = toaster.pop({type: "wait", title: $translate.instant("angharad_loading_script_title"), body: $translate.instant("angharad_loading_script_message"), timeout: 0, showCloseButton: false});
		angharadFactory.runScript(script.name).then(function () {
			toaster.pop("success", $translate.instant("script_run", {name: script.name}), $translate.instant("script_run_success"));
		}, function (error) {
			toaster.pop("error", $translate.instant("script_run", {name: script.name}), $translate.instant("script_run_error"));
		})["finally"](function () {
			$rootScope.$broadcast("refresh");
			toaster.clear(loaderToast);
		});
	};
    
    ctrl.menuSelect = function(menuItem, element) {
        menuItem.action(element);
    };
    
    init();
}

angular.module("sagremorApp").component("sagContainer", {
    templateUrl: "components/container/container.template.html",
    controller: sagContainerController,
    transclude: true,
    bindings: {
        type: "=",
        menu: "=",
        title: "=",
        size: "=",
        element: "=",
        dashboard: "=",
        sagParams: "=",
        icon: "="
    }
});
