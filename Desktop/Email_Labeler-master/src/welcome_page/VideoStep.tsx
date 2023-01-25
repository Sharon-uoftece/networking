import React from "react";

function VideoStep() {
    
    return(
        <div className="tut-vid-section" id="vid-tut">
            <div className="tut-connector">
                <p className="connector-one-text">Quick Tutorial</p>
            </div>
            <div className="tut-header">
                <p>How to use MailLabeller</p>
            </div>
            <div className="tut-vid-setup">
                <video  
                className="tut-vids"
                autoPlay
                muted
                loop
                src={require('./tutStep1.mp4')} />
                <video  
                className="tut-vids"
                autoPlay
                muted
                loop
                src={require('./tutStep2.mp4')} />
                <video  
                className="tut-vids"
                autoPlay
                muted
                loop
                src={require('./tutStep3.mp4')} />
            </div>
            <div className="tut-step-title">
                <p className="tut-title-1">Step 1: Select # of Email and Start</p>
                <p className="tut-title-2">Step 2: Label each email</p>
                <p className="tut-title-3">Step 3: Submit your results</p>
            </div>
            <div className="tut-step-explain">
                <p className="tut-step-explain-1">Select minimum 1 email and maximum 9 emails, then click the start 
                button to proceed to the actual labelling page.</p>
                <p className="tut-step-explain-2">There will be 27 pieces of information displayed, they will help you
                better identify if an email containes sensitive information.</p>
                <p className="tut-step-explain-3">When you are ready to submit, simply click the sumbit button. After that, 
                you will also be able to see the history of your records when you are signed in.</p>
            </div>
        </div>
    )
};

export default VideoStep