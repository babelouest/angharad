import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';

class ModalDevice extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      add: props.add,
      device: props.device,
      deviceTypes: props.deviceTypes,
      deviceList: props.deviceList,
      deviceError: {},
      typeError: false,
      nameError: false,
      cb: props.cb
    }

    this.closeModal = this.closeModal.bind(this);
    this.selectType = this.selectType.bind(this);
    this.changeName = this.changeName.bind(this);
    this.changeOptionString = this.changeOptionString.bind(this);
    this.changeOptionNumeric = this.changeOptionNumeric.bind(this);
    this.changeOptionBoolean = this.changeOptionBoolean.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }

  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).modal('show');
    $(ReactDOM.findDOMNode(this)).on('hidden.bs.modal', this.state.handleHideModal);
  }

  closeModal(e, result) {
    if (this.state.cb) {
      if (!result) {
        this.setState({deviceError: {}, nameError: false, typeError: false}, () => {
          this.state.cb(false);
          $(ReactDOM.findDOMNode(this)).modal('hide');
        });
      } else {
        let deviceError = {}, hasError = false, nameError = false, typeError = false;
        if (this.state.add) {
          if (!this.state.device.name) {
            hasError = true;
            nameError = 1;
          } else {
            this.state.deviceList.forEach(device => {
              if (device.name === this.state.device.name) {
                hasError = true;
                nameError = 2;
              }
            });
          }
        }
        if (!this.state.device.type_uid) {
          typeError = true;
        } else {
          this.state.deviceTypes.forEach(type => {
            if (this.state.device.type_uid === type.uid) {
              type.options.forEach(option => {
                if (!option.optional && !this.state.device.options[option.name]) {
                  deviceError[option.name] = true;
                  hasError = true;
                }
              });
            }
          });
        }
        if (!hasError) {
          this.setState({deviceError: deviceError, nameError: nameError, typeError: typeError, device: {}}, () => {
            this.state.cb(true, this.state.device);
            $(ReactDOM.findDOMNode(this)).modal('hide');
          });
        } else {
          this.setState({deviceError: deviceError, nameError: nameError, typeError: typeError});
        }
      }
    }
  }
  
  selectType(e, uid) {
    e.preventDefault();
    let device = this.state.device;
    device.type_uid = uid;
    this.setState({device: device});
  }
  
  changeName(e) {
    let device = this.state.device;
    device.name = e.target.value;
    this.setState({device: device});
  }
  
  changeOptionString(e, name) {
    let device = this.state.device;
    device.options[name] = e.target.value;
    this.setState({device: device});
  }
  
  changeOptionNumeric(e, name) {
    let device = this.state.device;
    device.options[name] = parseInt(e.target.value);
    this.setState({device: device});
  }
  
  changeOptionBoolean(e, name) {
    let device = this.state.device;
    device.options[name] = !device.options[name];
    this.setState({device: device});
  }

	render() {
    let title;
    if (this.state.add) {
      title = i18next.t("config.benoic-device-modal-title-add");
    } else {
      title = i18next.t("config.benoic-device-modal-title-edit");
    }
    let deviceOptionJsx = [], nameJsx, nameErrorJsx, nameErrorClass = "", typeErrorJsx;
    let typeList = [], curType = i18next.t("config.benoic-device-modal-type-list-select");
    this.state.deviceTypes.forEach((type, index) => {
      if (type.enabled) {
        typeList.push(
          <li key={index}><a className="dropdown-item" href="#" onClick={(e) => this.selectType(e, type.uid)}>{type.name}</a></li>
        );
        if (this.state.device.type_uid === type.uid) {
          curType = type.name;
        }
      }
    });
    if (this.state.device.type_uid) {
      deviceOptionJsx.push(<hr key={-2}/>);
      deviceOptionJsx.push(<h5 key={-1}>{i18next.t("config.benoic-device-modal-options")}</h5>);
      this.state.deviceTypes.forEach(type => {
        if (this.state.device.type_uid === type.uid) {
          type.options.forEach((option, index) => {
            let requiredStar = "", errorMessageJsx, errorClass = "";
            if (!option.optional) {
              requiredStar = <span className="text-danger"> *</span>;
            }
            if (this.state.deviceError[option.name]) {
              errorMessageJsx = <span className="text-danger"> {i18next.t("config.benoic-device-modal-option-error")}</span>
              errorClass = " is-invalid";
            }
            if (option.type === "string") {
              deviceOptionJsx.push(
                <div className="form-floating mb-3" key={index}>
                  <input type="text"
                         className={"form-control" + errorClass}
                         id={"deviceOption"+option.name}
                         onChange={(e) => this.changeOptionString(e, option.name)}
                         value={this.state.device.options[option.name]||""}/>
                  <label htmlFor={"deviceOption"+option.name}>
                    {i18next.t("config.benoic-device-modal-option-"+option.name, option.name)}
                    {requiredStar}
                    {errorMessageJsx}
                  </label>
                </div>
              );
            } else if (option.type === "numeric") {
              deviceOptionJsx.push(
                <div className="form-floating mb-3" key={index}>
                  <input type="number"
                         className={"form-control" + errorClass}
                         id={"deviceOption"+option.name}
                         onChange={(e) => this.changeOptionNumeric(e, option.name)}
                         value={this.state.device.options[option.name]||""}/>
                  <label htmlFor={"deviceOption"+option.name}>
                    {i18next.t("config.benoic-device-modal-option-"+option.name, option.name)}
                    {requiredStar}
                    {errorMessageJsx}
                  </label>
                </div>
              );
            } else if (option.type === "boolean") {
              deviceOptionJsx.push(
                <div className="form-check" key={index}>
                  <input type="checkbox"
                         className="form-check-input"
                         value="" id={"deviceOption"+option.name}
                         onChange={(e) => this.changeOptionBoolean(e, option.name)}
                         checked={!!this.state.device.options[option.name]}/>
                  <label className="form-check-label" htmlFor={"deviceOption"+option.name}>
                    {i18next.t("config.benoic-device-modal-option-"+option.name, option.name)}
                  </label>
                </div>
              );
            }
          });
        }
      });
    }
    if (this.state.nameError === 1) {
      nameErrorJsx = <span className="text-danger"> {i18next.t("config.benoic-device-modal-option-error")}</span>
      nameErrorClass = " is-invalid";
    } else if (this.state.nameError === 2) {
      nameErrorJsx = <span className="text-danger"> {i18next.t("config.benoic-device-modal-error-name-duplicate")}</span>
      nameErrorClass = " is-invalid";
    }
    if (this.state.typeError) {
      typeErrorJsx = <span className="text-danger"> {i18next.t("config.benoic-device-modal-option-error")}</span>
    }
		return (
      <div className="modal" tabIndex="-1" id="modalDevice">
        <div className="modal-dialog">
          <div className="modal-content">
            <form onSubmit={(e) => this.closeModal(e, true)}>
              <div className="modal-header">
                <h5 className="modal-title">
                  {title}
                </h5>
                <button type="button" className="btn-close" aria-label="Close" onClick={(e) => this.closeModal(e, false)}></button>
              </div>
              <div className="modal-body">
                <div className="mb-3">
                  <label className="form-label">
                    {i18next.t("config.benoic-device-modal-type-list")}
                  </label>
                  <div className="dropdown">
                    <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false" disabled={!this.state.add}>
                      {curType}
                    </button>
                    <ul className="dropdown-menu">
                      {typeList}
                    </ul>
                  </div>
                  <label className="form-label">
                    {typeErrorJsx}
                  </label>
                </div>
                <div className="form-floating mb-3">
                  <input type="text" className={"form-control" + nameErrorClass} id="deviceName" onChange={this.changeName} value={this.state.device.name} disabled={!this.state.add}/>
                  <label htmlFor="deviceName">
                    {i18next.t("config.benoic-device-modal-name")}
                    <span className="text-danger"> *</span>
                    {nameErrorJsx}
                  </label>
                </div>
                {deviceOptionJsx}
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

export default ModalDevice;
