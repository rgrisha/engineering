import React from 'react'
import { Switch, Route } from 'react-router-dom'
import WeatherNow from './WeatherNow'
import History from './History'
import About from './About'

const Main = () => (
  <main>
    <Switch>
      <Route exact path='/' component={WeatherNow}/>
      <Route path='/history' component={History}/>
      <Route path='/about' component={About}/>
    </Switch>
  </main>
)

export default Main

