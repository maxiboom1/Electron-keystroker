const fs = require('fs');

class AppConfig {
    constructor() {
        this.configFilePath = "";
        this._serialPort = "COM3"; // Use a private variable
        this.cues = this._initializeCues(); // Initialize cues
    }

    // Cue model creator
    _createCueModel() {
        return {
            active: false,
            app: "",
            keyTap: {
                key: "",
                modifiers: []
            }
        };
    }

    _initializeCues() {
        const cues = {};
        for (let i = 1; i <= 9; i++) {
            cues[`cue${i}`] = this._createCueModel();
        }
        return cues;
    }

    async setCue(cue, number) { 
        // Set all cues to inactive
        for (let i = 1; i <= 10; i++) {
            this.cues[`cue${i}`].active = false;
        }

        // Set the specified cue
        this.cues[`cue${number}`] = {
            ...cue,
            active: true // Set this cue as active
        };

        await this.saveToFile();
    }

    getActiveCue() {
        // Return the first active cue found
        return Object.values(this.cues).find(cue => cue.active) || null;
    }

    getCueByNumber(num) {
        return this.cues[`cue${num}`];
    }

    // Method to get the serial port
    getSerialPort() {
        return this._serialPort;
    }

    // Method to set the serial port
    setSerialPort(serialPort) {
        this._serialPort = serialPort;
    }

    async setConfigFilePath(path) {
        this.configFilePath = path;
        await this.loadFromFile();
    }

    async loadFromFile() {
        if (fs.existsSync(this.configFilePath)) {
            try {
                const data = fs.readFileSync(this.configFilePath, 'utf-8');
                const config = JSON.parse(data);
                Object.assign(this, config);
            } catch (error) {
                console.error('Failed to parse config file:', error.message);
            }
        } else {
            console.log('Config file does not exist, loading defaults.');
        }
    }

    async saveToFile() {
        try {
            const data = JSON.stringify(this, null, 2);
            fs.writeFileSync(this.configFilePath, data, 'utf-8');
        } catch (error) {
            console.error('Failed to save config file:', error.message);
        }
    }
}

const appConfig = new AppConfig();

module.exports = appConfig;
