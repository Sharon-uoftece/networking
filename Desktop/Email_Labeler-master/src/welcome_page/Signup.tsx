import react, {useEffect, useState} from "react";
import axios from "axios";
import {Page, Header} from "../common";
import userData from "../userData";
import forbidUsers from "../forbidUsers";
import { exit } from "process";

function Signup({ page, setPage, currentUser, setCurrentUser}: { page: number, setPage: (page: number) => void, currentUser: string, setCurrentUser: (currentUser: string) => void}) {
    const [userName, setUserName] = useState('');
    const [password, setPassword] = useState('');
    const [forbid, setForbid] = useState(false);

    function signupHandler(e:React.SyntheticEvent) {
        e.preventDefault();

        console.log(userName, password);
        for (var i=0; i<forbidUsers.length; i++) {
            if (userName == forbidUsers[i].user) {
                setForbid(true);
                break;
            } 
        }
        if (forbid == false) {
            setUserName(userName);
            setPassword(password);
            signupHandle(e);
        }
    }

    const signupHandle = async(e:React.SyntheticEvent) => {
        e.preventDefault();
        
        const myData = {
            user: userName,
            password: password
        }
        console.log("frontend signup handler");

        const result = await fetch('http://localhost:8000/signup', {
            method: 'POST',
            mode: 'cors',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(myData)
        })
        .then((response) => console.log(response))
        .catch(err => console.log("ERROR:", err));

        console.log(JSON.stringify(result));
    }


    function usernameHandler(e:React.SyntheticEvent) {
        e.preventDefault();
        setForbid(false);
        setUserName(e.target.value);
    }

    function passwordHandler(e:React.SyntheticEvent) {
        e.preventDefault();
        setPassword(e.target.value);
    }
    
    return(
        <div>
            <div className="log-in-form">
                <h1>Sign up here...</h1>
                <form onSubmit={signupHandler}>
                    <label>ID: </label>
                    <input 
                        type="text"
                        placeholder="Enter name"
                        // onChange={(e) =>setUserName(e.target.value)}
                        onChange={(e) => usernameHandler(e)}
                    />
                    {/* {userNameErr?<span>&nbsp;User name does not exist in system...</span>:null} */}
                    <br/> <br/>
                    <label>Password: &nbsp;</label>
                    <input 
                        type="password"
                        placeholder="Enter password"
                        // onChange={(e) =>setPassword(e.target.value)}
                        onChange={passwordHandler}
                    />
                    {/* {passwordErr?<span>&nbsp;Wrong Password...</span>:null} */}
                    <br/> <br/>
                    {/* <select onChange={(e) => setInterest(e.target.value)}>
                        <option>Label</option>
                        <option>Comment</option>
                    </select> */}
                    {/* {(userNameErr || passwordErr)?<span>&nbsp;Wrong Credentials...</span>:null} */}
                    {forbid?<span>&nbsp; no access</span>:null}
                    <button type="submit">Sign up</button>
                    <br /><br />
                </form>
            </div>
        </div>
    )
}

export default Signup