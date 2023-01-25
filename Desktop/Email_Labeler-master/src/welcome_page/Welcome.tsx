import React, { useState, useLayoutEffect } from "react";
import { Page } from "../common";
import {Link as LinkClick} from 'react-router-dom';

function Welcome({ page, setPage }:
  {
      page: number,
      setPage: (page: number) => void
  }) {



  useLayoutEffect(() => {
    window.scrollTo(0, 0);
  }, []);

  return (
    <div className="welcome" id='welcome'>
      <p className="welcome-text">WELCOME!</p>
      <p className="welcome-sub-block"> <mark className="mark">
        &nbsp;This is the space where you could help &nbsp; &nbsp;improve accuracy
        of our Machine&nbsp; &nbsp;Learning Model to identify Data Breaches.&nbsp; </mark>
      </p>
      {/* <nav className="welcome-log-in-nav">
        <LinkClick to="/login" className="welcome-log-in">
          <button className="welcome-button">Log in</button>
        </LinkClick>
      </nav>                      */}
    </div>
  );
}

export default Welcome;
