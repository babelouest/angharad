import React, { Component } from 'react';
import i18next from 'i18next';

import apiManager from '../lib/APIManager';
import ServiceAdminButtons from './ServiceAdminButtons';

class MockService extends Component {
  constructor(props) {
    super(props);

    this.state = {
      deviceOverview: props.deviceOverview,
      element: props.element,
      adminMode: props.adminMode,
      curCommand: false,
      curParameters: {},
      curResult: false
    }
    
    this.mockExecSelect = this.mockExecSelect.bind(this);
    this.changeParamString = this.changeParamString.bind(this);
    this.changeParamInteger = this.changeParamInteger.bind(this);
    this.changeParamFloat = this.changeParamFloat.bind(this);
    this.changeParamBoolean = this.changeParamBoolean.bind(this);
    this.runCommand = this.runCommand.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  mockExecSelect(e, command) {
    e.preventDefault();
    if (command === "exec1") {
      this.setState({curCommand: command, curParameters: {param1: "test1", param2: 42, param3: 22.6}, curResult: false});
    } else if (command === "exec2") {
      this.setState({curCommand: command, curParameters: {param1: "test2"}, curResult: false});
    } else if (command === "exec3" || command === "exec4") {
      this.setState({curCommand: command, curParameters: {}, curResult: false});
    }
  }
  
  changeParamString(e, param) {
    let curParameters = this.state.curParameters;
    curParameters[param] = e.target.value;
    this.setState({curParameters: curParameters});
  }
  
  changeParamInteger(e, param) {
    let curParameters = this.state.curParameters;
    curParameters[param] = parseInt(e.target.value);
    if (isNaN(curParameters[param])) {
      curParameters[param] = 0;
    }
    this.setState({curParameters: curParameters});
  }
  
  changeParamFloat(e, param) {
    let curParameters = this.state.curParameters;
    curParameters[param] = parseFloat(e.target.value);
    if (isNaN(curParameters[param])) {
      curParameters[param] = 0.0;
    }
    this.setState({curParameters: curParameters});
  }
  
  changeParamBoolean(e, param) {
    let curParameters = this.state.curParameters;
    curParameters[param] = !curParameters[param];
    this.setState({curParameters: curParameters});
  }
  
  runCommand() {
    let params = [];
    Object.keys(this.state.curParameters).forEach(param => {
      if (param !== "param3") {
        if (this.state.curParameters[param] != undefined) {
          params.push(this.state.curParameters[param]);
        }
      } else {
        params.push(this.state.curParameters[param]);
      }
    });
    apiManager.APIRequestCarleon("mock-service/" + encodeURIComponent(this.state.element.name) + "/command/" + encodeURIComponent(this.state.curCommand) + "/" + params.join("/"))
    .then(res => {
      this.setState({curResult: res});
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    });
  }
  
	render() {
    let adminButtonsJsx, parametersJsx, runButtonJsx, resultJsx;
    if (this.state.adminMode) {
      adminButtonsJsx =
        <ServiceAdminButtons type={"mock-service"} deviceOverview={this.state.deviceOverview} element={this.state.element} />
    }
    if (this.state.curCommand === "exec1") {
      let errorClassParam1;
      if (this.state.curParameters.param1 === "") {
        errorClassParam1 = " is-invalid";
      } else {
        errorClassParam1 = " is-valid";
      }
      parametersJsx =
        <div>
          <div className="form-floating mb-3 mt-3">
            <input type="text"
                   className={"form-control" + errorClassParam1}
                   id={"param1"}
                   onChange={(e) => this.changeParamString(e, "param1")}
                   value={this.state.curParameters.param1}/>
            <label htmlFor={"param1"}>
              {i18next.t("services.carleon-service-mock1-param1")}
              <span className="text-danger"> *</span>
            </label>
          </div>
          <div className="form-floating mb-3 mt-3">
            <input type="number"
                   className="form-control is-valid"
                   step="1"
                   id={"param2"}
                   onChange={(e) => this.changeParamInteger(e, "param2")}
                   value={this.state.curParameters.param2}/>
            <label htmlFor={"param2"}>
              {i18next.t("services.carleon-service-mock1-param2")}
              <span className="text-danger"> *</span>
            </label>
          </div>
          <div className="form-floating mb-3 mt-3">
            <input type="number"
                   className="form-control is-valid"
                   step=".01"
                   id={"param3"}
                   onChange={(e) => this.changeParamFloat(e, "param3")}
                   value={this.state.curParameters.param3}/>
            <label htmlFor={"param3"}>
              {i18next.t("services.carleon-service-mock1-param3")}
              <span className="text-danger"> *</span>
            </label>
          </div>
        </div>
      runButtonJsx = <button type="button" className="btn btn-primary" onClick={this.runCommand}>{i18next.t("services.services.carleon-service-mock-run")}</button>
    } else if (this.state.curCommand === "exec2") {
      let errorClassParam1;
      if (this.state.curParameters.param1 === "") {
        errorClassParam1 = " is-invalid";
      } else {
        errorClassParam1 = " is-valid";
      }
      parametersJsx =
        <div>
          <div className="form-floating mb-3 mt-3">
            <input type="text"
                   className={"form-control" + errorClassParam1}
                   id={"param1"}
                   onChange={(e) => this.changeParamString(e, "param1")}
                   value={this.state.curParameters.param1}/>
            <label htmlFor={"param1"}>
              {i18next.t("services.carleon-service-mock1-param1")}
              <span className="text-danger"> *</span>
            </label>
          </div>
        </div>
      runButtonJsx = <button type="button" className="btn btn-primary" onClick={this.runCommand}>{i18next.t("services.services.carleon-service-mock-run")}</button>
    } else if (this.state.curCommand === "exec3" || this.state.curCommand === "exec4") {
      runButtonJsx = <button type="button" className="btn btn-primary" onClick={this.runCommand}>{i18next.t("services.services.carleon-service-mock-run")}</button>
    }
    if (this.state.curResult) {
      resultJsx =
      <div className="text-left">
        <pre>
          {JSON.stringify(this.state.curResult, null, 2)}
        </pre>
      </div>
    }
		return (
      <div className="border rounded agrd-element">
        <div className="row">
          <div className="col-10 text-center">
            <h4>
              {this.state.element.name}
            </h4>
            <div className="dropdown">
              <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                {i18next.t("services.carleon-service-mock-exec-list")}
              </button>
              <ul className="dropdown-menu">
                <li><a className="dropdown-item" href="#" onClick={(e) => this.mockExecSelect(e, "exec1")}>exec1</a></li>
                <li><a className="dropdown-item" href="#" onClick={(e) => this.mockExecSelect(e, "exec2")}>exec2</a></li>
                <li><a className="dropdown-item" href="#" onClick={(e) => this.mockExecSelect(e, "exec3")}>exec3</a></li>
                <li><a className="dropdown-item" href="#" onClick={(e) => this.mockExecSelect(e, "exec4")}>exec4</a></li>
              </ul>
            </div>
            {parametersJsx}
            {runButtonJsx}
          </div>
          <div className="col-2">
            {adminButtonsJsx}
          </div>
        </div>
        <div className="row elt-top">
          <div className="col">
            {resultJsx}
          </div>
        </div>
      </div>
		);
	}
}

export default MockService;
