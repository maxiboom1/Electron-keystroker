const { ipcRenderer } = require('electron');
const { showNotification } = require('./notifications.js');

// Get appConfig and render on gui
async function fetchAppConfig() {
    try {
        const appConfig = await ipcRenderer.invoke('getAppConfig');
        const gpi1 = appConfig.gpi1[0];
        document.getElementById('comPort').value = appConfig.serialPort;
        document.getElementById('gpi1app').value = gpi1.app;
        document.getElementById('gpi1key').value = gpi1.keyTap.key;
        document.getElementById('gpi1mod1').value = gpi1.keyTap.modifiers[0];
        document.getElementById('gpi1mod2').value = gpi1.keyTap.modifiers[1];
    } catch (error) {
        console.error('Error fetching app config:', error);
    }
}

// Get appConfig and render on gui
async function fetchCue(number) {
    try {
        const cue = await ipcRenderer.invoke('getCueByNumber', number);
        return cue;
    } catch (error) {
        console.error('Error fetching cue:', error);
    }
}

// Updates appConfig (ipc channel ti main.js) on "save config" click
async function saveConfig() {
    const config = __getAllValues();
    
    if (config.error) {
        showNotification(config.message, true);
        return;
    }
    
    const result = await ipcRenderer.invoke('update-config', config);

    if (result.succeed) {
        showNotification(result.message);
    } else {
        showNotification(result.message, true);
    }
}

// Collects all config input fields on UI
function __getAllValues() {
    const inputIds = ['gpi1app', 'gpi1key', 'gpi1mod1', 'gpi1mod2', 'comPort'];
    const [app, key, mod1, mod2, serialPort] = inputIds.map(id => document.getElementById(id).value);
    
    if (!key) {
        return { error: true, message: `Key missing. Please set key before saving.` }; 
    }

    const keyData = { app, keyTap: { key, modifiers: [mod1, mod2] } };

    return { keysData: [keyData], serialPort};
}

function toggleConfigPage() {
    const configPage = document.getElementById("configPage");
    configPage.classList.toggle("hidden");
}

module.exports = { fetchAppConfig, saveConfig , toggleConfigPage, fetchCue};
