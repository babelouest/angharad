import React, { Component } from 'react';

import i18next from 'i18next';

import ConnectButton from './ConnectButton';

import messageDispatcher from '../lib/MessageDispatcher';
import routage from '../lib/Routage';

class TopMenu extends Component {
  constructor(props) {
    super(props);

    this.state = {
      config: props.config,
      oidcStatus: props.oidcStatus,
      deviceOverview: props.deviceOverview,
      serviceList: props.serviceList,
      adminMode: props.adminMode
    };
    
    this.navigateTo = this.navigateTo.bind(this);
    this.navigateToProperties = this.navigateToProperties.bind(this);
    this.setAdminMode = this.setAdminMode.bind(this);
    this.refresh = this.refresh.bind(this);
  }
  
  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  navigateTo(e, route) {
    e.preventDefault();
    messageDispatcher.sendMessage('Navigation', {route: {path: route}});
  }
  
  navigateToProperties(e) {
    e.preventDefault();
    messageDispatcher.sendMessage('Navigation', {route: {path: "properties"}});
  }
  
  setAdminMode(e) {
    e.preventDefault();
    messageDispatcher.sendMessage('Mode', {admin: true});
  }
  
  refresh(e) {
    e.preventDefault();
    messageDispatcher.sendMessage('Component', {status: "refresh"});
  }
  
