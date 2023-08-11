import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';

import RoundPercent from './RoundPercent';
import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';

class ModalConfirm extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      title: props.title,
      message: props.message,
      cb: props.cb
    }
    this.closeModal = this.closeModal.bind(this);
  }

  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).modal('show');
    $(ReactDOM.findDOMNode(this)).on('hidden.bs.modal', this.state.handleHideModal);
  }

  closeModal(e, result) {
    $(ReactDOM.findDOMNode(this)).modal('hide');
    if (this.state.cb) {
      this.state.cb(result);
    }
  }
  
	render() {
		return (
      <div className="modal" tabIndex="-1" id="modalConfirm">
        <div className="modal-dialog">
          <div className="modal-content">
            <form onSubmit={(e) => this.closeModal(e, true)}>
              <div className="modal-header">
                <h5 className="modal-title">
                  {this.state.title}
                </h5>
                <button type="button" className="btn-close" aria-label="Close" onClick={(e) => this.closeModal(e, false)}></button>
              </div>
              <div className="modal-body">
                <div className="mb-3">
                  <label className="form-label">
                    {this.state.message}
                  </label>
                </div>
              </div>
              <div className="modal-footer">
                <button type="submit" className="btn btn-secondary" onClick={(e) => this.closeModal(e, false)}>{i18next.t("modal.cancel")}</button>
                <button type="button" className="btn btn-primary" onClick={(e) => this.closeModal(e, true)}>{i18next.t("modal.ok")}</button>
              </div>
            </form>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalConfirm;
