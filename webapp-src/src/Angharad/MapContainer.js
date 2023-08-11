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
      profileList: props.profileList,
      adminMode: props.adminMode,
      mapIndex: props.mapIndex
    }
    
    this.addMap = this.addMap.bind(this);
  }
  
  static getDerivedStateFromProps(props, state) {
    return props;
  }

  addMap() {
    messageDispatcher.sendMessage("Map", {add: true, map: {name: "", description: "", enabled: true, image: false, elements: []}});
  }
  
  render() {
    let addButtonJsx;
    if (this.state.adminMode) {
      addButtonJsx = 
        <button type="button" className="btn btn-secondary" onClick={this.addMap}>
          <i className="fa fa-plus" aria-hidden="true"></i>
        </button>
    }
    let adminButtonsJsx;
    let curMapJsx, curIndex = 0;
    this.state.profileList.forEach((profile, index) => {
      if (!!profile.image && Array.isArray(profile.elements) && (profile.enabled || this.state.adminMode)) {
        if (curIndex === this.state.mapIndex) {
          if (this.state.adminMode) {
            adminButtonsJsx =
              <MapAdminButtons map={profile} />
          }
          curMapJsx =
            <div className="row">
              <div className="col-sm-10">
                <Map config={this.state.config}
                     map={profile}
                     deviceOverview={this.state.deviceOverview} 
                     serviceList={this.state.serviceList} 
                     script={this.state.script} 
                     scheduler={this.state.scheduler} 
                     adminMode={this.state.adminMode} />
              </div>
              <div className="col-sm-2">
                {adminButtonsJsx}
              </div>
            </div>
        }
        curIndex++;
      }
    });
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
        {curMapJsx}
      </div>
    );
  }
}

export default MapContainer;
