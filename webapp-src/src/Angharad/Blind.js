import React, { Component } from 'react';
import i18next from 'i18next';

import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';
import ComponentAdminButtons from './ComponentAdminButtons';
import RoundPercent from './RoundPercent';

class Blind extends Component {
  constructor(props) {
    super(props);

    this.state = {
      device: props.device,
      name: props.name,
      element: props.element,
      adminMode: props.adminMode
    }
    
    this.openBlind = this.openBlind.bind(this);
    this.openMonitor = this.openMonitor.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  openBlind(e) {
    e.preventDefault();
    messageDispatcher.sendMessage("Component", {status: "blindModal", device: this.state.device, name: this.state.name, element: this.state.element});
  }
  
  openMonitor(e) {
    e.preventDefault();
    messageDispatcher.sendMessage("Component", {status: "monitor", device: this.state.device, type: "blind", name: this.state.name, element: this.state.element});
  }

	render() {
    let adminButtonsJsx;
    if (this.state.adminMode) {
      adminButtonsJsx =
        <ComponentAdminButtons device={this.state.device} name={this.state.name} type={"blind"} element={this.state.element} />
    } else if (this.state.element.monitored) {
      adminButtonsJsx = 
        <a className="dropdown-item" href="#" onClick={this.openMonitor} title={i18next.t("components.monitor")}>
          <i className="fa fa-line-chart elt-left" aria-hidden="true"></i>
        </a>
    }
		return (
      <div className="row elt-top border rounded agrd-element">
        <div className="col-2">
          <label className="d-flex align-items-center">
            <i className="fa fa-window-maximize elt-left" aria-hidden="true">
            </i>
            {this.state.element.display}
          </label>
        </div>
        <div className="col-8">
          <a href="#" onClick={this.openBlind}>
            <RoundPercent value={this.state.element.value} />
          </a>
        </div>
        <div className="col-2">
          {adminButtonsJsx}
        </div>
      </div>
		);
	}
}

export default Blind;
