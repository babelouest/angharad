import React, { Component } from 'react';
import i18next from 'i18next';

import messageDispatcher from '../lib/MessageDispatcher';
import apiManager from '../lib/APIManager';
import MapPlace from './MapPlace';
import MapMpdServiceButton from './MapMpdServiceButton';
import MapWeathermapServiceButton from './MapWeathermapServiceButton';

class Map extends Component {
  constructor(props) {
    super(props);

    this.state = {
      config: props.config,
      map: props.map,
      deviceOverview: props.deviceOverview,
      serviceList: props.serviceList,
      script: props.script,
      scheduler: props.scheduler,
      adminMode: props.adminMode,
      curDrag: false,
      curIndex: -1
    }
    
    this.navigate = this.navigate.bind(this);
    this.dropElt = this.dropElt.bind(this);
    this.dragElt = this.dragElt.bind(this);
    this.dragMoveElt = this.dragMoveElt.bind(this);
    this.selectElement = this.selectElement.bind(this);
  }
  
  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  navigate(e, next) {
    e.preventDefault();
    messageDispatcher.sendMessage("Map", {status: (next?"next":"previous")});
  }

  dragElt(e, device, type, name) {
    this.setState({curDrag: {device: device, type: type, name: name}, curIndex: -1});
  }
  
  dragMoveElt(e, index) {
    console.log(this.state.map.elements[index]);
    this.setState({curDrag: false, curIndex: index});
  }

  allowDrop(e) {
    e.preventDefault();
  }
  
