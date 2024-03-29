import Cookies from 'js-cookie';

import { createRemoteJWKSet, jwtVerify } from 'jose-browser-runtime';

class OIDCConnector {

  constructor() {
  }

  init(parameters) {
    // internal
    this.refreshToken = false;
    this.accessToken = false;
    this.idToken = false;
    this.changeStatusCb = [];
    this.connected = false;
    this.parameters = {};
    this.jwks = false;
    this.pkce = false;
    if (window.location.pathname !== "/") {
      this.localStorageKey = parameters.storagePrefix + "-" + window.btoa(unescape(encodeURIComponent(window.location.pathname))).replace(/\=+$/m,'');
    } else {
      this.localStorageKey = parameters.storagePrefix;
    }
    this.refreshTimeout = false;

    if (parameters) {
      this.parameters.storageType = parameters.storageType || "none";
      this.parameters.responseType = parameters.responseType || "code";
      this.parameters.required_scope = parameters.required_scope || false;
      this.parameters.scope = parameters.scope || "";
      this.parameters.openidConfigUrl = parameters.openidConfigUrl || "";
      this.parameters.authUrl = parameters.authUrl || "";
      this.parameters.tokenUrl = parameters.tokenUrl || "";
      this.parameters.clientId = parameters.clientId || "";
      this.parameters.clientPassword = parameters.clientPassword || "";
      this.parameters.redirectUri = parameters.redirectUri || "";
      this.parameters.userinfoUrl = parameters.userinfoUrl || "";
      this.parameters.usePkce = !!parameters.usePkce;
      this.parameters.refreshTokenLoop = !!parameters.refreshTokenLoop;
      this.parameters.additionalParameters = parameters.additionalParameters || false;
      if (parameters.changeStatusCb) {
        this.changeStatusCb.push(parameters.changeStatusCb);
      }
    }

    if (this.parameters.openidConfigUrl) {
      $.ajax({
        type: "GET",
        url: this.parameters.openidConfigUrl,
        success: (result) => {
          this.parameters.authUrl = result.authorization_endpoint;
          this.parameters.tokenUrl = result.token_endpoint;
          this.parameters.userinfoUrl = result.userinfo_endpoint;
          if (result.jwks_uri) {
            this.jwks = createRemoteJWKSet(new URL(result.jwks_uri));
          }
          this.parseInitialUrl();
        },
        error: (error) => {
          this.broadcastMessage("error");
        }
      });
    } else {
      this.parseInitialUrl();
    }
  }

  setParameter(param, value) {
    this.parameters[param] = value;
  }
  
  getParameter(param) {
    if (this.parameters[param]) {
      return this.parameters[param];
    } else {
      return false;
    }
  }
  
