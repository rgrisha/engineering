import React, { Component } from 'react';
import './WeatherNow.css';
import API from '../api'
import '../../node_modules/font-awesome/css/font-awesome.min.css';

export default class WeatherNow extends Component {

  constructor(props) {
    super(props);
    this.state = {
      data: {}
    }
  }

  componentDidMount() {
    API.lastData()
      .then(
        (data) => {this.setState({data})}
      )
      .catch(
        (err) => {console.error(`error fetching weatherNow data:  ${err}`)})
  }

  render() {
    const {data} = this.state;


    if(!data.measurements) {
      return null;
    }

    return (
      <section id="weather-now">
        <div id="temperature" className="nowItem fa fa-thermometer">Temperatūra {data.measurements[0].temp}°C</div>
        <div id="pressure" className="nowItem fa fa-compress">Slėgis {data.measurements[0].pressure} hPa</div>
        <div id="humidity" className="nowItem fa fa-tint">Santykinis drėgnis {data.measurements[0].humidity}%</div>
        <div id="last-dt" className="">Paskutinis atnaujinimas {data.dt}</div>
      </section>
      );
  }

}

