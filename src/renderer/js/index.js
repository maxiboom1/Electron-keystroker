const { ipcRenderer } = require('electron');
const appConfig = require("../services/appConfig");
document.getElementById("setBtn").addEventListener("click", saveConfig);
document.getElementById("logo").addEventListener("click",() => modal.style.display = "block");

// Modal
const modal = document.getElementById("logoModal");
window.addEventListener("click", function(event) {if (event.target == modal) {modal.style.display = "none";}});

// Collects all config input fields on UI
function __getAllValues() {
    const keysData = [];
    const serialPort = document.getElementById('comPort').value; 
    const global = true;
    const app = document.getElementById(`gpi1app`).value;
    const key = document.getElementById(`gpi1key`).value;
    const mod1 = document.getElementById(`gpi1mod1`).value;
    const mod2 = document.getElementById(`gpi1mod2`).value;

    const keyData = {
        app: app,
        keyTap: {
            key: key,
            modifiers: [mod1, mod2],
        },
    };

    keysData.push(keyData);

    return {keysData, serialPort, global};
}

// Fetch config and render it on UI
async function __setAllValues() {
    const values = await appConfig;
    const gpi1 = values.gpi1[0]
    //console.log(values);
    
    document.getElementById('comPort').value = values.serialPort;
    document.getElementById('gpi1app').value = gpi1.app;
    document.getElementById('gpi1key').value = gpi1.keyTap.key;
    document.getElementById('gpi1mod1').value = gpi1.keyTap.modifiers[0];
    document.getElementById('gpi1mod2').value = gpi1.keyTap.modifiers[1];

}

// Updates appConfig (ipc channel ti main.js) on "save config" click
async function saveConfig() {
    const config = __getAllValues();
    ipcRenderer.send('update-config', config);
    // Optionally, listen for a reply
    ipcRenderer.on('update-config-reply', (event, response) => {
        console.log(response);
        showNotification(response);
    });
}

function showNotification(message, isError) {
    const notification = document.getElementById('notification');
    const notificationText = document.getElementById('notification-text');

    notificationText.textContent = message;
    notification.classList.remove('hidden');

    if (isError) {
        notification.classList.add('error');
    } else {
        notification.classList.remove('error');
    }

    setTimeout(() => {notification.classList.add('hidden');}, 5000);
}

// Events from main.js
ipcRenderer.on('update-led', (event, color) => {
    console.log("front: ", color)
    const statusIndicator = document.getElementById('statusIndicator');
    if(color === 'green') {
      statusIndicator.textContent = 'GPIO BOX: 🟢'; // Green circle emoji
    } else {
      statusIndicator.textContent = 'GPIO BOX: 🔴'; // Red circle emoji, back to default
    }
  });

__setAllValues();