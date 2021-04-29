angular.module("sagremorApp").directive("sagGenericInjector", function ($compile, sagGenericInjectorManager, sharedData) {
    
    var template = "<directive element=\"elt\"></directive>";
    
    var templateNotFound = "<div class=\"container has-error\" data-translate=\"injector_not_found\"></div>";
     
    var templateDisabled = "<div class=\"container has-error\" data-translate=\"injector_disabled\"></div>";
     
    return{
        scope: {
            type: "=",
            elt: "="
        },
        link: function(scope, element) {
            var config = _.find(sagGenericInjectorManager.components, {type: scope.type});
			var service = sharedData.get("carleonServices", scope.type);
			if (config.carleonService && service && service.enabled) {
				content = $compile(template.replace(/directive/g, config.directive))(scope);
			} else if (config.carleonService && service && !service.enabled) {
				content = $compile(templateDisabled)(scope);
			} else if (!config.carleonService) {
				content = $compile(template.replace(/directive/g, config.directive))(scope);
			} else {
				content = $compile(templateNotFound)(scope);
			}
            element.append(content);
        }
    }});
