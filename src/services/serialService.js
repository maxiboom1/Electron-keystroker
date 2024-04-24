const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const EventEmitter = require('events');

class SerialService extends EventEmitter {
    constructor() {
        super();
        this.isConnected = false;
        this.port = null;
        this.parser = null;
        this.lastPath = null;
        this.reconnectInterval = null;
    }

    connect(path) {
        this.lastPath = path;  // Store the path for potential reconnection
        if (!this.port) {
            try {
                
                this.port = new SerialPort({ path: path, baudRate: 9600 });
                
                this.port.on('open', () => {
                    console.log(`Serial port ${path} opened.`);
                    this.isConnected = true;
                    this.emit('port-opened');
                    setTimeout(()=>this.sendHeartBeat(),2000);
                    clearInterval(this.reconnectInterval);
                    this.reconnectInterval = null;
                });

                this.port.on('error', (err) => {
                    console.error('Serial connection error:', err.message);
                    this.emit('error', err);
                    this.attemptReconnect();
                });

                this.port.on('close', () => {
                    console.log('Serial port closed.');
                    this.isConnected = false;
                    this.emit('disconnected');
                    this.attemptReconnect();  // Attempt to reconnect
                });

                this.parser = this.port.pipe(new ReadlineParser());
                  
                this.parser.on('data', (data) => {
                    let cleanedData = data.trim();
                    this.emit('data', cleanedData);
                    if (cleanedData === "ack") {
                        console.log('Device connected and acknowledged.');
                        this.emit('connected');
                    }
                });

            } catch (e) {
                console.error("Serial communication initialization failed: ", e);
                this.emit('error', e);
                this.attemptReconnect();  // Attempt to reconnect on failure
            }
        }
    }

    attemptReconnect() {
        if (this.reconnectInterval) return;  // Prevent multiple intervals
        this.reconnectInterval = setInterval(() => {
            console.log('Attempting to reconnect...');
            if (!this.isConnected && this.lastPath) {
                this.port = null;
                this.connect(this.lastPath);
            }
        }, 5000);
    }

    closeConnection() {
        if (this.reconnectInterval) {
            clearInterval(this.reconnectInterval);
            this.reconnectInterval = null;
        }
        
        if (this.port) {
            this.port.close((err) => {
                if (err) {
                    console.error("Failed to close port: ", err.message);
                } else {
                    console.log("Port closed successfully.");
                }
            });
            this.port = null;  // Ensure the port is reset after closing
            this.isConnected = false;
        }
    }
    
    sendHeartBeat() {
        if (this.port && this.port.isOpen) {
            this.port.write("heartbeat\n", (err) => {
                if (err) {
                    console.error('Error on write: ', err.message);
                    this.emit('error', err);
                } else {
                    console.log('Heartbeat signal sent');
                }
            });
        }
    }

}

const serialService = new SerialService();

module.exports = serialService;

