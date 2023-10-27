import React, { Component } from 'react';

import i18next from 'i18next';

import ConnectButton from './ConnectButton';

import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';
import routage from '../lib/Routage';
import storage from '../lib/Storage';
import oidcConnector from '../lib/OIDCConnector';

import ModalConfirm from './ModalConfirm';
import LangDropdown from './LangDropdown';
import ModalDevice from './ModalDevice';

class Config extends Component {
  constructor(props) {
    super(props);
    this.state = {
      config: props.config,
      adminMode: props.adminMode,
      submodules: props.submodules,
      deviceTypes: props.deviceTypes,
      deviceList: props.deviceList,
      serviceList: props.serviceList,
      mapList: props.mapList,
      longSession: !!storage.getValue("longSession"),
      quickConnect: !!storage.getValue("quickConnect"),
      autoConnect: !!storage.getValue("autoConnect"),
      configRoute: storage.getValue("configRoute"),
      confirm: {show: false, name: "", title: "", message: "", cb: false, data: false},
      curDevice: {name: "", description: "", display: "", enabled: true, type_uid: "", option: {}},
      curDeviceAdd: false,
      showModalDevice: false
    };
    this.changeDefaultRoute = this.changeDefaultRoute.bind(this);
    this.toggleLongSession = this.toggleLongSession.bind(this);
    this.toggleQuickConnect = this.toggleQuickConnect.bind(this);
    this.toggleAutoConnect = this.toggleAutoConnect.bind(this);
    this.deviceConnect = this.deviceConnect.bind(this);
    this.deviceRemove = this.deviceRemove.bind(this);
    this.deviceRemoveConfirm = this.deviceRemoveConfirm.bind(this);
    this.deviceAdd = this.deviceAdd.bind(this);
    this.deviceEdit = this.deviceEdit.bind(this);
    this.cbDeviceSave = this.cbDeviceSave.bind(this);
    this.hideConfirm = this.hideConfirm.bind(this);
    this.hideModalDevice = this.hideModalDevice.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  changeDefaultRoute(e, route) {
    e.preventDefault();
    storage.setValue("configRoute", route);
    this.setState({configRoute: route});
  }

  toggleLongSession(e) {
    this.setState({longSession: !this.state.longSession}, () => {
      storage.setValue("longSession", this.state.longSession);
      oidcConnector.setParameter("responseType", this.state.longSession?"code":"token id_token");
    });
  }

  toggleQuickConnect(e) {
    this.setState({quickConnect: !this.state.quickConnect}, () => {
      storage.setValue("quickConnect", this.state.quickConnect);
      oidcConnector.setParameter("additionalParameters", this.state.quickConnect?"g_continue":false);
    });
  }

  toggleAutoConnect(e) {
    this.setState({autoConnect: !this.state.autoConnect}, () => {
      storage.setValue("autoConnect", this.state.autoConnect);
    });
  }

  deviceConnect(e, device) {
    if (device.connected) {
      apiManager.APIRequestBenoic("device/"+encodeURIComponent(device.name)+"/disconnect").
      then(() => {
        messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("config.benoic-device-disconnected")});
        messageDispatcher.sendMessage('Devices', {status: "update"});
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    } else {
      apiManager.APIRequestBenoic("device/"+encodeURIComponent(device.name)+"/connect").
      then(() => {
        messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("config.benoic-device-connected")});
        messageDispatcher.sendMessage('Devices', {status: "update"});
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
  }

  deviceRemove(e, device) {
    this.setState({confirm: {show: true,
                             name: "deviceRemove",
                             title: i18next.t("config.benoic-device-remove-title"),
                             message: i18next.t("config.benoic-device-remove-message", {name: device.name}),
                             cb: this.deviceRemoveConfirm,
                             data: device}});
  }

  deviceRemoveConfirm(result) {
    if (result) {
      apiManager.APIRequestBenoic("device/"+encodeURIComponent(this.state.confirm.data.name), "DELETE").
      then(() => {
        this.setState({confirm: {show: false, title: "", message: "", cb: false, data: false}}, () => {
          messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("config.benoic-device-removed")});
          messageDispatcher.sendMessage('Devices', {status: "update"});
        });
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
  }
  
  deviceAdd() {
    this.setState({curDevice: {name: "", description: "", display: "", enabled: true, type_uid: "", options: {}}, curDeviceAdd: true, showModalDevice: true});
  }
  
  deviceEdit(e, device) {
    this.setState({curDevice: device, curDeviceAdd: false, showModalDevice: true});
  }
  
  cbDeviceSave(result, device) {
    if (result) {
      if (this.state.curDeviceAdd) {
        apiManager.APIRequestBenoic("device/", "POST", device).
        then(() => {
          apiManager.APIRequestBenoic("device/"+encodeURIComponent(device.name)+"/connect").
          then(() => {
            messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("config.benoic-device-added", {name: device.name})});
            messageDispatcher.sendMessage('Devices', {status: "update"});
          })
          .catch((err) => {
            messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
          });
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      } else {
        apiManager.APIRequestBenoic("device/"+encodeURIComponent(device.name), "PUT", device).
        then(() => {
          messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("config.benoic-device-updated", {name: device.name})});
          messageDispatcher.sendMessage('Devices', {status: "update"});
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      }
    }
    var modal = bootstrap.Modal.getOrCreateInstance(document.querySelector('#modalDevice'));
    modal.hide();
  }

  hideConfirm() {
    this.setState({confirm: {show: false, title: "", message: "", cb: false, data: false}});
  }

  hideModalDevice() {
    this.setState({showModalDevice: false});
  }

	render() {
    let selectedConfigRoute = i18next.t("config.default-route-system"), classValue;
    classValue = "dropdown-item";
    if (!this.state.configRoute) {
      classValue += " active";
    }
    let benoicModulesJsx, carleonModulesJsx, benoicDevicesJsx, deviceOption = {}, routeList = [<li key={-1}><a className={classValue} href="#" onClick={(e) => this.changeDefaultRoute(e, "")}>{i18next.t("config.default-route-system")}</a></li>];
    this.state.submodules.forEach(submodule => {
      if (submodule.name === "benoic" && submodule.enabled) {
        let modulesListJsx = [];
        this.state.deviceTypes.forEach((deviceType, index) => {
          deviceOption[deviceType.name] = deviceType.options;
          let enabledIcon;
          if (deviceType.enabled) {
            enabledIcon = <i className="fa fa-check" aria-hidden="true"></i>
          } else {
            enabledIcon = <i className="fa fa-times" aria-hidden="true"></i>
          }
          modulesListJsx.push(
          <tr key={index}>
            <th scope="row">
              {deviceType.name}
            </th>
            <td>
              <span className="td-truncate text-truncate">
                {deviceType.description}
              </span>
            </td>
            <td>
              {enabledIcon}
            </td>
          </tr>
          );
        });
        benoicModulesJsx = <div>
          <h3>
            {i18next.t("config.submodules-benoic-types-list")}
          </h3>
          <table className="table">
            <thead>
              <tr>
                <th scope="col">
                  {i18next.t("config.submodules-benoic-types-list-name")}
                </th>
                <th scope="col">
                  {i18next.t("config.submodules-benoic-types-list-description")}
                </th>
                <th scope="col">
                  {i18next.t("config.submodules-benoic-types-list-enabled")}
                </th>
              </tr>
            </thead>
            <tbody>
              {modulesListJsx}
            </tbody>
          </table>
        </div>;
        let deviceListJsx = [];
        this.state.deviceList.forEach((device, index) => {
          let enabledButtonJsx, trClass;
          if (device.connected) {
            enabledButtonJsx = <i className="fa fa-toggle-off" aria-hidden="true"></i>;
          } else {
            enabledButtonJsx = <i className="fa fa-toggle-on" aria-hidden="true"></i>;
            trClass="bg-warning";
          }
          deviceListJsx.push(
          <tr key={index} className={trClass}>
            <th scope="row">
              {device.name}
            </th>
            <td>
              <span className="td-truncate text-truncate">
                {device.description}
              </span>
            </td>
            <td>
              <div className="btn-group">
                <button type="button" className="btn btn-primary" title={i18next.t("config.benoic-device-enable")} onClick={(e) => this.deviceConnect(e, device)}>
                  {enabledButtonJsx}
                </button>
                <button type="button" className="btn btn-primary" title={i18next.t("config.benoic-device-edit")} onClick={(e) => this.deviceEdit(e, device)} disabled={!this.state.adminMode}>
                  <i className="fa fa-edit" aria-hidden="true"></i>
                </button>
                <button type="button" className="btn btn-primary" title={i18next.t("config.benoic-device-remove")} onClick={(e) => this.deviceRemove(e, device)} disabled={!this.state.adminMode}>
                  <i className="fa fa-trash" aria-hidden="true"></i>
                </button>
              </div>
            </td>
          </tr>
          );
        });
        benoicDevicesJsx = <div>
          <h3>
            {i18next.t("config.benoic-device-list")}
          </h3>
          <button type="button" className="btn btn-primary" title={i18next.t("config.benoic-device-add")} onClick={this.deviceAdd} disabled={!this.state.adminMode}>
            <i className="fa fa-plus" aria-hidden="true"></i>
          </button>
          <table className="table">
            <thead>
              <tr>
                <th scope="col">
                  {i18next.t("config.benoic-device-list-name")}
                </th>
                <th scope="col">
                  {i18next.t("config.benoic-device-list-description")}
                </th>
                <th scope="col">
                  {i18next.t("config.benoic-device-list-action")}
                </th>
              </tr>
            </thead>
            <tbody>
              {deviceListJsx}
            </tbody>
          </table>
        </div>;
      } else if (submodule.name === "carleon" && submodule.enabled) {
        let modulesListJsx = [];
        this.state.serviceList.forEach((service, index) => {
          let enabledIcon;
          if (service.enabled) {
            enabledIcon = <i className="fa fa-check" aria-hidden="true"></i>
          } else {
            enabledIcon = <i className="fa fa-times" aria-hidden="true"></i>
          }
          modulesListJsx.push(
          <tr key={index}>
            <th scope="row">
              {service.name}
            </th>
            <td>
              <span className="td-truncate text-truncate">
                {service.description}
              </span>
            </td>
            <td>
              {enabledIcon}
            </td>
          </tr>
          );
        });
        carleonModulesJsx = <div>
          <h3>
            {i18next.t("config.submodules-carleon-types-list")}
          </h3>
          <table className="table">
            <thead>
              <tr>
                <th scope="col">
                  {i18next.t("config.submodules-carleon-types-list-name")}
                </th>
                <th scope="col">
                  {i18next.t("config.submodules-carleon-types-list-description")}
                </th>
                <th scope="col">
                  {i18next.t("config.submodules-carleon-types-list-enabled")}
                </th>
              </tr>
            </thead>
            <tbody>
              {modulesListJsx}
            </tbody>
          </table>
        </div>
      }
    });
    this.state.mapList.forEach((map, index) => {
      classValue = "dropdown-item";
      if (this.state.configRoute === ("map/"+index)) {
        classValue += " active";
        selectedConfigRoute = map.name;
      }
      routeList.push(
        <li key={index}><a className={classValue} href="#" onClick={(e) => this.changeDefaultRoute(e, "map/"+index)}>{map.name}</a></li>
      );
    });
    classValue = "dropdown-item";
    if (this.state.configRoute === ("components")) {
      classValue += " active";
      selectedConfigRoute = i18next.t("config.default-route-components");
    }
    routeList.push(
      <li key={routeList.length+1}><a className={classValue} href="#" onClick={(e) => this.changeDefaultRoute(e, "components")}>{i18next.t("config.default-route-components")}</a></li>
    );
    return (
      <div>
        <hr/>
        <div>
          <h3>
            {i18next.t("config.select-lang")}
          </h3>
          <LangDropdown config={this.state.config}/>
        </div>
        <hr/>
        <div>
          <h3>
            {i18next.t("config.select-default-route")}
          </h3>
          <div className="dropdown">
            <button className="btn btn-secondary dropdown-toggle" type="button" id="routeDropdown" data-bs-toggle="dropdown" aria-expanded="false">
              {selectedConfigRoute}
            </button>
            <ul className="dropdown-menu" aria-labelledby="routeDropdown">
              {routeList}
            </ul>
          </div>
        </div>
        <hr/>
        <div>
          <h3>
            {i18next.t("config.select-session")}
          </h3>
          <div className="form-check">
            <input className="form-check-input" type="checkbox" checked={this.state.quickConnect} id="quickConnect" onChange={(e) => this.toggleQuickConnect(e)} />
            <label className="form-check-label" htmlFor="quickConnect">
              {i18next.t("config.quick-connect")}
            </label>
          </div>
          <div className="form-check">
            <input className="form-check-input" type="checkbox" checked={this.state.longSession} id="longSession" onChange={(e) => this.toggleLongSession(e)} />
            <label className="form-check-label" htmlFor="longSession">
              {i18next.t("config.long-session")}
            </label>
          </div>
          <div className="form-check">
            <input className="form-check-input" type="checkbox" checked={this.state.autoConnect} id="autoConnect" onChange={(e) => this.toggleAutoConnect(e)} />
            <label className="form-check-label" htmlFor="autoConnect">
              {i18next.t("config.auto-connect")}
            </label>
          </div>
          <p className="fs-6 fst-italic">
            {i18next.t("config.select-session-warning")}
          </p>
        </div>
        <hr/>
        {benoicDevicesJsx}
        <hr/>
        {benoicModulesJsx}
        <hr/>
        {carleonModulesJsx}
        {this.state.confirm.show ? <ModalConfirm handleHideModal={this.hideConfirm}
                                    name={this.state.confirm.name}
                                    title={this.state.confirm.title}
                                    message={this.state.confirm.message}
                                    cb={this.state.confirm.cb} /> : null}
        {this.state.showModalDevice ? <ModalDevice handleHideModal={this.hideModalDevice}
                                      add={this.state.curDeviceAdd}
                                      device={this.state.curDevice}
                                      deviceTypes={this.state.deviceTypes}
                                      deviceList={this.state.deviceList}
                                      cb={this.cbDeviceSave} /> : null}
      </div>
    );
	}
}

export default Config;
