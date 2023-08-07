import React, { Component } from 'react';
import i18next from 'i18next';

import MockService from './MockService';
import messageDispatcher from '../lib/MessageDispatcher';

class MockServices extends Component {
  constructor(props) {
    super(props);

    this.state = {
      service: props.service,
      deviceOverview: props.deviceOverview,
      adminMode: props.adminMode
    }
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  addService() {
    messageDispatcher.sendMessage("Service", {add: true, type: "mock-service", element: {name: "", description: ""}});
  }

	render() {
    let mockServiceList = [], addButtonJsx;
    this.state.service.element.forEach((element, index) => {
      mockServiceList.push(
        <div className="col-sm elt-top" key={index}>
          <MockService element={element} deviceOverview={this.state.deviceOverview} adminMode={this.state.adminMode}/>
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
              {i18next.t("services.mock-services")}
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

export default MockServices;
