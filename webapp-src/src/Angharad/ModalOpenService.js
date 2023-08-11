import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';

import MockService from './MockService';
import MpdService from './MpdService';

class ModalOpenService extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      type: props.type,
      element: props.element,
      deviceOverview: props.deviceOverview,
      streamList: props.streamList
    }
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).modal('show');
    $(ReactDOM.findDOMNode(this)).on('hidden.bs.modal', this.state.handleHideModal);
  }

	render() {
    let bodyJsx;
    if (this.state.type === "mock-service") {
      bodyJsx = <MockService element={this.state.element} deviceOverview={this.state.deviceOverview} adminMode={false} />
    } else if (this.state.type === "service-mpd") {
      bodyJsx = <MpdService element={this.state.element} deviceOverview={this.state.deviceOverview} adminMode={false} streamList={this.state.streamList} config={this.state.config} />
    }
		return (
      <div className="modal fade" tabIndex="-1" id="modalOpenService">
        <div className="modal-dialog">
          <div className="modal-content">
            <div className="modal-body">
              <h5 className="modal-title">
                {this.state.element.name}
              </h5>
              {bodyJsx}
            </div>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalOpenService;
