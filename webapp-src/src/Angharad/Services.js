import React, { Component } from 'react';
import i18next from 'i18next';

import MockServices from './MockServices';
import MpdServices from './MpdServices';

class Components extends Component {
  constructor(props) {
    super(props);

    this.state = {
      config: props.config,
      serviceList: props.serviceList,
      filterServices: props.filterServices,
      deviceOverview: props.deviceOverview,
      streamList: props.streamList,
      adminMode: props.adminMode
    }
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

	render() {
    let serviceListJsx = [];
    if (this.state.filterServices.indexOf("mock-service") > -1) {
      this.state.serviceList.forEach(service => {
        if (service.name === "mock-service" && service.enabled) {
          serviceListJsx.push(
            <MockServices key={"mock-service"}
                          service={service}
                          deviceOverview={this.state.deviceOverview}
                          adminMode={this.state.adminMode}/>
          );
        }
      });
    }
    if (this.state.filterServices.indexOf("service-mpd") > -1) {
      this.state.serviceList.forEach(service => {
        if (service.name === "service-mpd" && service.enabled) {
          serviceListJsx.push(
            <MpdServices key={"service-mpd"}
                         service={service}
                         deviceOverview={this.state.deviceOverview}
                         adminMode={this.state.adminMode}
                         streamList={this.state.streamList}
                         config={this.state.config}/>
          );
        }
      });
    }
		return (
      <div>
        <h3>
          {i18next.t("services.title")}
        </h3>
        <hr/>
        {serviceListJsx}
      </div>
		);
	}
}

export default Components;
