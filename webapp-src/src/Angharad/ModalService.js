import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';

class ModalService extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      type: props.type,
      element: props.element,
      name: props.name,
      add: props.add,
      serviceList: props.serviceList,
      deviceOverview: props.deviceOverview,
      cb: props.cb,
      isValid: true,
      nameValid: true,
      nameError: false,
      errorParam: {}
    }

    this.closeModal = this.closeModal.bind(this);
    this.changeName = this.changeName.bind(this);
    this.changeDecription = this.changeDecription.bind(this);
    this.selectAttachedSwitch = this.selectAttachedSwitch.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).modal('show');
    $(ReactDOM.findDOMNode(this)).on('hidden.bs.modal', this.state.handleHideModal);
  }

  changeName(e) {
    let element = this.state.element, isValid = true, nameValid = true, nameError = false;
    element.name = e.target.value;
    if (!element.name) {
      nameValid = false;
    } else {
      this.state.serviceList.forEach((service) => {
        if (service.name === this.state.type) {
          service.element.forEach(curElement => {
            if (element.name === curElement.name) {
              isValid = false;
              nameValid = false;
              nameError = i18next.t("services.carleon-service-name-taken");
            }
          });
        }
      });
    }
    this.setState({element: element, isValid: isValid, nameValid: nameValid, nameError: nameError});
  }
  
  changeProperty(e, name, type = "string") {
    let element = this.state.element;
    if (type === "string") {
      element[name] = e.target.value;
    } else if (type === "number") {
      element[name] = parseInt(e.target.value);
    } else if (type === "boolean") {
      element[name] = !element[name];
    }
    this.setState({element: element});
  }

  changeDecription(e) {
    let element = this.state.element;
    element.description = e.target.value;
    this.setState({element: element});
  }
  
  selectAttachedSwitch(e, dev, sw) {
    e.preventDefault();
    let element = this.state.element;
    element.device = dev;
    element["switch"] = sw;
    this.setState({element: element});
  }

  closeModal(e, result) {
    $(ReactDOM.findDOMNode(this)).modal('hide');
    if (this.state.cb) {
      if (result) {
        this.state.cb(true, this.state.type, this.state.element, this.state.add);
      } else {
        this.state.cb(false);
      }
    }
  }
  
	render() {
    let optionsJsx, requiredStar, errorMessageJsx, errorClass = "";
    if (this.state.type === "service-mpd") {
      let errorHostJsx;
      if (this.state.errorParam.host) {
        errorHostJsx = <span className="text-danger"> {this.state.nameError}</span>;
      }
      let attachedSwitch = i18next.t("services.carleon-service-modal-mpd-switch-none");
      let switchList = [<li key={0}><a className="dropdown-item" href="#" onClick={(e) => this.selectAttachedSwitch(e, false, false)}>{i18next.t("services.carleon-service-modal-mpd-switch-none")}</a></li>];
      Object.keys(this.state.deviceOverview).forEach(dev => {
        if (this.state.deviceOverview[dev].switches) {
          Object.keys(this.state.deviceOverview[dev].switches).forEach(sw => {
            if (this.state.deviceOverview[dev].switches[sw].enabled) {
              switchList.push(<li key={dev+"$"+sw}><a className="dropdown-item" href="#" onClick={(e) => this.selectAttachedSwitch(e, dev, sw)}>{this.state.deviceOverview[dev].switches[sw].display}</a></li>);
            }
            if (this.state.element.device && this.state.element["switch"] && dev === this.state.element.device && sw === this.state.element["switch"]) {
              attachedSwitch = this.state.deviceOverview[dev].switches[sw].display;
            }
          });
        }
      });
      optionsJsx =
      <div>
        <div className="form-floating mb-3">
          <input type="text" className={"form-control" + errorClass} id="serviceHost" value={this.state.element.host||""} onChange={(e) => this.changeProperty(e, "host")}/>
          <label htmlFor="serviceHost">
            {i18next.t("services.carleon-service-modal-mpd-host")}
            <span className="text-danger"> *</span>
          </label>
          {errorHostJsx}
        </div>
        <div className="form-floating mb-3">
          <input type="number" min="0" max="65535" step="1" className="form-control" id="servicePort" value={this.state.element.port||""} onChange={(e) => this.changeProperty(e, "port", "number")}/>
          <label htmlFor="servicePort">
            {i18next.t("services.carleon-service-modal-mpd-port")}
          </label>
        </div>
        <div className="form-floating mb-3">
          <input type="password" className="form-control" id="servicePassword" value={this.state.element.password||""} onChange={(e) => this.changeProperty(e, "password")}/>
          <label htmlFor="servicePassword">
            {i18next.t("services.carleon-service-modal-mpd-password")}
          </label>
        </div>
        <div className="mb-3">
          <label className="form-label">
            {i18next.t("services.carleon-service-modal-mpd-switch")}
          </label>
          <div className="dropdown">
            <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
              {attachedSwitch}
            </button>
            <ul className="dropdown-menu">
              {switchList}
            </ul>
          </div>
        </div>
      </div>
    } else if (this.state.type === "service-weathermap") {
      let errorAppidJsx;
      optionsJsx =
      <div>
        <div className="form-floating mb-3">
          <input type="text" className={"form-control" + errorClass} id="serviceUri" maxLength="128" value={this.state.element.uri||""} onChange={(e) => this.changeProperty(e, "uri")}/>
          <label htmlFor="serviceUri">
            {i18next.t("services.carleon-service-modal-weathermap-uri")}
            <span className="text-danger"> *</span>
          </label>
          {errorAppidJsx}
        </div>
        <div className="form-floating mb-3">
          <input type="text" className={"form-control" + errorClass} id="serviceAppid" maxLength="128" value={this.state.element.appid||""} onChange={(e) => this.changeProperty(e, "appid")}/>
          <label htmlFor="serviceAppid">
            {i18next.t("services.carleon-service-modal-weathermap-appid")}
            <span className="text-danger"> *</span>
          </label>
          {errorAppidJsx}
        </div>
        <div className="form-floating mb-3">
          <input type="text" className={"form-control" + errorClass} id="serviceLat" maxLength="128" value={this.state.element.lat||""} onChange={(e) => this.changeProperty(e, "lat")}/>
          <label htmlFor="serviceLat">
            {i18next.t("services.carleon-service-modal-weathermap-lat")}
            <span className="text-danger"> *</span>
          </label>
          {errorAppidJsx}
        </div>
        <div className="form-floating mb-3">
          <input type="text" className={"form-control" + errorClass} id="serviceLon" maxLength="16" value={this.state.element.lon||""} onChange={(e) => this.changeProperty(e, "lon")}/>
          <label htmlFor="serviceLon">
            {i18next.t("services.carleon-service-modal-weathermap-lon")}
            <span className="text-danger"> *</span>
          </label>
          {errorAppidJsx}
        </div>
        <div className="form-floating mb-3">
          <select className="form-select" id="serviceUnits" onChange={(e) => this.changeProperty(e, "units")} value={this.state.element.units}>
            <option value="standard">{i18next.t("services.carleon-service-modal-weathermap-units-standard")}</option>
            <option value="metric">{i18next.t("services.carleon-service-modal-weathermap-units-metric")}</option>
            <option value="imperial">{i18next.t("services.carleon-service-modal-weathermap-units-imperial")}</option>
          </select>
          <label htmlFor="serviceUnits">
            {i18next.t("services.carleon-service-modal-weathermap-units")}
          </label>
        </div>
        <div className="form-floating mb-3">
          <select className="form-select" id="serviceLang" onChange={(e) => this.changeProperty(e, "lang")} value={this.state.element.lang}>
            <option value="fr">{i18next.t("services.carleon-service-modal-weathermap-lang-fr")}</option>
            <option value="en">{i18next.t("services.carleon-service-modal-weathermap-lang-en")}</option>
          </select>
          <label htmlFor="serviceLang">
            {i18next.t("services.carleon-service-modal-weathermap-lang")}
          </label>
        </div>
      </div>
    }
    if (this.state.add) {
      requiredStar = <span className="text-danger"> *</span>;
      if (!this.state.nameValid) {
        if (this.state.nameError) {
          errorMessageJsx = <span className="text-danger"> {this.state.nameError}</span>
        }
        errorClass = " is-invalid";
      }
    }
		return (
      <div className="modal" tabIndex="-1" id="modalService">
        <div className="modal-dialog">
          <div className="modal-content">
            <form onSubmit={(e) => this.closeModal(e, true)}>
              <div className="modal-header">
                <h5 className="modal-title">
                  {i18next.t("services.carleon-service-modal-edit")}
                </h5>
                <button type="button" className="btn-close" aria-label="Close" onClick={(e) => this.closeModal(e, false)}></button>
              </div>
              <div className="modal-body">
                <div className="mb-3">
                  {i18next.t("services.carleon-service-modal-type-"+this.state.type)}
                </div>
                <div className="form-floating mb-3">
                  <input type="text" className={"form-control" + errorClass} id="serviceName" value={this.state.element.name} disabled={!this.state.add} onChange={this.changeName}/>
                  <label htmlFor="serviceName">
                    {i18next.t("services.carleon-service-modal-name")}
                    {requiredStar}
                  </label>
                  {errorMessageJsx}
                </div>
                <div className="form-floating mb-3">
                  <input type="text" className="form-control" id="serviceDescription" value={this.state.element.description} onChange={this.changeDecription}/>
                  <label htmlFor="serviceDescription">
                    {i18next.t("services.carleon-service-modal-description")}
                  </label>
                </div>
                {optionsJsx}
              </div>
              <div className="modal-footer">
                <button type="submit" className="btn btn-secondary" onClick={(e) => this.closeModal(e, false)}>{i18next.t("modal.close")}</button>
                <button type="button" className="btn btn-primary" onClick={(e) => this.closeModal(e, true)} disabled={!this.state.isValid || !this.state.element.name}>{i18next.t("modal.ok")}</button>
              </div>
            </form>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalService;
