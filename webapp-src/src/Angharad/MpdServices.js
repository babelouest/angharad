import React, { Component } from 'react';
import i18next from 'i18next';

import MpdService from './MpdService';
import messageDispatcher from '../lib/MessageDispatcher';

class MpdServices extends Component {
  constructor(props) {
    super(props);

    this.state = {
      config: props.config,
      service: props.service,
      deviceOverview: props.deviceOverview,
      streamList: props.streamList,
      adminMode: props.adminMode
    }
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  addService() {
    messageDispatcher.sendMessage("Service", {add: true, type: "service-mpd", element: {name: "", description: ""}});
  }

	render() {
    let mockServiceList = [], addButtonJsx;
    this.state.service.element.forEach((element, index) => {
      mockServiceList.push(
        <div className="col-sm-12 col-md-6 col-lg-4" key={index}>
          <MpdService element={element} deviceOverview={this.state.deviceOverview} streamList={this.state.streamList} adminMode={this.state.adminMode} config={this.state.config}/>
        </div>
      );
    });
    if (this.state.adminMode) {
      addButtonJsx = 
        <button type="button" className="btn btn-secondary" onClick={this.addService}>
          <i className="fa fa-plus" aria-hidden="true"></i>
        </button>
    }
		return (
      <div>
        <div className="row">
          <div className="col-10">
            <h4>
              {i18next.t("services.mpd-services")}
            </h4>
          </div>
          <div className="col-2 align-items-end">
            {addButtonJsx}
          </div>
        </div>
        <div className="row">
          {mockServiceList}
        </div>
        <hr/>
      </div>
		);
	}
}

export default MpdServices;
