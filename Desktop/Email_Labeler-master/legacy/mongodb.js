import mongoose from "mongoose";


export async function connect(username, password, error) {
    const token = `mongodb+srv://${username}:${password}@cluster0.mlexu.mongodb.net/?retryWrites=true&w=majority`;
    console.log("Using token: ", token);
    return mongoose.disconnect()
        .then(() => mongoose.connect(token))
        .then(() => console.log("Connected to DB"))
        .catch((err) => error.val = err);
};

const userSchema = new mongoose.Schema({
    email: String,
    password: String,
    timeCreated: {
        type: Date,
        default: Date.now
    }
});

export const User = mongoose.model('User', userSchema);
