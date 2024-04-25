const { app, Tray, Menu } = require('electron');
const path = require('path');
const { focusWindow } = require('./robotActions'); // Adjust the path as needed

function setupTray(win) {
  
  // This path points directly to the resources directory in packaged apps
  const iconPath = process.env.NODE_ENV === 'development' 
      ? path.join(__dirname, '..', 'assets', 'logo.png') // Development path
      : path.join(__dirname,'assets', 'logo.PNG'); // Packaged app path


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
