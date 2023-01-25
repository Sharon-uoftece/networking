import express from "express";
import helmet from "helmet";
import { connect, User } from "./mongodb.js";
import bodyParser from "body-parser";

const OK = 200;
const RESOURCE_CREATED = 201;
const BAD_REQUEST = 400;
const NOT_FOUND = 403;
const CONFLICT = 409;
const SERVER_ERR = 500;

const PORT = process.env.PORT || 8180;
const username = process.env.USERNAME || 'user';
const password = process.env.PASSWORD || 'jtqY6658';
const app = express();
app.use(bodyParser.urlencoded({extended: false}));
app.use(bodyParser.json());
// app.use(express.json());

app.use((req, res, next) => {
    res.append('Access-Control-Allow-Origin', ["*"]);
    res.append("Access-Control-Allow-Methods", "GET,POST,DELETE,PUT");
    res.append("Access-Control-Allow-Headers", "Content-Type");
    next();
});

const error = {
    val: ""
};

await connect(username, password, error);
if (error.val != "") {
    throw new Error(error.val);
}

app.post("/sunlife-app/login",
    async (req, res, next) => {
        const email = req.body.email;
        const password = req.body.password;
        let abort = false;
        if (!email || !password) {
            res.status(BAD_REQUEST).send();
        }
        let user = await User
            .findOne({ email: email })
            // .findOne({ email: email})
            .catch(err => {
                console.log(err);
                res.status(SERVER_ERR).send();
                abort = true;
            });

        if (abort) return next();
        if (user == null) {
            res.status(NOT_FOUND).send("Username does not exist...");
            return next();
        }
        user = await User
            .findOne({ email: email, password: password })
            // .findOne({ email: email})
            .catch(err => {
                console.log(err);
                res.status(SERVER_ERR).send();
                abort = true;
            });
        if (abort) return next();
        if (user == null) {
            res.status(NOT_FOUND).send("Password does not match...");
            return next();
        }
        res.status(OK).send(user);
        return next();
    });

app.post("/sunlife-app/register",
    async (req, res, next) => {
        console.log(req.body);
        const email = req.body.email;
        const password = req.body.password;
        let abort = false;
        if (!email || !password) {
            res.status(BAD_REQUEST).send();
        }
        const user = await User
            .exists({ email: email })
            .catch(err => {
                console.log(err);
                res.status(SERVER_ERR).send();
                abort = true;
            });

        if (abort) return next();
        if (user != null) {
            res.status(CONFLICT).send();
            return next();
        }

        const newUser = new User({
            email: email,
            password: password,
        });
        await newUser.save();
        res.status(RESOURCE_CREATED).send(newUser);
        return next();
    });


app.listen(
    PORT,
    () => console.log(`Listening on port ${PORT}`)
);