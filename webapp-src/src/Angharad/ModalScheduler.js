import React, { Component } from 'react';
import i18next from 'i18next';
import "flatpickr/dist/flatpickr.css";
import Flatpickr from "react-flatpickr";

import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';

class ModalScheduler extends Component {
  constructor(props) {
    super(props);

    this.state = {
      scheduler: props.scheduler,
      schedulerList: props.schedulerList,
      scriptList: props.scriptList,
      add: props.add,
      cb: props.cb,
      nameValid: true,
      nameError: false
    }
    
    this.closeModal = this.closeModal.bind(this);
    this.changeDate = this.changeDate.bind(this);
    this.changeName = this.changeName.bind(this);
    this.changeDescription = this.changeDescription.bind(this);
    this.handleEnabled = this.handleEnabled.bind(this);
    this.selectSchedulerRepeat = this.selectSchedulerRepeat.bind(this);
    this.changeRepeatValue = this.changeRepeatValue.bind(this);
    this.handleRepeatDayOfWeek = this.handleRepeatDayOfWeek.bind(this);
    this.handleRemoveAfter = this.handleRemoveAfter.bind(this);
    this.selectScript = this.selectScript.bind(this);
    this.handleScriptEnabled = this.handleScriptEnabled.bind(this);
    this.removeScriptAt = this.removeScriptAt.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  changeName(e) {
    let scheduler = this.state.scheduler;
    scheduler.name = e.target.value;
    let isValid = true, nameValid = true, nameError = false;
    if (!scheduler.name) {
      nameValid = false;
    } else {
      this.state.schedulerList.forEach((curScheduler) => {
        if (curScheduler.name === scheduler.name) {
          isValid = false;
          nameValid = false;
          nameError = i18next.t("schedulers.name-taken");
        }
      });
    }
    this.setState({scheduler: scheduler, isValid: isValid, nameValid: nameValid, nameError: nameError});
  }
  
  changeDescription(e) {
    let scheduler = this.state.scheduler;
    scheduler.description = e.target.value;
    this.setState({scheduler: scheduler});
  }
  
  handleEnabled() {
    let scheduler = this.state.scheduler;
    scheduler.enabled = !scheduler.enabled;
    this.setState({scheduler: scheduler});
  }
  
  selectSchedulerRepeat(e, repeat) {
    e.preventDefault();
    let scheduler = this.state.scheduler;
    scheduler.repeat = repeat;
    if (repeat === 3) {
      scheduler.repeat_value = 127;
    } else {
      scheduler.repeat_value = 1;
    }
    this.setState({scheduler: scheduler});
  }
  
  changeDate(e) {
    if (e[0] && e[0] instanceof Date) {
      let scheduler = this.state.scheduler;
      scheduler.next_time = e[0].getTime()/1000;
      this.setState({scheduler: scheduler});
    }
  }
  
  changeRepeatValue(e) {
    let scheduler = this.state.scheduler;
    scheduler.repeat_value = parseInt(e.target.value);
    this.setState({scheduler: scheduler});
  }
  
  handleRepeatDayOfWeek(e, repeat) {
    let scheduler = this.state.scheduler;
    if (scheduler.repeat_value&repeat) {
      scheduler.repeat_value -= repeat;
    } else {
      scheduler.repeat_value += repeat;
    }
    this.setState({scheduler: scheduler});
  }
  
  handleRemoveAfter() {
    let scheduler = this.state.scheduler;
    scheduler.remove_after = !scheduler.remove_after;
    this.setState({scheduler: scheduler});
  }
  
  selectScript(e, script) {
    e.preventDefault();
    let scheduler = this.state.scheduler;
    scheduler.scripts.push({name: script.name, enabled: true});
    this.setState({scheduler: scheduler});
  }
  
  handleScriptEnabled(e, index) {
    e.preventDefault();
    let scheduler = this.state.scheduler;
    scheduler.scripts[index].enabled = !scheduler.scripts[index].enabled;
    this.setState({scheduler: scheduler});
  }
  
  removeScriptAt(e, index) {
    let scheduler = this.state.scheduler;
    scheduler.scripts.splice(index, 1);
    this.setState({scheduler: scheduler});
  }

  closeModal(e, result) {
    e.preventDefault();
    if (this.state.cb) {
      if (result) {
        this.state.cb(true, this.state.scheduler, this.state.add);
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
    let repeatValueJsx;
    if ([0,1,2,4,5].indexOf(this.state.scheduler.repeat) > -1) {
      repeatValueJsx =
        <div className="form-floating mb-3">
          <input type="number" min="1" step="1" className="form-control" id="schedulerRepeatValud" value={this.state.scheduler.repeat_value} onChange={this.changeRepeatValue}/>
          <label htmlFor="schedulerRepeatValud">
            {i18next.t("schedulers.repeat-value")}
          </label>
        </div>
    } else if (this.state.scheduler.repeat === 3) {
      repeatValueJsx =
        <div>
          <div className="form-check form-switch">
            <input className="form-check-input"
                   type="checkbox"
                   role="switch"
                   id="schedulerRepeatSunday"
                   checked={this.state.scheduler.repeat_value&1}
                   onChange={(e) => this.handleRepeatDayOfWeek(e, 1)} />
            <label className="form-check-label" htmlFor="schedulerRepeatSunday">
              {i18next.t("schedulers.repeat-dow-sunday")}
            </label>
          </div>
          <div className="form-check form-switch">
            <input className="form-check-input"
                   type="checkbox"
                   role="switch"
                   id="schedulerRepeatMonday"
                   checked={this.state.scheduler.repeat_value&2}
                   onChange={(e) => this.handleRepeatDayOfWeek(e, 2)} />
            <label className="form-check-label" htmlFor="schedulerRepeatMonday">
              {i18next.t("schedulers.repeat-dow-monday")}
            </label>
          </div>
          <div className="form-check form-switch">
            <input className="form-check-input"
                   type="checkbox"
                   role="switch"
                   id="schedulerRepeatTuesday"
                   checked={this.state.scheduler.repeat_value&4}
                   onChange={(e) => this.handleRepeatDayOfWeek(e, 4)} />
            <label className="form-check-label" htmlFor="schedulerRepeatTuesday">
              {i18next.t("schedulers.repeat-dow-tuesday")}
            </label>
          </div>
          <div className="form-check form-switch">
            <input className="form-check-input"
                   type="checkbox"
                   role="switch"
                   id="schedulerRepeatWednesday"
                   checked={this.state.scheduler.repeat_value&8}
                   onChange={(e) => this.handleRepeatDayOfWeek(e, 8)} />
            <label className="form-check-label" htmlFor="schedulerRepeatWednesday">
              {i18next.t("schedulers.repeat-dow-wednesday")}
            </label>
          </div>
          <div className="form-check form-switch">
            <input className="form-check-input"
                   type="checkbox"
                   role="switch"
                   id="schedulerRepeatThursday"
                   checked={this.state.scheduler.repeat_value&16}
                   onChange={(e) => this.handleRepeatDayOfWeek(e, 16)} />
            <label className="form-check-label" htmlFor="schedulerRepeatThursday">
              {i18next.t("schedulers.repeat-dow-tuesday")}
            </label>
          </div>
          <div className="form-check form-switch">
            <input className="form-check-input"
                   type="checkbox"
                   role="switch"
                   id="schedulerRepeatFriday"
                   checked={this.state.scheduler.repeat_value&32}
                   onChange={(e) => this.handleRepeatDayOfWeek(e, 32)} />
            <label className="form-check-label" htmlFor="schedulerRepeatFriday">
              {i18next.t("schedulers.repeat-dow-friday")}
            </label>
          </div>
          <div className="form-check form-switch">
            <input className="form-check-input"
                   type="checkbox"
                   role="switch"
                   id="schedulerRepeatSaturday"
                   checked={this.state.scheduler.repeat_value&64}
                   onChange={(e) => this.handleRepeatDayOfWeek(e, 64)} />
            <label className="form-check-label" htmlFor="schedulerRepeatSaturday">
              {i18next.t("schedulers.repeat-dow-saturday")}
            </label>
          </div>
        </div>
    }
		let scriptListSelect = [];
    this.state.scriptList.forEach((script, index) => {
      scriptListSelect.push(
        <li key={index}><a className="dropdown-item" href="#" onClick={(e) => this.selectScript(e, script)}>{script.name}</a></li>
      );
    });
    let scriptListJsx = [];
    this.state.scheduler.scripts.forEach((script, index) => {
      scriptListJsx.push(
        <div className="row elt-top" key={index}>
          <div className="col-10">
            <div className="form-check form-switch">
              <input className="form-check-input"
                     type="checkbox"
                     role="switch"
                     id={"schedulerScriptEnabled-"+index}
                     checked={this.state.scheduler.scripts[index].enabled}
                     onChange={(e) => this.handleScriptEnabled(e, index)} />
              <label className="form-check-label" htmlFor={"schedulerScriptEnabled-"+index}>
                {script.name}
              </label>
            </div>
          </div>
          <div className="col-2">
            <button type="button" className="btn btn-secondary btn-sm" onClick={(e) => this.removeScriptAt(e, index)}>
              <i className="fa fa-trash" aria-hidden="true"></i>
            </button>
          </div>
        </div>
      );
    });
    return (
      <div className="modal" tabIndex="-1" id="modalScheduler">
        <div className="modal-dialog">
          <div className="modal-content">
            <form onSubmit={(e) => this.closeModal(e, true)}>
              <div className="modal-header">
                <h5 className="modal-title">
                  {i18next.t("schedulers.modal-edit")}
                </h5>
                <button type="button" className="btn-close" aria-label="Close" onClick={(e) => this.closeModal(e, false)}></button>
              </div>
              <div className="modal-body">
                <div className="form-floating mb-3">
                  <input type="text" className={"form-control" + errorClass} id="schedulerName" value={this.state.scheduler.name} disabled={!this.state.add} onChange={this.changeName}/>
                  <label htmlFor="schedulers">
                    {i18next.t("schedulers.modal-name")}
                    {requiredStar}
                  </label>
                  {errorMessageJsx}
                </div>
                <div className="form-floating mb-3">
                  <input type="text" className="form-control" id="schedulerescription" value={this.state.scheduler.description} onChange={this.changeDescription}/>
                  <label htmlFor="schedulerescription">
                    {i18next.t("schedulers.modal-description")}
                  </label>
                </div>
                <div className="form-check form-switch">
                  <input className="form-check-input"
                         type="checkbox"
                         role="switch"
                         id="schedulerEnabled"
                         checked={this.state.scheduler.enabled}
                         onChange={this.handleEnabled} />
                  <label className="form-check-label" htmlFor="schedulerEnabled">
                    {i18next.t("schedulers.enabled")}
                  </label>
                </div>
                <div className="form-check form-switch">
                  <input className="form-check-input"
                         type="checkbox"
                         role="switch"
                         id="schedulerRemoveAfter"
                         checked={this.state.scheduler.remove_after}
                         onChange={this.handleRemoveAfter} />
                  <label className="form-check-label" htmlFor="schedulerRemoveAfter">
                    {i18next.t("schedulers.remove-after")}
                  </label>
                </div>
                <hr/>
                <div className="form-floating mb-3">
                  <Flatpickr
                    data-enable-time
                    value={new Date(this.state.scheduler.next_time*1000)}
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
                    onChange={this.changeDate}
                  />
                  <label htmlFor="nextDate" className={errorClass}>
                    {i18next.t("schedulers.next-time")}
                  </label>
                </div>
                <div className="mb-3">
                  <label className="form-label">
                    {i18next.t("schedulers.repeat")}
                  </label>
                  <div className="dropdown">
                    <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                      {i18next.t("schedulers.repeat-"+this.state.scheduler.repeat)}
                    </button>
                    <ul className="dropdown-menu">
                      <li><a className="dropdown-item" href="#" onClick={(e) => this.selectSchedulerRepeat(e, -1)}>{i18next.t("schedulers.repeat--1")}</a></li>
                      <li><a className="dropdown-item" href="#" onClick={(e) => this.selectSchedulerRepeat(e, 0)}>{i18next.t("schedulers.repeat-0")}</a></li>
                      <li><a className="dropdown-item" href="#" onClick={(e) => this.selectSchedulerRepeat(e, 1)}>{i18next.t("schedulers.repeat-1")}</a></li>
                      <li><a className="dropdown-item" href="#" onClick={(e) => this.selectSchedulerRepeat(e, 2)}>{i18next.t("schedulers.repeat-2")}</a></li>
                      <li><a className="dropdown-item" href="#" onClick={(e) => this.selectSchedulerRepeat(e, 3)}>{i18next.t("schedulers.repeat-3")}</a></li>
                      <li><a className="dropdown-item" href="#" onClick={(e) => this.selectSchedulerRepeat(e, 4)}>{i18next.t("schedulers.repeat-4")}</a></li>
                      <li><a className="dropdown-item" href="#" onClick={(e) => this.selectSchedulerRepeat(e, 5)}>{i18next.t("schedulers.repeat-5")}</a></li>
                    </ul>
                  </div>
                </div>
                {repeatValueJsx}
                <hr/>
                <div className="mb-3">
                  <label className="form-label">
                    {i18next.t("schedulers.scripts")}
                  </label>
                  <div className="dropdown">
                    <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                      {i18next.t("schedulers.scripts-list")}
                    </button>
                    <ul className="dropdown-menu">
                      {scriptListSelect}
                    </ul>
                  </div>
                </div>
                <div className="mb-3">
                  {scriptListJsx}
                </div>
              </div>
              <div className="modal-footer">
                <button type="submit" className="btn btn-secondary" onClick={(e) => this.closeModal(e, false)}>{i18next.t("modal.close")}</button>
                <button type="button" className="btn btn-primary" onClick={(e) => this.closeModal(e, true)} disabled={!this.state.scheduler.name || !this.state.scheduler.next_time || !this.state.nameValid || !this.state.scheduler.scripts.length}>{i18next.t("modal.ok")}</button>
              </div>
            </form>
          </div>
        </div>
      </div>
		);
	}
}

export default ModalScheduler;
