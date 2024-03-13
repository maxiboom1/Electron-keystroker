const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const appConfig = require('../services/appConfig.js');
const EventEmitter = require('events');
class SerialEmitter extends EventEmitter {}
const serialEmitter = new SerialEmitter();

function listenSerial(){
    
    try{
    
    const port = new SerialPort({ path: appConfig.getComPort, baudRate: 9600 });
    
    const parser = new ReadlineParser();
    
    port.on('error', (err) => {setTimeout(listenSerial, 5000);});

    port.pipe(parser);

    parser.on('data', (data)=>{ serialEmitter.emit('serial-data', data) }); 

    } catch(e){
        console.error("Serial communication failed: ", e);
    }

}

module.exports = {
    listenSerial,
    serialEmitter
}


// function scanForDevices() {
//     SerialPort.list().then((ports) => {
//       ports.forEach((port) => {
//         console.log(`Found port: ${port.path}`);  
//       });
//     }).catch((err) => {
//       console.error('Error listing ports:', err);
//     });
//   }