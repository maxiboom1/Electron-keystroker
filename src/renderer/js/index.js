const { ipcRenderer } = require('electron');
const { fetchAppConfig, saveConfig, showConfigPage, setActiveCue } = require('./js/config.js');

document.getElementById("set-cue-button").addEventListener("click", saveConfig);

document.getElementById("navbar-icon").addEventListener("click",showComConfig);

// Add event listeners to cue buttons
const buttons = document.querySelectorAll('.btn-square');
buttons.forEach(button => {
    
    // Triggered by left-mouse click
    button.addEventListener('click', handleCueClick);
    
    // Triggered by right-mouse click
    button.addEventListener('contextmenu', function(event) {
        //event.preventDefault(); // Prevents the default right-click menu from appearing
        const cueData = button.dataset.cue; // Fetch data-cue attribute from button
        showConfigPage(cueData);
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

function showComConfig(){
    document.getElementById("app-config-container").classList.remove('hidden');  
}

// Entry point on load - request config from main
fetchAppConfig();

// *************************** MODAL ******************************** //

// Open modal
document.getElementById("logo").addEventListener("click",() => openModal("about-app-container"));

// Close modal
const modal = document.getElementById("modal");
window.addEventListener("click", function(event) {if (event.target == modal) {modal.style.display = "none";}});

// Open the modal with specific content
function openModal(contentId) {
    const modalContent = document.getElementById("modal-content");
    const content = document.getElementById(contentId);
    console.log(content.innerHTML)
    modalContent.innerHTML = content.innerHTML; // Update the modal content
    modal.style.display = "flex"; // Open the modal
}