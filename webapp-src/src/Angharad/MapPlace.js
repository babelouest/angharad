import React, { Component } from 'react';
import i18next from 'i18next';

import messageDispatcher from '../lib/MessageDispatcher';
import apiManager from '../lib/APIManager';

class MapPlace extends Component {
  constructor(props) {
    super(props);

    this.state = {
      title: props.title,
      eltList: props.eltList,
      type: props.type,
      deviceOverview: props.deviceOverview,
      cbDrag: props.cbDrag
    }
  }
  
  static getDerivedStateFromProps(props, state) {
    return props;
  }

  dragElt(e, device, type, name) {
    this.state.cbDrag(e, device, type, name);
  }
  
  render() {
    let eltListJsx = [];
    if (this.state.type === "switch") {
      this.state.eltList.forEach((elt, index) => {
        eltListJsx.push(
          <div key={elt.device+"-sw-"+elt.name} className="col-sm-3 border rounded elt-left" draggable={true} onDragStart={(e) => this.dragElt(e, elt.device, "switch", elt.name)}>
            <label className="d-flex align-items-center">
              <i className="fa fa-power-off elt-left" aria-hidden="true"></i>
              {elt.display}
            </label>
          </div>
        );
      });
    } else if (this.state.type === "dimmer") {
      this.state.eltList.forEach((elt, index) => {
        eltListJsx.push(
          <div key={elt.device+"-di-"+elt.name} className="col-sm-3 border rounded elt-left" draggable={true} onDragStart={(e) => this.dragElt(e, elt.device, "dimmer", elt.name)}>
            <label className="d-flex align-items-center">
              <i className="fa fa-lightbulb-o elt-left" aria-hidden="true"></i>
              {elt.display}
            </label>
          </div>
        );
      });
    } else if (this.state.type === "blind") {
      this.state.eltList.forEach((elt, index) => {
        eltListJsx.push(
          <div key={elt.device+"-di-"+elt.name} className="col-sm-3 border rounded elt-left" draggable={true} onDragStart={(e) => this.dragElt(e, elt.device, "blind", elt.name)}>
            <label className="d-flex align-items-center">
              <i className="fa fa-window-maximize elt-left" aria-hidden="true"></i>
              {elt.display}
            </label>
          </div>
        );
      });
    } else if (this.state.type === "sensor") {
      this.state.eltList.forEach((elt, index) => {
        eltListJsx.push(
          <div key={elt.device+"-di-"+elt.name} className="col-sm-3 border rounded elt-left" draggable={true} onDragStart={(e) => this.dragElt(e, elt.device, "sensor", elt.name)}>
            <label className="d-flex align-items-center">
              <i className="fa fa-line-chart elt-left" aria-hidden="true"></i>
              {elt.display}
            </label>
          </div>
        );
      });
    } else if (this.state.type === "serviceMock") {
      this.state.eltList.forEach((elt, index) => {
        eltListJsx.push(
          <div key={"serMoc-"+elt.name} className="col-sm-3 border rounded elt-left" draggable={true} onDragStart={(e) => this.dragElt(e, undefined, "serviceMock", elt.name)}>
            <label className="d-flex align-items-center">
              {elt.name}
            </label>
          </div>
        );
      });
    } else if (this.state.type === "mpdService") {
      this.state.eltList.forEach((elt, index) => {
        eltListJsx.push(
          <div key={"serMpd-"+elt.name} className="col-sm-3 border rounded elt-left" draggable={true} onDragStart={(e) => this.dragElt(e, undefined, "mpdService", elt.name)}>
            <label className="d-flex align-items-center">
              {elt.name}
            </label>
          </div>
        );
      });
    } else if (this.state.type === "script") {
      this.state.eltList.forEach((elt, index) => {
        eltListJsx.push(
          <div key={"script-"+index} className="col-sm-3 border rounded elt-left" draggable={true} onDragStart={(e) => this.dragElt(e, undefined, "script", elt.name)}>
            <label className="d-flex align-items-center">
              <i className="fa fa-play-circle elt-left" aria-hidden="true"></i>
              {elt.name}
            </label>
          </div>
        );
      });
    } else if (this.state.type === "scheduler") {
      this.state.eltList.forEach((elt, index) => {
        eltListJsx.push(
          <div key={"scheduler-"+index} className="col-sm-3 border rounded elt-left" draggable={true} onDragStart={(e) => this.dragElt(e, undefined, "scheduler", elt.name)}>
            <label className="d-flex align-items-center">
              <i className="fa fa-calendar elt-left" aria-hidden="true"></i>
            </label>
          </div>
        );
      });
    }
    return (
      <div className="accordion-item">
        <h2 className="accordion-header">
          <button className="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target={"#collapse-"+this.state.type} aria-expanded="true" aria-controls={"collapse-"+this.state.type}>
            {this.state.title}
          </button>
        </h2>
        <div id={"collapse-"+this.state.type} className="accordion-collapse collapse" data-bs-parent="#accordionElements">
          <div className="accordion-body">
            <div className="row">
              {eltListJsx}
            </div>
          </div>
        </div>
      </div>
    );
  }
}

export default MapPlace;
