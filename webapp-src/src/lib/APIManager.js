import * as jose from 'jose-browser-runtime';

import messageDispatcher from './MessageDispatcher';
import oidcConnector from './OIDCConnector';
import i18next from 'i18next';

class APIManager {
  constructor() {
    this.apiPrefix = "";
    this.token = "";
    this.token_expires_at = 0;
    this.signJwk = false;
    this.counter = 0;
    this.config = {
      angharadRoot: "",
      angharadApi: "",
      benoicApi: "",
      carleonApi: "",
      taliesinRoot: "",
      taliesinApi: ""
    };
  }

  setConfig(config) {
    Object.assign(this.config, config);
  }
  
  setToken(token, token_expires_at) {
    this.token = token;
    this.token_expires_at = token_expires_at;
  }
  
  setSignJwk(signJwk) {
    this.signJwk = signJwk;
  }
  
  getConfig(apiPrefix) {
    return this.apiPrefix;
  }

	request(url, method="GET", data=false, accept="application/json; charset=utf-8") {
		if (this.counter <= 100) {
			this.counter++;
			let curDate = new Date();
			if (!this.token || this.token_expires_at*1000 > curDate.getTime()) {
        this.counter--;
				return this.APIRequestExecute(url, method, data, accept);
			} else {
        this.counter--;
        return oidcConnector.runRefreshToken()
        .then((res) => {
          this.token = res.token;
          this.token_expires_at = (curDate.getTime()/1000)+res.expires_in;
          return this.APIRequestExecute(url, method, data, accept);
        })
        .catch(() => {
          messageDispatcher.sendMessage('App', {action: "sessionTimeout"});
        });
			}
		} else {
			return $.Deferred().reject("error too busy");
		}
	}
	
	APIRequestExecute(url, method, data, accept, retry = true) {
    let headers = {
      Authorization: "Bearer " + this.token,
      accept: accept
    };
    let contentType = null;
    let jsonData = !!data?JSON.stringify(data):null;
    if (data) {
      contentType = "application/json; charset=utf-8";
    }
		return $.ajax({
			method: method,
			url: url,
			data: jsonData,
			contentType: contentType,
			headers: headers
		})
    .fail((err) => {
      if (err.status === 401) {
        if (retry) {
          oidcConnector.runRefreshToken();
          return $.Deferred().reject(err);
        } else {
          messageDispatcher.sendMessage('App', {action: "sessionTimeout"});
          return $.Deferred().reject('disconnected');
        }
      } else {
        return $.Deferred().reject(err);
      }
    });
	}
	
  APIRequestAngharad(url, method, data, accept) {
    return this.APIRequestExecute(this.config.angharadRoot + "/" + this.config.angharadApi + "/" + url, method, data, accept);
  }
	
  APIRequestBenoic(url, method, data, accept) {
    return this.APIRequestExecute(this.config.angharadRoot + "/" + this.config.benoicApi + "/" + url, method, data, accept);
  }
	
  APIRequestCarleon(url, method, data, accept) {
    return this.APIRequestExecute(this.config.angharadRoot + "/" + this.config.carleonApi + "/" + url, method, data, accept);
  }
	
  APIRequestTaliesin(url, method, data, accept) {
    return this.APIRequestExecute(this.config.taliesinRoot + "/" + this.config.taliesinApi + "/" + url, method, data, accept);
  }
}

let apiManager = new APIManager();

export default apiManager;
