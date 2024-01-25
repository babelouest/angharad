import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';

import RoundPercent from './RoundPercent';
import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';

class ModalBlind extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      device: props.device,
      name: props.name,
      element: props.element,
      deviceOverview: props.deviceOverview,
      updating: false
    }

    this.setBlindValue = this.setBlindValue.bind(this);
    this.addBlindValue = this.addBlindValue.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).modal('show');
    $(ReactDOM.findDOMNode(this)).on('hidden.bs.modal', this.state.handleHideModal);
  }

  setBlindValue(e, value) {
    this.setState({updating: true}, () => {
      apiManager.APIRequestBenoic("device/" + encodeURIComponent(this.state.device) + "/blind/" + encodeURIComponent(this.state.name) + "/set/" + value, "PUT")
      .then((result) => {
        let element = this.state.element;
        element.value = result.value;
        this.setState({element: element}, () => {
          messageDispatcher.sendMessage("Component", {status: "update", type: "blind", device: this.state.device, name: this.state.name, value: result.value});
        });
      })
      .always(() => {
        this.setState({updating: false});
      });
    });
  }
  
  addBlindValue(e, value) {
    this.setBlindValue(e, this.state.element.value+value);
  }
  
	render() {
    let valueJsx, batteryJsx;
    if (this.state.element.options.battery) {
      let sensor = this.state.deviceOverview[this.state.element.options.battery.device]?.sensors[this.state.element.options.battery.sensor];
      if (sensor) {
        valueJsx = sensor.value;
        if (!isNaN(valueJsx) && valueJsx != Math.round(valueJsx)) {
          valueJsx = valueJsx.toFixed(2)
        }
        if (sensor.options?.unit) {
          valueJsx += sensor.options.unit;
        }
        if (isNaN(sensor.value) || sensor.value <= 0) {
          batteryJsx =
            <span className="badge text-bg-light">
              <i className="fa fa-battery-empty elt-left" aria-hidden="true"></i>
              {valueJsx}
            </span>
        } else if (sensor.value <= 25) {
          batteryJsx =
            <span className="badge text-bg-light">
              <i className="fa fa-battery-quarters elt-left" aria-hidden="true"></i>
              {valueJsx}
            </span>
        } else if (sensor.value <= 50) {
          batteryJsx =
            <span className="badge text-bg-light">
              <i className="fa fa-battery-half elt-left" aria-hidden="true"></i>
              {valueJsx}
            </span>
        } else if (sensor.value <= 75) {
          batteryJsx =
            <span className="badge text-bg-light">
              <i className="fa fa-battery-three-quarters elt-left" aria-hidden="true"></i>
              {valueJsx}
            </span>
        } else {
          batteryJsx =
            <span className="badge text-bg-light">
              <i className="fa fa-battery-full elt-left" aria-hidden="true"></i>
              {valueJsx}
            </span>
        }
      }
    }
		return (
      <div className="modal fade" tabIndex="-1" id="modalBlind">
        <div className="modal-dialog">
          <div className="modal-content">
            <div className="modal-body">
              <h5 className="modal-title">
                {this.state.element.display}
              </h5>
              <div className="mb-3">
                {batteryJsx}
                <RoundPercent value={this.state.element.value} />
              </div>
              <div className="mb-3 text-center">
                <div className="btn-group" role="group">
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 0)} disabled={this.state.updating}>
                    0%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 25)} disabled={this.state.updating}>
                    25%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 50)} disabled={this.state.updating}>
                    50%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 75)} disabled={this.state.updating}>
                    75%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 100)} disabled={this.state.updating}>
                    100%
                  </button>
                </div>
              </div>
              <div className="mb-3 text-center">
                <div className="btn-group" role="group">
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 101)} disabled={this.state.updating}>
                    <i className="fa fa-power-off" aria-hidden="true"></i>
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addBlindValue(e, -5)} disabled={this.state.element.value < 5} disabled={this.state.updating}>
                    -5%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addBlindValue(e, -1)} disabled={this.state.element.value === 0} disabled={this.state.updating}>
                    -1%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addBlindValue(e, 1)} disabled={this.state.element.value >= 100} disabled={this.state.updating}>
                    +1%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addBlindValue(e, 5)} disabled={this.state.element.value > 95} disabled={this.state.updating}>
                    +5%
                  </button>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalBlind;