	render() {
    let hasSwitch = 0, hasDimmer = 0, hasSensor = 0, hasHeater = 0, hasBlind = 0, benoicSubmenu = [<li key={-3}><a className="dropdown-item" href="#map" onClick={(e) => this.navigateTo(e, "map")}>{i18next.t("menu.maps")}</a></li>, <li key={-2}><hr className="dropdown-divider"/></li>, <li key={-1}><a className="dropdown-item" href="#components" onClick={(e) => this.navigateTo(e, "components")}>{i18next.t("menu.all-components-menu")}</a></li>], adminModeJsx = (this.state.adminMode?" active":"");
    Object.keys(this.state.deviceOverview).forEach(name => {
      if (this.state.deviceOverview[name].switches) {
        hasSwitch += Object.keys(this.state.deviceOverview[name].switches).length;
      }
      if (this.state.deviceOverview[name].dimmers) {
        hasDimmer += Object.keys(this.state.deviceOverview[name].dimmers).length;
      }
      if (this.state.deviceOverview[name].sensors) {
        hasSensor += Object.keys(this.state.deviceOverview[name].sensors).length;
      }
      if (this.state.deviceOverview[name].heaters) {
        hasHeater += Object.keys(this.state.deviceOverview[name].heaters).length;
      }
      if (this.state.deviceOverview[name].blinds) {
        hasBlind += Object.keys(this.state.deviceOverview[name].blinds).length;
      }
    });
    if (hasSwitch) {
      benoicSubmenu.push(<li key={0}><a className="dropdown-item" href="#components/switch" onClick={(e) => this.navigateTo(e, "components/switch")}>{i18next.t("menu.switch-list")}</a></li>);
    }
    if (hasDimmer) {
      benoicSubmenu.push(<li key={1}><a className="dropdown-item" href="#components/dimmer" onClick={(e) => this.navigateTo(e, "components/dimmer")}>{i18next.t("menu.dimmer-list")}</a></li>);
    }
    if (hasSensor) {
      benoicSubmenu.push(<li key={2}><a className="dropdown-item" href="#components/sensor" onClick={(e) => this.navigateTo(e, "components/sensor")}>{i18next.t("menu.sensor-list")}</a></li>);
    }
    if (hasHeater) {
      benoicSubmenu.push(<li key={3}><a className="dropdown-item" href="#components/heater" onClick={(e) => this.navigateTo(e, "components/heater")}>{i18next.t("menu.heater-list")}</a></li>);
    }
    if (hasBlind) {
      benoicSubmenu.push(<li key={4}><a className="dropdown-item" href="#components/blind" onClick={(e) => this.navigateTo(e, "components/blind")}>{i18next.t("menu.blind-list")}</a></li>);
    }
    benoicSubmenu.push(<li key={5}><hr className="dropdown-divider"/></li>);
    benoicSubmenu.push(<li key={6}><a className="dropdown-item" href="#services" onClick={(e) => this.navigateTo(e, "services")}>{i18next.t("menu.all-services-menu")}</a></li>);
    this.state.serviceList.forEach((service, index) => {
      if (service.element && service.element.length) {
        benoicSubmenu.push(<li key={"service-"+index}><a className="dropdown-item" href={"#services/"+service.name} onClick={(e) => this.navigateTo(e, "services/"+service.name)}>{i18next.t("menu."+service.name, service.name)}</a></li>);
      }
    });
    benoicSubmenu.push(<li key={"script-sep"}><hr className="dropdown-divider"/></li>);
    benoicSubmenu.push(<li key={"scripts"}><a className="dropdown-item" href="#scripts" onClick={(e) => this.navigateTo(e, "scripts")}>{i18next.t("menu.all-scripts-menu")}</a></li>);
    benoicSubmenu.push(<li key={"schedulers"}><a className="dropdown-item" href="#schedulers" onClick={(e) => this.navigateTo(e, "schedulers")}>{i18next.t("menu.all-schedulers-menu")}</a></li>);
    return (
      <div>
        <a className="close-navbar-toggler collapsed" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
        </a>
        <nav className="navbar navbar-expand-lg nav-pills navbar-light bg-light">
          <div className="container-fluid">
            <a className="navbar-brand"
               href="#"
              data-bs-toggle="collapse"
               onClick={(e) => this.navigateTo(e, false)}>
              Angharad
            </a>
            <button className="navbar-toggler"
                    type="button"
                    data-bs-toggle="collapse"
                    data-bs-target="#angharadTopMenu"
                    aria-controls="angharadTopMenu"
                    aria-expanded="false"
                    aria-label="Toggle navigation">
              <span className="navbar-toggler-icon"></span>
            </button>
            <div className="collapse navbar-collapse" id="angharadTopMenu">
              {this.state.oidcStatus === "connected" ? <ul className="navbar-nav me-auto mb-2 mb-lg-0">
                <li className="nav-item dropdown">
                  <a className="nav-link dropdown-toggle" href="#" role="button" data-bs-toggle="dropdown" aria-expanded="false">
                    {i18next.t("menu.list-menu")}
                  </a>
                  <ul className="dropdown-menu">
                    {benoicSubmenu}
                  </ul>
                </li>
              </ul> : null}
              <ul className="navbar-nav ms-auto flex-nowrap text-right">
                {this.state.oidcStatus === "connected" ? <li className="nav-item">
                  <a className="nav-link" aria-current="page" href="#" onClick={this.refresh} data-bs-toggle="collapse">
                    <i className="fa fa-refresh" aria-hidden="true"></i>
                  </a>
                </li> : null}
                <li className="nav-item dropdown">
                  <a className="nav-link dropdown-toggle" href="#" role="button" data-bs-toggle="dropdown" aria-expanded="false">
                    <i className="fa fa-cog" aria-hidden="true"></i>
                  </a>
                  <ul className="dropdown-menu">
                    {this.state.oidcStatus === "connected" ? <li>
                      <a className={"dropdown-item" + adminModeJsx} href="#" onClick={this.setAdminMode} data-bs-toggle="collapse">
                        {i18next.t("menu.admin")}
                      </a>
                    </li> : null}
                    <li>
                      <a className="dropdown-item" href="#properties" onClick={this.navigateToProperties} data-bs-toggle="collapse">
                        {i18next.t("menu.config")}
                      </a>
                    </li>
                  </ul>
                </li>
                <li className="nav-item">
                  <ConnectButton status={this.state.oidcStatus}/>
                </li>
              </ul>
            </div>
          </div>
        </nav>
      </div>
    );
	}
}

export default TopMenu;
