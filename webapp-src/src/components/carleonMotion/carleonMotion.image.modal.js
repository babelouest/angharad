angular.module("sagremorApp")
    .controller("carleonMotionImageModalCtrl",
    function($scope, $uibModalInstance, $translate, toaster, carleonMotionFactory, imgArray, index) {
        var self = this;
        
        this.imgArray = imgArray;
        this.index = index;
        this.currentImage = {
			name: "",
			title: "",
			img: "",
			stream: "",
			snapshotUri: ""
		};
        
        function init() {
			self.currentImage = self.imgArray[self.index];
			$scope.$broadcast("carleonMotionImageIframe", self.currentImage);
        }
        
        this.close = function () {
            $uibModalInstance.dismiss("close");
        };
        
        this.displayTitle = function () {
			return $translate.instant("image_title") + self.currentImage.title;
		};
		
		this.changeImage = function (delta) {
			self.index += delta;
			self.currentImage = self.imgArray[self.index];
			$scope.$broadcast("carleonMotionImageIframe", self.currentImage);
		};
		
		this.canPrevious = function () {
			return self.index > 0;
		};
		
		this.canNext = function () {
			return self.index < self.imgArray.length - 1;
		};
		
		this.snapshot = function () {
			carleonMotionFactory.snapshot(self.currentImage.name, self.currentImage.title).then(function () {
				toaster.pop("success", $translate.instant("carleon_motion_snapshot"), $translate.instant("carleon_motion_snapshot_success"));
			}, function () {
				toaster.pop("error", $translate.instant("carleon_motion_snapshot"), $translate.instant("carleon_motion_snapshot_error"));
			});
		};
		
		this.showIndex = function () {
			return (self.index + 1) + " / " + self.imgArray.length;
		};
		
        init();
    }
);
