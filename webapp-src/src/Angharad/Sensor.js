import React, { Component } from 'react';
import i18next from 'i18next';

import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';
import ComponentAdminButtons from './ComponentAdminButtons';

class Sensor extends Component {
  constructor(props) {
    super(props);

    this.state = {
      device: props.device,
      name: props.name,
      element: props.element,
      adminMode: props.adminMode
    }
    
    this.openMonitor = this.openMonitor.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  openMonitor(e) {
    e.preventDefault();
    messageDispatcher.sendMessage("Component", {status: "monitor", device: this.state.device, type: "sensor", name: this.state.name, element: this.state.element});
  }

	render() {
    let adminButtonsJsx, valueJsx = this.state.element.value.toFixed(2);
    if (this.state.adminMode) {
      adminButtonsJsx =
        <ComponentAdminButtons device={this.state.device} name={this.state.name} type={"sensor"} element={this.state.element} />
    } else if (this.state.element.monitored) {
      adminButtonsJsx = 
        <a className="dropdown-item" href="#" onClick={this.openMonitor} title={i18next.t("components.monitor")}>
          <i className="fa fa-line-chart elt-left" aria-hidden="true"></i>
        </a>
    }
    if (this.state.element.options && this.state.element.options.unit) {
      valueJsx += this.state.element.options.unit;
    }
		return (
      <div className="row elt-top border rounded agrd-element">
        <div className="col-2">
          <label className="d-flex align-items-center">
            <i className="fa fa-area-chart elt-left" aria-hidden="true">
            </i>
            {this.state.element.display}
          </label>
        </div>
        <div className="col-8 text-center">
          <pre>
            {valueJsx}
          </pre>
        </div>
        <div className="col-2">
          {adminButtonsJsx}
        </div>
      </div>
		);
	}
}

export default Sensor;
