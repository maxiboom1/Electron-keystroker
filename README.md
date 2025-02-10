# Electron-keystroker
 
## Change-log:

### 1.1.1

- New branch for working on cue palette 9 cues.
- Completely refactored appConfig data struct, and its methods.
- On front - implemented 9 styled buttons, with click handlers and "active" state. 
- Some starter characteristics for the logic:

    1. Each button on palette must to have active state. once i click on btn 3, its state should be changed to active while other selected before button should reset.
    2. If i enter the config (show config page), it should render the cue values of this button (eg perform fetch to main appConfig with corresponding cui number - so if i hit button 3, it should fetch cues.cue3 values and set it to app, key, modifiers.)
    3. I removed for now the comPort property from config, as this setup is global, so we will do it from other menu.

- Work is it the middle - I still have some parts undone.

### 1.1.0

- Refactored renderer js into index, config and notifications files. 
- Implemented ipcRenderer.invoke that returns data async, for set-config (user save new config), and getAppConfig on load

```
const appConfig = await ipcRenderer.invoke('getAppConfig');
```

## Notes

- After run npm i you should also run 

```
 ./node_modules/.bin/electron-rebuild -f -w robotjs

```

- Path are relative to html location, not to js.

## Installations

Electron:
```
npm install electron --save-dev
```
Python:

Install it globally on pc (ver 3.xx). Check "Add Python to PATH" option, to make it accessible from the command line.

Electron-rebuild:

```
npm install electron-rebuild --save-dev
```

## Dependencies

This Electron project's dependencies include 
- Node.js, 
- Python 2.7 (for building certain native modules), 
- Visual Studio Build Tools (for compiling native modules on Windows) - Download Visual Studio, and install "Desktop development with C++". 
- electron, 
- robotjs (and its prerequisites like node-gyp for building native addons), 
- node-window-manager for window management functions
- electron-rebuild