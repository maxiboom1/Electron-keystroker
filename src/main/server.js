const express = require('express');
const { focusWindowHTTP } = require("./robotActions");
const server = express();
const PORT = 3123; // You can choose your port here

function startServer() {
    
    server.use(express.json());

    server.post('/api', (req, res) => {
        const data = req.body;
        console.log(data)
        setTimeout(() => {
            //focusWindowHTTP(data);
        }, 50);
        
        res.send("ack");
    });

    server.listen(PORT, () => {
      console.log(`Server running on http://localhost:${PORT}`);
    });
}

module.exports = startServer;