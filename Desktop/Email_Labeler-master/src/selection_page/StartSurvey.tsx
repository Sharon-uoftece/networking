import React, { useState, useEffect } from "react";
import axios from "axios";
import { Button, NumericInput} from "@blueprintjs/core";
import { Select2 } from "@blueprintjs/select";
import { Page, Header } from "../common";

function StartSurvey(
    { numEmails, setNumEmails, page, setPage}:
        {
            numEmails: number,
            setNumEmails: (num: number) => void,
            page: number,
            setPage: (page: number) => void,
        }
) {

    function handleStartLabel() {
        setPage(Page.LabelGeneral);
    }

    function handleBackToWelcome() {
        setPage(Page.Welcome);
    }

    return (
        <>
            <div className="survey">
                <Button
                    icon="arrow-left"
                    intent="warning"
                    text={"Back to User Info Page"}
                    onClick={() => {
                        setPage(Page.UserInfo);
                    }}
                />
                <div className="survey-Block">
                    <div className="survey-Text">
                        <p>I want to label&nbsp;</p>
                        <NumericInput
                            leftIcon="envelope"
                            size={2}
                            value={numEmails}
                            onValueChange={(val: number) => {
                                Number.isFinite(val) &&
                                    val >= 0 &&
                                    val <= 11 &&
                                    setNumEmails(val);
                            }}
                            large={true}
                            intent="warning"
                            max={11}
                            min={1}
                        />
                        <p>&nbsp;emails today.&nbsp;</p>
                    </div>
                </div>
                <button className="survey-button" onClick={handleStartLabel}>START</button>
            </div>
        </>
    );
}

export default StartSurvey;