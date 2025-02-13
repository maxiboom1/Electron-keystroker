const { app, BrowserWindow, Menu } = require('electron');
const path = require('path');

function createWindow() {
    
    Menu.setApplicationMenu(null);  // This hides the default menu

    let win = new BrowserWindow({
        width: 800,
        height: 750,
        icon: path.join(app.getAppPath(), 'src', 'assets', 'favicon.ico'),
        resizable: true,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
            zoomFactor: 1.0
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

    return win;
}

module.exports = createWindow;
