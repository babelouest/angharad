import React, { Component } from 'react';
import i18next from 'i18next';

import Script from './Script';
import messageDispatcher from '../lib/MessageDispatcher';

class Scripts extends Component {
  constructor(props) {
    super(props);

    this.state = {
      script: props.script,
      scheduler: props.scheduler,
      adminMode: props.adminMode
    }
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  addScript() {
    messageDispatcher.sendMessage("Script", {add: true, script: {name: "", description: "", actions: []}});
  }

	render() {
    let scriptList = [], addButtonJsx;
    this.state.script.forEach((script, index) => {
      scriptList.push(
      <div className="col-sm-4" key={"scr-"+index}>
        <Script deviceOverview={this.state.deviceOverview} serviceList={this.state.serviceList} script={script} adminMode={this.state.adminMode}/>
      </div>
      );
    });
    if (this.state.adminMode) {
      addButtonJsx = 
        <button type="button" className="btn btn-secondary" onClick={this.addScript}>
          <i className="fa fa-plus" aria-hidden="true"></i>
        </button>
    }
		return (
      <div>
        <div className="row">
          <div className="col-10">
            <h4>
              {i18next.t("scripts.title")}
            </h4>
          </div>
          <div className="col-2 align-items-end">
            {addButtonJsx}
          </div>
        </div>
        <hr/>
        <div className="row">
          {scriptList}
        </div>
      </div>
		);
	}
}

export default Scripts;
