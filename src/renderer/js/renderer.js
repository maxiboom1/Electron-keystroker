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