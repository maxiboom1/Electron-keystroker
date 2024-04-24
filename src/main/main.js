const { app, BrowserWindow } = require("electron");
const { ipcMain } = require("electron");
const appConfig = require("../services/appConfig.js");
const { listenSerial, serialEmitter } = require("../services/serialService.js");
const createWindow = require('./windowManager');
const setupTray = require('./trayManager');
const { focusWindow } = require("./robotActions");

app.whenReady().then(() => {
  const win = createWindow();
  setupTray(win);
  listenSerial();
});

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});

serialEmitter.on("serial-data", (data) => {
  console.log("Received serial data on main serial listener:", data);

  if (data === "tick") {
    focusWindow();
  } else if (data === "ack") {
    BrowserWindow.getAllWindows().forEach(window => {
      window.webContents.send('update-led', 'green');
    });
  } else {
    console.error("Unexpected serial data received:", data);
  }
});

ipcMain.on("update-config", async (event, config) => {
  try {
    await appConfig.setConfig(config);
    event.reply("update-config-reply", "Configuration updated successfully");
  } catch (error) {
    console.error("Failed to update configuration:", error);
    event.reply("update-config-reply", "Failed to update configuration");
  }
});
