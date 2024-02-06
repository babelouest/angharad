import React, { Component } from 'react';
import i18next from 'i18next';

import messageDispatcher from '../lib/MessageDispatcher';
import apiManager from '../lib/APIManager';

class MapWeathermapServiceButton extends Component {
  constructor(props) {
    super(props);

    this.state = {
      element: props.element,
      service: props.service,
      index: props.index,
      style: props.style,
      selectCb: props.selectCb,
      adminMode: props.adminMode,
      data: false
    }
    
    messageDispatcher.subscribe('WeathermapServiceButton', (message) => {
      if (message.status === "refresh") {
        this.refreshData()
        .then(() => {
          this.getWeatherData();
        });
      }
    });
    
    this.getWeatherData = this.getWeatherData.bind(this);
    this.loopWeatherData = this.loopWeatherData.bind(this);
    this.refreshData = this.refreshData.bind(this);
    
    this.loopWeatherData();
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  refreshData() {
    return apiManager.APIRequestCarleon("service-weathermap/" + encodeURIComponent(this.state.element.name) + "/data", "PUT")
    .then((result) => {
      this.getWeatherData();
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    })
  }

  getWeatherData() {
    return apiManager.APIRequestCarleon("service-weathermap/" + encodeURIComponent(this.state.element.name) + "/data")
    .then((result, resp) => {
      if (resp != "nocontent") {
        this.setState({data: result});
      }
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    })
  }
  
  loopWeatherData() {
    this.getWeatherData()
    .then(() => {
      let time = 300000;
      clearTimeout(this.timeout);
      this.timeout = setTimeout(() => {
        this.loopWeatherData();
      }, time);
    });
  }
  
  render() {
    let className = "map-img-element btn btn-secondary", dataJsx;
    if (this.state.adminMode) {
      className += " btn-danger";
    }
    if (this.state.data) {
      let tempUnit;
      if (this.state.service.units === "metric") {
        tempUnit = "°C";
      } else if (this.state.service.units === "imperial") {
        tempUnit = "°F";
      } else {
        tempUnit = "°K";
      }
      dataJsx =
      <div>
        <img src={"https://openweathermap.org/img/wn/"+this.state.data.data.weather[0].icon+".png"} />
        {this.state.data.data.main.temp+tempUnit} ({this.state.data.data.main.feels_like+tempUnit})
      </div>
    } else {
      dataJsx = <i className="fa fa-question" aria-hidden="true"></i>
    }
    return (
      <a href="#" className={className} title={this.state.element.name} style={this.state.style} onClick={(e) => this.state.selectCb(e, this.state.element, this.state.index)}>
        {dataJsx}
      </a>
    );
  }
}

export default MapWeathermapServiceButton;
