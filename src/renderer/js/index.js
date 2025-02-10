const { ipcRenderer } = require('electron');
const { fetchAppConfig, saveConfig } = require('./js/config.js');

document.getElementById("setBtn").addEventListener("click", saveConfig);
document.getElementById("logo").addEventListener("click",() => modal.style.display = "block");

// Modal
const modal = document.getElementById("logoModal");
window.addEventListener("click", function(event) {if (event.target == modal) {modal.style.display = "none";}});

// Async com-status updates from main.js
ipcRenderer.on('update-led', (event, color) => {
    
    const led = document.getElementById('statusIndicator');

    if(color === 'green') {
        led.textContent = 'GPIO BOX: 🟢'; 
    } else if(color === 'yellow') {
        led.textContent = 'GPIO BOX: 🟡'; 
    } else {
        led.textContent = 'GPIO BOX: 🔴';
    } 
});


// Entry point on load - request config from main
fetchAppConfig();