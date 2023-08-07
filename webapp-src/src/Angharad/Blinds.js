import React, { Component } from 'react';
import i18next from 'i18next';

import Blind from './Blind';

class Blinds extends Component {
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
    let blindList = [];
    Object.keys(this.state.deviceOverview).forEach(device => {
      Object.keys(this.state.deviceOverview[device].blinds).forEach(name => {
        if (this.state.adminMode || this.state.deviceOverview[device].blinds[name].enabled) {
          blindList.push(
            <div className="col-sm " key={device+"-"+name}>
              <Blind device={device} name={name} element={this.state.deviceOverview[device].blinds[name]} adminMode={this.state.adminMode}/>
            </div>
          );
        }
      });
    });
		return (
      <div>
        <h4>
          {i18next.t("components.blinds")}
        </h4>
        <div className="row text-center">
          {blindList}
        </div>
        <hr/>
      </div>
		);
	}
}

export default Blinds;
