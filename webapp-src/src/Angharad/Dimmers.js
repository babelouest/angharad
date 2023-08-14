import React, { Component } from 'react';
import i18next from 'i18next';

import Dimmer from './Dimmer';

class Dimmers extends Component {
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
    let dimmerList = [];
    Object.keys(this.state.deviceOverview).forEach(device => {
      if (this.state.deviceOverview[device].dimmers) {
        Object.keys(this.state.deviceOverview[device].dimmers).forEach(name => {
          if (this.state.adminMode || this.state.deviceOverview[device].dimmers[name].enabled) {
            dimmerList.push(
              <div className="col-sm-12 col-md-6" key={device+"-"+name}>
                <Dimmer device={device} name={name} element={this.state.deviceOverview[device].dimmers[name]} adminMode={this.state.adminMode}/>
              </div>
            );
          }
        });
      }
    });
		return (
      <div>
        <h4>
          {i18next.t("components.dimmers")}
        </h4>
        <div className="row text-center">
          {dimmerList}
        </div>
        <hr/>
      </div>
		);
	}
}

export default Dimmers;
