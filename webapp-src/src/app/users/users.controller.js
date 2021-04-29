angular.module("sagremorApp")
    .controller("usersCtrl",
    function($scope, $translate, toaster, DTOptionsBuilder, angharadFactory, sagremorConfirm) {
      
        var self = this;
        
        this.userList = [];
        this.tokenList = [];
        
        this.options = {};
        
        this.newUser = false;
        this.user = {
			login: "",
			password: "",
			enabled: true
		};
		this.usersEnabled = true;
		
		this.searchOptions = {
			login: "",
			enabled: "",
		};
        
        this.init = function () {
			if ($translate.use()) {
				self.options = DTOptionsBuilder.newOptions()
								.withLanguageSource("components/core/i18n/datatables."+$translate.use()+".json");
			}
			self.getUserList();
			self.getTokenList();
		};
		
		this.getUserList = function () {
			angharadFactory.getUserList().then(function (result) {
				self.userList = result;
			}, function (error) {
				if (error.status === 404) {
					self.usersEnabled = false;
				} else {
					toaster.pop("error", $translate.instant("users_get_user_list"), $translate.instant("users_get_user_list_error"));
				}
			});
		};
		
		this.addUser = function () {
			self.newUser = true;
		};
		
		this.cancelNewUser = function () {
			self.newUser = false;
			self.user = {
				login: "",
				password: "",
				enabled: true
			};
		};
		
		this.isNewUserValid = function () {
			var found = _.find(self.userList, {login: self.user.login});
			
			if (found) {
				return false;
			}
			
			return !!self.user.login && self.user.login.length > 0 && !!self.user.password && self.user.password.length > 0;
		};
		
		this.createNewUser = function () {
			angharadFactory.addUser(self.user).then(function () {
				self.userList.push(self.user);
				self.newUser = false;
				self.user = {
					login: "",
					password: "",
					enabled: true
				};
				toaster.pop("success", $translate.instant("users_save_user"), $translate.instant("users_save_user_success"));
			}, function () {
				toaster.pop("error", $translate.instant("users_save_user"), $translate.instant("users_save_user_error"));
			});
		};
		
		this.displayChangePassword = function (user) {
			user.changePassword = true;
		};
		
		this.cancelChangePassword = function (user) {
			user.changePassword = false;
			user.password = "";
		};
		
		this.changePassword = function (user) {
			angharadFactory.setUser(user.login, user).then(function () {
				user.password = "";
				user.changePassword = false;
				toaster.pop("success", $translate.instant("users_save_user"), $translate.instant("users_save_user_success"));
			}, function () {
				toaster.pop("error", $translate.instant("users_save_user"), $translate.instant("users_save_user_error"));
			});
		};
		
		this.enableUser = function (user) {
			angharadFactory.setUser(user.login, {enabled: user.enabled}).then(function () {
				toaster.pop("success", $translate.instant("users_save_user"), $translate.instant("users_save_user_success"));
			}, function () {
				toaster.pop("error", $translate.instant("users_save_user"), $translate.instant("users_save_user_error"));
			});
		};
		
		this.removeUser = function (user) {
			return sagremorConfirm.open($translate.instant("users_user_remove"), $translate.instant("users_user_remove_confirm", {login: user.login})).then(function () {
				return angharadFactory.removeUser(user.login).then(function () {
					_.remove(self.userList, {login: user.login});
					toaster.pop("success", $translate.instant("users_user_remove"), $translate.instant("users_user_remove_success"));
				}, function () {
					toaster.pop("error", $translate.instant("users_user_remove"), $translate.instant("users_user_remove_error"));
				});
			});
		};
		
		this.getTokenList = function () {
			var options = {
				login: self.searchOptions.login
			};
			if (self.searchOptions.enabled === "true") {
				options.enabled = true;
			} else if (self.searchOptions.enabled === "false") {
				options.enabled = false;
			}
			
			angharadFactory.getTokenList(options).then(function (result) {
				self.tokenList = result;
			}, function () {
				toaster.pop("error", $translate.instant("users_get_token_list"), $translate.instant("users_get_token_list_error"));
			});
		};
		
		this.displayToken = function (token) {
			return token.substring(0, 4) + "****-****-****-****-********" + token.substring(32);
		};
		
		this.revokeToken = function (token) {
			return sagremorConfirm.open($translate.instant("users_token_revoke"), $translate.instant("users_token_revoke_confirm")).then(function () {
				return angharadFactory.revokeToken(token.token).then(function () {
					token.enabled = false;
					toaster.pop("success", $translate.instant("users_token_revoke"), $translate.instant("users_token_revoke_success"));
				}, function () {
					toaster.pop("error", $translate.instant("users_token_revoke"), $translate.instant("users_token_revoke_error"));
				});
			});
		};
		
        this.init();
        
		$scope.$on("changeLang", function () {
			self.options = DTOptionsBuilder.newOptions()
							.withLanguageSource("components/core/i18n/datatables."+$translate.use()+".json");
		});
    }
);
