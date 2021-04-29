function sagSwitchController (benoicFactory, sagremorParams, $translate) {
    var ctrl = this;
    
    ctrl.element.valueChecked = (ctrl.element.value === 1);
    ctrl.adminMode = sagremorParams.adminMode;
    ctrl.element.newDisplay = ctrl.element.display;
    ctrl.messages = {};
    
    function init() {
        ctrl.element.type = "switch";
    }
    
    ctrl.toggleSwitcher = function () {
        ctrl.element.value = ctrl.element.valueChecked?1:0;
        benoicFactory.setElement(ctrl.element.device, 'switch', ctrl.element.name, ctrl.element.value);
    };
    
    init();
}

angular.module('sagremorApp').component('sagSwitch', {
    templateUrl: 'components/switch/switch.template.html',
    controller: sagSwitchController,
    bindings: {
        element: '='
    }
})
.run(function(sagGenericInjectorManager) {
    sagGenericInjectorManager.add({
        type: "switch",
        directive: "sag-switch",
        carleonService: false
    });
});
