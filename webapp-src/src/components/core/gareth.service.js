angular.module("sagremorApp")
	.factory("garethFactory", 
	function(angharadConfig, angharadBackendService, sagremorParams) {

	var urlBase = angharadConfig.baseUrl + angharadConfig.prefixGareth;
	var dataFactory = {};

	/* Gareth endpoints */
	dataFactory.getAllMessages = function (limit, offset) {
		return angharadBackendService.httpRequest("GET", urlBase + "message", null, {limit: limit, offset: offset});
	};

	dataFactory.getFilteredMessages = function (filter, limit, offset) {
		return angharadBackendService.httpRequest("GET", urlBase + "message/" + filter, null, {limit: limit, offset: offset});
	};

	/* Filters */
	dataFactory.getFilterList = function () {
		return angharadBackendService.httpRequest("GET", urlBase + "filter");
	};

	dataFactory.getFilter = function (name) {
		return angharadBackendService.httpRequest("GET", urlBase + "filter/" + name);
	};

	dataFactory.addFilter = function (filter) {
		return angharadBackendService.httpRequest("POST", urlBase + "filter/", filter);
	};

	dataFactory.setFilter = function (name, filter) {
		return angharadBackendService.httpRequest("PUT", urlBase + "filter/" + name, filter);
	};

	dataFactory.removeFilter = function (name) {
		return angharadBackendService.httpRequest("DELETE", urlBase + "filter/" + name);
	};

	/* Alerts */
	dataFactory.getAlertList = function () {
		return angharadBackendService.httpRequest("GET", urlBase + "alert");
	};

	dataFactory.getAlert = function (type, name) {
		return angharadBackendService.httpRequest("GET", urlBase + "alert/" + type + "/" + name);
	};

	dataFactory.addAlert = function (type, alert) {
		return angharadBackendService.httpRequest("POST", urlBase + "alert/" + type, alert);
	};

	dataFactory.setAlert = function (type, name, alert) {
		return angharadBackendService.httpRequest("PUT", urlBase + "alert/" + type + "/" + name, alert);
	};

	dataFactory.removeAlert = function (type, name) {
		return angharadBackendService.httpRequest("DELETE", urlBase + "alert/" + type + "/" + name);
	};

	return dataFactory;
});
