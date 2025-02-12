const fs = require('fs');

class AppConfig {
    
    constructor() {
        this.configFilePath = "";
        this._serialPort = "COM3"; // Use a private variable
        this.cues = this._initializeCues(); // Initialize cues
    }

    // *********************************  Initialization  ********************************* //
    _createCueModel() {
        return {
            active: false,
            app: "Some app",
            name:"My custom name",
            keyTap: {
                key: "A",
                modifiers: ["alt","control"]
            }
        };
    }

    _initializeCues() {
        const cues = {};
        for (let i = 1; i <= 9; i++) { cues[`cue${i}`] = this._createCueModel() }
        return cues;
    }

    // *********************************  Cues  ********************************* //
    
    // Provides {cues,serialPort} on app start.
    getConfig(){
        return {
            cues: this.cues,
            serialPort: this._serialPort
        }
    }
    
    // Return cues.cue${number} for frontpage - when user enter config of cue.
    getCueByNumber(number) {
        return this.cues[`cue${number}`];
    }
    
    // Get active cue 
    getActiveCue() {
        // Return the first active cue found
        return Object.values(this.cues).find(cue => cue.active) || null;
    }

    // Sets cues.cue${number}.active - triggered by user cue select
    async setCueToActive(number) {  
        // Set all cues to inactive
        for (let i = 1; i <= 9; i++) { this.cues[`cue${i}`].active = false }
        
        // Set the specified cue
        this.cues[`cue${number}`].active = true;
        
        await this.saveToFile();
    }

    // Modify cue. expected cue incoming without "active" prop. Triggered when user saves edited cue.
    async modifyCue(cue, number){
        this.cues[`cue${number}`] = Object.assign(this.cues[`cue${number}`], cue);
        await this.saveToFile();
    }



    // *********************************  Serial Port  ********************************* //

    // Method to get the serial port
    getSerialPort() {
        return this._serialPort;
    }
    // Method to set the serial port
    setSerialPort(serialPort) {
        this._serialPort = serialPort;
    }

    // *********************************  Config caching  ********************************* //

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
