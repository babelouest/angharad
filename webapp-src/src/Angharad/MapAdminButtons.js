import React, { Component } from 'react';
import i18next from 'i18next';

import messageDispatcher from '../lib/MessageDispatcher';

class MapAdminButtons extends Component {
  constructor(props) {
    super(props);

    this.state = {
      map: props.map,
      mapPlace: props.mapPlace
    }
    
    this.openMapModal = this.openMapModal.bind(this);
    this.openPlaceElements = this.openPlaceElements.bind(this);
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  openMapModal(e, remove) {
    e.preventDefault();
    messageDispatcher.sendMessage("Map", {map: this.state.map, add: false, remove: remove});
  }
  
  openPlaceElements(e) {
    e.preventDefault();
    messageDispatcher.sendMessage("Map", {status: "place", map: this.state.map});
  }

	render() {
    let selectedJsx = "";
    if (this.state.mapPlace) {
      selectedJsx = " active";
    }
		return (
      <div className="dropdown">
        <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
          <i className="fa fa-cog" aria-hidden="true"></i>
        </button>
        <ul className="dropdown-menu">
          <li>
            <a className={"dropdown-item"+selectedJsx} href="#" onClick={(e) => this.openPlaceElements(e, false)}>
              <i className="fa fa-gears elt-left" aria-hidden="true"></i>
              {i18next.t("maps.map-place-elements")}
            </a>
          </li>
          <li>
            <a className="dropdown-item" href="#" onClick={(e) => this.openMapModal(e, false)}>
              <i className="fa fa-edit elt-left" aria-hidden="true"></i>
              {i18next.t("modal.edit")}
            </a>
          </li>
          <li>
            <a className="dropdown-item" href="#" onClick={(e) => this.openMapModal(e, true)}>
              <i className="fa fa-trash elt-left" aria-hidden="true"></i>
              {i18next.t("modal.remove")}
            </a>
          </li>
        </ul>
      </div>
		);
	}
}

export default MapAdminButtons;
