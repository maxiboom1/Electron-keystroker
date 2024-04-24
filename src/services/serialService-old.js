const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const appConfig = require('./appConfig.js');
const EventEmitter = require('events');
class SerialEmitter extends EventEmitter {}
const serialEmitter = new SerialEmitter();

function listenSerial(){
    
    try{
        const port = new SerialPort({ path: appConfig.serialPort, baudRate: 9600 });
        const parser = port.pipe(new ReadlineParser());
        sendHeartBeat(port);
        
        parser.on('data', (data) => {
            let cleanedData = data.trim(); 
            serialEmitter.emit('serial-data', cleanedData);            
        });

    } catch(e){

        console.error("Serial communication failed: ", e);

    }

}

function sendHeartBeat(port){
    port.write("heartbeat\n", (err) => {
        if (err) {
          return console.log('Error on write: ', err.message);
        }
        console.log('Heartbeat signal sent');
      });
}

module.exports = {
    listenSerial,
    serialEmitter
}
