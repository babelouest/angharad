angular.module("sagremorApp")
    .factory("benoicFactory", 
    function($http, angharadConfig, angharadBackendService) {

    var urlBase = angharadConfig.baseUrl + angharadConfig.prefixBenoic;
    var dataFactory = {};

    dataFactory.getDeviceTypes = function () {
        return angharadBackendService.httpRequest("GET", urlBase + "deviceTypes/");
    };

    dataFactory.reloadDeviceTypes = function () {
        return angharadBackendService.httpRequest("PUT", urlBase + "deviceTypes/reload");
    };

    dataFactory.getDeviceList = function () {
        return angharadBackendService.httpRequest("GET", urlBase + "device/");
    };

    dataFactory.getDevice = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name);
    };
    
    dataFactory.addDevice = function (device) {
        return angharadBackendService.httpRequest("POST", urlBase + "device/", device);
    };

    dataFactory.setDevice = function (device) {
        return angharadBackendService.httpRequest("PUT", urlBase + "device/" + device.name, device);
    };

    dataFactory.removeDevice = function (name) {
        return angharadBackendService.httpRequest("DELETE", urlBase + "device/" + name);
    };
    
    dataFactory.connectDevice = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name + "/connect");
    };

    dataFactory.disconnectDevice = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name + "/disconnect");
    };

    dataFactory.pingDevice = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name + "/ping");
    };

    dataFactory.getDeviceOverview = function (name) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + name + "/overview");
    };

    dataFactory.setElement = function (deviceName, elementType, elementName, elementValue, additionalOptions) {
        return angharadBackendService.httpRequest("GET", urlBase + "device/" + deviceName + "/" + elementType + "/" + elementName + "/" + elementValue, null, additionalOptions);
    };

    dataFactory.updateElement = function (deviceName, elementType, elementName, element) {
        return angharadBackendService.httpRequest("PUT", urlBase + "device/" + deviceName + "/" + elementType + "/" + elementName, element);
    };

    dataFactory.getMonitor = function (deviceName, elementType, elementName, from) {
        return angharadBackendService.httpRequest("GET", urlBase + "monitor/" + deviceName + "/" + elementType + "/" + elementName, null, !!from?{from: from}:null);
    };
    
    dataFactory.addTag = function (deviceName, elementType, elementName, tag) {
        return angharadBackendService.httpRequest("PUT", urlBase + "device/" + deviceName + "/" + elementType + "/" + elementName + "/" + tag);
    };

    dataFactory.removeTag = function (deviceName, elementType, elementName, tag) {
        return angharadBackendService.httpRequest("DELETE", urlBase + "device/" + deviceName + "/" + elementType + "/" + elementName + "/" + tag);
    };

    return dataFactory;
});
