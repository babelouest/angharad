import React, { Component } from 'react';
import i18next from 'i18next';

import Switches from './Switches';
import Dimmers from './Dimmers';
import Sensors from './Sensors';
import Blinds from './Blinds';

class Components extends Component {
  constructor(props) {
    super(props);

    this.state = {
      deviceOverview: props.deviceOverview,
      serviceList: props.serviceList,
      filterComponents: props.filterComponents,
      adminMode: props.adminMode
    }
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

	render() {
    let componentListJsx = [];
    if (this.state.filterComponents.indexOf("switch") > -1) {
      componentListJsx.push(
        <Switches key={"switch"}
                  deviceOverview={this.state.deviceOverview}
                  adminMode={this.state.adminMode}/>
      );
    }
    if (this.state.filterComponents.indexOf("dimmer") > -1) {
      componentListJsx.push(
        <Dimmers key={"dimmer"}
                  deviceOverview={this.state.deviceOverview}
                  adminMode={this.state.adminMode}/>
      );
    }
    if (this.state.filterComponents.indexOf("sensor") > -1) {
      componentListJsx.push(
        <Sensors key={"sensor"}
                  deviceOverview={this.state.deviceOverview}
                  adminMode={this.state.adminMode}/>
      );
    }
    if (this.state.filterComponents.indexOf("blind") > -1) {
      componentListJsx.push(
        <Blinds key={"blind"}
                deviceOverview={this.state.deviceOverview}
                adminMode={this.state.adminMode}/>
      );
    }
		return (
      <div>
        <h3>
          {i18next.t("components.title")}
        </h3>
        <hr/>
        {componentListJsx}
      </div>
		);
	}
}

export default Components;
