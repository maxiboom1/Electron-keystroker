const robot = require('robotjs');
const { windowManager } = require("node-window-manager");
const appConfig = require("../services/appConfig.js");

// Function to focus a window by its title
function focusWindow() {

    const gpi1 = appConfig.getGpi1[0]; 
    const appName = gpi1.app;

    if (appName.length < 1) {
        sendKeystroke(gpi1.keyTap);
        return;
    }
    
    const windows = windowManager.getWindows();
    const targetWindow = windows.find(win => win.getTitle().includes(appName));
    if (targetWindow) {
        targetWindow.bringToTop();
        targetWindow.maximize();
        sendKeystroke(gpi1.keyTap);
    }

    return;
}

// Function to send keystrokes
function sendKeystroke(keys) {
    const filteredModifiers = keys.modifiers.filter(modifier => modifier);
    console.log(keys.key, filteredModifiers);
    if (filteredModifiers.length === 0) {
        robot.keyTap(keys.key);
    } else if (filteredModifiers.length === 1) {
        robot.keyTap(keys.key, filteredModifiers[0]);
    } else {
        robot.keyTap(keys.key, filteredModifiers);
    }
}

function getAvailableWindows(){
    const windows = windowManager.getWindows();
    const titles = windows.map(win => win.getTitle()).filter(title => title && title.trim() !== '' && title !== 'Default IME'); 
    //console.log(titles);
}

function focusWindowHTTP(gpi1) {
    const appName = gpi1.app;

    if (appName.length < 1) {
        console.log("gg");
        sendKeystroke(gpi1.keyTap);
        return;
    }

    const windows = windowManager.getWindows();
    const targetWindow = windows.find(win => win.getTitle().includes(appName));
    if (targetWindow) {
        targetWindow.bringToTop();
        targetWindow.maximize();
        sendKeystroke(gpi1.keyTap);
    }

    return;
}
module.exports = {
    focusWindow,
    sendKeystroke,
    focusWindowHTTP,
    getAvailableWindows
};
