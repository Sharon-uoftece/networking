import React, { useState, useLayoutEffect, useEffect } from "react";
import { Page } from "./common";
import {Button,} from "@blueprintjs/core";
import correctData from "./correctData"


function Submission({page,setPage,sensitivityMap, numEmails}:
    {page: number; setPage:(page:number) => void;sensitivityMap: Record<string, Record<string, any>>; numEmails: number}) {
    const [emailScore,setEmailScore] = useState(-1);
    const [dataToCompare, setDataToCompare] = useState<any>([]);
  
    function getScore() {
        let score = 0;
        console.log(dataToCompare);
        for (const element of Object.keys(sensitivityMap)){
            if (parseInt(element) > numEmails) {
                break;
            }
            const index = parseInt(element) - 1;
            if (sensitivityMap[element]["sensitive"] == correctData[index]["sensitive"]) {
                score += 1;
            }
        }
        setEmailScore(score);
    }

    useEffect(getScore, []);

    return (
        <div className="score-page">
            <Button
                icon="arrow-left"
                intent="warning"
                text={"Back to User Info Page"}
                onClick={() => {
                    setPage(Page.UserInfo);
                }}
            />
            <p className="show-score">Thank you for completing this survey! </p>
            <p className="show-score-2">You scored {emailScore}/{numEmails} on this survey</p>
            <p className="show-score-3">You now have {emailScore} Reward Points</p>
            <button className="redeem-button">redeem Reward Points</button>
        </div>
    )
}

export default Submission;