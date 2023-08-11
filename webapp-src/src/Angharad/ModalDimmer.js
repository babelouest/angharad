import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';

import RoundPercent from './RoundPercent';
import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';

class ModalDimmer extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      device: props.device,
      name: props.name,
      element: props.element
    }

    this.setDimmerValue = this.setDimmerValue.bind(this);
    this.toggleDimmerValue = this.toggleDimmerValue.bind(this);
    this.addDimmerValue = this.addDimmerValue.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).modal('show');
    $(ReactDOM.findDOMNode(this)).on('hidden.bs.modal', this.state.handleHideModal);
  }

  setDimmerValue(e, value) {
    apiManager.APIRequestBenoic("device/" + encodeURIComponent(this.state.device) + "/dimmer/" + encodeURIComponent(this.state.name) + "/set/" + value, "PUT")
    .then((result) => {
      let element = this.state.element;
      element.value = result.value;
      this.setState({element: element}, () => {
        messageDispatcher.sendMessage("Component", {status: "update", type: "dimmer", device: this.state.device, name: this.state.name, value: result.value});
      });
    });
  }
  
  toggleDimmerValue(e) {
    let value = 0;
    if (!this.state.element.value) {
      value = 101;
    }
    this.setDimmerValue(e, value);
  }
  
  addDimmerValue(e, value) {
    this.setDimmerValue(e, this.state.element.value+value);
  }
  
	render() {
		return (
      <div className="modal fade" tabIndex="-1" id="modalDimmer">
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
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.toggleDimmerValue(e)}>
                    <i className="fa fa-power-off" aria-hidden="true"></i>
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setDimmerValue(e, 25)}>
                    25%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setDimmerValue(e, 50)}>
                    50%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setDimmerValue(e, 75)}>
                    75%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.setDimmerValue(e, 100)}>
                    100%
                  </button>
                </div>
              </div>
              <div className="mb-3 text-center">
                <div className="btn-group" role="group">
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addDimmerValue(e, -5)} disabled={this.state.element.value < 5}>
                    -5%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addDimmerValue(e, -1)} disabled={this.state.element.value === 0}>
                    -1%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addDimmerValue(e, 1)} disabled={this.state.element.value >= 100}>
                    +1%
                  </button>
                  <button type="button" className="btn btn-outline-secondary" onClick={(e) => this.addDimmerValue(e, 5)} disabled={this.state.element.value > 95}>
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

export default ModalDimmer;
