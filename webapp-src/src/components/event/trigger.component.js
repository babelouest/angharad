function sagTriggerController ($translate, toaster, angharadFactory, sagremorParams, sharedData) {
    var ctrl = this;
    
    function init() {
    }
    
    this.enableTrigger = function () {
		angharadFactory.enableTrigger(ctrl.element.name, ctrl.element.enabled).then(function (result) {
			sharedData.add("angharadTrigger", result.name, result);
			ctrl.element = result;
			toaster.pop("success", $translate.instant("trigger_enable"), $translate.instant("trigger_enable_success"));
		}, function () {
			toaster.pop("error", $translate.instant("trigger_enable"), $translate.instant("trigger_enable_error"));
		});
	};
    
    init();
}

angular.module("sagremorApp").component("sagTrigger", {
    templateUrl: "components/event/trigger.template.html",
    controller: sagTriggerController,
    bindings: {
        element: "="
    }
})
.run(function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "trigger",
        directive: "sag-trigger",
        carleonService: false
    });
})
.filter;
