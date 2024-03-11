# Electron-keystroker
 
## Notes

- After run npm i you should also run 

```
 ./node_modules/.bin/electron-rebuild -f -w robotjs

```

- Path are relative to html location, not to js.


## Dependencies

This Electron project's dependencies include 
Node.js, 
Python 2.7 (for building certain native modules), 
Visual Studio Build Tools (for compiling native modules on Windows), 
electron, 
robotjs (and its prerequisites like node-gyp for building native addons), 
node-window-manager for window management functions
electron-rebuild


        <table class="table-dark">
            <thead>
                <tr>
                    <th scope="col">#</th>
                    <th scope="col">Focus application</th>
                    <th scope="col">Key</th>
                    <th scope="col">Modifiers</th>
                </tr>
            </thead>
            <tbody>
                <!-- GPI-1 -->
                <tr>
                    <th scope="row">GPI-1</th>
                    <td><input type="text" id="gpi1app"></td>
                    <td><input type="text" id="gpi1key"></td>
                    <td>
                        <select name="gpi1mod1" id="gpi1mod1">
                            <option value="">Select modifier-1</option>
                            <option value="alt">Alt</option>
                            <option value="command">Command (win)</option>
                            <option value="control">Control</option>
                            <option value="shift">Shift</option>
                        </select>
                        <select name="gpi1mod2" id="gpi1mod2">
                            <option value="">Select modifier-2</option>
                            <option value="alt">Alt</option>
                            <option value="command">Command (win)</option>
                            <option value="control">Control</option>
                            <option value="shift">Shift</option>
                        </select>
                    </td>
                </tr>

                <!-- Serial comm. -->
                <tr>
                    <th scope="row">COM PORT</th>
                    <td><select name="comPort" id="comPort">
                            <option value="">Select Port</option>
                            <option value="COM1">COM-1</option>
                            <option value="COM2">COM-2</option>
                            <option value="COM3">COM-3</option>
                            <option value="COM4">COM-4</option>
                            <option value="COM5">COM-5</option>
                            <option value="COM6">COM-6</option>
                            <option value="COM7">COM-7</option>
                            <option value="COM8">COM-8</option>
                            <option value="COM9">COM-9</option>
                            <option value="COM10">COM-10</option>
                            <option value="COM11">COM-11</option>
                            <option value="COM12">COM-12</option>

                        </select>
                    </td>
                    <td></td>
                    <td></td>
                </tr>
            </tbody>
        </table>
        <button type="button" class="btn btn-primary btn-lg btn-block" id="setBtn">Save configuration</button>