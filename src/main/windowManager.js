const { app, BrowserWindow, Menu } = require('electron');
const path = require('path');

function createWindow() {
    
    Menu.setApplicationMenu(null);  // This hides the default menu

    let win = new BrowserWindow({
        width: 630,
        height: 740,
        icon: path.join(app.getAppPath(), 'src', 'assets', 'favicon.ico'),
        resizable: true,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
            zoomFactor: 1.0, // Ensure default zoom
        },
    });

    //win.webContents.openDevTools(); // Open dev tools

    win.loadFile(path.join(app.getAppPath(), 'src', 'renderer', 'index.html'));

    win.on('close', (event) => {
        if (!app.isQuitting) {
            event.preventDefault();
            win.hide();
        }
    });

    win.webContents.on('context-menu', (e) => {
        e.preventDefault();
    });

    win.webContents.on('did-finish-load', () => {
        win.webContents.setZoomFactor(0.8);
      });

    return win;
}

module.exports = createWindow;
