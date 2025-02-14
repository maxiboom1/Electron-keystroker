const { ipcRenderer } = require('electron');
const { fetchAppConfig, saveCueConfig, showCueConfig, setActiveCue, closeAppConfig, setAppConfig } = require('./js/config.js');
require(`./js/menu.js`); // Connect menu.js file
const appVersion = "1.1.6";

// *************************** Event Listeners ******************************** //
document.getElementById("set-cue-config").addEventListener("click", saveCueConfig);
document.getElementById("set-app-config").addEventListener("click", setAppConfig);
document.getElementById("close-app-config").addEventListener("click", closeAppConfig);
document.getElementById("logo").addEventListener("click",() => openModal("about-app-container")); // Open about on logo click
// listeners to cue buttons
const buttons = document.querySelectorAll('.btn-square');
buttons.forEach(button => {
    
    // Triggered by left-mouse click
    button.addEventListener('click', handleCueClick);
    
    // Triggered by right-mouse click
    button.addEventListener('contextmenu', function(event) {
        const cueData = button.dataset.cue; // Fetch "data-cue" attr from button
        showCueConfig(cueData);
});});
// Async com-status updates from main.js
ipcRenderer.on('update-led', (event, color) => {
    
    const led = document.getElementById('gpio-led');

    if(color === 'green') {
        led.textContent = 'GPIO BOX: 🟢'; 
    } else if(color === 'yellow') {
        led.textContent = 'GPIO BOX: 🟡'; 
    } else {
        led.textContent = 'GPIO BOX: 🔴';
    } 
});

async function handleCueClick(event){
    // Fetch the cue values based on the button clicked
    const cueNumber = event.currentTarget.getAttribute('data-cue');
    await setActiveCue(cueNumber);
}

// *************************** MODAL ******************************** //

// Close modal
const modal = document.getElementById("modal");
window.addEventListener("click", function(event) {if (event.target == modal) {modal.style.display = "none";}});

// Open the modal with specific content
function openModal(contentId) {
    const modalContent = document.getElementById("modal-content");
    const content = document.getElementById(contentId);
    modalContent.innerHTML = content.innerHTML; // Update the modal content
    modal.style.display = "flex"; // Open the modal
}


// Entry point on load - request config from main and set app version
fetchAppConfig();
document.getElementById('app-version').textContent ='I/O Systems Keystroker '+ appVersion;