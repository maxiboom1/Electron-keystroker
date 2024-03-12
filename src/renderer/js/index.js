const appConfig = require("../services/appConfig");

document.getElementById("setBtn").addEventListener("click", saveConfig);

// Modal
var modal = document.getElementById("logoModal");
var img = document.getElementById("logo");
img.onclick = function() {modal.style.display = "block";};
window.onclick = function(event) {
  if (event.target == modal) {modal.style.display = "none";}
}

// Updates appConfig on "save config" click
async function saveConfig() {
    await appConfig.setConfig(__getAllValues());
    showNotification();
}

// Collects all config input fields on UI
function __getAllValues() {
    const keysData = [];
    const serialPort = document.getElementById('comPort').value; 
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

    return {keysData, serialPort};
}

// Fetch config and render it on UI
async function __setAllValues() {
    const values = await appConfig;
    const gpi1 = values.gpi1[0]
    console.log(gpi1.keyTap);
    
    document.getElementById('comPort').value = values.serialPort;
    document.getElementById('gpi1app').value = gpi1.app;
    document.getElementById('gpi1key').value = gpi1.keyTap.key;
    document.getElementById('gpi1mod1').value = gpi1.keyTap.modifiers[0];
    document.getElementById('gpi1mod2').value = gpi1.keyTap.modifiers[1];

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

    setTimeout(() => {
        notification.classList.add('hidden');
    }, 5000); // Hide the notification after 5 seconds
}
__setAllValues();