import React, { Component } from 'react';
import i18next from 'i18next';

import WeathermapService from './WeathermapService';
import messageDispatcher from '../lib/MessageDispatcher';

class WeathermapServices extends Component {
  constructor(props) {
    super(props);

    this.state = {
      config: props.config,
      service: props.service,
      adminMode: props.adminMode
    }
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  addService() {
    messageDispatcher.sendMessage("Service", {add: true, type: "service-weathermap", element: {name: "", description: "", units: "metric", lang: i18next.language, uri: "https://api.openweathermap.org/data/2.5/weather"}});
  }

	render() {
    let weathermapServiceList = [], addButtonJsx;
    this.state.service.element.forEach((element, index) => {
      weathermapServiceList.push(
        <div className="col-sm-12 col-md-6 col-lg-4" key={index}>
          <WeathermapService element={element} config={this.state.config} adminMode={this.state.adminMode} />
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
              {i18next.t("services.weathermap-services")}
            </h4>
          </div>
          <div className="col-2 align-items-end">
            {addButtonJsx}
          </div>
        </div>
        <div className="row">
          {weathermapServiceList}
        </div>
        <hr/>
      </div>
		);
	}
}

export default WeathermapServices;
