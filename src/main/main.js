const { app, BrowserWindow, dialog } = require("electron");
const { ipcMain } = require("electron");
const path = require('path');
const appConfig = require("../services/appConfig.js");
const serialService = require("../services/serialService.js");
const createWindow = require('./windowManager');
const setupTray = require('./trayManager');
const { focusWindow } = require("./robotActions");
const expressApp = require('./server'); // Assuming the file is named server.js

const gotTheLock = app.requestSingleInstanceLock();

if (!gotTheLock) {
    dialog.showErrorBox("Application Already Running", "Another instance of the application is already running.");
    app.quit();
    return;
}

app.whenReady().then(async () => {

    // Set userFolderPath in appConfig
    await appConfig.setConfigFilePath(path.join(app.getPath('userData'), 'config.json'));

    const win = createWindow();
    setupTray(win);
    serialService.connect(appConfig.getSerialPort());
    //getAvailableWindows();
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

serialService.on('data', (data) => {
    console.log("Received serial data on main serial listener:", data);
    if (data === "tick") {
        focusWindow();
    } else {
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

ipcMain.handle('appClose', async (event) => { 
    app.isQuitting = true; 
    app.quit(); 
    return;
});

ipcMain.handle('getConfig', async (event) => { return appConfig.getConfig() });

ipcMain.handle('getCueByNumber', async (event, number) => { return appConfig.getCueByNumber(number); });

ipcMain.handle('modifyCue', async (event, cue, number) => { return appConfig.modifyCue(cue, number) });

ipcMain.handle('setCueToActive', async (event, number) => { return appConfig.setCueToActive(number) });

ipcMain.handle('setConfig', async (event, config) => { 
    
    if(config.serialPort !== appConfig.getSerialPort()){
        console.log(`switching from ${appConfig.getSerialPort()} to ${config.serialPort}`);
        appConfig.setConfig(config);
        serialService.closeConnection();
        serialService.connect(config.serialPort);
    } else {
        appConfig.setConfig(config);
    }
    return;
});
