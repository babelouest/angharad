angular.module("sagremorApp")
    .controller("FiltersModalCtrl",
    function($scope, $q, $uibModalInstance, $translate, toaster, DTOptionsBuilder, garethFactory, sharedData) {
        var self = this;
        
        this.filterList = sharedData.all("garethFilters");
        this.AlertSmtpList = sharedData.all("garethAlertsSmtp");
        this.AlertHttpList = sharedData.all("garethAlertsHttp");
        this.editFilter = false;
        this.addFilter = false;
        this.newClause = false;
        this.newAlertSmtp = false;
        this.newAlertHttp = false;
        this.newAlertHttpHeader = false;
        this.displayAlert = false;
        this.customFilter = {
			name: "",
			description: "",
			filter_clauses: [
			],
			filter_alerts: {
				smtp: [],
				http: []
			}
		};
		
		self.menu = [
			{
				name: "smtp", 
				display: $translate.instant("logs_menu_smtp"), 
				action: function () {
					self.addAlertSmtp();
				}
			},
			{
				name: "http", 
				display: $translate.instant("logs_menu_http"), 
				action: function () {
					self.addAlertHttp();
				}
			}
		];
		
		self.menuSelect = function(menuItem, element) {
			menuItem.action(element);
		};
    
		this.customClause = {
			element: "",
			operator: "",
			value: ""
		};
		
		this.customHttpAlert = {
			name: "",
			method: "",
			url: "",
			body: "",
			http_headers: [
			]
		};
		this.customHttpAlertHeader = {
			key: "",
			value: ""
		};
		this.existingHttpAlert = "";
		this.customHttpAlertMethodSelect = "GET";
		
		this.customSmtpAlert = {
			name: "",
			host: "",
			port: 0,
			tls: false,
			check_ca: false,
			user: "",
			password: "",
			from: "",
			to: "",
			cc: "",
			bcc: "",
			subject: "",
			body: ""
		};
		this.existingSmtpAlert = "";
        
        function init() {
        }
        
		$scope.$on("garethChange", function () {
			self.filterList = sharedData.all("garethFilters");
			self.AlertList = sharedData.all("garethAlerts");
		});
        
        this.newFilter = function () {
			self.editFilter = true;
			self.addFilter = true;
			self.customFilter = {
				name: "",
				description: "",
				filter_clauses: [
				],
				filter_alerts: {
					smtp: [],
					http: []
				}
			};
		};
		
		this.addClause = function () {
			self.newClause = true;
		};
		
		this.isClauseValid = function () {
			return !!self.customClause.element && !!self.customClause.operator && !!self.customClause.value;
		};
		
		this.addNewClause = function () {
			if (self.customClause.element === "priority") {
				self.customClause.value = parseInt(self.customClause.value);
			}
			self.customFilter.filter_clauses.push(self.customClause);
			self.customClause = {
				element: "",
				operator: "",
				value: ""
			};
			self.newClause = false;
		};
		
		this.removeClause = function (index){
			self.customFilter.filter_clauses.splice(index, 1);
		};
		
		this.cancelAddClause = function () {
			self.newClause = false;
		};
		
		this.getClauseElement = function (element) {
			return $translate.instant("clause_option_" + element);
		};
        
		this.getClauseOperator = function (operator) {
			switch (operator) {
				case "=":
					return $translate.instant("clause_operator_equal");
					break;
				case "!=":
					return $translate.instant("clause_operator_different");
					break;
				case "<":
					return $translate.instant("clause_operator_lower");
					break;
				case "<=":
					return $translate.instant("clause_operator_lower_equal");
					break;
				case ">":
					return $translate.instant("clause_operator_higher");
					break;
				case ">=":
					return $translate.instant("clause_operator_higher_equal");
					break;
				case "contains":
					return $translate.instant("clause_operator_contains");
					break;
				case "notcontains":
					return $translate.instant("clause_operator_notcontains");
					break;
			}
		};
		
		this.getClauseValue = function (clause) {
			if (clause.element === "priority") {
				switch (clause.value) {
					case 0:
						return $translate.instant("logs_priority_none");
						break;
					case 1:
						return $translate.instant("logs_priority_low");
						break;
					case 2:
						return $translate.instant("logs_priority_medium");
						break;
					case 3:
						return $translate.instant("logs_priority_high");
						break;
					case 4:
						return $translate.instant("logs_priority_critical");
						break;
				}
			} else {
				return clause.value;
			}
		};
		
		this.addAlertSmtp = function () {
			self.newAlertSmtp = true;
		};
		
		this.addAlertHttp = function () {
			self.newAlertHttp = true;
		};
		
		this.isSmtpAlertValid = function () {
			if (!!self.existingSmtpAlert) {
				return true;
			}
			var found = _.find(self.AlertSmtpList, function (alert) {
				return alert.name === self.customSmtpAlert.name;
			});
			if (found) {
				return false;
			}
			return !!self.customSmtpAlert.name && 
					!!self.customSmtpAlert.host && 
					!!self.customSmtpAlert.from && 
					!!self.customSmtpAlert.to && 
					!!self.customSmtpAlert.subject && 
					!!self.customSmtpAlert.body;
		};
		
		this.cancelAddSmtpAlert = function () {
			self.newAlertSmtp = false;
		};
		
		this.postNewSmtpAlert = function () {
			if (!!self.existingSmtpAlert) {
				self.customFilter.filter_alerts.smtp.push(self.existingSmtpAlert.name);
				self.existingSmtpAlert = "";
				self.newAlertSmtp = false;
			} else {
				garethFactory.addAlert("smtp", self.customSmtpAlert).then(function () {
					self.customFilter.filter_alerts.smtp.push(self.customSmtpAlert.name);
					sharedData.add("garethAlertsSmtp", self.customSmtpAlert.name, self.customSmtpAlert);
					toaster.pop("success", $translate.instant("logs_alert"), $translate.instant("logs_alert_save_success"));
					$scope.$broadcast("garethChange");
				}, function () {
					toaster.pop("error", $translate.instant("logs_alert"), $translate.instant("logs_alert_save_error"));
				})["finally"](function () {
					self.customSmtpAlert = {
						name: "",
						host: "",
						port: 0,
						tls: false,
						check_ca: false,
						user: "",
						password: "",
						from: "",
						to: "",
						subject: "",
						body: ""
					};
					self.newAlertSmtp = false;
				});
			}
		};
		
		this.addAlertHttpHeader = function () {
			self.newAlertHttpHeader = true;
		};
		
		this.cancelAddAlertHttpHeader = function () {
			self.newAlertHttpHeader = false;
		};
		
		this.addNewAlertHttpHeader = function () {
			self.customHttpAlert.http_headers.push(self.customHttpAlertHeader);
			self.customHttpAlertHeader = {
				key: "",
				value: ""
			};
			self.newAlertHttpHeader = false;
		};
		
		this.isHttpAlertValid = function () {
			if (!!self.existingHttpAlert) {
				return true;
			}
			var found = _.find(self.AlertHttpList, function (alert) {
				return alert.name === self.customHttpAlert.name;
			});
			if (found) {
				return false;
			}
			return !!self.customHttpAlert.name && 
					(!!self.customHttpAlert.method || !!self.customHttpAlertMethodSelect) && 
					!!self.customHttpAlert.url;
		};
		
		this.postNewHttpAlert = function () {
			if (!!self.existingHttpAlert) {
				self.customFilter.filter_alerts.http.push(self.existingHttpAlert.name);
				self.existingHttpAlert = "";
				self.newAlertHttp = false;
			} else {
				if (self.customHttpAlertMethodSelect !== "custom") {
					self.customHttpAlert.method = self.customHttpAlertMethodSelect;
				}
				garethFactory.addAlert("http", self.customHttpAlert).then(function () {
					self.customFilter.filter_alerts.http.push(self.customHttpAlert.name);
					sharedData.add("garethAlertsHttp", self.customHttpAlert.name, self.customHttpAlert);
					toaster.pop("success", $translate.instant("logs_alert"), $translate.instant("logs_alert_save_success"));
					$scope.$broadcast("garethChange");
				}, function () {
					toaster.pop("error", $translate.instant("logs_alert"), $translate.instant("logs_alert_save_error"));
				})["finally"](function () {
					self.customHttpAlertHeader = {
						key: "",
						value: ""
					};
					self.customHttpAlert = {
						name: "",
						method: "",
						url: "",
						body: "",
						http_headers: [
						]
					};
					self.newAlertHttp = false;
					self.newAlertHttpHeader = false;
				});
			}
		};
		
		this.cancelAddHttpAlert = function () {
			self.newAlertHttp = false;
		};
		
		this.removeAlert = function (type, index) {
			self.customFilter.filter_alerts[type].splice(index, 1);
		};
		
		this.isFilterValid = function () {
			return !!self.customFilter.name && self.customFilter.filter_clauses.length > 0;
		};
		
		this.cancelNewFilter = function () {
			self.editFilter = false;
			self.addFilter = false;
		};
		
		this.postNewFilter = function () {
			var promise = null;
			if (self.addFilter) {
				promise = garethFactory.addFilter(self.customFilter);
			} else {
				promise = garethFactory.setFilter(self.customFilter.name, self.customFilter)
			}
			promise.then(function () {
				sharedData.add("garethFilters", self.customFilter.name, self.customFilter);
				toaster.pop("success", $translate.instant("logs_filter"), $translate.instant("logs_filter_save_success"));
				$scope.$broadcast("garethChange");
			}, function () {
				toaster.pop("error", $translate.instant("logs_filter"), $translate.instant("logs_filter_save_error"));
			})["finally"](function () {
				self.editFilter = false;
				self.addFilter = false;
			});
		};
		
		this.removeCustomFilter = function (filter) {
			garethFactory.removeFilter(filter.name).then(function () {
				sharedData.remove("garethFilters", filter.name);
				toaster.pop("success", $translate.instant("logs_filter"), $translate.instant("logs_filter_remove_success"));
				$scope.$broadcast("garethChange");
			}, function () {
				toaster.pop("error", $translate.instant("logs_filter"), $translate.instant("logs_filter_remove_error"));
			});
		};
		
		this.editCustomFilter = function (filter) {
			self.customFilter = filter;
			self.editFilter = true;
			self.addFilter = false;
		};
		
		this.showAlerts = function () {
			self.displayAlert = true;
		};
		
		this.showFilters = function () {
			self.displayAlert = false;
		};
		
		this.removeSmtpAlert = function (alert) {
			garethFactory.removeAlert("smtp", alert.name).then(function () {
				sharedData.remove("garethAlertsSmtp", alert.name);
				toaster.pop("success", $translate.instant("logs_alert"), $translate.instant("logs_alert_remove_success"));
				$scope.$broadcast("garethChange");
			}, function () {
				toaster.pop("error", $translate.instant("logs_alert"), $translate.instant("logs_alert_remove_error"));
			});
		};
		
		this.removeHttpAlert = function (alert) {
			garethFactory.removeAlert("http", alert.name).then(function () {
				sharedData.remove("garethAlertsHttp", alert.name);
				toaster.pop("success", $translate.instant("logs_alert"), $translate.instant("logs_alert_remove_success"));
				$scope.$broadcast("garethChange");
			}, function () {
				toaster.pop("error", $translate.instant("logs_alert"), $translate.instant("logs_alert_remove_error"));
			});
		};
		
		this.close = function () {
			$uibModalInstance.close("ok");
		};
        
        init();
    }
);
