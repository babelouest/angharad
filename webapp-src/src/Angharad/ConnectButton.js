import React, { Component } from 'react';

import i18next from 'i18next';

import oidcConnector from '../lib/OIDCConnector';

class ConnectButton extends Component {
  constructor(props) {
    super(props);

    this.state = {
      status: props.status
    };
  }
  
  static getDerivedStateFromProps(props, state) {
    return props;
  }

  logOut() {
    oidcConnector.disconnect();
  }
  
  logIn() {
    oidcConnector.connect();
  }

	render() {
    if (!this.state.status) {
      return <div className="nav-link">{i18next.t("common.connected")}</div>;
    } else if (this.state.status === "connecting") {
      return <div className="nav-link">{i18next.t("common.connecting")}</div>;
    } else if (this.state.status === "connected") {
      return <a className="nav-link" onClick={this.logOut} href="#">{i18next.t("common.log-out")}</a>;
    } else {
      return <a className="nav-link" onClick={this.logIn} href="#">{i18next.t("common.log-in")}</a>;
    }
	}
}

export default ConnectButton;
