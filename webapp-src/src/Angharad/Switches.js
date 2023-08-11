import React, { Component } from 'react';
import i18next from 'i18next';

import Switch from './Switch';

class Switches extends Component {
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
    let switchList = [];
    Object.keys(this.state.deviceOverview).forEach(device => {
      if (this.state.deviceOverview[device].switches) {
        Object.keys(this.state.deviceOverview[device].switches).forEach(name => {
          if (this.state.adminMode || this.state.deviceOverview[device].switches[name].enabled) {
            switchList.push(
              <div className="col-sm-4" key={device+"-"+name}>
                <Switch device={device} name={name} element={this.state.deviceOverview[device].switches[name]} adminMode={this.state.adminMode}/>
              </div>
            );
          }
        });
      }
    });
		return (
      <div>
        <h4>
          {i18next.t("components.switches")}
        </h4>
        <div className="row text-center">
          {switchList}
        </div>
        <hr/>
      </div>
		);
	}
}

export default Switches;
