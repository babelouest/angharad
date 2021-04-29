function sagDimmerController (benoicFactory, sagremorParams, $translate) {
    var ctrl = this;
    
    ctrl.element.valueChecked = (ctrl.element.value === 1);
    ctrl.adminMode = sagremorParams.adminMode;
    ctrl.element.newDisplay = ctrl.element.display;
    
    function init() {
        ctrl.element.type = "dimmer";
    }
    
    ctrl.setDimmer = function (value) {
        if (value === 0 && ctrl.element.value === 0) {
			benoicFactory.setElement(ctrl.element.device, "dimmer", ctrl.element.name, 101).then(function (result) {
				ctrl.element.value = result.value;
			});
		} else if (value >= 0 && value <= 100) {
			benoicFactory.setElement(ctrl.element.device, "dimmer", ctrl.element.name, value).then(function () {
				ctrl.element.value = value;
			});
		}
    };
    
    ctrl.addDimmer = function (value) {
		if (ctrl.element.value + value >= 0 && ctrl.element.value + value <= 100) {
			benoicFactory.setElement(ctrl.element.device, "dimmer", ctrl.element.name, ctrl.element.value + value).then(function () {
				ctrl.element.value += value;
			});
		}
	}
    
    ctrl.Dimmervalue = function () {
		return ctrl.element.value + "%"
	};
    
    init();
}

angular.module("sagremorApp").component("sagDimmer", {
    templateUrl: "components/dimmer/dimmer.template.html",
    controller: sagDimmerController,
    bindings: {
        element: "="
    }
})
.run(function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "dimmer",
        directive: "sag-dimmer",
        carleonService: false
    });
});
