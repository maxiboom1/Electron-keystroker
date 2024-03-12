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


