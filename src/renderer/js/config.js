const { ipcRenderer } = require('electron');
const { showNotification } = require('./notifications.js');


// ************** Onload config fetching and rendering ************** //

async function fetchAppConfig(renderOnlyConfig = false) {
    
    try {
    
    // Get config as {cues{}, number: serialPort, boolean: allowGlobals}
    const config = await ipcRenderer.invoke('getConfig');
    document.getElementById('serial-port').value = config.serialPort;
    document.getElementById('allow-globals').checked = config.allowGlobals;   
    
    if(renderOnlyConfig) return; // If we render only config values - return here.
    
    renderCues(config.cues);

    } catch (error) {
        console.error('Error fetching app config:', error);
    }
}

function renderCues(cues) {
    for (let i = 1; i <= 9; i++) {
        const cue = cues[`cue${i}`]; // Fetch cue by number
        const button = document.querySelector(`button[data-cue="${i}"]`);

        if (cue) {
            button.textContent = cue.name || `Button ${i}`; // Set button text to cue name or default
            updateButtonModifiers(button, cue.keyTap.key, cue.keyTap.modifiers, cue.active);
        }
    }
}

function updateButtonModifiers(button, key, modifiers, isActive) {
    // Clear any existing modifier indicators
    const existingModifiers = button.querySelector('.modifier-container');
    if (existingModifiers) {
        button.removeChild(existingModifiers);
    }

    const modifierContainer = document.createElement('div');
    modifierContainer.className = 'modifier-container';

    // Add key representation
    if (key) {
        const keyElement = document.createElement('span');
        keyElement.className = 'key-indicator';
        keyElement.textContent = key;
        modifierContainer.appendChild(keyElement);
    }

    // Add modifiers
    modifiers.forEach(mod => {
        if(mod){// Don't render empty modifiers
            const modifierElement = document.createElement('span');
            modifierElement.className = 'key-modifier key-indicator';
            modifierElement.textContent = mod;
            modifierContainer.appendChild(modifierElement);
        }
    });

    // Append modifier container to button
    button.appendChild(modifierContainer);

    // Set active state styling
    if (isActive) {
        button.style.border = '3px solid red'; // Change border color to red
        button.style.background = '#007bff';
    } else {
        button.style.border = ''; // Reset border if not active
        button.style.background = '';
    }
}

// **************************** Cue editing / saving **************************** //


async function showCueConfig(cueNumber) {
    const selectedCue = await fetchCue(cueNumber);

    // Check if the selectedCue object has the expected structure
    if (selectedCue) {
        document.getElementById('cue-config-header').textContent = `Settings for cue-${cueNumber}`;
        // Set the cue name
        document.getElementById('name').value = selectedCue.name || '';

        // Set the application name
        document.getElementById('app').value = selectedCue.app || '';

        // Set the key to send
        document.getElementById('key').value = selectedCue.keyTap.key || '';

        // Set the modifiers
        const mod1Select = document.getElementById('mod1');
        const mod2Select = document.getElementById('mod2');

        // Resetting the selects
        mod1Select.value = ''; // Resetting to 'None'
        mod2Select.value = ''; // Resetting to 'None'

        // Set Modifier-1 if it exists
        if (selectedCue.keyTap.modifiers.length > 0) {
            mod1Select.value = selectedCue.keyTap.modifiers[0];
        }

        // Set Modifier-2 if it exists
        if (selectedCue.keyTap.modifiers.length > 1) {
            mod2Select.value = selectedCue.keyTap.modifiers[1];
        }
    }

    // Show the config page (if hidden)
    document.getElementById('cue-config-container').classList.remove('hidden');
}

async function fetchCue(number) {
    try {
        const cue = await ipcRenderer.invoke('getCueByNumber', number);
        return cue;
    } catch (error) {
        console.error('Error fetching cue:', error);
    }
}

async function saveCueConfig() {
    const cueNumber = document.getElementById('cue-config-header').textContent.slice(-1);
    const name = document.getElementById('name').value;
    const app = document.getElementById('app').value;
    const key = document.getElementById('key').value;
    const mod1 = document.getElementById('mod1').value;
    const mod2 = document.getElementById('mod2').value;
    const allowGlobals = document.getElementById('allow-globals').checked;

    // Config validation
    if (name === "" || key === "" ) {
        showNotification("Complete all fields first", true);
        return;
    }
    // Config App name validation
    if (!allowGlobals && app === "" ) {
        showNotification("Globals keystrokes disabled. Enter app name to focus", true);
        return;
    }


    const cue = {
        app,
        name,
        keyTap: {
            key,
            modifiers: [mod1,mod2]
        }
    };
    try {
        await ipcRenderer.invoke('modifyCue',cue,cueNumber);
        showNotification(`Config for ${name} updated `);
    } catch (error) {
        showNotification(`Failed updating ${name} :(`, true);
        console.error(error);
    }

    await fetchAppConfig();
    
    // Hide the config page
    document.getElementById('cue-config-container').classList.add('hidden');
}

// ************** Selecting active cue button ************** //

async function setActiveCue(number) {
    try {
        await ipcRenderer.invoke('setCueToActive', number);
    } catch (error) {
        showNotification(`Failed to set cue`, true);
    }

    fetchAppConfig();
}

// **************************** Application settings page **************************** //

function showAppConfig(){
    const renderOnlyConfig = true;
    fetchAppConfig(renderOnlyConfig);
    document.getElementById("app-main-div").classList.add('d-none');  
    document.getElementById('app-config-div').classList.remove('d-none');  
}

function closeAppConfig(){
    document.getElementById("app-main-div").classList.remove('d-none');  
    document.getElementById("app-config-div").classList.add('d-none');   
}

async function setAppConfig(){
    const config = {
        serialPort: document.getElementById('serial-port').value,
        allowGlobals: document.getElementById('allow-globals').checked
    }
    try {
        await ipcRenderer.invoke('setConfig', config);
        showNotification(`Application settings updated`);
        closeAppConfig();
    } catch (error) {
        showNotification(`Failed to update app settings`, true);
    }

}


module.exports = { 
    fetchAppConfig, 
    saveCueConfig , 
    showCueConfig, 
    setActiveCue,
    showAppConfig,
    closeAppConfig,
    setAppConfig
};
