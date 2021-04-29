function carleonMpdController($scope, $q, $window, $translatePartialLoader, $translate, $interval, angharadConfig, carleonFactory, carleonMpdFactory, benoicFactory, sagremorParams, sharedData, toaster) {
	var ctrl = this;

	this.mpd = {};
	this.mpdElement = {};
	this.playlists = [];
	this.selectedPlaylist = false;
	this._interval = null;
	this.hasFocus = true;

	function init() {
		ctrl.mpdElement = _.find(sharedData.get("carleonServices", "service-mpd").element, function (mpd) {
			return mpd.name === ctrl.element.name;
		});
		
		var promises = [
			carleonMpdFactory.getMpdStatus(ctrl.element.name),
			carleonMpdFactory.getMpdPlaylists(ctrl.element.name)
		];

		$q.all(promises).then(function(responses) {
			ctrl.mpd = responses[0];
			ctrl.playlists = responses[1];
			if (ctrl.mpd.state === "play" || ctrl.mpd.state === "pause") {
				startRefreshStatusInterval();
			}
		}, function() {
			toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_load_error", {
				name: ctrl.element.name
			}));
		});
	}

	function startRefreshStatusInterval() {
		ctrl._interval = $interval(function() {
			if (ctrl.hasFocus) {
				ctrl.refreshStatus();
			}
		}, 1000 * 10);
	}

	this.refreshStatus = function() {
		carleonMpdFactory.getMpdStatus(ctrl.element.name).then(function(response) {
			ctrl.mpd = response;
			if ((ctrl.mpd.state === "play" || ctrl.mpd.state === "pause") && ctrl._interval == null) {
				startRefreshStatusInterval();
			} else if (ctrl._interval != null && ctrl.mpd.state !== "play" && ctrl.mpd.state !== "pause") {
				$interval.cancel(ctrl._interval);
				ctrl._interval = null;
			}
		}, function() {
			toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_load_error", {
				name: ctrl.element.name
			}));
		});
	};

	this.displayVolume = function() {
		return ctrl.mpd.volume + "%";
	};

	this.setVolume = function(delta) {
		var newVolume = ctrl.mpd.volume + delta;
		if (newVolume >= 0 && newVolume <= 100) {
			carleonMpdFactory.setMpdVolume(ctrl.element.name, newVolume).then(function() {
				ctrl.mpd.volume = newVolume;
			}, function() {
				toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_volume_error"));
			});
		}
	};

	this.displayVolumeCommand = function(delta) {
		return (delta > 0 ? "+" : "") + delta + " %"
	};

	this.setAction = function(action) {
		var actionUrl = action;
		if (action === "repeat") {
			actionUrl += ("/" + (ctrl.mpd.repeat?"0":"1"));
			ctrl.mpd.repeat = !ctrl.mpd.repeat;
		}
		if (action === "random") {
			actionUrl += ("/" + (ctrl.mpd.random?"0":"1"));
			ctrl.mpd.random = !ctrl.mpd.random;
		}
		if (action === "play" && ctrl.mpdElement.device && ctrl.mpdElement.switch) {
			benoicFactory.setElement(ctrl.mpdElement.device, "switch", ctrl.mpdElement.switch, "1");
		}
		if (action === "stop" && ctrl.mpdElement.device && ctrl.mpdElement.switch) {
			benoicFactory.setElement(ctrl.mpdElement.device, "switch", ctrl.mpdElement.switch, "0");
		}
		carleonMpdFactory.setMpdAction(ctrl.element.name, actionUrl).then(function() {
			ctrl.refreshStatus();
		}, function() {
			toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_volume_error"));
		});
	};

	this.loadPlaylist = function() {
		if (!!ctrl.selectedPlaylist) {
			if (ctrl.mpdElement.device && ctrl.mpdElement.switch) {
				benoicFactory.setElement(ctrl.mpdElement.device, "switch", ctrl.mpdElement.switch, "1");
			}
			carleonMpdFactory.loadMpdPlaylist(ctrl.element.name, ctrl.selectedPlaylist).then(function() {
				ctrl.refreshStatus();
			}, function() {
				toaster.pop("error", $translate.instant("carleon_mpd"), $translate.instant("carleon_mpd_load_playlist_error"));
			});
		}
	};

	$scope.$on("refreshCarleonServices", function() {
		ctrl.refreshStatus();
	});

	$scope.$on("carleonServicesChanged", function() {
		init();
	});

	$scope.$on("$destroy", function() {
		$interval.cancel(ctrl._interval);
		ctrl._interval = null;
	});

	$window.onblur = function() {
		ctrl.hasFocus = false;
	};

	$window.onfocus = function() {
		ctrl.hasFocus = true;
	};

	init();
}

