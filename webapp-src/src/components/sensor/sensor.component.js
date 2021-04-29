function sagSensorController (benoicFactory, sagremorParams, $translate) {
    var ctrl = this;
    
    ctrl.adminMode = sagremorParams.adminMode;
    ctrl.element.newDisplay = ctrl.element.display;
    ctrl.messages = {};
    
    function init() {
        ctrl.element.type = "sensor";
    }
    
    ctrl.elementDisplayValue = function() {
		var value = ctrl.element.value;
		if (!isNaN(parseFloat(value)) && isFinite(value)) {
			value = (Math.round(value * 100) / 100);
		}
        if (!!ctrl.element && !!ctrl.element.options.unit) {
			value += " " + ctrl.element.options.unit
		}
		return value;
	};
    
    init();
}

angular.module("sagremorApp").component("sagSensor", {
    templateUrl: "components/sensor/sensor.template.html",
    controller: sagSensorController,
    bindings: {
        element: "="
    }
})
.run(function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "sensor",
        directive: "sag-sensor",
        carleonService: false
    });
});
