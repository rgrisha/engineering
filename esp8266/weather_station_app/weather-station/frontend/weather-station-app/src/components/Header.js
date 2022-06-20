import React from 'react'
import { Link } from 'react-router-dom'
import './Header.css';

const Header = () => (
  <header>
    <div className="container">
      <div id="branding">
        <h1><i className="fa fa-rocket"></i> <span className="highlight">Oro </span>stotelė</h1>
      </div>
      <nav>
        <ul>
          <li><Link to='/'>Dabar</Link></li>
          <li><Link to='/history'>Istorija</Link></li>
          <li><Link to='/about'>About</Link></li>
        </ul>
      </nav>
    </div>
  </header>
)

export default Header



