const fs = require('fs');
const path = require('path');

class AppConfig {
    
    constructor() {
        this.loadFromFile('config.json');
    }
    port= 4000; // Default Server Port:
    serialPort = "COM3";  // Default serial port 

    gpi1 = {
        app: "Google Chrome", 
        keyTap: {
            key:"tab",
            modifiers:['control', 'shift']
        }
    };

    // We loading the config.json once onload
    loadFromFile(filename)  {
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
    saveToFile()  {
        const configFilePath = path.join(__dirname,"config.json");
        const data = JSON.stringify(this, null, 2);
        fs.writeFileSync(configFilePath, data, 'utf-8');
    }

    setGPI(index , newGPIValue ) {
        if (index >= 1 && index <= 6) {
            const gpiProperty = `gpi${index}`;
            this[gpiProperty] = newGPIValue;
        } else {
            console.error('Invalid GPI index. Index must be between 1 and 7.');
        }
    }

    setComPort(comPort){
        this.serialPort = comPort;
    }

}

const appConfig = new AppConfig();

module.exports = appConfig;
