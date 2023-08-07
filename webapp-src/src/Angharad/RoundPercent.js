import React, { Component } from 'react';
import i18next from 'i18next';

class RoundPercent extends Component {
  constructor(props) {
    super(props);

    this.state = {
      value: props.value
    }
}

  static getDerivedStateFromProps(props, state) {
    return props;
  }

	render() {
    return (
      <div className={"c100 center p" + this.state.value}>
          <span>
            {this.state.value}%
          </span>
          <div className="slice">
              <div className="bar"></div>
              <div className="fill"></div>
          </div>
      </div>
		);
	}
}

export default RoundPercent;
