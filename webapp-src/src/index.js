/**
 * 
 * Angharad front-end application
 * 
 * Copyright 2023 Nicolas Mora <mail@babelouest.org>
 * 
 * License MIT
 * 
 */

import React from 'react';
import { createRoot } from 'react-dom/client';
import i18next from 'i18next';
import Backend from 'i18next-http-backend';
import LanguageDetector from 'i18next-browser-languagedetector';

import App from './Angharad/App';

import storage from './lib/Storage';
import messageDispatcher from './lib/MessageDispatcher';
import apiManager from './lib/APIManager';
import oidcConnector from './lib/OIDCConnector';
import ErrorConfig from './lib/ErrorConfig';

function getServerConfigAngharad(rootUrl) {
  return apiManager.request(rootUrl+"/.well-known/angharad-configuration");
}

function getServerConfigTaliesin(rootUrl) {
  return apiManager.request(rootUrl+"/.well-known/taliesin-configuration");
}

var initApp = () => {
  const container = document.getElementById('root');
  const root = createRoot(container);
  apiManager.request("config.json")
  .then((frontEndConfig) => {
    if (!frontEndConfig.lang) {
      frontEndConfig.lang = ["en","fr"];
    }
    storage.setStorageType(frontEndConfig.storageType);
    let proms = [];
    proms.push(getServerConfigAngharad(frontEndConfig.angharadRootUrl));
    if (frontEndConfig.taliesinRootUrl) {
      proms.push(getServerConfigTaliesin(frontEndConfig.taliesinRootUrl));
    } else {
      proms.push(Promise.reject('no taliesin'));
    }
    Promise.allSettled(proms)
    .then((results) => {
      let backendConfig = results[0];
      let taliesinConfig = results[1];
      if (backendConfig.status === "fulfilled") {
        let oidc = {
          status: "connecting"
        }
        if (!backendConfig.value.oidc) {
          oidc = false;
        }
        let scope = [];
        if (taliesinConfig.status === "fulfilled") {
          scope.push(taliesinConfig.value.oauth_scope_user);
        }
        if (frontEndConfig.oidc.scope) {
          scope.push(frontEndConfig.oidc.scope);
        } else {
          if (backendConfig.value.oauth_scope) {
            scope.push(backendConfig.value.oauth_scope);
          }
        }
        let config = {
          oidc: oidc,
          frontend: frontEndConfig,
          backend: backendConfig.value,
          taliesinConfig: taliesinConfig.value||false,
          angharadEndpoint: frontEndConfig.angharad_endpoint || backendConfig.value.angharad_endpoint,
          benoicEndpoint: frontEndConfig.benoic_endpoint || backendConfig.value.benoic_endpoint,
          carleonEndpoint: frontEndConfig.carleon_endpoint || backendConfig.value.carleon_endpoint,
          scope: scope.join(" ") || false,
          oidc_server_remote_config: frontEndConfig.oidc.oidc_server_remote_config || backendConfig.value.oidc_server_remote_config || false
        };
        let apiConfig = {
          angharadRoot: frontEndConfig.angharadRootUrl,
          angharadApi: config.angharadEndpoint,
          benoicApi: config.benoicEndpoint,
          carleonApi: config.carleonEndpoint,
          taliesinRoot: frontEndConfig.taliesinRootUrl||false,
          taliesinApi: taliesinConfig.value?.api_prefix||false
        };
        oidcConnector.init({
          storagePrefix: "angharadOidc",
          storageType: storage.storageType,
          responseType: storage.getValue("longSession")?"code":"token id_token",
          openidConfigUrl: config.oidc_server_remote_config,
          authUrl: frontEndConfig.oidc.authUrl,
          tokenUrl: frontEndConfig.oidc.tokenUrl,
          clientId: frontEndConfig.oidc.clientId,
          redirectUri: frontEndConfig.oidc.redirectUri,
          usePkce: frontEndConfig.oidc.usePkce,
          scope: config.scope,
          userinfoUrl: frontEndConfig.oidc.userinfoUrl,
          refreshTokenLoop: frontEndConfig.oidc.refreshTokenLoop,
          additionalParameters: frontEndConfig.oidc.additionalParameters,
          changeStatusCb: function (newStatus, token, expires_in, profile) {
            messageDispatcher.sendMessage('OIDC', {status: newStatus, token: token, expires_in: expires_in, profile: profile});
          }
        });
        apiManager.setConfig(apiConfig);
        root.render(<App config={config} />);
      } else {
        root.render(<ErrorConfig message={"Error getting Angharad backend config"}/>);
      }
    });
  })
  .catch((error) => {
    root.render(<ErrorConfig message={"Error getting Angharad ftontend config"}/>);
  });
}

try {
  i18next
  .use(Backend)
  .use(LanguageDetector)
  .init({
    fallbackLng: 'en',
    ns: ['translations'],
    defaultNS: 'translations',
    backend: {
      loadPath: 'locales/{{lng}}/{{ns}}.json'
    }
  })
  .then(() => {
    initApp();
  });
} catch (e) {
  $("#root").html('<div class="alert alert-danger" role="alert">' +
                    '<i class="fas fa-exclamation-triangle"></i>' +
                    '<span class="btn-icon-right">You must use a browser compatible with Angharad</span>' +
                  '</div>');
}
