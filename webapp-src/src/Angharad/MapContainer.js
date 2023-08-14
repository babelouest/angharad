import React, { Component } from 'react';
import i18next from 'i18next';

import MapAdminButtons from './MapAdminButtons';
import messageDispatcher from '../lib/MessageDispatcher';
import Map from './Map';

class MapContainer extends Component {
  constructor(props) {
    super(props);

    this.state = {
      config: props.config,
      deviceOverview: props.deviceOverview,
      serviceList: props.serviceList,
      script: props.script,
      scheduler: props.scheduler,
      mapList: props.mapList,
      adminMode: props.adminMode,
      mapIndex: props.mapIndex
    }
    
    this.addMap = this.addMap.bind(this);
    this.selectMap = this.selectMap.bind(this);
  }
  
  static getDerivedStateFromProps(props, state) {
    return props;
  }

  addMap() {
    messageDispatcher.sendMessage("Map", {add: true, map: {name: "", description: "", enabled: true, image: false, elements: [], index: this.state.mapList.length}});
  }
  
  selectMap(e, index) {
    e.preventDefault();
    messageDispatcher.sendMessage("Map", {status: "select", index: index});
  }
  
  render() {
    let addButtonJsx, mapLinksJsx = [];
    if (this.state.adminMode) {
      addButtonJsx = 
        <button type="button" className="btn btn-secondary" onClick={this.addMap}>
          <i className="fa fa-plus" aria-hidden="true"></i>
        </button>
    }
    let adminButtonsJsx;
    if (this.state.adminMode) {
      adminButtonsJsx =
        <MapAdminButtons map={this.state.mapList[this.state.mapIndex]} />
    }
    this.state.mapList.forEach((map, index) => {
      let className = "btn elt-left elt-top";
      if (index === this.state.mapIndex) {
        className += " btn-primary";
      } else {
        className += " btn-outline-secondary";
      }
      mapLinksJsx.push(
        <a key={index} className={className} href={"#/map/"+index} onClick={(e) => this.selectMap(e, index)}>
          <i className="fa fa-map elt-left" aria-hidden="true"></i>
          {map.name}
        </a>
      );
    });
    let classNameMapJsx = "col", classNameButtonJsx = "hidden";
    if (this.state.adminMode) {
      classNameMapJsx = "col-sm-10";
      classNameButtonJsx = "col-sm-2";
    }
    return (
      <div>
        <div className="row">
          <div className="col-sm-11">
            <h4>
              {i18next.t("maps.title")}
            </h4>
          </div>
          <div className="col-sm-1 align-items-end">
            {addButtonJsx}
          </div>
        </div>
        <hr/>
        <div className="row elt-top">
          <div className="col text-center">
            {mapLinksJsx}
          </div>
        </div>
        <div className="row">
          <div className={classNameMapJsx}>
            {this.state.mapList[this.state.mapIndex] ? <Map config={this.state.config}
                 map={this.state.mapList[this.state.mapIndex]}
                 deviceOverview={this.state.deviceOverview} 
                 serviceList={this.state.serviceList} 
                 script={this.state.script} 
                 scheduler={this.state.scheduler} 
                 adminMode={this.state.adminMode} /> : null}
          </div>
          <div className={classNameButtonJsx}>
            {adminButtonsJsx}
          </div>
        </div>
      </div>
    );
  }
}

export default MapContainer;
