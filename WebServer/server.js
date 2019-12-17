const express = require('express')
const app = express()
const port = 3000

app.use(express.json()) // for parsing application/json
app.use(express.urlencoded({ extended: true })) // for parsing application/x-www-form-urlencoded

// define the default error handler
app.use (function (error, request, response, next){
    console.error("WARNING: error detected...")
    console.error(error.stack)
    response.status(500).send("Error handling request: " + error.message)
});

// handler for a received HTTP GET request
app.get('/', (request, response) => {
    console.log('request body:', request.body);
    return response.send('Received a GET HTTP request');
});

// handler for a received HTTP POST request
app.post('/', (request, response) => {
    console.log('request body:', request.body);
    return response.send('Received a POST HTTP request');
});

// handler for application start
app.listen(port, () =>
    console.log("Example app listening on port " + port)
);

