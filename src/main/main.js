const { app, BrowserWindow, Tray, Menu } = require('electron');
const { ipcMain } = require('electron');
const { windowManager } = require('node-window-manager');
const appConfig = require('../services/appConfig.js');
const { listenSerial, serialEmitter } = require('../services/serialService.js');

const robot = require('robotjs');
const path = require('path');
let win;
let tray = null;

function createWindow() {
  win = new BrowserWindow({
    width: 1000,
    height: 310,
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
    {label: "Simulate",click: () => {
      focusWindow();
    }},
    { label: 'Exit', click: () => {
        app.isQuitting = true;
        app.quit();
      }
    },
    
  ]);

  tray.setContextMenu(contextMenu);

  tray.on('click', () => {
    win.isVisible() ? win.hide() : win.show();
  });
}

app.whenReady().then(() => {
  createWindow();
  setupTray();
  listenSerial();
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

// Function to focus a window by its title
function focusWindow() {
  const gpi1 = appConfig.getGpi1[0];
  const appName = gpi1.app;
  
  if (appName.length < 1) { return false; }
  const windows = windowManager.getWindows();
  const targetWindow = windows.find(win => win.getTitle().includes(appName));
  if (targetWindow) {
    targetWindow.bringToTop();
    targetWindow.maximize();
    sendKeystroke(gpi1.keyTap);
  }

  return targetWindow;
};

// Main function to handle and send keyStrokes
const sendKeystroke = (keys) => {

  // Filter out empty strings from modifiers
  const filteredModifiers = keys.modifiers.filter(modifier => modifier);
  if (filteredModifiers.length === 0) {
    robot.keyTap(keys.key);
  } else if (filteredModifiers.length === 1) {
    robot.keyTap(keys.key, filteredModifiers[0]);
  } else {
    robot.keyTap(keys.key, filteredModifiers);
  }
};

ipcMain.on('update-config', async (event, config) => {
  console.log('ipc triggered');
  await appConfig.setConfig(config);
  // Optionally, send a response back to the renderer
  event.reply('update-config-reply', 'Configuration updated successfully');
});

// Listen for serial data events
serialEmitter.on('serial-data', (data) => {
    console.log('Received serial data:', data);
    
});