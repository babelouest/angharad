import React, { Component } from 'react';
import i18next from 'i18next';

import messageDispatcher from '../lib/MessageDispatcher';

class ServiceAdminButtons extends Component {
  constructor(props) {
    super(props);

    this.state = {
      type: props.type,
      deviceOverview: props.deviceOverview,
      element: props.element
    }
    
    this.openComponentModal = this.openComponentModal.bind(this);
    this.openRemoveModal = this.openRemoveModal.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  openComponentModal(e, add) {
    e.preventDefault();
    messageDispatcher.sendMessage("Service", {remove: false, add: add, type: this.state.type, element: this.state.element});
  }
  
  openRemoveModal(e) {
    e.preventDefault();
    messageDispatcher.sendMessage("Service", {remove: true, type: this.state.type, element: this.state.element});
  }
  
	render() {
		return (
      <div className="dropdown">
        <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
          <i className="fa fa-cog" aria-hidden="true"></i>
        </button>
        <ul className="dropdown-menu">
          <li>
            <a className="dropdown-item" href="#" onClick={(e) => this.openComponentModal(e, false)}>
              <i className="fa fa-edit elt-left" aria-hidden="true"></i>
              {i18next.t("services.edit")}
            </a>
          </li>
          <li>
            <a className="dropdown-item" href="#" onClick={this.openRemoveModal}>
              <i className="fa fa-trash elt-left" aria-hidden="true"></i>
              {i18next.t("services.remove")}
            </a>
          </li>
        </ul>
      </div>
		);
	}
}

export default ServiceAdminButtons;
