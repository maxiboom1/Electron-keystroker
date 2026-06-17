# Electron-keystroker
 
## Application overview

The primary goal is to develop a GPIO box, enabling by GPI shortcuts triggering of app focus and keystroke transmission.
This setup involves a GPIO box and a Electron desktop application.
Currently, GPIO box is ESP32 based, and connection method is serial (via USB).

Keystroker application operates on the target PC, monitoring serial triggers to execute keystrokes.
It utilizes the robotjs library for key transmission and node-window-manager for managing application focus.
The application has appConfig class that storing cues.
Cue is a data object that includes keystroke values (key, and modifiers), and app name to focus. It also has "name" and "active" properties related to app management => name is the custom name for cue that will be printed on ui cue button, and active is the selected cue - this cue will be fired if we get GPI trigger.
In total - we initialize 9 cues in appConfig.
The app can handle up to 2 modifiers.
Cue data struct:
```
cues:{
    cue1:{
        "active": false,
        "app": "",
        "name": "",
        "keyTap": {
        "key": "",
        "modifiers": []
        }
    },

}
```

AppConfig cue methods:

```
// Provides {cues,serialPort} on app start to render front page UI.
getConfig(){}

// Return cues.cue${number} for front page - when user enter config of cue.
getCueByNumber(number){}

// Returns active cue when GPI trigger received - and we fire it.
getActiveCue()

// Sets cues.cue${number}.active - triggered by user cue select
async setCueToActive(number){}

// Modify cue. expected cue incoming without "active" prop. Triggered when user saves edited cue on UI config page.
async modifyCue(cue, number)

```


## Change-log:

### 1.2.0

- Release build renamed to Keystroker 1.2.0.
- Consolidated the top menu into Settings with Preferences, About, and Exit.
- Reduced button font weight choices to the two reliably rendered states: Regular and Bold.
- Updated About screen year to 2026.

### 1.1.9

- Release build for the Appearance settings and bundled controller driver updates.

### 1.1.8

- Added Appearance settings for cue button typography and selected-state styling.
- Added a live cue button preview that reflects appearance changes before saving.
- Added configurable button font family and font weight, including Calibri.

### 1.1.7

- Removed the top in-app header to keep the cue grid as the primary operator surface.
- Centered cue labels inside buttons and bottom-aligned shortcut indicators.
- Added General settings for cue label size, shortcut label size, and selected button background color.
- Improved selected cue visibility with configurable background color plus red border.
- Updated cue and shortcut typography to use normal font weight.
- Added bundled CH34x USB serial controller driver download from Communication settings.

### 1.1.6

- Finished with app-settings UI - implemented solid settings canvas, with left-sided vertical tabs togglers.
- Save settings now sends config{serialPort, allowGlobals} - while allowGlobals allows user to leave the app field empty and send global keystrokes.
- Cue-button key indicator styling
- Adjusted AppConfig methods and props to handle allowGlobals.
- App version dynamic injection from index.js (should be moved to backend appVersion prop.)

### 1.1.5

- Style: Hidden Vertical scrollbar now on app - small change with big impact app visibility
- Style: Added sticky option to top menu - it always on the top of the screen
- Added new config page struct html with tabs (not styled yet)
### 1.1.4

- Added Application menu: File (Exit), Settings(preferences), About(about page).

### 1.1.3

- HTML && CSS Name convention changed:
* Moved from camelCase (```communicationConfig```) to kebab-case (```app-config-container```), which is a good practice for HTML and CSS.
* Semantic class names like navbar, footer, modal, and notification.
* The use of descriptive IDs like ```gpio-led```, ```set-cue-config```, and ```about-app-container``` makes it easier to understand the functionality of each element.
- CSS file was cleaned from some duplicate rules, minimized and separated by page areas (General, navbar, main buttons, config forms etc..)
- RobotJs focusWindow() adapted to new appConfig methods and cue structure

### 1.1.2

- Changed appConfig data struct and methods.
- UI buttons now Reflects cue custom labels, key+ modifiers data, and active status (red border).
- If user click with mouse on cue button - it set it to active. Then we re-sync the UI.
- If user right-click the cue button - it opens cue setting - where user can modify name, app, key and modifiers.
when he click "save" - it save it to appConfig (without sending "active" prop). Then we re-sync the UI.
- An minimal app overview documentation attempt ("application overview).

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

- After running `npm i`, rebuild RobotJS for the Electron runtime:

```
npm run rebuild:robotjs

```

- Path are relative to html location, not to js.

## Installations

Electron:
```
npm install electron --save-dev
```
Python:

Install it globally on pc (ver 3.xx). Check "Add Python to PATH" option, to make it accessible from the command line.

Electron rebuild:

```
npm install @electron/rebuild@3.7.2 --save-dev
```

## Dependencies

This Electron project's dependencies include 
- Node.js, 
- Python 3.x (for building native modules),
- Visual Studio Build Tools (for compiling native modules on Windows) - Download Visual Studio, and install "Desktop development with C++". 
- electron, 
- robotjs (and its prerequisites like node-gyp for building native addons), 
- node-window-manager for window management functions
- @electron/rebuild
