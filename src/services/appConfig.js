const fs = require('fs');
const path = require('path');

class AppConfig {
    
    constructor() {
        this.loadFromFile('config.json');
    }
    serialPort = "COM3";  
    gpi1 = {
        app: "Google Chrome", 
        keyTap: {
            key:"tab",
            modifiers:['control', 'shift']
        }
    };
    
    // We loading the config.json once onload
    async loadFromFile(filename)  {
        try {
            const configFilePath = path.join(__dirname, filename);
            const data = fs.readFileSync(configFilePath, 'utf-8');
            const config = JSON.parse(data);
            Object.assign(this, config);
        } catch (error) {
            console.error('Error loading config file:', error.message);
        }
    }
    // Save config changes to config.json
    async saveToFile()  {
        const configFilePath = path.join(__dirname,"config.json");
        const data = JSON.stringify(this, null, 2);
        fs.writeFileSync(configFilePath, data, 'utf-8');
    }

    async setConfig(config) {
        this.gpi1 = null;
        this.gpi1 = config.keysData;
        this.serialPort = config.serialPort;
        this.saveToFile();
    }

    get getGpi1(){
        return this.gpi1;
    }
    
    get getComPort(){
        return this.serialPort;
    }
    setComPort(comPort){
        this.serialPort = comPort;
    }

}

const appConfig = new AppConfig();

module.exports = appConfig;
