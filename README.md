# Electron-keystroker
 
## Notes

- After run npm i you should also run 

```
 ./node_modules/.bin/electron-rebuild -f -w robotjs

```

- Path are relative to html location, not to js.

## Instalations

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





{
  "name": "serial-keystroker",
  "version": "1.0.0",
  "description": "keystroker",
  "main": "src/main/main.js",
  "productName": "Serial-Keystroker",
  "scripts": {
    "start": "cross-env NODE_ENV=development electron-forge start",
    "test": "echo \"Error: no test specified\" && exit 1",
    "postinstall": "electron-builder install-app-deps && electron-rebuild -f -w robotjs -v 30.0.1 --abi=123",
    "package": "electron-forge package",
    "make": "cross-env NODE_ENV=production electron-forge make"
  },
  "build": {
    "appId": "IOSystems keystroker_v1",
    "productName": "Serial Keystroker",
    "directories": {
      "output": "dist"
    },
    "win": {
      "target": "nsis",
      "icon": "src/assets/favicon.ico"
    },
    "nsis": {
      "oneClick": false,
      "allowElevation": true,
      "allowToChangeInstallationDirectory": true,
      "createDesktopShortcut": true,
      "createStartMenuShortcut": true
    }
  },
  "author": "Alex Samih-zade",
  "license": "ISC",
  "devDependencies": {
    "@electron-forge/cli": "^7.4.0",
    "@electron-forge/maker-deb": "^7.4.0",
    "@electron-forge/maker-rpm": "^7.4.0",
    "@electron-forge/maker-squirrel": "^7.4.0",
    "@electron-forge/maker-zip": "^7.4.0",
    "@electron-forge/plugin-auto-unpack-natives": "^7.4.0",
    "@electron-forge/plugin-fuses": "^7.4.0",
    "@electron/fuses": "^1.8.0",
    "cross-env": "^7.0.3",
    "electron": "^30.0.1",
    "electron-builder": "^24.13.3",
    "electron-rebuild": "^3.2.9",
    "node-abi": "^3.62.0"
  },
  "dependencies": {
    "@serialport/parser-readline": "^12.0.0",
    "electron-squirrel-startup": "^1.0.0",
    "node-window-manager": "^2.2.4",
    "robotjs": "^0.6.0",
    "serialport": "^12.0.0"
  }
}
