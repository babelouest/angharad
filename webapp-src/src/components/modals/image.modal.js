angular.module("sagremorApp")
    .controller("ImageModalCtrl",
    function($scope, $uibModalInstance, $translate, title, src) {
        var self = this;
        
        this.title = title;
        this.src = src;
        
        function init() {
        }
        
        this.close = function () {
            $uibModalInstance.dismiss("close");
        };
        
        this.displayTitle = function () {
			return $translate.instant("image_title") + self.title;
		};
		
        init();
    }
);
