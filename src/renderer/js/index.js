document.getElementById("setBtn").addEventListener("click", saveConfig);

function __getAllValues1() {
    const keysData = [];
    const comPort = document.getElementById('comPort').value;
    for (let i = 1; i <= 6; i++) {
        const app = document.getElementById(`gpi${i}app`).value;
        const key = document.getElementById(`gpi${i}key`).value;
        const mod1 = document.getElementById(`gpi${i}mod1`).value;
        const mod2 = document.getElementById(`gpi${i}mod2`).value;

        const keyData = {
            app: app,
            keyTap: {
                key: key,
                modifiers: [mod1, mod2],
            },
        };

        keysData.push(keyData);
    }

    return keysData;
}

async function __setAllValues() {
    const values = await fetchData(window.location.origin + `/api/get-config`, "GET", null);

    // Set the value for the serial port
    if (values.serialPort) {
        document.getElementById('comPort').value = values.serialPort;
    }

    // Iterate over GPI configurations and set their values
    for (let i = 1; i <= 6; i++) {
        const gpiConfig = values[`gpi${i}`];

        if (gpiConfig) {
            if (gpiConfig.app) {
                document.getElementById(`gpi${i}app`).value = gpiConfig.app;
            }

            if (gpiConfig.keyTap) {
                if (gpiConfig.keyTap.key) {
                    document.getElementById(`gpi${i}key`).value = gpiConfig.keyTap.key;
                }

                const modifiers = gpiConfig.keyTap.modifiers;
                if (modifiers && modifiers.length >= 2) {
                    document.getElementById(`gpi${i}mod1`).value = modifiers[0];
                    document.getElementById(`gpi${i}mod2`).value = modifiers[1];
                }
            }
        }
    }
}

async function fetchData(url, method, msg) {
    
    try {
        const response = await fetch(url, {
            method,
            headers: {
                'Content-Type': 'application/json',
            },
            body: msg,
        });

        if (response.ok) {
            const data = await response.json();
            return data;
        } else {
            console.error(`Failed to ${method} data at URL: ${url}`);
        }
    } catch (error) {
        console.error(`Error while fetching data at URL: ${url}`, error);
    }
}

function showNotification(message, isError) {
    const notification = document.getElementById('notification');
    const notificationText = document.getElementById('notification-text');

    notificationText.textContent = message;
    notification.classList.remove('hidden');

    if (isError) {
        notification.classList.add('error');
    } else {
        notification.classList.remove('error');
    }

    setTimeout(() => {
        notification.classList.add('hidden');
    }, 5000); // Hide the notification after 5 seconds
}

async function saveConfig1() {
    const url = window.location.origin + `/api/set-config`;
    const comPortUrl = window.location.origin + `/api/set-comport`;
    const config = __getAllValues();
    const comPort = document.getElementById("comPort").value;

    try {
        const res = await fetchData(comPortUrl,"POST", JSON.stringify({"port":comPort}));
        const res1 = await fetchData(url, 'POST', JSON.stringify(config));
        if (res.status === 'success' && res1.status === "success") {
            showNotification('Config saved', false); // Show success notification
        } else {
            showNotification('Error saving config', true); // Show error notification
        }
    } catch (error) {
        console.error('Error:', error);
        showNotification('Error saving config', true); // Show error notification
    }
}

function __getAllValues() {
    const keysData = [];
    const comport = document.getElementById('comPort').value;
    for (let i = 1; i <= 6; i++) {
        const app = document.getElementById(`gpi${i}app`).value;
        const key = document.getElementById(`gpi${i}key`).value;
        const mod1 = document.getElementById(`gpi${i}mod1`).value;
        const mod2 = document.getElementById(`gpi${i}mod2`).value;

        const keyData = {
            app: app,
            keyTap: {
                key: key,
                modifiers: [mod1, mod2],
            },
        };

        keysData.push(keyData);
    }

    return {keysData, comport};
}

async function saveConfig() {
    const url = window.location.origin + `/api/set-config`;

    try {
        const res = await fetchData(url, 'POST', JSON.stringify(__getAllValues()));
        if (res.status === 'success') {
            showNotification('Config saved', false); // Show success notification
        } else {
            showNotification('Error saving config', true); // Show error notification
        }
    } catch (error) {
        console.error('Error:', error);
        showNotification('Error saving config', true); // Show error notification
    }
}


__setAllValues();