const { ipcRenderer } = require('electron');
const { showNotification } = require('./notifications.js');

// Get appConfig and render on gui
async function fetchAppConfig() {
    try {
        const config = await ipcRenderer.invoke('getConfig');
        renderCues(config.cues);
    } catch (error) {
        console.error('Error fetching app config:', error);
    }
}

function renderCues(cues) {
    console.log(cues)
    for (let i = 1; i <= 9; i++) {
        const cue = cues[`cue${i}`]; // Fetch cue by number
        const button = document.querySelector(`button[data-cue="${i}"]`);

        if (cue) {
            button.textContent = cue.name || `Button ${i}`; // Set button text to cue name or default
            updateButtonModifiers(button, cue.keyTap.key, cue.keyTap.modifiers, cue.active);
        }
    }
}

// Function to update button appearance based on key, modifiers, and active state
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
        const modifierElement = document.createElement('span');
        modifierElement.className = 'modifier-indicator';
        modifierElement.textContent = mod;
        modifierContainer.appendChild(modifierElement);
    });

    // Append modifier container to button
    button.appendChild(modifierContainer);

    // Set active state styling
    if (isActive) {
        button.style.border = '2px solid red'; // Change border color to red
    } else {
        button.style.border = ''; // Reset border if not active
    }
}

// Updates appConfig (ipc channel ti main.js) on "save config" click
async function saveConfig() {
    const cueNumber = document.getElementById('configHeader').textContent.slice(-1);
    const name = document.getElementById('name').value;
    const app = document.getElementById('app').value;
    const key = document.getElementById('key').value;
    const mod1 = document.getElementById('mod1').value;
    const mod2 = document.getElementById('mod2').value;

    // Config validation
    if (name === "" || key === "" || app === "") {
        showNotification("Complete all fields first", true);
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
    document.getElementById('configPage').classList.add('hidden');
}

// Fetch cue by cueNumber, and renders on config page
async function showConfigPage(cueNumber) {
    const selectedCue = await fetchCue(cueNumber);

    // Check if the selectedCue object has the expected structure
    if (selectedCue) {
        document.getElementById('configHeader').textContent = `Settings for cue-${cueNumber}`;
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
    document.getElementById('configPage').classList.remove('hidden');
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

// Get appConfig and render on gui
async function setActiveCue(number) {
    try {
        await ipcRenderer.invoke('setCueToActive', number);
    } catch (error) {
        showNotification(`Failed to set cue`, true);
    }

    fetchAppConfig();
}

module.exports = { 
    fetchAppConfig, 
    saveConfig , 
    showConfigPage, 
    setActiveCue
};
