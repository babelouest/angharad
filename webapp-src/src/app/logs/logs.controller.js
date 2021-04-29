angular.module("sagremorApp")
    .controller("logsCtrl",
    function($scope, $translate, $uibModal, toaster, DTOptionsBuilder, garethFactory, sharedData) {
      
        var self = this;
        
        this.messageList = [];
        this.filterList = [];
        this.options = {};
        
        this.searchOptions = {
			limit: 20,
			offset: 0
		};
		
		$scope.$on("garethChange", function () {
			self.getFilters();
			self.search();
		});
        
        this.init = function () {
			if ($translate.use()) {
				self.options = DTOptionsBuilder.newOptions()
								.withLanguageSource("components/core/i18n/datatables."+$translate.use()+".json")
								.withOption("order", [1, "desc"]);
				self.getFilters();
				self.search();
			}
		};
		
		
		this.getPriority = function (message) {
			switch (message.priority) {
				case "NONE":
					return $translate.instant("logs_priority_none");
					break;
				case "LOW":
					return $translate.instant("logs_priority_low");
					break;
				case "MEDIUM":
					return $translate.instant("logs_priority_medium");
					break;
				case "HIGH":
					return $translate.instant("logs_priority_high");
					break;
				case "CRITICAL":
					return $translate.instant("logs_priority_critical");
					break;
				default:
					return $translate.instant("logs_priority_not_found");
					break;
			}
		};
		
		this.search = function () {
			if (self.searchOptions.limit > 0 && self.searchOptions.offset >= 0) {
				if (!!self.searchOptions.filter) {
					garethFactory.getFilteredMessages(self.searchOptions.filter.name, self.searchOptions.limit, self.searchOptions.offset).then(function (result) {
						self.messageList = result;
					});
				} else {
					garethFactory.getAllMessages(self.searchOptions.limit, self.searchOptions.offset).then(function (result) {
						self.messageList = result;
					});
				}
			} else {
				toaster.pop("error", $translate.instant("logs_search"), $translate.instant("logs_search_error"));
			}
		};
		
		this.getFilters = function () {
			self.filterList = sharedData.all("garethFilters");
		};
		
		this.filterManagement = function () {
			var modalInstance = $uibModal.open({
				animation: true,
				templateUrl: "app/logs/filters.modal.html",
				controller: "FiltersModalCtrl",
				controllerAs: "FiltersModalCtrl",
				size: "md",
			});
		};
		
		$scope.$on("changeLang", function () {
			self.options = DTOptionsBuilder.newOptions()
							.withLanguageSource("components/core/i18n/datatables."+$translate.use()+".json");
		});
		
        this.init();
        
    }
);
