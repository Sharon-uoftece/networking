import React, { useEffect, useState } from "react"
import {Page, Header} from "./common";
import {Button,} from "@blueprintjs/core";

function LabelHistory({ page, setPage, currentUser, setCurrentUser}: { page: number, setPage: (page: number) => void, currentUser: string, setCurrentUser: (currentUser: string) => void}) {
    const [history, setHistory] = useState([]);
    const [numLabels, setNumLabels] = useState(0);
    const [userLabel, setUserLabel] = useState([]);

    const labelSubmitHandler = async() => {
        const response = await fetch('http://localhost:8000/getLabelHistory');
        var data = await response.json();
        // setHistory(data);
        const userHistory = data.filter((item: any) => item.user === currentUser)
            .map((item: any) => <p>{JSON.stringify(item)}<br></br></p>);
        setNumLabels(userHistory.length);
        setUserLabel(userHistory);
    }

    useEffect(() => {
        const fetchLabels = async() => await labelSubmitHandler();
        fetchLabels();
    }, []);
    
    return(
        <div>
            <Header />
            <Button
                icon="arrow-left"
                intent="warning"
                text={"Back to User Info Page"}
                onClick={() => {
                    setPage(Page.UserInfo);
                }}
            />
             <p> </p>
            <div className="history-show">
                <h1>You have labelled {numLabels} emails.</h1>
                {userLabel}
            </div>
            

        </div>
    )
}

export default LabelHistory