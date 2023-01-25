// const {readFileSync, writeFileSync} = require('fs');
// const first = readFileSync('./first.txt', 'utf8');
// const second = readFileSync('./second.txt', 'utf8');
// console.log(first,second);
// writeFileSync("./result.txt", `Here is the result: ${first}, ${second}`);

// const http = require('http');
// const server = http.createServer((req, res) => {
//         const url = req.url;
//         if (url != '/' && url != '/login') {
//             res.writeHead(404, {'content-type':'text/html'});
//             res.write('page does not exist');
//             res.end();
//         }
//         else {
//             res.writeHead(200, {'content-type':'text/html'})
//             res.write('<h1>home page<h1>');
//             console.log("sharon is creating the website");
//             res.end();
//         }
//     }
// );

// server.listen(5000);

const express = require('express');
// const cors = require('cors');
const app = express(); 
// app.use(cors);
app.use(express.json());
const fs = require('fs');

app.use((req, res, next) => {
    res.append('Access-Control-Allow-Origin', ["*"]);
    res.append("Access-Control-Allow-Methods", "GET,POST,DELETE,PUT");
    res.append("Access-Control-Allow-Headers", "Content-Type");
    next();
});

app.post('/login/',async (req,res) => {
    console.log("inside backend /login");
    // console.log("backend receive body:", req.body);
    const userName = req.body.user;
    let date_time = new Date();
    let date = ("0" + date_time.getDate()).slice(-2);
    let month = ("0" + (date_time.getMonth() + 1)).slice(-2);
    let year = date_time.getFullYear();
    let hours = date_time.getHours();
    let minutes = date_time.getMinutes();
    let seconds = date_time.getSeconds();
    var dataToPush = {
        user: userName,
        timestamp: year + "-" + month + "-" + date + " " + hours + ":" + minutes + ":" + seconds
    }
    fs.appendFileSync('./userLogInRecords.txt', JSON.stringify(dataToPush) + '\r\n');
    console.log("testing server.js /login");
})

app.post('/signup/',async (req,res) => {
    console.log("inside backend /signup");
    // console.log("backend receive body:", req.body);
    const userName = req.body.user;
    let date_time = new Date();
    let date = ("0" + date_time.getDate()).slice(-2);
    let month = ("0" + (date_time.getMonth() + 1)).slice(-2);
    let year = date_time.getFullYear();
    let hours = date_time.getHours();
    let minutes = date_time.getMinutes();
    let seconds = date_time.getSeconds();
    var dataToPush = {
        user: userName,
        timestamp: year + "-" + month + "-" + date + " " + hours + ":" + minutes + ":" + seconds
    }
    fs.appendFileSync('./userLogInRecords.txt', JSON.stringify(dataToPush) + '\r\n');
    console.log("testing server.js /signup");
})

app.post('/submitLabel',async (req,res) => {
    console.log("inside backend /submitLabel");
    let date_time = new Date();
    let date = ("0" + date_time.getDate()).slice(-2);
    let month = ("0" + (date_time.getMonth() + 1)).slice(-2);
    let year = date_time.getFullYear();
    let hours = date_time.getHours();
    let minutes = date_time.getMinutes();

    for (const element of Object.keys(req.body)) {
        if (req.body[element].emailId == 0) {
            continue;
        }
        const userName = req.body[element].user;
        const emailId = req.body[element].emailId;
        const label = req.body[element].label;
        const confidence = req.body[element].confidence;
        console.log("submitlabel debug", element, label);

        var dataToPush = {
            user: userName,
            emailId: emailId,
            sensitive: label,
            confidence: confidence,
            timestamp: year + "-" + month + "-" + date + " " + hours + ":" + minutes
        }
        fs.appendFileSync('./labelRecords.txt', JSON.stringify(dataToPush) + '\r\n');
    }
    
    console.log("backend receive body:", req.body);
    console.log("testing server.js /submitLabel" );
})

app.get('/getLabelHistory',(req,res) => {
    console.log("inside /getLabelHistory");
    const {readFileSync} = require('fs');
    const labelHistory = readFileSync('./labelRecords.txt', 'utf8');
    const historyArr = labelHistory.split("\r\n");
    historyArr.pop();
    return res.json(historyArr.map((value) => JSON.parse(value)));
})

app.get('/testTimeStamp',(req,res) => {
    const newUser = req.data;

    let date_time = new Date();
    let date = ("0" + date_time.getDate()).slice(-2);
    let month = ("0" + (date_time.getMonth() + 1)).slice(-2);
    let year = date_time.getFullYear();
    let hours = date_time.getHours();
    let minutes = date_time.getMinutes();
    let seconds = date_time.getSeconds();

    var dataToPush = {
        user: newUser,
        timestamp: year + "-" + month + "-" + date + " " + hours + ":" + minutes + ":" + seconds
    }

    fs.appendFileSync('./timeStampTest.txt', JSON.stringify(dataToPush) + '\r\n');

    return res.json("already write data to test.txt");
})

// app.get('/users/:userToLogIn/:password',(req,res) => {
//     console.log(req.params);

//     for(let i= 0; i < users.length; i++) {
//         if (users[i].username == req.params.userToLogIn) {
//             console.log("user found inside database");
//             if (users[i].password == req.params.password) {
//                 console.log("user/psw matched, ready to log in");
//             }
//         }
//     }

//     const newUser = users.map((user) => {
//         const {username} = user;
//         let date_time = new Date();
//         let date = ("0" + date_time.getDate()).slice(-2);
//         let month = ("0" + (date_time.getMonth() + 1)).slice(-2);
//         let year = date_time.getFullYear();
//         let hours = date_time.getHours();
//         let minutes = date_time.getMinutes();
//         let seconds = date_time.getSeconds();

//         var dataToPush = {
//             user: username,
//             timestamp: year + "-" + month + "-" + date + " " + hours + ":" + minutes + ":" + seconds
//         }

//         fs.appendFileSync('./timeStampTest.txt', JSON.stringify(dataToPush) + '\r\n');
//         return {dataToPush}; 
//     })
//     return res.json(newUser);
// })


// app.get('/users/:targetUser', (req,res) => {
//     const {targetUser} = req.params;
//     const userSharon = users.find((user) => user.username === targetUser);
//     return res.json(userSharon);
// })

// app.get('/about', (req,res)=>{
//     res.status(200).send('about page');
// })

// app.all('*', (req,res) => {
//     res.status(404).send('resource not found');
// })

app.listen(8000, ()=> {
    console.log("server is listening on port 8000");
})


