angular.module("sagremorApp")
	.controller("ErrorCtrl",
	function($scope, sagremorParams) {

		var self = this;

		this.errorMessage = "";

		this.init = function() {
			if (!!sagremorParams.errorMessage) {
				self.errorMessage = sagremorParams.errorMessage;
			}
		};

		$scope.$on("generalError", function () {
			self.init();
		});

		self.init();
	}
);