  parseInitialUrl() {
    var storedData;
    var token;
    if (this.parameters.responseType === "code") {
      var code = this.getCodeFromQuery();
      if (code) {
        this.getRefreshTokenFromCode(code, (refreshToken) => {
          if (refreshToken) {
            this.refreshToken = refreshToken.refresh_token;
            this.storeRefreshToken(this.refreshToken);
            if (refreshToken.access_token) {
              this.accessToken = {access_token: refreshToken.access_token, iat: refreshToken.iat, expires_in: parseInt(refreshToken.expires_in)};
              this.storeAccessToken(this.accessToken);
              if (refreshToken.id_token) {
                this.idToken = refreshToken.id_token;
                this.storeIDToken(this.idToken);
              }
              this.broadcastMessage("connected", this.accessToken.access_token, this.accessToken.expires_in);
              this.getConnectedProfile((res, profile) => {
                if (res) {
                  this.broadcastMessage("profile", null, null, profile);
                } else {
                  this.broadcastMessage("profile", null, null, false);
                }
              });
              this.refreshTokenLoop(refreshToken.refresh_token, this.accessToken.expires_in);
            } else {
              this.broadcastMessage("error");
            }
          } else {
            this.broadcastMessage("error");
          }
        });
        window.history.pushState(null, "", document.location.href.split("?")[0]);
      } else {
        storedData = this.getStoredData();
        if (storedData && storedData.accessToken && this.isTokenValid(storedData.accessToken)) {
          this.accessToken = storedData.accessToken;
          var curDate = new Date();
          let expires_in = Math.floor((((this.accessToken.iat + this.accessToken.expires_in)*1000) - curDate.getTime())/1000);
          this.broadcastMessage("connected", this.accessToken.access_token, expires_in);
          this.getConnectedProfile((res, profile) => {
            if (res) {
              this.broadcastMessage("profile", null, null, profile);
            } else {
              this.broadcastMessage("profile", null, null, false);
            }
          });
          if (storedData.refreshToken) {
            var curDate = new Date();
            var timeout = Math.floor(((this.accessToken.iat + this.accessToken.expires_in)*1000 - curDate.getTime())/1000);
            this.refreshTokenLoop(storedData.refreshToken, timeout);
          }
        } else if (storedData && storedData.refreshToken) {
          this.accessToken = false;
          this.refreshToken = storedData.refreshToken;
          this.storeAccessToken(false);
          this.executeRefreshToken(storedData.refreshToken, (result, accessToken) => {
            if (result) {
              var curDate = new Date();
              this.accessToken = accessToken;
              this.storeAccessToken(accessToken);
              let expires_in = Math.floor((((accessToken.iat + accessToken.expires_in)*1000) - curDate.getTime())/1000);
              this.refreshTokenLoop(storedData.refreshToken, expires_in);
              this.broadcastMessage("connected", accessToken.access_token, expires_in);
              this.getConnectedProfile((res, profile) => {
                if (res) {
                  this.broadcastMessage("profile", null, null, profile);
                } else {
                  this.broadcastMessage("profile", null, null, false);
                }
              });
            } else {
              this.broadcastMessage("error");
              this.storeIDToken(false);
            }
          });
        } else {
          this.broadcastMessage("disconnected");
        }
      }
    } else {
      if (this.parameters.responseType.search("token") > -1) {
        token = this.getAccessTokenFromFragment();
        if (token) {
          this.accessToken = token;
          this.storeAccessToken(token);
          this.broadcastMessage("connected", this.accessToken.access_token, this.accessToken.expires_in);
        } else {
          storedData = this.getStoredData();
          if (storedData && storedData.accessToken && this.isTokenValid(storedData.accessToken)) {
            var curDate = new Date();
            this.accessToken = storedData.accessToken;
            let expires_in = Math.floor((((storedData.accessToken.iat + storedData.accessToken.expires_in)*1000) - curDate.getTime())/1000);
            this.broadcastMessage("connected", this.accessToken.access_token, expires_in);
          } else {
            this.broadcastMessage("error");
            this.accessToken = false;
            this.storeAccessToken(false);
            this.storeIDToken(false);
          }
        }
      }
      if (this.parameters.responseType.search("id_token") > -1) {
        var idToken = this.getQueryParams(document.location.hash).id_token
        if (idToken) {
          this.idToken = this.getQueryParams(document.location.hash).id_token;
          this.storeIDToken(this.idToken);
        } else {
          storedData = this.getStoredData();
          if (storedData && storedData.IDToken) {
            this.idToken = storedData.IDToken;
          } else {
            this.idToken = false;
          }
        }
      }
      this.getConnectedProfile((res, profile) => {
        if (res) {
          this.broadcastMessage("profile", null, null, profile);
        } else {
          this.broadcastMessage("profile", null, null, false);
        }
      });
    }
  }

