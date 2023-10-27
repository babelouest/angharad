import React, { Component } from 'react';
import i18next from 'i18next';

import messageDispatcher from '../lib/MessageDispatcher';
import apiManager from '../lib/APIManager';

class MapMpdServiceButton extends Component {
  constructor(props) {
    super(props);

    this.state = {
      element: props.element,
      index: props.index,
      style: props.style,
      selectCb: props.selectCb,
      adminMode: props.adminMode,
      status: {state: "stop"}
    }
    
    messageDispatcher.subscribe('MpdServiceButton', (message) => {
      if (message.status === "refresh") {
        this.getPlayerStatus();
      }
    });
    
    this.getPlayerStatus = this.getPlayerStatus.bind(this);
    this.loopPlayerStatus = this.loopPlayerStatus.bind(this);
    
    this.loopPlayerStatus();
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  loopPlayerStatus() {
    this.getPlayerStatus()
    .then(() => {
      let time = 300000;
      if (this.state.status.state !== "stop") {
        time = 10000;
      }
      clearTimeout(this.timeout);
      this.timeout = setTimeout(() => {
        this.loopPlayerStatus();
      }, time);
    });
  }

  getPlayerStatus() {
    return apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/status")
    .then(result => {
      return this.setState({status: result});
    })
    .catch(err => {
      if (err.status !== 401) {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      }
    });
  }
  
  render() {
    let className = "map-img-element btn";
    if (this.state.adminMode) {
      className += " btn-danger";
    } else {
      if (this.state.status.state === "stop") {
        className += " btn-secondary";
      } else {
        className += " btn-warning";
      }
    }
    return (
      <a href="#" className={className} title={this.state.element.name} style={this.state.style} onClick={(e) => this.state.selectCb(e, this.state.element, this.state.index)}>
        <i className="fa fa-music" aria-hidden="true"></i>
      </a>
    );
  }
}

export default MapMpdServiceButton;
