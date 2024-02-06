import React, { Component } from 'react';
import i18next from 'i18next';

import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';
import Notification from '../lib/Notification';
import ServiceAdminButtons from './ServiceAdminButtons';

function formatDate(date) {
  var hours = date.getHours();
  var minutes = date.getMinutes();
  hours = hours ? hours : 12; // the hour '0' should be '12'
  hours = hours < 10 ? '0'+hours : hours;
  minutes = minutes < 10 ? '0'+minutes : minutes;
  var strTime = hours + ':' + minutes;
  return strTime;
}

class WeathermapService extends Component {
  constructor(props) {
    super(props);

    this.state = {
      config: props.config,
      element: props.element,
      adminMode: props.adminMode,
      data: false
    }
    
    this.getWeatherData = this.getWeatherData.bind(this);
    this.loopWeatherData = this.loopWeatherData.bind(this);
    this.refreshData = this.refreshData.bind(this);
    
    this.getWeatherData()
    .then(() => {
      this.loopWeatherData();
    });
  }

  static getDerivedStateFromProps(props, state) {
    return props;
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

  refreshData() {
    apiManager.APIRequestCarleon("service-weathermap/" + encodeURIComponent(this.state.element.name) + "/data", "PUT")
    .then((result) => {
      messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("message.refresh")});
      this.getWeatherData();
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    })
  }

	render() {
    let adminButtonsJsx;
    if (this.state.adminMode) {
      adminButtonsJsx =
        <div className="col-2">
          <ServiceAdminButtons type={"service-weathermap"} deviceOverview={this.state.deviceOverview} element={this.state.element} />
        </div>
    }
    let dataJsx, titleJsx, rainJsx, snowJsx;
    let refreshDate = new Date();
    if (this.state.data) {
      let curDate = new Date();
      let diff = -(curDate.getTimezoneOffset()*60);
      refreshDate = new Date(this.state.data.data.dt*1000);
      let sunriseDate = new Date((this.state.data.data.sys.sunrise-diff+this.state.data.data.timezone)*1000);
      let sunsetDate = new Date((this.state.data.data.sys.sunset-diff+this.state.data.data.timezone)*1000);
      let localDate = new Date(((curDate.getTime() / 1000) - diff + this.state.data.data.timezone)*1000);

      titleJsx = this.state.data.data.name;
      let tempUnit, pressureUnit = "hPa", humUnit = "%", speedUnit, degUnit = "°", cloudsUnit = "%", rainUnit = "mm", snowUnit = "cm";
      if (this.state.element.units === "metric") {
        tempUnit = "°C";
        speedUnit = "m/s";
      } else if (this.state.element.units === "imperial") {
        tempUnit = "°F";
        speedUnit = "mi/s";
      } else {
        tempUnit = "°K";
        speedUnit = "m/s";
      }
      if (this.state.data.data.rain) {
        let rain3HJsx;
        if (this.state.data.data.rain["3h"]) {
          rain3HJsx =
            <div className="col-6 text-center">
              <h5>
                <div>🌧</div>
                {i18next.t("services.carleon-service-weathermap-rain-3h")}
              </h5>
              {this.state.data.data.rain["3h"]+rainUnit}
            </div>
        }
        rainJsx =
        <div>
          <hr/>
          <div className="row">
            <div className="col-6 text-center">
              <h5>
                <div>🌧</div>
                {i18next.t("services.carleon-service-weathermap-rain-1h")}
              </h5>
              {this.state.data.data.rain["1h"]+rainUnit}
            </div>
            {rain3HJsx}
          </div>
        </div>
      }
      if (this.state.data.data.snow) {
        let snow3HJsx;
        if (this.state.data.data.snow["3h"]) {
          snow3HJsx =
            <div className="col-6 text-center">
              <h5>
                <div>🌨</div>
                {i18next.t("services.carleon-service-weathermap-snow-3h")}
              </h5>
              {this.state.data.data.snow["3h"]+snowUnit}
            </div>
        }
        snowJsx =
        <div>
          <hr/>
          <div className="row">
            <div className="col-6 text-center">
              <h5>
                <div>🌨</div>
                {i18next.t("services.carleon-service-weathermap-snow-1h")}
              </h5>
              {this.state.data.data.snow["1h"]+snowUnit}
            </div>
            {snow3HJsx}
          </div>
        </div>
      }
      dataJsx =
        <div>
          <div className="row">
            <div className="col-3 text-center">
              <div><img src={"https://openweathermap.org/img/wn/"+this.state.data.data.weather[0].icon+".png"} /></div>
              {this.state.data.data.weather[0].description}
            </div>
            <div className="col-3 text-center">
              <div>🕑</div>
              {formatDate(localDate)}
            </div>
            <div className="col-3 text-center">
              <div>🌅</div>
              {formatDate(sunriseDate)}
            </div>
            <div className="col-3 text-center">
              <div>🌇</div>
              {formatDate(sunsetDate)}
            </div>
          </div>
          <hr/>
          <div className="row">
            <div className="col-6 text-center">
              <h5>
                <div>🌡</div>
                {i18next.t("services.carleon-service-weathermap-temp")}
              </h5>
              {this.state.data.data.main.temp+tempUnit}
            </div>
            <div className="col-6 text-center">
              <h5>
                <div>🌡</div>
                {i18next.t("services.carleon-service-weathermap-temp-feels-like")}
              </h5>
              {this.state.data.data.main.feels_like+tempUnit}
            </div>
          </div>
          <hr/>
          <div className="row">
            <div className="col-3 text-center">
              <h5>
                <div>🌡</div>
                {i18next.t("services.carleon-service-weathermap-temp-min")}
              </h5>
              {this.state.data.data.main.temp_min+tempUnit}
            </div>
            <div className="col-3 text-center">
              <h5>
                <div>🌡</div>
                {i18next.t("services.carleon-service-weathermap-temp-max")}
              </h5>
              {this.state.data.data.main.temp_max+tempUnit}
            </div>
            <div className="col-3 text-center">
              <h5>
                {i18next.t("services.carleon-service-weathermap-pressure")}
              </h5>
              {this.state.data.data.main.pressure+pressureUnit}
            </div>
            <div className="col-3 text-center">
              <h5>
                {i18next.t("services.carleon-service-weathermap-humidity")}
              </h5>
              {this.state.data.data.main.humidity+humUnit}
            </div>
          </div>
          <hr/>
          <div className="row">
            <div className="col-3 text-center">
              <h5>
                <div>🌬</div>
                {i18next.t("services.carleon-service-weathermap-wind-speed")}
              </h5>
              {this.state.data.data.wind.speed+speedUnit}
            </div>
            <div className="col-3 text-center">
              <h5>
                <div>🌬</div>
                {i18next.t("services.carleon-service-weathermap-wind-gust")}
              </h5>
              {this.state.data.data.wind.gust!==undefined?this.state.data.data.wind.gust+speedUnit:"/"}
            </div>
            <div className="col-3 text-center">
              <h5>
                <div>↗</div>
                {i18next.t("services.carleon-service-weathermap-wind-deg")}
              </h5>
              {this.state.data.data.wind.deg+degUnit}
            </div>
            <div className="col-3 text-center">
              <h5>
                <div>☁</div>
                {i18next.t("services.carleon-service-weathermap-clouds")}
              </h5>
              {this.state.data.data.clouds.all+cloudsUnit}
            </div>
          </div>
          {rainJsx}
          {snowJsx}
        </div>
    } else {
      titleJsx = this.state.element.name;
      dataJsx = 
      <div className="row">
        <div className="alert alert-secondary" role="alert">
          {i18next.t("services.carleon-service-weathermap-no-data")}
        </div>
      </div>
    }
    return (
      <div className="row border rounded border-3 elt-top weathermap-element agrd-element">
        <div className="col-sm">
          <div className="row">
            <div className="col-8 text-center">
              <label className="d-flex align-items-center">
                <button type="button" className="btn btn-secondary elt-left" onClick={this.refreshData}>
                  <i className="fa fa-refresh" aria-hidden="true"></i>
                </button>
                <h4>
                  {titleJsx}
                </h4>
              </label>
            </div>
            <div className="col-2 text-right">
              <span className="badge bg-primary">
                ⟳ ({formatDate(refreshDate)})
              </span>
            </div>
            <div className="col-2">
              {adminButtonsJsx}
            </div>
          </div>
          {dataJsx}
        </div>
      </div>
    );
  }
}

export default WeathermapService;
