import React, { Component } from 'react';
import i18next from 'i18next';

import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';
import ScriptSchedAdminButtons from './ScriptSchedAdminButtons';

class Scheduler extends Component {
  constructor(props) {
    super(props);

    this.state = {
      scriptList: props.scriptList,
      scheduler: props.scheduler
    }
    
    this.toggleScheduler = this.toggleScheduler.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  toggleScheduler() {
    let url = "scheduler/" + encodeURIComponent(this.state.scheduler.name) + "/enable/";
    if (this.state.scheduler.enabled) {
      url += "0";
    } else {
      url += "1";
    }
    apiManager.APIRequestAngharad(url, "PUT")
    .then(() => {
      messageDispatcher.sendMessage("Scheduler", {status: "refresh"});
    })
    .catch((err) => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    });
  }
  
	render() {
    let adminButtonsJsx;
    if (this.state.adminMode) {
      adminButtonsJsx =
        <ScriptSchedAdminButtons type={"scheduler"} scheduler={this.state.scheduler} script={false} />
    }
    let nextTimeJsx;
    if (this.state.scheduler.enabled) {
      let nextTime = new Date(this.state.scheduler.next_time*1000);
      nextTimeJsx = nextTime.toLocaleString('fr-CA',{hour12: false, year: 'numeric', month: 'numeric', day: 'numeric', hour: '2-digit', minute:'2-digit'});
    } else {
      nextTimeJsx = i18next.t("schedulers.disabled")
    }
		return (
      <div className="row elt-top agrd-element">
        <div className="col-10 d-grid gap-2">
          <div className="border rounded border-3 scheduler-box text-center">
            <h4>
              <i className="fa fa-calendar elt-left" aria-hidden="true"></i>
              {this.state.scheduler.name}
            </h4>
            <hr/>
            <div className="form-check form-switch text-center">
              <input className="form-check-input"
                     type="checkbox"
                     role="switch"
                     id={"sched-"+this.state.scheduler.name}
                     checked={this.state.scheduler.enabled}
                     onChange={this.toggleScheduler} />
              <label className="form-check-label" htmlFor={"sched-"+this.state.scheduler.name}>
                {i18next.t("schedulers.enabled")}
              </label>
            </div>
            <hr/>
            <div>
              <h5>
                {i18next.t("schedulers.next-time")}
              </h5>
              <pre>
                {nextTimeJsx}
              </pre>
            </div>
          </div>
        </div>
        <div className="col-2">
          {adminButtonsJsx}
        </div>
      </div>
		);
	}
}

export default Scheduler;
