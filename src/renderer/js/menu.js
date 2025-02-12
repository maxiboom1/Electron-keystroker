const { showAppConfig } = require('./config.js');
const { ipcRenderer } = require('electron');

// Get all menu items
const menuItems = document.querySelectorAll('.menu-item');

// Add event listeners to each menu item
menuItems.forEach((menuItem) => {
    // Toggle submenu visibility on click
    menuItem.addEventListener('click', (event) => {
        event.stopPropagation(); // Prevent the click from bubbling up
        closeAllSubmenus(); // Close all other submenus
        const submenu = menuItem.querySelector('.submenu');
        if (submenu) {
            submenu.style.display = submenu.style.display === 'block' ? 'none' : 'block';
        }
    });

    // Close submenu when clicking outside
    document.addEventListener('click', () => {
        closeAllSubmenus();
    });
});

// Add event listeners to submenu items
const submenuItems = document.querySelectorAll('.submenu-item');
submenuItems.forEach((submenuItem) => {
    submenuItem.addEventListener('click', (event) => {
        event.stopPropagation(); // Prevent the click from bubbling up
        menuRouter(submenuItem.textContent);
        closeAllSubmenus();
    });
});

// Function to close all submenus
function closeAllSubmenus() {
    const submenus = document.querySelectorAll('.submenu');
    submenus.forEach((submenu) => {
        submenu.style.display = 'none';
    });
}

async function menuRouter(event){
    if(event === "Exit"){
        await ipcRenderer.invoke('appClose');
    }

    if(event === "Preferences"){
        showAppConfig();
    }

    if(event === "About App"){
        openModal("about-app-container");
    }
    
}