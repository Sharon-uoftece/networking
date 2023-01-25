import React, { useState, useLayoutEffect, useEffect } from "react";
import {Button, FormGroup, Alert, Slider as BSlider} from "@blueprintjs/core";
import {Popover2} from "@blueprintjs/popover2";
import {Page, Tab} from "../common";
import EmailData from "../label_page/emailData";
import Slider from "react-slick";
import "slick-carousel/slick/slick.css";
import "slick-carousel/slick/slick-theme.css";
 
function EmailBox({ index, email, sensitivityMap, setSensitivityMap}: 
  {index: number, email: any, sensitivityMap: Record<string, boolean>, setSensitivityMap: (val: any) => void}) {
  const [confidence, setConfidence] = useState(1);
  const [pop, setPop] = useState(false);
  const [popoverContent, setPopoverContent] = useState(Tab.SenderInfo);

  function handelScatterPlot() {
    setPopoverContent(Tab.ScatterPlot);
  }

  function handelEmailHistory() {
    setPopoverContent(Tab.EmailHistory);
  }

  function handelSenderInfo() {
    setPopoverContent(Tab.SenderInfo);
  }

  const settings = {
    dots: true,
    infinite: true,
    speed: 500,
    slidesToShow: 1,
    slidesToScroll: 1
  };
  return (
    <div className="email-element">
      <Slider {...settings}>
        <div className="email-box-header">
          <p className="email-header">{"Email " + (index + 1)}</p>
          <p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</p>

        <FormGroup 
        // className="email-box-labels" 
        inline={true} style={{ position: "relative", top: -20 }}>
          <Popover2
            interactionKind="click"
            isOpen={pop}
            fill={false}
            placement="bottom-end"
            content={
              <div className="popover">
                <div className="popover-button">
                  <div>
                    <button 
                      className="popover-content-button"
                      onClick={handelSenderInfo}
                    > SenderInfo
                    </button>
                    <hr className="popover-separator"/>
                  </div>

                  <div>
                    <button 
                      className="popover-content-button"
                      onClick={handelScatterPlot}
                    > ScatterPlot
                    </button>
                    <hr className="popover-separator"/>
                  </div>
            
                  <div>
                    <button 
                      className="popover-content-button"
                      onClick={handelEmailHistory}
                    > EmailHistory
                    </button>
                  </div>

                </div>
                <div className="popover-content-detail">
                  {popoverContent == Tab.SenderInfo && <div>
                    <p>Day Since Hire: {JSON.stringify(email.day_since_hire, null, 2).slice(1,-1)}</p> 
                    <p>Division: {JSON.stringify(email.division, null, 2).slice(1,-1)}</p> 
                  </div>}

                  {popoverContent == Tab.ScatterPlot && <div>
                    <p>Below display {email.sender} scatter plot: </p>
                  </div>
                  }

                  {popoverContent == Tab.EmailHistory && <div>
                    <p>Below display {email.sender} email history: </p>
                  </div>
                  }
                </div>

              </div>
            }
          >
            <button 
              className="sender-button"
              id={"sender-button" + (index + 1)}
              onClick={()=> {
                  console.log("sender" + (index+1));
                  setPop(!pop);
                }
              }
            > 
              More Info regarding Sender {email.sender}
            </button>
          </Popover2>
        
        </FormGroup>
      </div>

      <hr className="separator" />
      <div className="email-content">
        <p>EmailId: {JSON.stringify(email.mid, null, 2).slice(1,-1)}</p> 
        <p>DateSent: {JSON.stringify(email.year, null, 2).slice(1,-1)}/{JSON.stringify(email.month, null, 2).slice(1,-1)}/{JSON.stringify(email.day, null, 2).slice(1,-1)}</p> 
        <p>Sender: {JSON.stringify(email.sender, null, 2).slice(1,-1)}</p> 
        <p>Recipient: {JSON.stringify(email.rcpt, null, 2).slice(1,-1)}</p> 
        <p>Recipient Count: {JSON.stringify(email.rcpt_count, null, 2).slice(1,-1)}</p> 
        <p>Files: {JSON.stringify(email.files, null, 2).slice(1,-1)}</p> 
        <p>Files Count: {JSON.stringify(email.files_sensitive_count, null, 2).slice(1,-1)}</p> 
        <p>Files Size: {JSON.stringify(email.size, null, 2).slice(1,-1)}</p> 
      </div>

      <FormGroup className="email-box-labels" inline={true} style={{ position: "relative", top: -0.5 }}>

          <button 
            className={"label-button"} 
            id={"label-button-" + (index + 1)}
            onClick={() => {
              const newMap = { ...sensitivityMap };
              newMap[index + 1] = true;
              setSensitivityMap(newMap);
              document.getElementById("label-button-" + (index + 1))!.style.backgroundColor="rgb(182, 59, 59)";
              document.getElementById("label-button-" + (index + 1))!.style.color="rgb(255,255,255)";
              document.getElementById("label-button-non-" + (index + 1))!.style.backgroundColor="rgb(255, 255, 255)";
              document.getElementById("label-button-non-" + (index + 1))!.style.color="rgb(0, 0, 0)";
            }}
          >
            Anomalous
          </button>

          <button 
            className={"label-button-non"}
            id={"label-button-non-" + (index + 1)} 
            onClick={() => {
              const newMap = { ...sensitivityMap };
              newMap[index + 1] = false;
              setSensitivityMap(newMap);
              document.getElementById("label-button-non-" + (index + 1))!.style.backgroundColor="rgb(45, 125, 45)";
              document.getElementById("label-button-non-" + (index + 1))!.style.color="rgb(255,255,255)";
              document.getElementById("label-button-" + (index + 1))!.style.backgroundColor="rgb(255, 255, 255)";
              document.getElementById("label-button-" + (index + 1))!.style.color="rgb(0, 0, 0)";
            }}
          >
            Non-Anomalous
          </button>

          <BSlider
            className="confidence-slider"
            min={1}
            max={10}
            stepSize={0.1}
            labelPrecision={0.1}
            onChange={(val) => setConfidence(val)}
            value={confidence}
            intent="none"
          />

        </FormGroup>
      </Slider>
    </div>
  );
}

