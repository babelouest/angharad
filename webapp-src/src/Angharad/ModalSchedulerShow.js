import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';

import Scheduler from './Scheduler';

class ModalSchedulerShow extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      scheduler: props.scheduler
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
		return (
      <div className="modal fade" tabIndex="-1" id="modalDimmer">
        <div className="modal-dialog">
          <div className="modal-content">
            <div className="modal-body">
              <Scheduler scheduler={this.state.scheduler} />
            </div>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalSchedulerShow;
