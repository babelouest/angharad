import React, { Component } from 'react';
//import ReactECharts from 'echarts-for-react';
import * as echarts from 'echarts';

import apiManager from '../lib/APIManager';

class SimpleMonitor extends Component {
  constructor(props) {
    super(props);

    this.state = {
      device: props.device,
      type: props.type,
      name: props.name,
      element: props.element,
      from: 86400
    }
    
    this.updateData = this.updateData.bind(this);
    
    this.updateData();
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  updateData() {
    apiManager.APIRequestBenoic("monitor/" + encodeURIComponent(this.state.device) + "/" + encodeURIComponent(this.state.type) + "/" + encodeURIComponent(this.state.name) + "?from=" + (Math.floor(Date.now()/1000) - this.state.from))
    .then(result => {
      let data = [];
      result.forEach(val => {
        data.push([new Date(val.timestamp*1000).toISOString(), val.value]);
      });

      let option = {
        title: {
          text: this.state.name
        },
        tooltip: {
          trigger: 'axis'
        },
        legend: {
          data: [this.state.name]
        },
        xAxis: {
          type:"time",
        },
        yAxis: {
          type: 'value'
        },
        series: [{
          type: 'line',
          symbol: 'none',
          data: data,
          areaStyle: {}
        }]
      };
      var myChart = echarts.init(document.getElementById('myCharts'));
      myChart.setOption(option);
    });
  }
  
  componentDidUpdate() {
    this.updateData();
  }
  
  render() {
    return (
      <div id="myCharts" className="myCharts"></div>
    );
  }
}

export default SimpleMonitor;