function Label({ numEmails, page, setPage }:
  {numEmails: number,page: number,setPage: (page: number) => void,}) {

  const [alertExitPage, setAlertExitPage] = useState(false);
  const [submit, setSubmit] = useState(false);
  const [confidence, setConfidence] = useState(0);
  const [emails, setEmails] = useState<any>([]);
  
  function handleSubmit() {
    console.log("Sensitivity map: ", sensitivityMap);
    setSubmit(!submit);
  }

  useEffect(() => {
    // fetch("https://random-data-api.com/api/stripe/random_stripe?size=" + numEmails)
    //   .then(response => response.json())
    //   .then(jsonData => {
    //     // console.log("JSON DATA: ", jsonData);
    //     setEmails(jsonData)
    //   });
    const emailsToShow = [];
    for (let i = 0; i < numEmails; i++) {
      emailsToShow.push(EmailData[i]);
    }
    setEmails(emailsToShow);
  }, [numEmails]);

  // {
  //   "email1": {
  //     sensitive: false,
  //     confidence: 0.7,
  //     marked: true
  //   }
  // }
  const initialMap: Record<string, Record<string, any>> = {};
  for (let i = 0; i < numEmails; i++) {
    initialMap[i + 1] = {
      sensitive: false,
      confidence: 0,
      marked: false
    };
  }
  const [sensitivityMap, setSensitivityMap] = useState(initialMap);

  const mappingFunc = (email: any, index: number, map: any, setMap: any) => {
    return (
      <EmailBox index={index} email={email} sensitivityMap={map} setSensitivityMap={setMap}/>
    );
  };

  const handleExitCancel = () => {
    setAlertExitPage(!alertExitPage);
  };

  const handleExitConfirm = () => {
    setPage(Page.Survey);
  };

  useLayoutEffect(() => {
    window.scrollTo(0, 0);
  }, []);

  return (
    <div className="email-grid-big-block">
      <Button
        icon="arrow-left"
        intent="warning"
        text={"Back"}
        onClick={() => {
          setAlertExitPage(!alertExitPage);
        }}
      />

      <Alert
        className="alert-box"
        isOpen={alertExitPage}
        confirmButtonText="Exit"
        cancelButtonText="Cancel"
        icon="undo"
        intent="danger"
        onCancel={handleExitCancel}
        onConfirm={handleExitConfirm}
      >
        <h2 className="alert-header">Are you sure you want to exit?</h2>
        <p className="alert-sub">Your data will be lost.</p>
      </Alert>

      <div>
          {/* <div className="email-grid"> */}
            {emails.map((email: any, index: number) => mappingFunc(email, index, sensitivityMap, setSensitivityMap))}
          {/* </div> */}
      </div>
      
      {/* <button 
          className="submit-button-2"
          onClick={() => {
            console.log("Setting submit true");
            setSubmit(true);
          }}>
          SUBMIT
      </button> */}

      <Alert
        className="submit-box"
        isOpen={submit}
        icon="clean"
        intent="success"
        confirmButtonText="Submit"
        cancelButtonText="Cancel"
        onConfirm={handleSubmit}
        onCancel={()=>{
          setSubmit(false);
        }}
      >
        <h2 className="submit-header">Are you sure you want to submit?</h2>
        <p className="submit-sub">You can't undo this submit.</p>
        {/* <h2 className="submit-header">You have successfully submit the labelling.</h2>
        <p className="submit-sub">Thank you for your participation.</p> */}
      </Alert>
    </div>
  );
}

export default Label;
