import React from 'react';
import {BrowserRouter as Router, Route, Routes} from 'react-router-dom'
import './App.scss';
import {Page} from './common';
import MainFlow from './MainFlow';
import LogIn from './welcome_page/LogIn'


function App() {
  return (
    <Router>
      <div>
        <Routes>
          <Route path="/" element={<MainFlow />}/>
          {/* <Route path="/login" element={<LogIn page={page} setPage={setPage}/>}/> */}
        </Routes>
      </div>
    </Router>
  );
}

export default App;

