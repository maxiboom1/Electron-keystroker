const { app, BrowserWindow, dialog } = require("electron");
const { ipcMain } = require("electron");
const path = require('path');
const appConfig = require("../services/appConfig.js");
const serialService = require("../services/serialService.js");
const createWindow = require('./windowManager');
const setupTray = require('./trayManager');
const { focusWindow, getAvailableWindows } = require("./robotActions");
const expressApp = require('./server'); // Assuming the file is named server.js

const gotTheLock = app.requestSingleInstanceLock();

if (!gotTheLock) {
  // This instance was unable to obtain the lock and is a second instance
  dialog.showErrorBox("Application Already Running", "Another instance of the application is already running.");
  app.quit();
}

app.whenReady().then(async () => {

  // Set userFolderPath in appConfig
  await appConfig.setConfigFilePath(path.join(app.getPath('userData'), 'config.json'));
  
  const win = createWindow();
  setupTray(win);
  serialService.connect(appConfig.getComPort);
  getAvailableWindows();
  expressApp();
});

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});

serialService.on('port-opened', () => {
  console.log(`Event:port-opened`);

  BrowserWindow.getAllWindows().forEach(window => {
    window.webContents.send('update-led', 'yellow');
  });
});

serialService.on('connected', () => {
  console.log(`Event:connected`);
  BrowserWindow.getAllWindows().forEach(window => {
    window.webContents.send('update-led', 'green');
  });
});

serialService.on('data', (data)=> {
  console.log("Received serial data on main serial listener:", data);
  if (data === "tick") {
    focusWindow();
  }else {
    console.error("Unexpected serial data received:", data);
  }
});

serialService.on('disconnected', () => {
  console.log(`Event:disconnected`);
  BrowserWindow.getAllWindows().forEach(window => {
    window.webContents.send('update-led', 'red');
  });
});

serialService.on('error', (error) => console.log(`Error: ${error.message}`));

// --------------------------- Ipc listeners/handlers --------------------------- //

ipcMain.handle('getAppConfig', async (event) => { return appConfig; });

ipcMain.handle('update-config', async (event, config) => { 
  
  try {
    
    if(appConfig.serialPort !== config.serialPort){ 
      console.log(`switching from ${appConfig.serialPort} to ${config.serialPort}` )
      serialService.closeConnection();
      setTimeout(()=>serialService.connect(config.serialPort),2000);
    }
    appConfig.setConfig(config);

    return {
      succeed: true, 
      message: "Configuration updated successfully"
    };

  } catch (error) {
    
    console.error("Failed to update configuration:", error);
    
    return {
      succeed: false, 
      message: "Failed to update configuration. See logs for details"
    };

  }

});
