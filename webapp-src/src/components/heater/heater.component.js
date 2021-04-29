function sagHeaterController (benoicFactory, sagremorParams, $translate) {
    var ctrl = this;
    
    ctrl.element.valueChecked = (ctrl.element.value === 1);
    ctrl.adminMode = sagremorParams.adminMode;
    ctrl.element.newDisplay = "";
    ctrl.availableModes = [];
    
    function init() {
        ctrl.element.type = "heater";
        if (!!ctrl.element) {
			ctrl.element.newDisplay = ctrl.element.display;
			ctrl.availableModes = ctrl.element.value.availableModes;
		}
    }
    
    ctrl.setHeater = function (value) {
		if (value == null) {
			value = ctrl.element.value.command;
		} else {
			value = ctrl.element.value.command + value;
		}
        benoicFactory.setElement(ctrl.element.device, "heater", ctrl.element.name, value, {mode: ctrl.element.value.mode}).then(function () {
            ctrl.element.value.command = value;
        });
    };
    
    ctrl.displayCommandValue = function () {
		var command = !!ctrl.element?(Math.round(ctrl.element.value.command * 100) / 100):0;
        if (!!ctrl.element && !!ctrl.element.options.unit) {
			command += " " + ctrl.element.options.unit
		}
		return command;
	};
	
	ctrl.displayCommand = function (command) {
		var value = "";
		if (command < 0) {
			value = command;
		} else {
			value = "+" + command;
		}
        if (!!ctrl.element && !!ctrl.element.options.unit) {
			value += " " + ctrl.element.options.unit
		}
		return value
	};
	
	ctrl.modeDisplay = function (value) {
		return $translate.instant("heater_mode_" + value.toLowerCase());
	};
    
    init();
}

angular.module("sagremorApp").component("sagHeater", {
    templateUrl: "components/heater/heater.template.html",
    controller: sagHeaterController,
    bindings: {
        element: "="
    }
})
.run(function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "heater",
        directive: "sag-heater",
        carleonService: false
    });
});
