angular.module("sagremorApp")
    .controller("carleonMotionModalCtrl", 
    function($rootScope, $uibModalInstance, $translate, sharedData, toaster, carleonMotionFactory, motion) {
        var self = this;

        this.messages = {};
        this.add = true;
        this.addFileList = false;
        this.addStream = false;
        this.newFileList = {};
        this.newStream = {};

        function init() {
			if (!!motion) {
				self.motion = _.find(sharedData.get("carleonServices", "service-motion").element, function (elt) {
					return elt.name === motion.name;
				});
				self.add = false;
			} else {
				self.motion = {
					file_list: [],
					stream_list: []
				};
			}
        }
        
        this.addNewFileList = function () {
			self.addFileList = true;
		};
		
		this.removeFileList = function (index) {
			self.motion.file_list.splice(index, 1);
		};
		
		this.saveNewFileList = function () {
			self.motion.file_list.push(self.newFileList);
			self.newFileList = {};
			self.addFileList = false;
		};
		
		this.cancelNewFileList = function () {
			self.newFileList = {};
			self.addFileList = false;
		};
		
		this.isFileListValid = function () {
			return !!self.newFileList.name && !!self.newFileList.path && !!self.newFileList.thumbnail_path;
		};
		
		this.addNewStream = function () {
			self.addStream = true;
		};
        
		this.removeStream = function (index) {
			self.motion.stream_list.splice(index, 1);
		};
		
		this.saveNewStream = function () {
			self.motion.stream_list.push(self.newStream);
			self.newStream = {};
			self.addStream = false;
		};
		
		this.cancelNewStream = function () {
			self.newStream = {};
			self.addStream = false;
		};
		
		this.isStreamValid = function () {
			return !!self.newStream.name && !!self.newStream.uri;
		};
		
		this.isCameraValid = function () {
			var found = _.find(sharedData.get("carleonServices", "service-motion").element, function (element) {
				return element.name === self.motion.name;
			});
			return (!found || !self.add) && !!self.motion.name && !!self.motion.config_uri;
		};
		
        this.cancel = function () {
			$uibModalInstance.dismiss("close");
		};

        this.save = function () {
			if (self.add) {
				carleonMotionFactory.addMotionService(self.motion).then(function () {
					self.motion.type = "service-motion";
					self.motion.service = carleonMotionFactory;
					sharedData.get("carleonServices", "service-motion").element.push(self.motion);
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_motion_save"), $translate.instant("carleon_motion_save_success"));
				}, function () {
					toaster.pop("error", $translate.instant("carleon_motion_save"), $translate.instant("carleon_motion_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			} else {
				carleonMotionFactory.setMotionService(self.motion.name, self.motion).then(function () {
					self.motion.type = "service-motion";
					self.motion.service = carleonMotionFactory;
					$rootScope.$broadcast("carleonServicesChanged");
					toaster.pop("success", $translate.instant("carleon_motion_save"), $translate.instant("carleon_motion_save_success"));
				}, function () {
					toaster.pop("error", $translate.instant("carleon_motion_save"), $translate.instant("carleon_motion_save_error"));
				})["finally"](function () {
					$uibModalInstance.dismiss("close");
				});
			}
		};

        init();
    }
);
