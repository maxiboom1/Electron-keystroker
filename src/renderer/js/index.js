const { ipcRenderer } = require('electron');
const { fetchAppConfig, saveConfig, showConfigPage, setActiveCue } = require('./js/config.js');


document.getElementById("setBtn").addEventListener("click", saveConfig);
document.getElementById("logo").addEventListener("click",() => modal.style.display = "block");

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

async function handleCueClick(event){
    // Remove active class from all buttons
    const buttons = document.querySelectorAll('.btn-square');
    buttons.forEach(button => {button.classList.remove('active');});

    // Add active class to the clicked button
    const clickedButton = event.currentTarget;
    clickedButton.classList.add('active');

    // Fetch the cue values based on the button clicked
    const cueNumber = clickedButton.getAttribute('data-cue');
    await setActiveCue(cueNumber);
}


// Entry point on load - request config from main
fetchAppConfig();

