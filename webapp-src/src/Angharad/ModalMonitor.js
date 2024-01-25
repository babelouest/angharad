import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';
import "flatpickr/dist/flatpickr.css";
import Flatpickr from "react-flatpickr";

import SimpleMonitor from './SimpleMonitor';

class ModalMonitor extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      device: props.device,
      type: props.type,
      name: props.name,
      element: props.element,
      cb: props.cb,
      from: 86400,
      to: Math.floor(Date.now()/1000),
      data: []
    }

    this.closeModal = this.closeModal.bind(this);
    this.selectMonitoredFrom = this.selectMonitoredFrom.bind(this);
    this.changeDateTo = this.changeDateTo.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).modal('show');
    $(ReactDOM.findDOMNode(this)).on('hidden.bs.modal', this.state.handleHideModal);
  }

  closeModal() {
    $(ReactDOM.findDOMNode(this)).modal('hide');
    if (this.state.cb) {
      this.state.cb();
    }
  }
  
  selectMonitoredFrom(e, from) {
    e.preventDefault();
    this.setState({from: from});
  }
  
  changeDateTo(e) {
    if (e[0] && e[0] instanceof Date) {
      this.setState({to: e[0].getTime()/1000});
    }
  }
  
	render() {
		return (
      <div className="modal" tabIndex="-1" id="modalMonitor">
        <div className="modal-dialog">
          <div className="modal-content">
            <form onSubmit={(e) => this.closeModal(e, true)}>
              <div className="modal-header">
                <h5 className="modal-title">
                  {i18next.t("components.benoic-element-modal-monitor")}
                </h5>
                <button type="button" className="btn-close" aria-label="Close" onClick={this.closeModal}></button>
              </div>
              <div className="modal-body">
                <div className="mb-3 row">
                  <div className="col-8">
                    <div className="form-floating">
                      <Flatpickr
                        data-enable-time
                        value={new Date(this.state.to*1000)}
                        name="nextDate"
                        options={{
                          enableTime: true,
                          dateFormat: "Y/m/d H:i",
                          minuteIncrement: 1,
                          time_24hr: true,
                          altInputClass: "form-control",
                          altInput: true,
                          altFormat: "Y/m/d H:i"
                        }}
                        onChange={this.changeDateTo}
                      />
                      <label htmlFor="nextDate" className="form-label">
                        {i18next.t("components.benoic-element-modal-monitor-to")}
                      </label>
                    </div>
                  </div>
                  <div className="col-4">
                    <label className="form-label">
                      {i18next.t("components.benoic-element-modal-monitor-from")}
                    </label>
                    <div className="dropdown">
                      <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                        {i18next.t("components.benoic-element-modal-monitor-from-"+this.state.from)}
                      </button>
                      <ul className="dropdown-menu">
                        <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredFrom(e, 3600)}>{i18next.t("components.benoic-element-modal-monitor-from-3600")}</a></li>
                        <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredFrom(e, 7200)}>{i18next.t("components.benoic-element-modal-monitor-from-7200")}</a></li>
                        <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredFrom(e, 21600)}>{i18next.t("components.benoic-element-modal-monitor-from-21600")}</a></li>
                        <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredFrom(e, 43200)}>{i18next.t("components.benoic-element-modal-monitor-from-43200")}</a></li>
                        <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredFrom(e, 86400)}>{i18next.t("components.benoic-element-modal-monitor-from-86400")}</a></li>
                        <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredFrom(e, 172800)}>{i18next.t("components.benoic-element-modal-monitor-from-172800")}</a></li>
                        <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredFrom(e, 259200)}>{i18next.t("components.benoic-element-modal-monitor-from-259200")}</a></li>
                        <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredFrom(e, 604800)}>{i18next.t("components.benoic-element-modal-monitor-from-604800")}</a></li>
                        <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredFrom(e, 2592000)}>{i18next.t("components.benoic-element-modal-monitor-from-2592000")}</a></li>
                      </ul>
                    </div>
                  </div>
                </div>
                <div className="mb-3">
                  <SimpleMonitor data={this.state.data}
                                 device={this.state.device}
                                 type={this.state.type}
                                 name={this.state.name}
                                 display={this.state.element.display||this.state.name}
                                 value={this.state.element.value}
                                 unit={(this.state.element.options && this.state.element.options.unit)||false}
                                 to={this.state.to}
                                 from={this.state.from}/>
                </div>
              </div>
              <div className="modal-footer">
                <button type="submit" className="btn btn-secondary" onClick={this.closeModal}>{i18next.t("modal.close")}</button>
              </div>
            </form>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalMonitor;
