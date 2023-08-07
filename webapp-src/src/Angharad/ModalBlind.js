import React, { Component } from 'react';
import i18next from 'i18next';

import RoundPercent from './RoundPercent';
import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';

class ModalBlind extends Component {
  constructor(props) {
    super(props);

    this.state = {
      device: props.device,
      name: props.name,
      element: props.element
    }

    this.setBlindValue = this.setBlindValue.bind(this);
    this.addBlindValue = this.addBlindValue.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  setBlindValue(e, value) {
    apiManager.APIRequestBenoic("device/" + encodeURIComponent(this.state.device) + "/blind/" + encodeURIComponent(this.state.name) + "/set/" + value, "PUT")
    .then((result) => {
      let element = this.state.element;
      element.value = result.value;
      this.setState({element: element}, () => {
        messageDispatcher.sendMessage("Component", {status: "update", type: "blind", device: this.state.device, name: this.state.name, value: result.value});
      });
    });
  }
  
  addBlindValue(e, value) {
    this.setBlindValue(e, this.state.element.value+value);
  }
  
	render() {
		return (
      <div className="modal fade" tabIndex="-1" id="modalBlind">
        <div className="modal-dialog">
          <div className="modal-content">
            <div className="modal-body">
              <h5 className="modal-title">
                {this.state.element.display}
              </h5>
              <div className="mb-3">
                <RoundPercent value={this.state.element.value} />
              </div>
              <div className="mb-3 text-center">
                <div className="btn-group" role="group">
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 0)}>
                    0%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 25)}>
                    25%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 50)}>
                    50%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 75)}>
                    75%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 100)}>
                    100%
                  </button>
                </div>
              </div>
              <div className="mb-3 text-center">
                <div className="btn-group" role="group">
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setBlindValue(e, 101)}>
                    <i className="fa fa-power-off" aria-hidden="true"></i>
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addBlindValue(e, -5)} disabled={this.state.element.value < 5}>
                    -5%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addBlindValue(e, -1)} disabled={this.state.element.value === 0}>
                    -1%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addBlindValue(e, 1)} disabled={this.state.element.value >= 100}>
                    +1%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addBlindValue(e, 5)} disabled={this.state.element.value > 95}>
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
