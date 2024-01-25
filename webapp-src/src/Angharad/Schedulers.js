import React, { Component } from 'react';
import i18next from 'i18next';

import Scheduler from './Scheduler';
import messageDispatcher from '../lib/MessageDispatcher';

class Schedulers extends Component {
  constructor(props) {
    super(props);

    this.state = {
      scheduler: props.scheduler,
      adminMode: props.adminMode
    }
    
    this.addScheduler = this.addScheduler.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  addScheduler() {
    messageDispatcher.sendMessage("Scheduler", {add: true, scheduler: 
      {name: "", description: "", enabled: true, repeat: -1, next_time: (new Date()).getTime()/1000, repeat_value: 1, scripts: [], remove_after: false}
    });
  }

	render() {
    let schedulerList = [], addButtonJsx;
    this.state.scheduler.forEach((scheduler, index) => {
      schedulerList.push(
      <div className="col-sm-12 col-lg-4" key={"scr-"+index}>
        <Scheduler scheduler={scheduler} adminMode={this.state.adminMode}/>
      </div>
      );
    });
    if (this.state.adminMode) {
      addButtonJsx = 
        <button type="button" className="btn btn-secondary" onClick={this.addScheduler}>
          <i className="fa fa-plus" aria-hidden="true"></i>
        </button>
    }
		return (
      <div>
        <div className="row">
          <div className="col-10">
            <h4>
              {i18next.t("schedulers.title")}
            </h4>
          </div>
          <div className="col-2 align-items-end">
            {addButtonJsx}
          </div>
        </div>
        <hr/>
        <div className="row">
          {schedulerList}
        </div>
      </div>
		);
	}
}

export default Schedulers;
