angular.module("sagremorApp")
    .factory("carleonFactory", 
    function($http, $q, angharadConfig, angharadBackendService, sagremorParams) {

    var urlBase = angharadConfig.baseUrl + angharadConfig.prefixCarleon;
    var dataFactory = {};

    /* Base Carleon endpoints */
    dataFactory.getServiceList = function () {
        return angharadBackendService.httpRequest("GET", urlBase + "service");
    };

    dataFactory.reloadServiceList = function () {
        return angharadBackendService.httpRequest("PUT", urlBase + "service/reload");
    };

    dataFactory.enableService = function (service_name, enable) {
        return angharadBackendService.httpRequest("PUT", urlBase + "service/" + service_name + "/enable/" + (enable?"1":"0"));
    };

    dataFactory.elementCleanup = function (service_name, element) {
        return angharadBackendService.httpRequest("PUT", urlBase + "service/" + service_name + "/" + element + "/cleanup");
    };

    dataFactory.addTag = function (service_name, element, tag) {
        return angharadBackendService.httpRequest("PUT", urlBase + "service/" + service_name + "/" + element + "/" + tag);
    };

    dataFactory.removeTag = function (service_name, element, tag) {
        return angharadBackendService.httpRequest("DELETE", urlBase + "service/" + service_name + "/" + element + "/" + tag);
    };

    return dataFactory;
});
