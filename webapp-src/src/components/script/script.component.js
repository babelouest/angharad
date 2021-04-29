function sagScriptController ($rootScope, $translate, toaster, angharadFactory, sagremorParams) {
    var ctrl = this;
    
    this.loaderToast = null;
    
    function init() {
    }
    
    this.runScript = function () {
			toaster.clear(self.loaderToast);
			self.loaderToast = toaster.pop({type: "wait", title: $translate.instant("angharad_loading_script_title"), body: $translate.instant("angharad_loading_script_message"), timeout: 0, showCloseButton: false});
			angharadFactory.runScript(ctrl.element.name).then(function () {
				toaster.pop("success", $translate.instant("script_run", {name: ctrl.element.name}), $translate.instant("script_run_success"));
			}, function (error) {
				toaster.pop("error", $translate.instant("script_run", {name: ctrl.element.name}), $translate.instant("script_run_error"));
			})["finally"](function () {
				toaster.clear(self.loaderToast);
				$rootScope.$broadcast("refresh");
			});
		};
    
    init();
}

angular.module("sagremorApp").component("sagScript", {
    templateUrl: "components/script/script.template.html",
    controller: sagScriptController,
    bindings: {
        element: "="
    }
})
.run(function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "script",
        directive: "sag-script",
        carleonService: false
    });
});
