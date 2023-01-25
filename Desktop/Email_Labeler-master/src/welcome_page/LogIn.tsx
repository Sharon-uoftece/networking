import react, {useEffect, useState} from "react";
import axios from "axios";
import {Page, Header} from "../common";
import userData from "../userData";

function LogIn({ page, setPage, currentUser, setCurrentUser}: { page: number, setPage: (page: number) => void, currentUser: string, setCurrentUser: (currentUser: string) => void}) {
    const [userName, setUserName] = useState('');
    const [userNameErr, setUserNameErr] = useState(false);
    const [password, setPassword] = useState('');
    const [passwordErr, setPasswordErr] = useState(false);

    function loginHandler(e:React.SyntheticEvent) {
        e.preventDefault();
        let foundUsername = false;

        console.log(userName, password);
        for (var i=0; i<userData.length; i++) {
            if (userName == userData[i].user) {
                foundUsername = true;
                if (password == userData[i].password) {
                    // setPage(Page.Survey);
                    setPage(Page.UserInfo);
                    setCurrentUser(userName);
                    loginHandle(e);
                    setCurrentUser(userName);
                    console.log("username and psw match, ready to log in...");
                } else {
                    setPasswordErr(true);
                    console.log("username exist but wrong password, cannot log in...");
                }
            } 
        }
        if (!foundUsername) {
            setUserNameErr(true);
            console.log("username not in system");
        }
    }

    const loginHandle = async(e:React.SyntheticEvent) => {
        e.preventDefault();
        
        const myData = {
            user: userName,
            password: password
        }
        console.log("frontend login handler");

        const result = await fetch('http://localhost:8000/login', {
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
        setUserNameErr(false);
        setUserName(e.target.value);
    }

    function passwordHandler(e:React.SyntheticEvent) {
        e.preventDefault();
        setPasswordErr(false);
        setPassword(e.target.value);
    }
    
    return(
        <div>
            <div className="log-in-form">
                <h1>Log in here...</h1>
                {/* {data.map(item => 
                    <div>
                        <h1>{Object.values(JSON.parse(item))}</h1>
                    </div>
                )} */}
                <form onSubmit={loginHandler}>
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
                    {(userNameErr || passwordErr)?<span>&nbsp;Wrong Credentials...</span>:null}

                    <button type="submit">Log In</button>
                    <br /><br />
                </form>
            </div>
        </div>
    )
}

export default LogIn