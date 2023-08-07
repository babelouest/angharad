import React, { Component } from 'react';
import i18next from 'i18next';

import messageDispatcher from '../lib/MessageDispatcher';

class ScriptSchedAdminButtons extends Component {
  constructor(props) {
    super(props);

    this.state = {
      script: props.script,
      scheduler: props.scheduler,
      type: props.type
    }
    
    this.openComponentModal = this.openComponentModal.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  openComponentModal(e, remove) {
    e.preventDefault();
    if (this.state.script) {
      messageDispatcher.sendMessage("Script", {script: this.state.script, add: false, remove: remove});
    } else {
      messageDispatcher.sendMessage("Scheduler", {scheduler: this.state.scheduler, add: false, remove: remove});
    }
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
              {i18next.t("modal.edit")}
            </a>
          </li>
          <li>
            <a className="dropdown-item" href="#" onClick={(e) => this.openComponentModal(e, true)}>
              <i className="fa fa-trash elt-left" aria-hidden="true"></i>
              {i18next.t("modal.remove")}
            </a>
          </li>
        </ul>
      </div>
		);
	}
}

export default ScriptSchedAdminButtons;