  getQueryParams(qs) {
    qs = qs.split('+').join(' ');

    var params = {},
      tokens,
      re = /[#&]?([^=]+)=([^&]*)/g;

    tokens = re.exec(qs);
    while (tokens) {
      params[decodeURIComponent(tokens[1])] = decodeURIComponent(tokens[2]);
      tokens = re.exec(qs);
    }

    return params;
  }

  getAccessTokenFromFragment() {
    var params = this.getQueryParams(document.location.hash);
    var curDate = new Date();
    if (params.access_token && params.expires_in)  {
      return {access_token: params.access_token, expires_in: parseInt(params.expires_in), iat: Math.floor(curDate.getTime()/1000)};
    } else {
      return false;
    }
  }

  getCodeFromQuery() {
    var params = this.getQueryParams(document.location.search.substring(1));
    return params.code||false;
  }

  refresh(cb) {
    if (this.parameters.responseType === "code" && this.refreshToken) {
      this.executeRefreshToken(this.refreshToken, (result, accessToken) => {
        if (result) {
          this.accessToken = accessToken;
          this.storeAccessToken(accessToken);
          this.refreshTokenLoop(this.refreshToken, this.accessToken.expires_in);
          this.broadcastMessage("refresh", accessToken.access_token, accessToken.expires_in);
          cb(accessToken.access_token);
        }
      });
    } else {
      cb(false);
    }
  }

  getStatus() {
    if (this.accessToken && this.isTokenValid(this.accessToken)) {
      return "connected";
    } else {
      this.accessToken = false;
      return "disconnected";
    }
  }

  storeAccessToken(token) {
    var storedObject = this.getStoredData();
    if (!storedObject) {
      storedObject = {};
    }
    storedObject.accessToken = token;

    if (this.parameters.storageType === "local") {
      return localStorage.setItem(this.localStorageKey, JSON.stringify(storedObject));
    } else if (this.parameters.storageType === "cookie") {
      return Cookies.set(this.localStorageKey, JSON.stringify(storedObject));
    } else {
      return false;
    }
  }

  storeIDToken(token) {
    var storedObject = this.getStoredData();
    if (!storedObject) {
      storedObject = {};
    }
    storedObject.IDToken = token;

    if (this.parameters.storageType === "local") {
      return localStorage.setItem(this.localStorageKey, JSON.stringify(storedObject));
    } else if (this.parameters.storageType === "cookie") {
      return Cookies.set(this.localStorageKey, JSON.stringify(storedObject));
    } else {
      return false;
    }
  }

  storeRefreshToken(token) {
    var storedObject = this.getStoredData();
    if (!storedObject) {
      storedObject = {};
    }
    storedObject.refreshToken = token;

    if (this.parameters.storageType === "local") {
      return localStorage.setItem(this.localStorageKey, JSON.stringify(storedObject));
    } else if (this.parameters.storageType === "cookie") {
      return Cookies.set(this.localStorageKey, JSON.stringify(storedObject));
    } else {
      return false;
    }
  }

  storeNonce(nonce) {
    var storedObject = this.getStoredData();
    if (!storedObject) {
      storedObject = {};
    }
    storedObject.nonce = nonce;

    if (this.parameters.storageType === "local") {
      return localStorage.setItem(this.localStorageKey, JSON.stringify(storedObject));
    } else if (this.parameters.storageType === "cookie") {
      return Cookies.set(this.localStorageKey, JSON.stringify(storedObject));
    } else {
      return false;
    }
  }

  storePkce(pkce) {
    var storedObject = this.getStoredData();
    if (!storedObject) {
      storedObject = {};
    }
    storedObject.pkce = pkce;

    if (this.parameters.storageType === "local") {
      return localStorage.setItem(this.localStorageKey, JSON.stringify(storedObject));
    } else if (this.parameters.storageType === "cookie") {
      return Cookies.set(this.localStorageKey, JSON.stringify(storedObject));
    } else {
      return false;
    }
  }

  makeRandomString(length) {
    var array            = new Uint8Array(length);
    var result           = '';
    var characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    var charactersLength = characters.length;
    window.crypto.getRandomValues(array);
    for ( var i = 0; i < length; i++ ) {
      while (array[i] >= charactersLength) {
        var oneValue = new Uint8Array(1);
        window.crypto.getRandomValues(oneValue);
        array[i] = oneValue[0];
      }
      result += characters.charAt(array[i]);
   }
   return result;
  }

  getStoredData() {
    var storage;
    if (this.parameters.storageType === "local") {
      storage = JSON.parse(localStorage.getItem(this.localStorageKey));
      if (storage) {
        return storage;
      } else {
        return {};
      }
    } else if (this.parameters.storageType === "cookie") {
      try {
        storage = JSON.parse(Cookies.get(this.localStorageKey));
        if (storage) {
          return storage;
        } else {
          return {};
        }
      } catch (err) {
        return {};
      }
    } else {
      return {};
    }
  }

  onChangeStatus(cb) {
    this.changeStatusCb.push(cb);
  }

  isTokenValid(token) {
    if (!!token) {
      var curDate = new Date();
      return ((token.iat + token.expires_in)*1000) > curDate.getTime();
    } else {
      return false;
    }
  }

  getRefreshTokenFromCode(code, cb) {
    var data = {grant_type: "authorization_code", client_id: this.parameters.clientId, redirect_uri: this.parameters.redirectUri, code: code};
    var storedData = this.getStoredData();
    if (this.parameters.usePkce && storedData.pkce) {
      data.code_verifier = storedData.pkce;
    }
    $.ajax({
      type: "POST",
      url: this.parameters.tokenUrl,
      data: data,
      success: (result, status, request) => {
        let scopes = result.scope.split(" ");
        if (!this.parameters.required_scope || scopes.indexOf(this.parameters.required_scope) > -1) {
          cb(result);
        } else {
          cb(false);
        }
      },
      error: (error) => {
        if (error.status === 403) {
          this.refreshToken = false;
        }
        this.accessToken = false;
        cb(false);
      }
    });
  }

  refreshTokenLoop(refreshToken, timeout) {
    if (this.parameters.refreshTokenLoop) {
      clearTimeout(this.refreshTimeout);
      this.refreshTimeout = setTimeout(() => {
        this.executeRefreshToken(refreshToken, (res, token) => {
          if (res) {
            var curDate = new Date();
            var timeout = Math.floor(((token.iat + token.expires_in)*1000 - curDate.getTime())/1000);
            this.refreshTokenLoop(refreshToken, timeout);
          }
        });
      }, (timeout - 60)*1000);
    }
  }

  runRefreshToken(cb) {
    if (this.getStoredData().refreshToken) {
      return this.executeRefreshToken(this.getStoredData().refreshToken, cb);
    } else {
      return $.Deferred().reject("disconnected");
    }
  }

  executeRefreshToken(refreshToken, cb) {
    return $.ajax({
      type: "POST",
      url: this.parameters.tokenUrl,
      data: {grant_type: "refresh_token", refresh_token: refreshToken},
      success: (result, status, request) => {
        this.accessToken = result.access_token;
        this.storeAccessToken(result);
        this.broadcastMessage("refresh", result.access_token, result.expires_in);
        if (cb) {
          cb(true, result);
        }
      },
      error: (error) => {
        if (error.status === 403 || error.status === 400) {
          this.refreshToken = false;
        }
        this.accessToken = false;
        this.storeAccessToken(false);
        this.storeRefreshToken(false);
        if (error.readyState === 0) {
          this.broadcastMessage("network error");
        } else {
          this.broadcastMessage("disconnected");
        }
        if (cb) {
          cb(false);
        }
      }
    })
    .then((result) => {
      return {token: result.access_token, expires_in: result.expires_in};
    });
  }

  broadcastMessage(status, token, expiration, profile) {
    for (var i in this.changeStatusCb) {
      this.changeStatusCb[i](status, token, expiration, profile);
    }
  }

  getConnectedProfile(cb) {
    if (this.getStoredData().IDToken && this.jwks) {
      jwtVerify(this.getStoredData().IDToken, this.jwks)
      .then((result) => {
        if (cb) {
          cb(true, result.payload);
        }
      })
      .catch((err) => {
        this.storeIDToken(false);
        if (err.code === "ERR_JWT_EXPIRED") {
          $.ajax({
            type: "GET",
            url: this.parameters.userinfoUrl,
            headers: {"Authorization": "Bearer " + this.accessToken.access_token},
            success: (result) => {
              if (cb) {
                cb(true, result);
              }
            },
            error: (error) => {
              this.accessToken = false;
              this.storeAccessToken(false);
              this.storeRefreshToken(false);
              if (cb) {
                cb(true, "error");
              }
            }
          });
        } else {
          this.accessToken = false;
          this.storeAccessToken(false);
          this.storeRefreshToken(false);
          if (cb) {
            cb(true, "error");
          }
        }
      });
    } else if (this.parameters.userinfoUrl && this.accessToken) {
      $.ajax({
        type: "GET",
        url: this.parameters.userinfoUrl,
        headers: {"Authorization": "Bearer " + this.accessToken.access_token},
        success: (result) => {
          if (cb) {
            cb(true, result);
          }
        },
        error: (error) => {
          this.accessToken = false;
          this.storeAccessToken(false);
          this.storeRefreshToken(false);
          this.storeIDToken(false);
          if (cb) {
            cb(true, "error");
          }
        }
      });
    } else {
      if (cb) {
        cb(false);
      }
    }
  }

  connect() {
    this.storeAccessToken(false);
    this.storeRefreshToken(false);
    this.storeIDToken(false);
    let token = this.getStoredData();
    if (token && this.isTokenValid(token.accessToken)) {
      var curDate = new Date();
      let expires_in = Math.floor((((token.iat + token.expires_in)*1000) - curDate.getTime())/1000);
      this.broadcastMessage("connected", token.accessToken.access_token, expires_in);
      this.getConnectedProfile((res, profile) => {
        if (res) {
          this.broadcastMessage("profile", null, null, profile);
        } else {
          this.broadcastMessage("profile", null, null, false);
        }
      });
    } else {
      token.accessToken = false;
      let nonce = this.makeRandomString(16), additionalParameters = "";
      this.storeAccessToken(false);
      this.storeNonce(nonce);
      if (this.parameters.additionalParameters) {
        additionalParameters  = "&" + this.parameters.additionalParameters;
      }
      if (this.parameters.usePkce && this.parameters.responseType === "code") {
        let pkce = this.makeRandomString(64);
        this.storePkce(pkce);
        const encoder = new TextEncoder();
        let scope = this.parameters.scope;
        if (scope.search("openid") === -1) {
          scope += " openid";
        }
        crypto.subtle.digest("SHA-256", encoder.encode(pkce))
        .then(pkceHashed => {
          let pkceHashedB64 = this.base64UrlArrayBuffer(pkceHashed);
          document.location = this.parameters.authUrl + "?response_type=" + this.parameters.responseType + "&client_id=" + this.parameters.clientId + "&redirect_uri=" + this.parameters.redirectUri + "&scope=" + scope + "&nonce=" + nonce + additionalParameters + "&code_challenge_method=S256&code_challenge=" + pkceHashedB64;
        });
      } else {
        let scope = this.parameters.scope;
        if (this.parameters.responseType.search("id_token") > -1 && scope.search("openid") === -1) {
          scope += " openid";
        }
        document.location = this.parameters.authUrl + "?response_type=" + this.parameters.responseType + "&client_id=" + this.parameters.clientId + "&redirect_uri=" + this.parameters.redirectUri + "&scope=" + scope + additionalParameters + "&nonce=" + nonce;
      }
    }
  }

  disconnect() {
    clearTimeout(this.refreshTimeout);
    if (this.parameters.storageType === "local") {
      localStorage.removeItem(this.localStorageKey);
    } else if (this.parameters.storageType === "cookie") {
      Cookies.remove(this.localStorageKey);
    }
    this.refreshToken = false;
    this.accessToken = false;
    this.storeAccessToken(false);
    this.storeRefreshToken(false);
    this.storeIDToken(false);
    this.broadcastMessage("disconnected");
  }

  /**
   * base64UrlArrayBuffer
   *
   * MIT LICENSE
   *
   * Copyright 2011 Jon Leighton
   * Copyright 2021 Nicolas Mora
   *
   * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
   * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
   * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
   * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
   * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
   * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
   * OR OTHER DEALINGS IN THE SOFTWARE.
  */
  base64UrlArrayBuffer(arrayBuffer) {
    var base64    = ''
    var encodings = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_'

    var bytes         = new Uint8Array(arrayBuffer)
    var byteLength    = bytes.byteLength
    var byteRemainder = byteLength % 3
    var mainLength    = byteLength - byteRemainder

    var a, b, c, d
    var chunk

    // Main loop deals with bytes in chunks of 3
    for (var i = 0; i < mainLength; i = i + 3) {
      // Combine the three bytes into a single integer
      chunk = (bytes[i] << 16) | (bytes[i + 1] << 8) | bytes[i + 2]

      // Use bitmasks to extract 6-bit segments from the triplet
      a = (chunk & 16515072) >> 18 // 16515072 = (2^6 - 1) << 18
      b = (chunk & 258048)   >> 12 // 258048   = (2^6 - 1) << 12
      c = (chunk & 4032)     >>  6 // 4032     = (2^6 - 1) << 6
      d = chunk & 63               // 63       = 2^6 - 1

      // Convert the raw binary segments to the appropriate ASCII encoding
      base64 += encodings[a] + encodings[b] + encodings[c] + encodings[d]
    }

    // Deal with the remaining bytes and padding
    if (byteRemainder == 1) {
      chunk = bytes[mainLength]

      a = (chunk & 252) >> 2 // 252 = (2^6 - 1) << 2

      // Set the 4 least significant bits to zero
      b = (chunk & 3)   << 4 // 3   = 2^2 - 1

      base64 += encodings[a] + encodings[b]
    } else if (byteRemainder == 2) {
      chunk = (bytes[mainLength] << 8) | bytes[mainLength + 1]

      a = (chunk & 64512) >> 10 // 64512 = (2^6 - 1) << 10
      b = (chunk & 1008)  >>  4 // 1008  = (2^6 - 1) << 4

      // Set the 2 least significant bits to zero
      c = (chunk & 15)    <<  2 // 15    = 2^4 - 1

      base64 += encodings[a] + encodings[b] + encodings[c]
    }

    return base64
  }
}

let oidcConnector = new OIDCConnector();

export default oidcConnector;
