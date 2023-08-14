import React, { Component } from 'react';
import i18next from 'i18next';

import Sensor from './Sensor';

class Sensors extends Component {
  constructor(props) {
    super(props);

    this.state = {
      deviceOverview: props.deviceOverview,
      adminMode: props.adminMode
    }
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

	render() {
    let sensorList = [];
    Object.keys(this.state.deviceOverview).forEach(device => {
      if (this.state.deviceOverview[device].sensors) {
        Object.keys(this.state.deviceOverview[device].sensors).forEach(name => {
          if (this.state.adminMode || this.state.deviceOverview[device].sensors[name].enabled) {
            sensorList.push(
              <div className="col-sm-12 col-md-6" key={device+"-"+name}>
                <Sensor device={device} name={name} element={this.state.deviceOverview[device].sensors[name]} adminMode={this.state.adminMode}/>
              </div>
            );
          }
        });
      }
    });
		return (
      <div>
        <h4>
          {i18next.t("components.sensors")}
        </h4>
        <div className="row text-center">
          {sensorList}
        </div>
        <hr/>
      </div>
		);
	}
}

export default Sensors;
