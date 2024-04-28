const { app, Tray, Menu } = require('electron');
const path = require('path');
const { focusWindow } = require('./robotActions'); // Adjust the path as needed

function setupTray(win) {
  
  const iconPath = path.join(__dirname, '..', 'assets', 'tray-logo.png');

  let tray = new Tray(iconPath);
  tray.setToolTip('I/O Systems Keystroker');

  const contextMenu = Menu.buildFromTemplate([
    { label: 'Show App', click: () => win.show() },
    {
      label: 'Simulate',
      click: () => {
          focusWindow();  // Utilizes focusWindow from robotActions.js
      }
  },
    { label: 'Exit', click: () => { 
      app.isQuitting = true; 
      app.quit(); 
      } 
    }
  ]);

  tray.setContextMenu(contextMenu);
  tray.on('click', () => {
    win.isVisible() ? win.hide() : win.show();
  });
}

module.exports = setupTray;
