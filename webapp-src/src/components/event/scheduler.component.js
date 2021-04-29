function sagSchedulerController ($translate, toaster, angharadFactory, sagremorParams, sharedData) {
    var ctrl = this;
    
    function init() {
    }
    
    this.enableScheduler = function () {
		angharadFactory.enableScheduler(ctrl.element.name, ctrl.element.enabled).then(function (result) {
			sharedData.add("angharadSchedulers", result.name, result);
			ctrl.element = result;
			toaster.pop("success", $translate.instant("scheduler_enable"), $translate.instant("scheduler_enable_success"));
		}, function () {
			toaster.pop("error", $translate.instant("scheduler_enable"), $translate.instant("scheduler_enable_error"));
		});
	};
    
    init();
}

angular.module("sagremorApp").component("sagScheduler", {
    templateUrl: "components/event/scheduler.template.html",
    controller: sagSchedulerController,
    bindings: {
        element: "="
    }
})
.run(function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "scheduler",
        directive: "sag-scheduler",
        carleonService: false
    });
})
.filter;
