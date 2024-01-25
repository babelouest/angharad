import React, { Component } from 'react';
import * as echarts from 'echarts';

import apiManager from '../lib/APIManager';

class SimpleMonitor extends Component {
  constructor(props) {
    super(props);

    this.state = {
      device: props.device,
      type: props.type,
      name: props.name,
      display: props.display,
      value: props.value,
      unit: props.unit,
      from: props.from,
      to: props.to
    }
    
    this.updateData = this.updateData.bind(this);
    
    this.updateData();
  }

  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  updateData() {
    let from = Math.floor(this.state.to - this.state.from);
    apiManager.APIRequestBenoic("monitor/" + encodeURIComponent(this.state.device) + "/" + encodeURIComponent(this.state.type) + "/" + encodeURIComponent(this.state.name) + "?from=" + from + "&to=" + this.state.to)
    .then(result => {
      let data = [];
      result.forEach(val => {
        data.push([new Date(val.timestamp*1000).toISOString(), val.value]);
      });
      let legend = this.state.display;
      if (isNaN(this.state.value)) {
        legend += " (" + this.state.unit.trim() + ")";
      } else {
        legend += ": " + this.state.value.toFixed(2);
        if (this.state.unit) {
          legend += this.state.unit;
        }
      }

      let option = {
        title: {
          text: legend
        },
        tooltip: {
          trigger: 'axis'
        },
        legend: {
          data: [legend]
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
