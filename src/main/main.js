const { app, BrowserWindow, Tray, Menu } = require('electron');
const { ipcMain } = require('electron');
const robot = require('robotjs');
const path = require('path');
let win;
let tray = null;

function createWindow() {
  win = new BrowserWindow({
    width: 800,
    height: 300,
    resizable: false, // Set resizable to false
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false
    }
  });

  win.loadFile('src/renderer/index.html');
  // Hide the default menu
  //win.setMenu(null);

  // Hide the window instead of closing it
  win.on('close', (event) => {
    if (!app.isQuitting) {
      event.preventDefault();
      win.hide();
    }
    return false;
  });

  // Prevent the default context menu from showing in the app window
  win.webContents.on('context-menu', (e) => {
    e.preventDefault();
  });
}

function setupTray() {
  tray = new Tray(path.join(__dirname, 'logo.png')); // Update with the path to your tray icon
  tray.setToolTip('I/O Systems Keystroker');

  const contextMenu = Menu.buildFromTemplate([
    { label: 'Show App', click: () => win.show() },
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

app.whenReady().then(() => {
  createWindow();
  setupTray();
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});

ipcMain.on('tick', (event, arg) => {
  robot.keyTap(arg);
  console.log(`Simulated ${arg} key press`);
});

ipcMain.on('serial-data', (event, arg) => {
  
  console.log('Received data:', data);}
  
);
