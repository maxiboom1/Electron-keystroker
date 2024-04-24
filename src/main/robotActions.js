const robot = require('robotjs');
const { windowManager } = require("node-window-manager");
const appConfig = require("../services/appConfig.js");

// Function to focus a window by its title
function focusWindow() {
    const gpi1 = appConfig.getGpi1[0]; 
    const appName = gpi1.app;

    if (appName.length < 1) {
        sendKeystroke(gpi1.keyTap);
        return false;
    }

    const windows = windowManager.getWindows();
    const targetWindow = windows.find(win => win.getTitle().includes(appName));
    if (targetWindow) {
        targetWindow.bringToTop();
        targetWindow.maximize();
        sendKeystroke(gpi1.keyTap);
    }

    return targetWindow;
}

// Function to send keystrokes
function sendKeystroke(keys) {
    const filteredModifiers = keys.modifiers.filter(modifier => modifier);
    if (filteredModifiers.length === 0) {
        robot.keyTap(keys.key);
    } else if (filteredModifiers.length === 1) {
        robot.keyTap(keys.key, filteredModifiers[0]);
    } else {
        robot.keyTap(keys.key, filteredModifiers);
    }
}

module.exports = {
    focusWindow,
    sendKeystroke
};