  dropElt(e) {
    if (this.state.adminMode) {
      let mapContainer = document.getElementById("mapContainer");
      let coordX = Math.round(Math.floor((e.clientX + window.scrollX - mapContainer.offsetLeft)*100/mapContainer.clientWidth)/5)*5;
      let coordY = Math.round(Math.floor((e.clientY + window.scrollY - mapContainer.offsetTop)*100/mapContainer.clientHeight)/5)*5;
      let map = this.state.map;
      if (this.state.curDrag) {
        let curDrag = this.state.curDrag;
        curDrag.coordX = coordX;
        curDrag.coordY = coordY;
        map.elements.push(curDrag);
        apiManager.APIRequestAngharad("profile/" + encodeURIComponent(map.name), "PUT", map)
        .then(() => {
          this.setState({map: map, curIndex: -1});
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      } else {
        let curDrag = map.elements[this.state.curIndex];
        if (curDrag) {
          curDrag.coordX = coordX;
          curDrag.coordY = coordY;
          map.elements[this.state.curIndex] = curDrag;
          apiManager.APIRequestAngharad("profile/" + encodeURIComponent(map.name), "PUT", map)
          .then(() => {
            this.setState({map: map, curIndex: -1});
          })
          .catch((err) => {
            messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
          });
        }
      }
    }
  }
  
  selectElement(e, element, index) {
    e.preventDefault();
    if (this.state.adminMode) {
      let map = this.state.map;
      map.elements.splice(index, 1);
      apiManager.APIRequestAngharad("profile/" + encodeURIComponent(map.name), "PUT", map)
      .then(() => {
        this.setState({map: map});
      })
      .catch((err) => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    } else {
      if (element.type === "switch") {
        let newValue = 0;
        if (this.state.deviceOverview[element.device].switches[element.name].value === 0) {
          newValue = 1;
        }
        apiManager.APIRequestBenoic("device/" + element.device + "/switch/" + element.name + "/set/" + newValue, "PUT")
        .then((results) => {
          messageDispatcher.sendMessage('Component', {status: "update", device: element.device, type: "switch", name: element.name, value: newValue});
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      } else if (element.type === "dimmer") {
        messageDispatcher.sendMessage("Component", {status: "dimmerModal", device: element.device, name: element.name, element: this.state.deviceOverview[element.device].dimmers[element.name]});
      } else if (element.type === "blind") {
        messageDispatcher.sendMessage("Component", {status: "blindModal", device: element.device, name: element.name, element: this.state.deviceOverview[element.device].blinds[element.name]});
      } else if (element.type === "sensor") {
        messageDispatcher.sendMessage("Component", {status: "monitor", device: element.device, type: "sensor", name: element.name, element: this.state.deviceOverview[element.device].sensors[element.name]});
      } else if (element.type === "serviceMock") {
        messageDispatcher.sendMessage("Service", {status: "open", type: "mock-service", element: element});
      } else if (element.type === "mpdService") {
        messageDispatcher.sendMessage("Service", {status: "open", type: "service-mpd", element: element});
      } else if (element.type === "weathermapService") {
        messageDispatcher.sendMessage("Service", {status: "open", type: "service-weathermap", element: element});
      } else if (element.type === "script") {
        apiManager.APIRequestAngharad("script/"+element.name+"/run")
        .then(() => {
          messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("scripts.executed")});
        })
        .catch((err) => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      } else if (element.type === "scheduler") {
        messageDispatcher.sendMessage("Scheduler", {show: true, scheduler: element.scheduler});
      }
    }
  }
  
  render() {
    let mapPlaceJsx;
    if (this.state.adminMode) {
      let benoicSwitches = [], benoicDimmers = [], benoicBlinds = [], benoicSensors = [], mockServices = [], mpdServices = [], weathermapServices = [];
      let switchListJsx, dimmerListJsx, blindListJsx, sensorListJsx, mockServiceJsx, mpdServiceJsx, weathermapServiceJsx, scriptJsx, schedulerJsx;
      Object.keys(this.state.deviceOverview).forEach(dev => {
        if (this.state.deviceOverview[dev].switches) {
          Object.keys(this.state.deviceOverview[dev].switches).forEach(elt => {
            if (this.state.deviceOverview[dev].switches[elt].enabled) {
              benoicSwitches.push({device: dev, name: elt, display: this.state.deviceOverview[dev].switches[elt].display});
            }
          });
        }
        if (benoicSwitches.length) {
          switchListJsx = <MapPlace title={i18next.t("components.switches")} eltList={benoicSwitches} deviceOverview={this.state.deviceOverview} type={"switch"} cbDrag={this.dragElt} adminMode={this.state.adminMode}/>
        }
        if (this.state.deviceOverview[dev].dimmers) {
          Object.keys(this.state.deviceOverview[dev].dimmers).forEach(elt => {
            if (this.state.deviceOverview[dev].dimmers[elt].enabled) {
              benoicDimmers.push({device: dev, name: elt, display: this.state.deviceOverview[dev].dimmers[elt].display});
            }
          });
        }
        if (benoicDimmers.length) {
          dimmerListJsx = <MapPlace title={i18next.t("components.dimmers")} eltList={benoicDimmers} deviceOverview={this.state.deviceOverview} type={"dimmer"} cbDrag={this.dragElt} adminMode={this.state.adminMode}/>
        }
        if (this.state.deviceOverview[dev].blinds) {
          Object.keys(this.state.deviceOverview[dev].blinds).forEach(elt => {
            if (this.state.deviceOverview[dev].blinds[elt].enabled) {
              benoicBlinds.push({device: dev, name: elt, display: this.state.deviceOverview[dev].blinds[elt].display});
            }
          });
        }
        if (benoicBlinds.length) {
          blindListJsx = <MapPlace title={i18next.t("components.blinds")} eltList={benoicBlinds} deviceOverview={this.state.deviceOverview} type={"blind"} cbDrag={this.dragElt} adminMode={this.state.adminMode}/>
        }
        if (this.state.deviceOverview[dev].sensors) {
          Object.keys(this.state.deviceOverview[dev].sensors).forEach(elt => {
            if (this.state.deviceOverview[dev].sensors[elt].enabled) {
              benoicSensors.push({device: dev, name: elt, display: this.state.deviceOverview[dev].sensors[elt].display});
            }
          });
        }
        if (benoicSensors.length) {
          sensorListJsx = <MapPlace title={i18next.t("components.sensors")} eltList={benoicSensors} deviceOverview={this.state.deviceOverview} type={"sensor"} cbDrag={this.dragElt} adminMode={this.state.adminMode}/>
        }
      });
      this.state.serviceList.forEach(service => {
        if (service.name === "mock-service") {
          service.element.forEach(elt => {
            mockServices.push(elt);
          });
          if (mockServices.length) {
            mockServiceJsx = <MapPlace title={i18next.t("services.mock-services")} eltList={mockServices} serviceList={this.state.serviceList} deviceOverview={this.state.deviceOverview} type={"serviceMock"} cbDrag={this.dragElt} adminMode={this.state.adminMode}/>
          }
        } else if (service.name === "service-mpd") {
          service.element.forEach(elt => {
            mpdServices.push(elt);
          });
          if (mpdServices.length) {
            mpdServiceJsx = <MapPlace title={i18next.t("services.mpd-services")} eltList={mpdServices} serviceList={this.state.serviceList} deviceOverview={this.state.deviceOverview} type={"mpdService"} cbDrag={this.dragElt} adminMode={this.state.adminMode}/>
          }
        } else if (service.name === "service-weathermap") {
          service.element.forEach(elt => {
            weathermapServices.push(elt);
          });
          if (weathermapServices.length) {
            weathermapServiceJsx = <MapPlace title={i18next.t("services.weathermap-services")} eltList={weathermapServices} type={"weathermapService"} cbDrag={this.dragElt} adminMode={this.state.adminMode}/>
          }
        }
      });
      if (this.state.script.length) {
        scriptJsx = <MapPlace title={i18next.t("scripts.title")} eltList={this.state.script} type={"script"} cbDrag={this.dragElt} adminMode={this.state.adminMode}/>
      }
      if (this.state.scheduler.length) {
        schedulerJsx = <MapPlace title={i18next.t("schedulers.title")} eltList={this.state.scheduler} type={"scheduler"} cbDrag={this.dragElt} adminMode={this.state.adminMode}/>
      }
      mapPlaceJsx =
      <div>
        <hr/>
        <div className="accordion" id="accordionElements">
          {switchListJsx}
          {dimmerListJsx}
          {blindListJsx}
          {sensorListJsx}
          {mockServiceJsx}
          {mpdServiceJsx}
          {weathermapServiceJsx}
          {scriptJsx}
          {schedulerJsx}
        </div>
      </div>
    }
    let elementList = [];
    this.state.map.elements.forEach((element, index) => {
      let iconJsx, style = {
        left: element.coordX+"%",
        top: element.coordY+"%"
      }
      if (element.type === "switch") {
        if (this.state.deviceOverview[element.device] && this.state.deviceOverview[element.device].switches && this.state.deviceOverview[element.device].switches[element.name] && (this.state.deviceOverview[element.device].switches[element.name].enabled || this.state.adminMode)) {
          if (this.state.deviceOverview[element.device].switches[element.name].value === 1) {
            iconJsx = <i className="fa fa-toggle-on" aria-hidden="true"></i>
          } else {
            iconJsx = <i className="fa fa-toggle-off" aria-hidden="true"></i>
          }
          let className = "map-img-element btn";
          if (this.state.adminMode) {
            className += " btn-danger";
          } else {
            if (this.state.deviceOverview[element.device].switches[element.name].value === 1) {
              className += " btn-warning";
            } else {
              className += " btn-secondary";
            }
          }
          elementList.push(
            <a key={index} href="#" className={className} title={this.state.deviceOverview[element.device].switches[element.name].display} style={style} onClick={(e) => this.selectElement(e, element, index)} draggable={true} onDragStart={(e) => this.dragMoveElt(e, index)}>
              {iconJsx}
            </a>
          );
        }
      } else if (element.type === "dimmer") {
        if (this.state.deviceOverview[element.device] && this.state.deviceOverview[element.device].dimmers && this.state.deviceOverview[element.device].dimmers[element.name] && (this.state.deviceOverview[element.device].dimmers[element.name].enabled || this.state.adminMode)) {
          let className = "map-img-element btn";
          if (this.state.adminMode) {
            className += " btn-danger";
          } else {
            if (this.state.deviceOverview[element.device].dimmers[element.name].value > 0) {
              className += " btn-warning";
            } else {
              className += " btn-secondary";
            }
          }
          elementList.push(
            <a key={index} href="#" className={className} title={this.state.deviceOverview[element.device].dimmers[element.name].display} style={style} onClick={(e) => this.selectElement(e, element, index)} draggable={true} onDragStart={(e) => this.dragMoveElt(e, index)}>
              <i className="fa fa-lightbulb-o elt-left" aria-hidden="true"></i>
              {this.state.deviceOverview[element.device].dimmers[element.name].value===99?100:this.state.deviceOverview[element.device].dimmers[element.name].value}%
            </a>
          );
        }
      } else if (element.type === "blind") {
        if (this.state.deviceOverview[element.device] && this.state.deviceOverview[element.device].blinds && this.state.deviceOverview[element.device].blinds[element.name] && (this.state.deviceOverview[element.device].blinds[element.name].enabled || this.state.adminMode)) {
          let className = "map-img-element btn";
          if (this.state.adminMode) {
            className += " btn-danger";
          } else {
            if (this.state.deviceOverview[element.device].blinds[element.name].value > 0) {
              className += " btn-warning";
            } else {
              className += " btn-secondary";
            }
          }
          elementList.push(
            <a key={index} href="#" className={className} title={this.state.deviceOverview[element.device].blinds[element.name].display} style={style} onClick={(e) => this.selectElement(e, element, index)} draggable={true} onDragStart={(e) => this.dragMoveElt(e, index)}>
              <i className="fa fa-window-maximize elt-left" aria-hidden="true"></i>
              {this.state.deviceOverview[element.device].blinds[element.name].value}%
            </a>
          );
        }
      } else if (element.type === "sensor") {
        if (this.state.deviceOverview[element.device] && this.state.deviceOverview[element.device].sensors && this.state.deviceOverview[element.device].sensors[element.name] && (this.state.deviceOverview[element.device].sensors[element.name].enabled || this.state.adminMode)) {
          let className = "map-img-element btn";
          let valueJsx = this.state.deviceOverview[element.device].sensors[element.name].value;
          if (!isNaN(valueJsx) && valueJsx != Math.round(valueJsx)) {
            valueJsx = valueJsx.toFixed(2)
          }
          if (this.state.deviceOverview[element.device].sensors[element.name].options && this.state.deviceOverview[element.device].sensors[element.name].options.unit) {
            valueJsx += this.state.deviceOverview[element.device].sensors[element.name].options.unit;
          }
          if (this.state.adminMode) {
            className += " btn-danger";
          } else {
            className += " btn-secondary";
          }
          elementList.push(
            <a key={index} href="#" className={className} title={this.state.deviceOverview[element.device].sensors[element.name].display} style={style} onClick={(e) => this.selectElement(e, element, index)} draggable={true} onDragStart={(e) => this.dragMoveElt(e, index)}>
              {valueJsx}
            </a>
          );
        }
      } else if (element.type === "serviceMock") {
        this.state.serviceList.forEach(service => {
          if (service.name === "mock-service") {
            service.element.forEach(elt => {
              if (elt.name === element.name) {
                let className = "map-img-element btn";
                if (this.state.adminMode) {
                  className += " btn-danger";
                } else {
                  className += " btn-secondary";
                }
                elementList.push(
                  <a key={index} href="#" className={className} title={element.name} style={style} onClick={(e) => this.selectElement(e, element, index)} draggable={true} onDragStart={(e) => this.dragMoveElt(e, index)}>
                    {element.name}
                  </a>
                );
              }
            });
          }
        });
      } else if (element.type === "mpdService") {
        this.state.serviceList.forEach(service => {
          if (service.name === "service-mpd") {
            service.element.forEach(elt => {
              if (elt.name === element.name) {
                elementList.push(
                  <div key={index} draggable={true} onDragStart={(e) => this.dragMoveElt(e, index)}>
                    <MapMpdServiceButton element={element}
                                         adminMode={this.state.adminMode}
                                         style={style}
                                         selectCb={this.selectElement}
                                         index={index} />
                  </div>
                );
              }
            });
          }
        });
      } else if (element.type === "weathermapService") {
        this.state.serviceList.forEach(service => {
          if (service.name === "service-weathermap") {
            service.element.forEach(elt => {
              if (elt.name === element.name) {
                elementList.push(
                  <div key={index} draggable={true} onDragStart={(e) => this.dragMoveElt(e, index)}>
                    <MapWeathermapServiceButton element={element}
                                                service={elt}
                                                adminMode={this.state.adminMode}
                                                style={style}
                                                selectCb={this.selectElement}
                                                index={index} />
                  </div>
                );
              }
            });
          }
        });
      } else if (element.type === "script") {
        let className = "map-img-element btn";
        if (this.state.adminMode) {
          className += " btn-danger";
        } else {
          className += " btn-secondary";
        }
        elementList.push(
          <a key={index}
             href="#"
             className={className}
             title={element.name}
             style={style}
             onClick={(e) => this.selectElement(e, element, index)}
             draggable={true}
             onDragStart={(e) => this.dragMoveElt(e, index)}>
            <i className="fa fa-play-circle elt-left" aria-hidden="true"></i>
            {element.name}
          </a>
        );
      } else if (element.type === "scheduler") {
        this.state.scheduler.forEach(scheduler => {
          if (scheduler.name === element.name) {
            let className = "map-img-element btn";
            if (this.state.adminMode) {
              className += " btn-danger";
            } else {
              className += " btn-secondary";
            }
            element.scheduler = scheduler;
            let classJsx = "fa fa-calendar-times-o";
            if (scheduler.enabled) {
              classJsx = "fa fa-calendar-check-o";
            }
            elementList.push(
              <a key={index} href="#" className={className} title={element.name} style={style} onClick={(e) => this.selectElement(e, element, index)} draggable={true} onDragStart={(e) => this.dragMoveElt(e, index)}>
                <div>
                  <i className={classJsx + " elt-left"} aria-hidden="true"></i>
                  {element.name}
                </div>
              </a>
            );
          }
        });
      }
    });
    return (
      <div className="text-center map-with-margin">
        <div className="map-container" id="mapContainer">
          <img src={this.state.map.image} className="map-img" onDrop={this.dropElt} onDragOver={this.allowDrop}/>
          {elementList}
        </div>
        {mapPlaceJsx}
      </div>
    );
  }
}

export default Map;
