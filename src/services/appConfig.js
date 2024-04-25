const fs = require('fs');

class AppConfig {

    constructor() {
        this.configFilePath = "";
    }

    global = false;
    serialPort = "COM3";
    gpi1 = [{
        app: "Google Chrome",
        keyTap: {
            key:"tab",
            modifiers:['control', 'shift']
        }
    }];

    // We loading the config.json once on load
    async loadFromFile() {
        if (fs.existsSync(this.configFilePath)) {
            try {
                const data = fs.readFileSync(this.configFilePath, 'utf-8');
                const config = JSON.parse(data);
                Object.assign(this, config);
            } catch (error) {
                console.error('Failed to parse config file:', error.message);
                // You could set defaults here or notify the user about corrupted configuration, etc.
            }
        } else {
            console.log('Config file does not exist, loading defaults.');
            // Set default configuration or notify the user that default settings are being used.
        }
    }

    // Save config changes to config.json
    async saveToFile() {
        try {
            const data = JSON.stringify(this, null, 2);
            fs.writeFileSync(this.configFilePath, data, 'utf-8');
        } catch (error) {
            console.error('Failed to save config file:', error.message);
        }
    }

    async setConfig(config) {
        this.gpi1 = null;
        this.gpi1 = config.keysData;
        this.serialPort = config.serialPort;
        this.global = config.global;
        await this.saveToFile();
    }

    get getGpi1() {
        return this.gpi1;
    }

    get getComPort() {
        return this.serialPort;
    }
    setComPort(comPort) {
        this.serialPort = comPort;
    }

    async setConfigFilePath(path) {
        this.configFilePath = path;
        await this.loadFromFile();
    }

}

const appConfig = new AppConfig();

module.exports = appConfig;
