import React, { Component } from 'react';
import ReactDOM from 'react-dom';
import i18next from 'i18next';

class ModalScript extends Component {
  constructor(props) {
    super(props);

    this.state = {
      handleHideModal: props.handleHideModal,
      script: props.script,
      add: props.add,
      serviceList: props.serviceList,
      deviceOverview: props.deviceOverview,
      scriptList: props.scriptList,
      cb: props.cb,
      isValid: true,
      nameValid: true,
      nameError: false,
      addActionParams: false,
      curActionType: "none"
    }

    this.closeModal = this.closeModal.bind(this);
    this.changeName = this.changeName.bind(this);
    this.changeDescription = this.changeDescription.bind(this);
    this.addAction = this.addAction.bind(this);
    this.addNewAction = this.addNewAction.bind(this);
    this.removeNewAction = this.removeNewAction.bind(this);
    this.removeActionAt = this.removeActionAt.bind(this);
    this.selectActionType = this.selectActionType.bind(this);
    this.selectActionSwitch = this.selectActionSwitch.bind(this);
    this.selectActionCommand = this.selectActionCommand.bind(this);
    this.changeDimmerActionCommand = this.changeDimmerActionCommand.bind(this);
    this.selectMpdPlayer = this.selectMpdPlayer.bind(this);
    this.changeMpdUri = this.changeMpdUri.bind(this);
    this.changeMpdVolume = this.changeMpdVolume.bind(this);
    this.changeMpdPlaylist = this.changeMpdPlaylist.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  componentDidMount() {
    $(ReactDOM.findDOMNode(this)).modal('show');
    $(ReactDOM.findDOMNode(this)).on('hidden.bs.modal', this.state.handleHideModal);
  }

  changeName(e) {
    let script = this.state.script;
    script.name = e.target.value;
    let isValid = true, nameValid = true, nameError = false;
    if (!script.name) {
      nameValid = false;
    } else {
      this.state.scriptList.forEach((curScript) => {
        if (curScript.name === script.name) {
          isValid = false;
          nameValid = false;
          nameError = i18next.t("scripts.name-taken");
        }
      });
    }
    this.setState({script: script, isValid: isValid, nameValid: nameValid, nameError: nameError});
  }
  
  changeDescription(e) {
    let script = this.state.script;
    script.description = e.target.value;
    this.setState({script: script});
  }
  
  addAction() {
    let addActionParams = {submodule: "benoic", command: 1, parameters: {element_type: "switch"}};
    Object.keys(this.state.deviceOverview).forEach(dev => {
      if (this.state.deviceOverview[dev].switches) {
        Object.keys(this.state.deviceOverview[dev].switches).forEach(sw => {
          if (this.state.deviceOverview[dev].switches[sw].enabled && !addActionParams.parameters.device && !addActionParams.element) {
            addActionParams.parameters.device = dev;
            addActionParams.element = sw;
          }
        });
      }
    });
    this.setState({addActionParams: addActionParams});
  }
  
  addNewAction() {
    let script = this.state.script;
    script.actions.push(this.state.addActionParams);
    this.setState({script: script, addActionParams: false, curActionType: "none"});
  }
  
  removeNewAction() {
    this.setState({addActionParams: false});
  }
  
  removeActionAt(e, index) {
    let script = this.state.script;
    script.actions.splice(index, 1);
    this.setState({script: script, addActionParams: false});
  }
  
  selectActionType(e, type) {
    e.preventDefault();
    let addActionParams = this.state.addActionParams;
    if (type === "switch") {
      addActionParams.parameters.element_type = type;
      addActionParams.submodule = "benoic";
      addActionParams.command = 1;
      delete(addActionParams.parameters.device);
      delete(addActionParams.element);
      Object.keys(this.state.deviceOverview).forEach(dev => {
        if (this.state.deviceOverview[dev].switches) {
          Object.keys(this.state.deviceOverview[dev].switches).forEach(sw => {
            if (this.state.deviceOverview[dev].switches[sw].enabled && !addActionParams.parameters.device && !addActionParams.element) {
              addActionParams.parameters.device = dev;
              addActionParams.element = sw;
            }
          });
        }
      });
    } else if (type === "dimmer") {
      addActionParams.parameters.element_type = type;
      addActionParams.submodule = "benoic";
      addActionParams.command = 0;
      delete(addActionParams.parameters.device);
      delete(addActionParams.element);
      Object.keys(this.state.deviceOverview).forEach(dev => {
        if (this.state.deviceOverview[dev].dimmers) {
          Object.keys(this.state.deviceOverview[dev].dimmers).forEach(di => {
            if (this.state.deviceOverview[dev].dimmers[di].enabled && !addActionParams.parameters.device && !addActionParams.element) {
              addActionParams.parameters.device = dev;
              addActionParams.element = di;
            }
          });
        }
      });
    } else if (type === "blind") {
      addActionParams.parameters.element_type = type;
      addActionParams.submodule = "benoic";
      addActionParams.command = 0;
      delete(addActionParams.parameters.device);
      delete(addActionParams.element);
      Object.keys(this.state.deviceOverview).forEach(dev => {
        if (this.state.deviceOverview[dev].blinds) {
          Object.keys(this.state.deviceOverview[dev].blinds).forEach(bl => {
            if (this.state.deviceOverview[dev].blinds[bl].enabled && !addActionParams.parameters.device && !addActionParams.element) {
              addActionParams.parameters.device = dev;
              addActionParams.element = bl;
            }
          });
        }
      });
    } else if (type === "mpd-clear-playlist") {
      addActionParams.submodule = "carleon";
      addActionParams.command = "clear_playlist";
      addActionParams.parameters = {};
      addActionParams.parameters.service = "service-mpd";
      delete(addActionParams.element);
      this.state.serviceList.forEach(service => {
        if (service.name === "service-mpd") {
          service.element.forEach(element => {
            if (!addActionParams.element) {
              addActionParams.element = element.name;
            }
          });
        }
      });
    } else if (type === "mpd-play-song") {
      addActionParams.submodule = "carleon";
      addActionParams.command = "play_song";
      addActionParams.parameters = {uri: ""};
      addActionParams.parameters.service = "service-mpd";
      delete(addActionParams.element);
      this.state.serviceList.forEach(service => {
        if (service.name === "service-mpd") {
          service.element.forEach(element => {
            if (!addActionParams.element) {
              addActionParams.element = element.name;
            }
          });
        }
      });
    } else if (type === "mpd-action") {
      addActionParams.submodule = "carleon";
      addActionParams.command = "action";
      addActionParams.parameters = {action: "play"};
      addActionParams.parameters.service = "service-mpd";
      delete(addActionParams.element);
      this.state.serviceList.forEach(service => {
        if (service.name === "service-mpd") {
          service.element.forEach(element => {
            if (!addActionParams.element) {
              addActionParams.element = element.name;
            }
          });
        }
      });
    } else if (type === "mpd-set-volume") {
      addActionParams.submodule = "carleon";
      addActionParams.command = "set_volume";
      addActionParams.parameters = {volume: 0};
      addActionParams.parameters.service = "service-mpd";
      delete(addActionParams.element);
      this.state.serviceList.forEach(service => {
        if (service.name === "service-mpd") {
          service.element.forEach(element => {
            if (!addActionParams.element) {
              addActionParams.element = element.name;
            }
          });
        }
      });
    } else if (type === "mpd-set-playlist") {
      addActionParams.submodule = "carleon";
      addActionParams.command = "load_playlist";
      addActionParams.parameters = {playlist: ""};
      addActionParams.parameters.service = "service-mpd";
      delete(addActionParams.element);
      this.state.serviceList.forEach(service => {
        if (service.name === "service-mpd") {
          service.element.forEach(element => {
            if (!addActionParams.element) {
              addActionParams.element = element.name;
            }
          });
        }
      });
    }
    this.setState({addActionParams: addActionParams, curActionType: type});
  }
  
  selectActionSwitch(e, device, element) {
    e.preventDefault();
    let addActionParams = this.state.addActionParams;
    addActionParams.parameters.device = device;
    addActionParams.element = element;
    this.setState({addActionParams: addActionParams});
  }
  
  selectActionCommand(e, command) {
    e.preventDefault();
    let addActionParams = this.state.addActionParams;
    addActionParams.command = command;
    this.setState({addActionParams: addActionParams});
  }
  
  changeDimmerActionCommand(e) {
    e.preventDefault();
    let addActionParams = this.state.addActionParams;
    addActionParams.command = parseInt(e.target.value);
    this.setState({addActionParams: addActionParams});
  }
  
  selectMpdPlayer(e, name) {
    e.preventDefault();
    let addActionParams = this.state.addActionParams;
    addActionParams.element = name;
    this.setState({addActionParams: addActionParams});
  }
  
  changeMpdUri(e) {
    e.preventDefault();
    let addActionParams = this.state.addActionParams;
    addActionParams.parameters.uri = e.target.value;
    this.setState({addActionParams: addActionParams});
  }
  
  changeMpdPlaylist(e) {
    e.preventDefault();
    let addActionParams = this.state.addActionParams;
    addActionParams.parameters.playlist = e.target.value;
    this.setState({addActionParams: addActionParams});
  }
  
  selectMpdAction(e, action) {
    e.preventDefault();
    let addActionParams = this.state.addActionParams;
    addActionParams.parameters.action = action;
    this.setState({addActionParams: addActionParams});
  }
  
  changeMpdVolume(e) {
    e.preventDefault();
    let addActionParams = this.state.addActionParams;
    addActionParams.parameters.volume = parseInt(e.target.value);
    this.setState({addActionParams: addActionParams});
  }
  
  closeModal(e, result) {
    $(ReactDOM.findDOMNode(this)).modal('hide');
    if (this.state.cb) {
      if (result) {
        this.state.cb(true, this.state.script, this.state.add);
      } else {
        this.state.cb(false);
      }
    }
  }
  
	render() {
    let requiredStar, errorMessageJsx, errorClass = "";
    if (this.state.add) {
      requiredStar = <span className="text-danger"> *</span>;
      if (!this.state.nameValid) {
        if (this.state.nameError) {
          errorMessageJsx = <span className="text-danger"> {this.state.nameError}</span>
        }
        errorClass = " is-invalid";
      }
    }
    let actionListJsx = [], addActionJsx, actionParamJsx;

    if (this.state.script.actions) {
      this.state.script.actions.forEach((action, index) => {
        if (action.parameters.element_type === "switch") {
          let name = i18next.t("scripts.modal-action-name-not-found", {dev: action.parameters.device, name: action.element}), commandJsx = action.command===1?i18next.t("scripts.modal-action-switch-command-1"):i18next.t("scripts.modal-action-switch-command-0");
          Object.keys(this.state.deviceOverview).forEach(dev => {
            if (this.state.deviceOverview[dev].switches) {
              Object.keys(this.state.deviceOverview[dev].switches).forEach(sw => {
                if (dev === action.parameters.device && sw === action.element) {
                  name = this.state.deviceOverview[dev].switches[sw].display;
                }
              });
            }
          });
          actionListJsx.push(
            <div key={index} className="mb-3">
              <div className="row">
                <div className="col-6">
                  <label className="d-flex align-items-center">
                    <i className="fa fa-power-off elt-left" aria-hidden="true">
                    </i>
                    {name}
                  </label>
                </div>
                <div className="col-4">
                  {commandJsx}
                </div>
                <div className="col-2">
                  <button type="button" className="btn btn-secondary btn-sm" onClick={(e) => this.removeActionAt(e, index)}>
                    <i className="fa fa-trash" aria-hidden="true"></i>
                  </button>
                </div>
              </div>
            </div>
          );
        } else if (action.parameters.element_type === "dimmer") {
          let name = i18next.t("scripts.modal-action-name-not-found", {dev: action.parameters.device, name: action.element}), commandJsx = action.command + "%";
          Object.keys(this.state.deviceOverview).forEach(dev => {
            if (this.state.deviceOverview[dev].dimmers) {
              Object.keys(this.state.deviceOverview[dev].dimmers).forEach(di => {
                if (dev === action.parameters.device && di === action.element) {
                  name = this.state.deviceOverview[dev].dimmers[di].display;
                }
              });
            }
          });
          actionListJsx.push(
            <div key={index} className="mb-3">
              <div className="row">
                <div className="col-6">
                  <label className="d-flex align-items-center">
                    <i className="fa fa-lightbulb-o elt-left" aria-hidden="true">
                    </i>
                    {name}
                  </label>
                </div>
                <div className="col-4">
                  {commandJsx}
                </div>
                <div className="col-2">
                  <button type="button" className="btn btn-secondary btn-sm" onClick={(e) => this.removeActionAt(e, index)}>
                    <i className="fa fa-trash" aria-hidden="true"></i>
                  </button>
                </div>
              </div>
            </div>
          );
        } else if (action.parameters.element_type === "blind") {
          let name = i18next.t("scripts.modal-action-name-not-found", {dev: action.parameters.device, name: action.element}), commandJsx = action.command + "%";
          Object.keys(this.state.deviceOverview).forEach(dev => {
            if (this.state.deviceOverview[dev].blinds) {
              Object.keys(this.state.deviceOverview[dev].blinds).forEach(bl => {
                if (dev === action.parameters.device && bl === action.element) {
                  name = this.state.deviceOverview[dev].blinds[bl].display;
                }
              });
            }
          });
          actionListJsx.push(
            <div key={index} className="mb-3">
              <div className="row">
                <div className="col-6">
                  <label className="d-flex align-items-center">
                    <i className="fa fa-window-maximize elt-left" aria-hidden="true">
                    </i>
                    {name}
                  </label>
                </div>
                <div className="col-4">
                  {commandJsx}
                </div>
                <div className="col-2">
                  <button type="button" className="btn btn-secondary btn-sm" onClick={(e) => this.removeActionAt(e, index)}>
                    <i className="fa fa-trash" aria-hidden="true"></i>
                  </button>
                </div>
              </div>
            </div>
          );
        } else if (action.parameters.service === "service-mpd") {
          if (action.command === "clear_playlist") {
            actionListJsx.push(
              <div key={index} className="mb-3">
                <div className="row">
                  <div className="col-4">
                    <label className="d-flex align-items-center">
                      <i className="fa fa-music elt-left" aria-hidden="true">
                      </i>
                      {i18next.t("scripts.modal-action-list-mpd-clear-playlist")}
                    </label>
                  </div>
                  <div className="col-6">
                    {action.element}
                  </div>
                  <div className="col-2">
                    <button type="button" className="btn btn-secondary btn-sm" onClick={(e) => this.removeActionAt(e, index)}>
                      <i className="fa fa-trash" aria-hidden="true"></i>
                    </button>
                  </div>
                </div>
              </div>
            );
          } else if (action.command === "play_song") {
            actionListJsx.push(
              <div key={index} className="mb-3">
                <div className="row">
                  <div className="col-4">
                    <label className="d-flex align-items-center">
                      <i className="fa fa-music elt-left" aria-hidden="true">
                      </i>
                      {i18next.t("scripts.modal-action-list-mpd-play-song")}
                    </label>
                  </div>
                  <div className="col-6">
                    {action.element} - {action.parameters.uri}
                  </div>
                  <div className="col-2">
                    <button type="button" className="btn btn-secondary btn-sm" onClick={(e) => this.removeActionAt(e, index)}>
                      <i className="fa fa-trash" aria-hidden="true"></i>
                    </button>
                  </div>
                </div>
              </div>
            );
          } else if (action.command === "action") {
            actionListJsx.push(
              <div key={index} className="mb-3">
                <div className="row">
                  <div className="col-4">
                    <label className="d-flex align-items-center">
                      <i className="fa fa-music elt-left" aria-hidden="true">
                      </i>
                      {i18next.t("scripts.modal-action-list-mpd-action")}
                    </label>
                  </div>
                  <div className="col-6">
                    {action.element} - {i18next.t("scripts.modal-mpd-action-list-"+action.parameters.action)}
                  </div>
                  <div className="col-2">
                    <button type="button" className="btn btn-secondary btn-sm" onClick={(e) => this.removeActionAt(e, index)}>
                      <i className="fa fa-trash" aria-hidden="true"></i>
                    </button>
                  </div>
                </div>
              </div>
            );
          } else if (action.command === "set_volume") {
            actionListJsx.push(
              <div key={index} className="mb-3">
                <div className="row">
                  <div className="col-4">
                    <label className="d-flex align-items-center">
                      <i className="fa fa-music elt-left" aria-hidden="true">
                      </i>
                      {i18next.t("scripts.modal-action-list-mpd-set-volume")}
                    </label>
                  </div>
                  <div className="col-6">
                    {action.element} - {action.parameters.volume}%
                  </div>
                  <div className="col-2">
                    <button type="button" className="btn btn-secondary btn-sm" onClick={(e) => this.removeActionAt(e, index)}>
                      <i className="fa fa-trash" aria-hidden="true"></i>
                    </button>
                  </div>
                </div>
              </div>
            );
          } else if (action.command === "load_playlist") {
            actionListJsx.push(
              <div key={index} className="mb-3">
                <div className="row">
                  <div className="col-4">
                    <label className="d-flex align-items-center">
                      <i className="fa fa-music elt-left" aria-hidden="true">
                      </i>
                      {i18next.t("scripts.modal-action-list-mpd-set-playlist")}
                    </label>
                  </div>
                  <div className="col-6">
                    {action.element} - {action.parameters.playlist}
                  </div>
                  <div className="col-2">
                    <button type="button" className="btn btn-secondary btn-sm" onClick={(e) => this.removeActionAt(e, index)}>
                      <i className="fa fa-trash" aria-hidden="true"></i>
                    </button>
                  </div>
                </div>
              </div>
            );
          }
        }
      });
    }
    if (this.state.addActionParams) {
      addActionJsx =
      <div className="mb-3">
        <label className="form-label">
          {i18next.t("scripts.modal-action-list")}
        </label>
        <div className="dropdown">
          <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
            {i18next.t("scripts.modal-action-list-"+this.state.curActionType)}
          </button>
          <ul className="dropdown-menu">
            <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionType(e, "switch")}>{i18next.t("scripts.modal-action-list-switch")}</a></li>
            <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionType(e, "dimmer")}>{i18next.t("scripts.modal-action-list-dimmer")}</a></li>
            <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionType(e, "blind")}>{i18next.t("scripts.modal-action-list-blind")}</a></li>
            <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionType(e, "mpd-clear-playlist")}>{i18next.t("scripts.modal-action-list-mpd-prefix")+i18next.t("scripts.modal-action-list-mpd-clear-playlist")}</a></li>
            <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionType(e, "mpd-play-song")}>{i18next.t("scripts.modal-action-list-mpd-prefix")+i18next.t("scripts.modal-action-list-mpd-play-song")}</a></li>
            <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionType(e, "mpd-action")}>{i18next.t("scripts.modal-action-list-mpd-prefix")+i18next.t("scripts.modal-action-list-mpd-action")}</a></li>
            <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionType(e, "mpd-set-playlist")}>{i18next.t("scripts.modal-action-list-mpd-prefix")+i18next.t("scripts.modal-action-list-mpd-set-playlist")}</a></li>
            <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionType(e, "mpd-set-volume")}>{i18next.t("scripts.modal-action-list-mpd-prefix")+i18next.t("scripts.modal-action-list-mpd-set-volume")}</a></li>
          </ul>
        </div>
      </div>
      if (this.state.curActionType === "switch") {
        let switchName = i18next.t("scripts.modal-action-select"), switchList = [];
        if (this.state.addActionParams.parameters.device && this.state.addActionParams.element) {
          switchName = this.state.deviceOverview[this.state.addActionParams.parameters.device]?.switches[this.state.addActionParams.element]?.display;
        }
        Object.keys(this.state.deviceOverview).forEach(dev => {
          if (this.state.deviceOverview[dev].switches) {
            Object.keys(this.state.deviceOverview[dev].switches).forEach(sw => {
              if (this.state.deviceOverview[dev].switches[sw].enabled) {
                switchList.push(<li key={dev+"$"+sw}><a className="dropdown-item" href="#" onClick={(e) => this.selectActionSwitch(e, dev, sw)}>{this.state.deviceOverview[dev].switches[sw].display}</a></li>);
              }
            });
          }
        });
        actionParamJsx =
        <div className="border rounded script-new-action">
          <div className="mb-3">
            <label className="form-label">
              {i18next.t("scripts.modal-action-switch-list")}
            </label>
            <div className="dropdown">
              <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                {switchName}
              </button>
              <ul className="dropdown-menu">
                {switchList}
              </ul>
            </div>
          </div>
          <div className="mb-3">
            <label className="form-label">
              {i18next.t("scripts.modal-action-command")}
            </label>
            <div className="dropdown">
              <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                {i18next.t("scripts.modal-action-switch-command-"+this.state.addActionParams.command)}
              </button>
              <ul className="dropdown-menu">
                <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionCommand(e, 1)}>{i18next.t("scripts.modal-action-switch-command-1")}</a></li>
                <li><a className="dropdown-item" href="#" onClick={(e) => this.selectActionCommand(e, 0)}>{i18next.t("scripts.modal-action-switch-command-0")}</a></li>
              </ul>
            </div>
          </div>
          <div className="mb-3">
            <div className="btn-group" role="group">
              <button type="button" className="btn btn-secondary" onClick={this.addNewAction}>
                <i className="fa fa-save" aria-hidden="true"></i>
              </button>
              <button type="button" className="btn btn-secondary" onClick={this.removeNewAction}>
                <i className="fa fa-trash" aria-hidden="true"></i>
              </button>
            </div>
          </div>
        </div>
      } else if (this.state.curActionType === "dimmer") {
        let dimmerName = i18next.t("scripts.modal-action-select"), dimmerList = [];
        if (this.state.addActionParams.parameters.device && this.state.addActionParams.element) {
          dimmerName = this.state.deviceOverview[this.state.addActionParams.parameters.device]?.dimmers[this.state.addActionParams.element]?.display;
        }
        Object.keys(this.state.deviceOverview).forEach(dev => {
          if (this.state.deviceOverview[dev].dimmers) {
            Object.keys(this.state.deviceOverview[dev].dimmers).forEach(di => {
              if (this.state.deviceOverview[dev].dimmers[di].enabled) {
                dimmerList.push(<li key={dev+"$"+di}><a className="dropdown-item" href="#" onClick={(e) => this.selectActionSwitch(e, dev, di)}>{this.state.deviceOverview[dev].dimmers[di].display}</a></li>);
              }
            });
          }
        });
        actionParamJsx =
        <div className="border rounded script-new-action">
          <div className="mb-3">
            <label className="form-label">
              {i18next.t("scripts.modal-action-dimmer-list")}
            </label>
            <div className="dropdown">
              <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                {dimmerName}
              </button>
              <ul className="dropdown-menu">
                {dimmerList}
              </ul>
            </div>
          </div>
          <div className="form-floating mb-3">
            <input type="number" min="0" max="100" step="1" className="form-control" id="dimmerValue" value={this.state.addActionParams.command} onChange={this.changeDimmerActionCommand}/>
            <label htmlFor="dimmerValue">
              {i18next.t("scripts.modal-action-command")}
            </label>
          </div>
          <div className="mb-3">
            <div className="btn-group" role="group">
              <button type="button" className="btn btn-secondary" onClick={this.addNewAction}>
                <i className="fa fa-save" aria-hidden="true"></i>
              </button>
              <button type="button" className="btn btn-secondary" onClick={this.removeNewAction}>
                <i className="fa fa-trash" aria-hidden="true"></i>
              </button>
            </div>
          </div>
        </div>
      } else if (this.state.curActionType === "blind") {
        let blindName = i18next.t("scripts.modal-action-select"), blindList = [];
        if (this.state.addActionParams.parameters.device && this.state.addActionParams.element) {
          blindName = this.state.deviceOverview[this.state.addActionParams.parameters.device]?.blinds[this.state.addActionParams.element]?.display;
        }
        Object.keys(this.state.deviceOverview).forEach(dev => {
          if (this.state.deviceOverview[dev].blinds) {
            Object.keys(this.state.deviceOverview[dev].blinds).forEach(bl => {
              if (this.state.deviceOverview[dev].blinds[bl].enabled) {
                blindList.push(<li key={dev+"$"+bl}><a className="dropdown-item" href="#" onClick={(e) => this.selectActionSwitch(e, dev, bl)}>{this.state.deviceOverview[dev].blinds[bl].display}</a></li>);
              }
            });
          }
        });
        actionParamJsx =
        <div className="border rounded script-new-action">
          <div className="mb-3">
            <label className="form-label">
              {i18next.t("scripts.modal-action-blind-list")}
            </label>
            <div className="dropdown">
              <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                {blindName}
              </button>
              <ul className="dropdown-menu">
                {blindList}
              </ul>
            </div>
          </div>
          <div className="form-floating mb-3">
            <input type="number" min="0" max="100" step="1" className="form-control" id="blindValue" value={this.state.addActionParams.command} onChange={this.changeDimmerActionCommand}/>
            <label htmlFor="blindValue">
              {i18next.t("scripts.modal-action-command")}
            </label>
          </div>
          <div className="mb-3">
            <div className="btn-group" role="group">
              <button type="button" className="btn btn-secondary" onClick={this.addNewAction}>
                <i className="fa fa-save" aria-hidden="true"></i>
              </button>
              <button type="button" className="btn btn-secondary" onClick={this.removeNewAction}>
                <i className="fa fa-trash" aria-hidden="true"></i>
              </button>
            </div>
          </div>
        </div>
      } else if (this.state.curActionType.startsWith("mpd-")) {
        let playerList = [], playerDropdownJsx, additionalParamsJsx;
        this.state.serviceList.forEach(service => {
          if (service.name === "service-mpd") {
            service.element.forEach((element, index) => {
              playerList.push(
                <li key={index}><a className="dropdown-item" href="#" onClick={(e) => this.selectMpdPlayer(e, element.name)}>{element.name}</a></li>
              );
            });
          }
        });
        playerDropdownJsx =
        <div className="mb-3">
          <label className="form-label">
            {i18next.t("scripts.modal-mpd-player-list")}
          </label>
          <div className="dropdown">
            <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
              {this.state.addActionParams.element}
            </button>
            <ul className="dropdown-menu">
              {playerList}
            </ul>
          </div>
        </div>
        if (this.state.curActionType === "mpd-play-song") {
          additionalParamsJsx =
            <div className="form-floating mb-3">
              <input type="text" className="form-control" id="mpdUriValue" value={this.state.addActionParams.parameters.uri} onChange={this.changeMpdUri}/>
              <label htmlFor="mpdUriValue">
                {i18next.t("scripts.modal-mpd-uri")}
              </label>
            </div>
        } else if (this.state.curActionType === "mpd-action") {
          additionalParamsJsx =
            <div className="mb-3">
              <label className="form-label">
                {i18next.t("scripts.modal-mpd-action-list")}
              </label>
              <div className="dropdown">
                <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                  {i18next.t("scripts.modal-mpd-action-list-"+this.state.addActionParams.parameters.action)}
                </button>
                <ul className="dropdown-menu">
                  <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMpdAction(e, "play")}>{i18next.t("scripts.modal-mpd-action-list-play")}</a></li>
                  <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMpdAction(e, "stop")}>{i18next.t("scripts.modal-mpd-action-list-stop")}</a></li>
                  <li><a className="dropdown-item" href="#" onClick={(e) => this.selectMpdAction(e, "pause")}>{i18next.t("scripts.modal-mpd-action-list-pause")}</a></li>
                </ul>
              </div>
            </div>
        } else if (this.state.curActionType === "mpd-set-volume") {
          additionalParamsJsx =
            <div className="form-floating mb-3">
              <input type="number" min="0" max="100" step="1" className="form-control" id="mpdVolumeValue" value={this.state.addActionParams.parameters.volume} onChange={this.changeMpdVolume}/>
              <label htmlFor="mpdVolumeValue">
                {i18next.t("scripts.modal-mpd-volume")}
              </label>
            </div>
        } else if (this.state.curActionType === "mpd-set-playlist") {
          additionalParamsJsx =
            <div className="form-floating mb-3">
              <input type="text" className="form-control" id="mpdPlaylistValue" value={this.state.addActionParams.parameters.playlist} onChange={this.changeMpdPlaylist}/>
              <label htmlFor="mpdPlaylistValue">
                {i18next.t("scripts.modal-mpd-playlist")}
              </label>
            </div>
        }
        actionParamJsx =
        <div className="border rounded script-new-action">
          {playerDropdownJsx}
          {additionalParamsJsx}
          <div className="mb-3">
            <div className="btn-group" role="group">
              <button type="button" className="btn btn-secondary" onClick={this.addNewAction}>
                <i className="fa fa-save" aria-hidden="true"></i>
              </button>
              <button type="button" className="btn btn-secondary" onClick={this.removeNewAction}>
                <i className="fa fa-trash" aria-hidden="true"></i>
              </button>
            </div>
          </div>
        </div>
      }
    }
		return (
      <div className="modal" tabIndex="-1" id="modalScript">
        <div className="modal-dialog">
          <div className="modal-content">
            <form onSubmit={(e) => this.closeModal(e, true)}>
              <div className="modal-header">
                <h5 className="modal-title">
                  {i18next.t("scripts.modal-edit")}
                </h5>
                <button type="button" className="btn-close" aria-label="Close" onClick={(e) => this.closeModal(e, false)}></button>
              </div>
              <div className="modal-body">
                <div className="form-floating mb-3">
                  <input type="text" className={"form-control" + errorClass} id="scriptName" value={this.state.script.name} disabled={!this.state.add} onChange={this.changeName}/>
                  <label htmlFor="scripts">
                    {i18next.t("scripts.modal-name")}
                    {requiredStar}
                  </label>
                  {errorMessageJsx}
                </div>
                <div className="form-floating mb-3">
                  <input type="text" className="form-control" id="serviceDescription" value={this.state.script.description} onChange={this.changeDescription}/>
                  <label htmlFor="serviceDescription">
                    {i18next.t("scripts.modal-description")}
                  </label>
                </div>
                <hr/>
                <div className="mb-3">
                  <div className="row">
                    <div className="col-10">
                      <label className="form-label">
                        {i18next.t("scripts.actions-list")}
                      </label>
                    </div>
                    <div className="col-2">
                      <button type="button" className="btn btn-secondary" onClick={this.addAction}>
                        <i className="fa fa-plus" aria-hidden="true"></i>
                      </button>
                    </div>
                  </div>
                </div>
                <div className="mb-3">
                  {actionListJsx}
                </div>
                {addActionJsx}
                {actionParamJsx}
              </div>
              <div className="modal-footer">
                <button type="submit" className="btn btn-secondary" onClick={(e) => this.closeModal(e, false)}>{i18next.t("modal.close")}</button>
                <button type="button" className="btn btn-primary" onClick={(e) => this.closeModal(e, true)} disabled={!this.state.isValid || !this.state.script.name}>{i18next.t("modal.ok")}</button>
              </div>
            </form>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalScript;
