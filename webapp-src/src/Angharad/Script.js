import React, { Component } from 'react';
import i18next from 'i18next';

import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';
import ScriptSchedAdminButtons from './ScriptSchedAdminButtons';

class Script extends Component {
  constructor(props) {
    super(props);

    this.state = {
      script: props.script
    }
    
    this.scriptRun = this.scriptRun.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  scriptRun() {
    apiManager.APIRequestAngharad("script/"+this.state.script.name+"/run")
    .then(() => {
      messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("scripts.executed")});
    })
    .catch((err) => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    });
  }
  
	render() {
    let adminButtonsJsx;
    if (this.state.adminMode) {
      adminButtonsJsx =
        <ScriptSchedAdminButtons type={"script"} script={this.state.script} scheduler={false} />
    }
		return (
      <div className="row elt-top agrd-element">
        <div className="col-10 d-grid gap-2">
          <button type="button" className="btn btn-primary text-start" title={this.state.script.description} onClick={this.scriptRun}>
            <i className="fa fa-play-circle push-right" aria-hidden="true"></i>
            {this.state.script.name}
          </button>
        </div>
        <div className="col-2">
          {adminButtonsJsx}
        </div>
      </div>
		);
	}
}

export default Script;
