import React from "react"
import {Page, Header} from "./common";

function UserInfo({ page, setPage, currentUser, setCurrentUser}: { page: number, setPage: (page: number) => void, currentUser: string, setCurrentUser: (currentUser: string) => void}) {
    function handleStartLabel() {
        setPage(Page.Survey);
    }
    function handleShowHistory() {
        setPage(Page.LabelHistory);
    }

    return(
        <div className="userinfo">
            <Header />
            <h1 className="userinfo-welcome-text">Hi {currentUser}!</h1>
            <button 
                className="userinfo-history-button"
                onClick={handleShowHistory}>
                See previous labeling history...
            </button>
            <button 
                className="userinfo-label-button"
                onClick={handleStartLabel}
            >
                Start new labelling here...
            </button>
            
        </div>
    )
}

export default UserInfo;