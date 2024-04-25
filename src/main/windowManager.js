const {app, BrowserWindow } = require('electron');
const path = require('path');

function createWindow() {
  let win = new BrowserWindow({
    width: 1000,
    height: 310,
    icon:path.join(app.getAppPath(),'src', 'assets', 'favicon.ico'),
    resizable: false,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
    },
  });

  win.loadFile(path.join(app.getAppPath(),'src', 'renderer', 'index.html'));

  win.on('close', (event) => {
    if (!app.isQuitting) {
      event.preventDefault();
      win.hide();
    }
  });

  win.webContents.on('context-menu', (e) => {
    e.preventDefault();
  });

  return win;
}

module.exports = createWindow;
