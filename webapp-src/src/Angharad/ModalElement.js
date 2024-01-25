import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';

class ModalElement extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      device: props.device,
      type: props.type,
      name: props.name,
      element: props.element,
      deviceOverview: props.deviceOverview,
      cb: props.cb
    }

    this.closeModal = this.closeModal.bind(this);
    this.changeLabel = this.changeLabel.bind(this);
    this.changeUnit = this.changeUnit.bind(this);
    this.handleEnabled = this.handleEnabled.bind(this);
    this.handleMonitored = this.handleMonitored.bind(this);
    this.selectMonitoredEvery = this.selectMonitoredEvery.bind(this);
    this.selectBlindBattery = this.selectBlindBattery.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).modal('show');
    $(ReactDOM.findDOMNode(this)).on('hidden.bs.modal', this.state.handleHideModal);
  }

  closeModal(e, result) {
    $(ReactDOM.findDOMNode(this)).modal('hide');
    if (this.state.cb) {
      if (result) {
        this.state.cb(true, this.state.device, this.state.type, this.state.name, this.state.element);
      } else {
        this.state.cb(false);
      }
    }
  }
  
  changeLabel(e) {
    let element = this.state.element;
    element.display = e.target.value;
    this.setState({element: element});
  }
  
  changeUnit(e) {
    let element = this.state.element;
    element.options.unit = e.target.value;
    this.setState({element: element});
  }

  handleEnabled() {
    let element = this.state.element;
    element.enabled = !element.enabled;
    this.setState({element: element});
  }
  
  handleMonitored() {
    let element = this.state.element;
    element.monitored = !element.monitored;
    if (element.monitored && !element.monitored_every) {
      element.monitored_every = 3600;
    }
    this.setState({element: element});
  }
  
  selectMonitoredEvery(e, value) {
    e.preventDefault();
    let element = this.state.element;
    element.monitored_every = value;
    this.setState({element: element});
  }
  
  selectBlindBattery(e, device, sensor) {
    e.preventDefault();
    let element = this.state.element;
    if (device && element) {
      element.options.battery = {
        device: device,
        sensor: sensor
      };
    } else {
      delete (element.options.battery);
    }
    this.setState({element: element});
  }

	render() {
    let monitoredEveryJsx, unitJsx, batteryElementJsx, batterySelected = i18next.t("components.benoic-element-modal-battery-none");
    if (this.state.type === "blind") {
      let sensorList = [<li key={"none"}><a className="dropdown-item" href="#" onClick={(e) => this.selectBlindBattery(e, false, false)}>{i18next.t("components.benoic-element-modal-battery-none")}</a></li>];
      Object.keys(this.state.deviceOverview).forEach(dev => {
        if (this.state.deviceOverview[dev].sensors) {
          Object.keys(this.state.deviceOverview[dev].sensors).forEach(name => {
            if (this.state.element.options?.battery?.device === dev && this.state.element.options?.battery?.sensor === name) {
              batterySelected = this.state.deviceOverview[dev].sensors[name].display;
            }
            sensorList.push(
              <li key={dev+"-"+name}><a className="dropdown-item" href="#" onClick={(e) => this.selectBlindBattery(e, dev, name)}>{this.state.deviceOverview[dev].sensors[name].display}</a></li>
            );
          });
        }
      });
      batteryElementJsx =
        <div>
          <hr/>
          <div className="mb-3">
            <label className="form-label">
              {i18next.t("components.benoic-element-modal-battery")}
            </label>
            <div className="dropdown">
              <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                {batterySelected}
              </button>
              <ul className="dropdown-menu">
                {sensorList}
              </ul>
            </div>
          </div>
        </div>
    }
    if (this.state.element.monitored) {
      monitoredEveryJsx =
        <div className="mb-3">
          <div className="dropdown">
            <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
              {i18next.t("components.benoic-element-modal-monitored-every-"+this.state.element.monitored_every)}
            </button>
            <ul className="dropdown-menu">
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 60)}>{i18next.t("components.benoic-element-modal-monitored-every-60")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 120)}>{i18next.t("components.benoic-element-modal-monitored-every-120")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 300)}>{i18next.t("components.benoic-element-modal-monitored-every-300")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 600)}>{i18next.t("components.benoic-element-modal-monitored-every-600")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 900)}>{i18next.t("components.benoic-element-modal-monitored-every-900")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 1200)}>{i18next.t("components.benoic-element-modal-monitored-every-1200")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 1800)}>{i18next.t("components.benoic-element-modal-monitored-every-1800")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 3600)}>{i18next.t("components.benoic-element-modal-monitored-every-3600")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 7200)}>{i18next.t("components.benoic-element-modal-monitored-every-7200")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 14400)}>{i18next.t("components.benoic-element-modal-monitored-every-14400")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 28800)}>{i18next.t("components.benoic-element-modal-monitored-every-28800")}</a></li>
              <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMonitoredEvery(e, 51000)}>{i18next.t("components.benoic-element-modal-monitored-every-51000")}</a></li>
            </ul>
          </div>
        </div>
    }
    if (this.state.type === "sensor") {
      unitJsx =
        <div className="form-floating mb-3">
          <input type="text" className="form-control" id="componentUnit" onChange={this.changeUnit} value={this.state.element.options.unit}/>
          <label htmlFor="componentUnit">
            {i18next.t("components.benoic-element-modal-unit")}
          </label>
        </div>

    }
		return (
      <div className="modal" tabIndex="-1" id="modalElement">
        <div className="modal-dialog">
          <div className="modal-content">
            <form onSubmit={(e) => this.closeModal(e, true)}>
              <div className="modal-header">
                <h5 className="modal-title">
                  {i18next.t("components.benoic-element-modal-edit")}
                </h5>
                <button type="button" className="btn-close" aria-label="Close" onClick={(e) => this.closeModal(e, false)}></button>
              </div>
              <div className="modal-body">
                <div className="mb-3">
                  <label className="form-label">
                    {i18next.t("components.benoic-element-modal-label")}
                  </label>
                </div>
                <div className="form-floating mb-3">
                  <input type="text" className="form-control" id="componentId" disabled={true} value={this.state.device + " - " + this.state.name}/>
                  <label htmlFor="componentId">
                    {i18next.t("components.benoic-element-modal-identifier")}
                  </label>
                </div>
                <div className="form-floating mb-3">
                  <input type="text" className="form-control" id="componentLabel" onChange={this.changeLabel} value={this.state.element.display}/>
                  <label htmlFor="componentLabel">
                    {i18next.t("components.benoic-element-modal-display")}
                  </label>
                </div>
                {unitJsx}
                <div className="form-check form-switch">
                  <input className="form-check-input"
                         type="checkbox"
                         role="switch"
                         id="componentEnabled"
                         checked={!this.state.element.enabled}
                         onChange={this.handleEnabled} />
                  <label className="form-check-label" htmlFor="componentEnabled">
                    {i18next.t("components.benoic-element-modal-enabled")}
                  </label>
                </div>
                <hr/>
                <div className="form-check form-switch">
                  <input className="form-check-input"
                         type="checkbox"
                         role="switch"
                         id="componentMonitored"
                         checked={this.state.element.monitored}
                         onChange={this.handleMonitored} />
                  <label className="form-check-label" htmlFor="componentMonitored">
                    {i18next.t("components.benoic-element-modal-monitored")}
                  </label>
                </div>
                {monitoredEveryJsx}
                {batteryElementJsx}
              </div>
              <div className="modal-footer">
                <button type="submit" className="btn btn-secondary" onClick={(e) => this.closeModal(e, false)}>{i18next.t("modal.close")}</button>
                <button type="button" className="btn btn-primary" onClick={(e) => this.closeModal(e, true)}>{i18next.t("modal.ok")}</button>
              </div>
            </form>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalElement;
