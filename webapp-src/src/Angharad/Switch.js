import React, { Component } from 'react';
import i18next from 'i18next';

import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';
import ComponentAdminButtons from './ComponentAdminButtons';

class Switch extends Component {
  constructor(props) {
    super(props);

    this.state = {
      device: props.device,
      name: props.name,
      element: props.element,
      adminMode: props.adminMode
    }
    
    this.handleSwitch = this.handleSwitch.bind(this);
    this.openMonitor = this.openMonitor.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  handleSwitch() {
    let newValue = 0;
    if (this.state.element.value === 0) {
      newValue = 1;
    }
    apiManager.APIRequestBenoic("device/" + this.state.device + "/switch/" + this.state.name + "/set/" + newValue, "PUT")
    .then((results) => {
      messageDispatcher.sendMessage('Component', {status: "update", device: this.state.device, type: "switch", name: this.state.name, value: newValue});
      let element = this.state.element;
      element.value = newValue;
      this.setState({element: element});
    })
    .catch((err) => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    });
  }

  openMonitor(e) {
    e.preventDefault();
    messageDispatcher.sendMessage("Component", {status: "monitor", device: this.state.device, type: "switch", name: this.state.name, element: this.state.element});
  }

	render() {
    let adminButtonsJsx;
    if (this.state.adminMode) {
      adminButtonsJsx =
        <ComponentAdminButtons device={this.state.device} name={this.state.name} type={"switch"} element={this.state.element} />
    } else if (this.state.element.monitored) {
      adminButtonsJsx = 
        <a className="dropdown-item" href="#" onClick={this.openMonitor} title={i18next.t("components.monitor")}>
          <i className="fa fa-line-chart elt-left" aria-hidden="true"></i>
        </a>
    }
		return (
      <div className="row elt-top border rounded agrd-element">
        <div className="col-6">
          <label className="d-flex align-items-center" htmlFor={"switcher-"+this.state.device+"-"+this.state.name}>
            <i className="fa fa-power-off elt-left" aria-hidden="true"></i>
            {this.state.element.display}
          </label>
        </div>
        <div className="col-4 text-center">
          <div className="form-switch">
            <input className="form-check-input"
                   type="checkbox"
                   role="switch"
                   id={"switcher-"+this.state.device+"-"+this.state.name}
                   checked={this.state.element.value===1}
                   onChange={this.handleSwitch} />
          </div>
        </div>
        <div className="col-2">
          {adminButtonsJsx}
        </div>
      </div>
		);
	}
}

export default Switch;
