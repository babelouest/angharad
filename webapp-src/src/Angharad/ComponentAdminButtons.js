import React, { Component } from 'react';
import i18next from 'i18next';

import messageDispatcher from '../lib/MessageDispatcher';

class ComponentAdminButtons extends Component {
  constructor(props) {
    super(props);

    this.state = {
      device: props.device,
      name: props.name,
      type: props.type,
      element: props.element
    }
    
    this.openComponentModal = this.openComponentModal.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  openComponentModal(e, status) {
    e.preventDefault();
    messageDispatcher.sendMessage("Component", {status: status, device: this.state.device, type: this.state.type, name: this.state.name, element: this.state.element});
  }

	render() {
    let monitorJsx;
    if (this.state.element.monitored) {
      monitorJsx =
        <a className="dropdown-item" href="#" onClick={(e) => this.openComponentModal(e, "monitor")}>
          <i className="fa fa-line-chart elt-left" aria-hidden="true"></i>
          {i18next.t("components.monitor")}
        </a>
    }
		return (
      <div className="dropdown">
        <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
          <i className="fa fa-cog" aria-hidden="true"></i>
        </button>
        <ul className="dropdown-menu">
          <li>
            <a className="dropdown-item" href="#" onClick={(e) => this.openComponentModal(e, "edit")}>
              <i className="fa fa-edit elt-left" aria-hidden="true"></i>
              {i18next.t("components.edit")}
            </a>
            {monitorJsx}
          </li>
        </ul>
      </div>
		);
	}
}

export default ComponentAdminButtons;
