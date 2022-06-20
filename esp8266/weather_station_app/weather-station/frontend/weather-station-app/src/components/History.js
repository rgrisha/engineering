import React, { Component } from 'react';
import './History.css';
import API from '../api'
import { LineChart, Line, CartesianGrid, XAxis, YAxis, Tooltip  } from 'recharts';


export default class History extends Component {

  constructor(props) {
    super(props);
    this.state = {
      data: []
    }
  }

  componentDidMount() {
    API.history()
      .then(
        (data) => {console.log("have data: ", data); this.setState({data})}
      )
      .catch(
        (err) => {console.error(`error fetching history data:  ${err}`)}
      )
  }

  render() {
    const {data} = this.state;

    if(!Array.isArray(data) || data.length === 0) {
      console.log("no data array");
      return null;
    }

    return (
      <section id="history">
        <LineChart width={600} height={300} data={data} margin={{top: 5, right: 30, left: 20, bottom: 5}}>
          <XAxis dataKey="hour"/>
          <YAxis/>
          <CartesianGrid strokeDasharray="3 3"/>
          <Tooltip/>
          <Line type="monotone" dataKey="temp" stroke="#82ca9d" />
        </LineChart>
        <div className="chartFooterName">
          Temperatūra per paskutines 24 valandas
        </div>
        <LineChart width={600} height={300} data={data} margin={{top: 5, right: 30, left: 20, bottom: 5}}>
          <XAxis dataKey="hour"/>
          <YAxis type="number" domain={['dataMin - 10', 'dataMax + 10']} />
          <CartesianGrid strokeDasharray="3 3"/>
          <Tooltip/>
          <Line type="monotone" dataKey="pressure" stroke="#82ca9d" />
        </LineChart>
        <div className="chartFooterName">
          Slėgis per paskutines 24 valandas
        </div>
        <LineChart width={600} height={300} data={data} margin={{top: 5, right: 30, left: 20, bottom: 5}}>
          <XAxis dataKey="hour"/>
          <YAxis/>
          <CartesianGrid strokeDasharray="3 3"/>
          <Tooltip/>
          <Line type="monotone" dataKey="humidity" stroke="#82ca9d" />
        </LineChart>
        <div className="chartFooterName">
          Drėgnis per paskutines 24 valandas
        </div>
      </section>
    );

  }

}

