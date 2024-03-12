
    // Get the modal
    var modal = document.getElementById("logoModal");

    // Get the image that opens the modal
    var img = document.getElementById("logo");

    // When the user clicks on the logo, open the modal 
    img.onclick = function() {
      modal.style.display = "block";
    };
    
    window.onclick = function(event) {
      if (event.target == modal) {
        modal.style.display = "none";
      }
    }


const { windowManager } = require('node-window-manager');
const { ipcRenderer } = require('electron');
const appConfig = require('../services/appConfig'); // Adjust the path as necessary
document.getElementById('btn1').addEventListener('click', focusWindow);



// Function to focus a window by its title
function focusWindow (){
  console.log(appConfig);
  const appName = document.getElementById('appToFocus').value;

  if(appName.length<1){ 
    return false;
  }
  const windows = windowManager.getWindows();
  const targetWindow = windows.find(win => win.getTitle().includes(appName));
  
  if (targetWindow) {
    targetWindow.bringToTop();
    targetWindow.maximize();
    simulateKeyPress();
  }

  return targetWindow;
};

function simulateKeyPress() {
  ipcRenderer.send('tick', 'enter');
}