angular.module("sagremorApp").component("serviceMpd", {
		templateUrl: "components/carleonMpd/carleonMpd.template.html",
		controller: carleonMpdController,
		bindings: {
			element: "="
		}
	})
	.factory("carleonMpdFactory", function($http, $uibModal, $translate, toaster, angharadConfig, angharadBackendService, sagremorConfirm, sharedData) {
		var urlBase = angharadConfig.baseUrl + angharadConfig.prefixCarleon;
		var carleonMpdFactory = {};

		/* Mpd service */
		carleonMpdFactory.getMpdServiceList = function() {
			return angharadBackendService.httpRequest("GET", urlBase + "service-mpd/");
		};

		carleonMpdFactory.getMpdService = function(name) {
			return angharadBackendService.httpRequest("GET", urlBase + "service-mpd/" + name);
		};

		carleonMpdFactory.addMpdService = function(mpdService) {
			return angharadBackendService.httpRequest("POST", urlBase + "service-mpd/", mpdService);
		};

		carleonMpdFactory.setMpdService = function(name, mpdService) {
			return angharadBackendService.httpRequest("PUT", urlBase + "service-mpd/" + name, mpdService);
		};

		carleonMpdFactory.removeMpdService = function(name) {
			return angharadBackendService.httpRequest("DELETE", urlBase + "service-mpd/" + name);
		};

		carleonMpdFactory.getMpdStatus = function(name) {
			return angharadBackendService.httpRequest("GET", urlBase + "service-mpd/" + name + "/status");
		};

		carleonMpdFactory.getMpdPlaylists = function(name) {
			return angharadBackendService.httpRequest("GET", urlBase + "service-mpd/" + name + "/playlists");
		};

		carleonMpdFactory.setMpdAction = function(name, action) {
			return angharadBackendService.httpRequest("PUT", urlBase + "service-mpd/" + name + "/action/" + action);
		};

		carleonMpdFactory.loadMpdPlaylist = function(name, playlist) {
			return angharadBackendService.httpRequest("PUT", urlBase + "service-mpd/" + name + "/playlist/" + playlist);
		};

		carleonMpdFactory.setMpdVolume = function(name, volume) {
			return angharadBackendService.httpRequest("PUT", urlBase + "service-mpd/" + name + "/volume/" + volume);
		};

		carleonMpdFactory.addService = function() {
			var modalInstance = $uibModal.open({
				animation: true,
				templateUrl: "components/carleonMpd/carleonMpd.modal.html",
				controller: "carleonMpdModalCtrl",
				controllerAs: "carleonMpdModalCtrl",
				size: "md",
				resolve: {
					mpd: function() {
						return null;
					}
				}
			});
		};

		carleonMpdFactory.editService = function(service) {
			var modalInstance = $uibModal.open({
				animation: true,
				templateUrl: "components/carleonMpd/carleonMpd.modal.html",
				controller: "carleonMpdModalCtrl",
				controllerAs: "carleonMpdModalCtrl",
				size: "md",
				resolve: {
					mpd: function() {
						return service;
					}
				}
			});
		};

		carleonMpdFactory.removeService = function(service) {
			return sagremorConfirm.open($translate.instant("carleon_mpd_remove"), $translate.instant("carleon_mpd_confirm")).then(function() {
				return carleonMpdFactory.removeMpdService(service.name).then(function() {
					var injector = sharedData.get("carleonServices", "service-mpd");
					_.remove(injector.element, function(element) {
						return element.name === service.name;
					});
					sharedData.add("carleonServices", "service-mpd", injector);
					toaster.pop("success", $translate.instant("carleon_mpd_remove"), $translate.instant("carleon_mpd_remove_success"));
				}, function() {
					toaster.pop("error", $translate.instant("carleon_mpd_remove"), $translate.instant("carleon_mpd_remove_error"));
				});
			});
		};

		return carleonMpdFactory;

	})
	.config(function run($translatePartialLoaderProvider) {
		$translatePartialLoaderProvider.addPart("carleonMpd");
	})
	.run(function(sagGenericInjectorManager, carleonMpdFactory) {
		sagGenericInjectorManager.add({
			type: "service-mpd",
			directive: "service-mpd",
			groupTitle: "carleon_mpd_title",
			service: carleonMpdFactory,
			carleonService: true,
			size: 2,
			leftMenu: {
				title: "carleon_music_title",
				target: "music"
			},
			widgetHeight: 4,
			widgetWidth: 3,
			icon: "music",
			commands: {
				action: {
					title: "carleon_mpd_command_action_title",
					parameters: {
						action: "carleon_mpd_command_action_parameter_action_title",
					},
					result: {
						type: "boolean",
						title: "carleon_mpd_command_action_result_title"
					}
				},
				load_playlist: {
					title: "carleon_mpd_command_playlist_title",
					parameters: {
						playlist: "carleon_mpd_command_playlist_parameter_plsylist_title",
					},
					result: {
						type: "boolean",
						title: "carleon_mpd_command_playlist_result_title"
					}
				},
				set_volume: {
					title: "carleon_mpd_command_volume_title",
					parameters: {
						volume: "carleon_mpd_command_volume_parameter_volume_title",
					},
					result: {
						type: "boolean",
						title: "carleon_mpd_command_volume_result_title"
					}
				}
			}
		});
	});
